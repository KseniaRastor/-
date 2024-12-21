#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <string.h>
#include <locale.h>
#include <time.h>

HANDLE hMutex;
HANDLE hMapFile;
HANDLE hEvent;
int CountUser = 0;

struct UserList
{
	char Login[64];
	char Password[64];
	char NameOfMailslot[128];
	struct UserList* next;
};
struct UserList* node,* head_copy,* previous;
struct UserList* head = NULL;

void PrintCurrentTime() {
	time_t now = time(NULL);
	struct tm* local = localtime(&now);
	printf("%02d.%02d.%4d %02d:%02d:%02d | ",
		local->tm_mday,
		local->tm_mon + 1,
		local->tm_year + 1900,
		local->tm_hour,
		local->tm_min,
		local->tm_sec);
	return;
}

void FuncForTimer() {
	HANDLE hTimer = CreateWaitableTimer(NULL, FALSE, NULL);
	if (hTimer == NULL) { return -503; }

	LARGE_INTEGER liDueTime;
	liDueTime.QuadPart = -(10000000LL * 60LL);	//1 мин
	if (!SetWaitableTimer(hTimer, &liDueTime, 0, NULL, NULL, 0)) {
		CloseHandle(hTimer);
		return -504;	
	}

	WaitForSingleObjectEx(hTimer, INFINITE, TRUE);
	return;
}

DWORD WINAPI SendOnlineUserList(LPVOID lpParam) {
	HANDLE hMailslot;
	DWORD  cbWritten;

	char ListOnlineUser[1024];
	char cUserName[128];
	char cut[] = "\n";

	while (1) {
		WaitForSingleObject(hEvent, INFINITE); // Ожидание события
		ListOnlineUser[0] = '\0';
		WaitForSingleObject(hMutex, INFINITE); // Блокировка мьютекса	

		if (CountUser > 0) {
			head_copy = head;
			while (head_copy != NULL) {
				strcpy(cUserName, head_copy->Login);
				strcat(ListOnlineUser, cUserName);
				strcat(ListOnlineUser, cut);
				head_copy = head_copy->next;
			}

			head_copy = head;
			while (head_copy != NULL) {	

				char NameOfMailslot[128];
				char postfix[7] = "Online";
				strcpy(NameOfMailslot, head_copy->NameOfMailslot);
				strcat(NameOfMailslot, postfix);

				do {
					hMailslot = CreateFileA(NameOfMailslot, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
				} while (hMailslot == INVALID_HANDLE_VALUE);

				if (!WriteFile(hMailslot, ListOnlineUser, strlen(ListOnlineUser), &cbWritten, NULL)) {
					CloseHandle(hMailslot);
					return -502;
				}
				CloseHandle(hMailslot);
				head_copy = head_copy->next;
			}
			ReleaseMutex(hMutex);
			ResetEvent(hEvent);
		}
		Sleep(200);
	}
	return 200;
}

int FuncMailStorage(const char* UserLogin, char* massage) {
	char FileExpansion[] = ".txt";
	char fullFileName[128];
	char designText[2];
	
	memmove(massage, massage + 1, strlen(massage));		//отсечение типа	
	strncpy(designText, massage, 1);					//обработка стиля текста
	designText[1] = '\0';
	memmove(massage, massage + 1, strlen(massage));

	snprintf(fullFileName, sizeof(fullFileName), "%s%s", UserLogin, FileExpansion);	
	FILE* file = fopen(fullFileName, "a");	
	if (file == NULL) {
		printf("Ошибка открытия файла для записи переписки");
		return 508;		
	}
	
	fprintf(file, "%s", designText);

	time_t now = time(NULL);
	struct tm* local = localtime(&now);
	fprintf(file, "%02d.%02d.%4d %02d:%02d:%02d ",
		local->tm_mday,
		local->tm_mon + 1,
		local->tm_year + 1900,
		local->tm_hour,
		local->tm_min,
		local->tm_sec);

	fprintf(file, "%s", massage);
	fclose(file);
	return 200;
}

DWORD WINAPI ThreadFuncForSending(LPVOID lpParam) {
	HANDLE hMailslot;
	DWORD  cbWritten;
	LPCSTR lpszMailslotName = NULL;
	char* buffer = (char*)lpParam;
	
	///
	//выделениие имени получателя для открытия уникального канала
	char* copy_ProtocolMessage = buffer;
	char cRecepientNameSize[4];
	char cUserNameSize[4];
	char cMsgSizeCorrect[5];
	char cMsgSize[5];
	char cNumberCurrentMsg[5];
	char cNumberTotalMsg[5];
	int iRecepientNameSize;
	int iUserNameSize;
	int iMsgSize;
	int iMsgSizeCorrect;
	int iNumberCurrentMsg;
	int iNumberTotalMsg;
	int start_positionFrom = 4;
	int start_positionTo = 7;
	int start_positionMsg = 10;
	char RecepientName[64];
	char UserName[64];
	BOOL FileMsg = FALSE;

	strncpy(cUserNameSize, copy_ProtocolMessage + start_positionFrom, 3);		//выделение имени отправителя
	cUserNameSize[3] = '\0';
	iUserNameSize = atoi(cUserNameSize);
	strncpy(UserName, copy_ProtocolMessage + 22, iUserNameSize);
	UserName[iUserNameSize] = '\0';

	strncpy(cRecepientNameSize, copy_ProtocolMessage + start_positionTo, 3);	//выделение имени получателя
	cRecepientNameSize[3] = '\0';
	iRecepientNameSize = atoi(cRecepientNameSize);
	strncpy(RecepientName, copy_ProtocolMessage + iUserNameSize + 22, iRecepientNameSize);
	RecepientName[iRecepientNameSize] = '\0';

	strncpy(cMsgSizeCorrect, copy_ProtocolMessage + 10, 4);				//размер сообщения
	cMsgSizeCorrect[4] = '\0';
	iMsgSizeCorrect = atoi(cMsgSizeCorrect);

	strncpy(cNumberCurrentMsg, copy_ProtocolMessage + 14, 4);				//номер передаваемого сообщения (для файла)
	cNumberCurrentMsg[4] = '\0';
	iNumberCurrentMsg = atoi(cNumberCurrentMsg);

	strncpy(cNumberTotalMsg, copy_ProtocolMessage + 18, 4);				//общее количество сообщений для файла
	cNumberTotalMsg[4] = '\0';
	iNumberTotalMsg = atoi(cNumberTotalMsg);
		

	//выделение сообщения и стиля форматирования текста
	int iMsgPosition = 22 + iUserNameSize + iRecepientNameSize;
	iMsgSize = strlen(buffer) - iMsgPosition + 1;
	
	if (((buffer[2] == '0') && (iMsgSize == iMsgSizeCorrect + 1)) || ((buffer[2] != '0') && (iMsgSize != iMsgSizeCorrect + 1))) {
		char* Msg = malloc(iMsgSize * sizeof(char) + 1);
		char* Msg_copy = malloc(iMsgSize * sizeof(char) + 1);
		char* PartProtocolDesign = malloc(iMsgSize * sizeof(char) + 1);
		char* PartProtocolType = malloc(iMsgSize * sizeof(char) + 10);
		char* PartNumCurrentMsg = malloc(iMsgSize * sizeof(char) + 1);
		char* PartNumTotalMsg = malloc(iMsgSize * sizeof(char) + 1);

		memset(Msg, 0, iMsgSize + 1);
		memset(Msg_copy, 0, iMsgSize + 1);
		memset(PartProtocolDesign, 0, iMsgSize + 1);
		memset(PartProtocolType, 0, iMsgSize + 1);

		strncpy(PartProtocolType, copy_ProtocolMessage + 2, 1);
		PartProtocolType[1] = '\0';
		if (strcmp(PartProtocolType, "0") == 0) {									//для текстового сообщения
			strncpy(PartProtocolDesign, copy_ProtocolMessage + 3, 1);
			PartProtocolDesign[1] = '\0';
			strcat(PartProtocolType, PartProtocolDesign);
		}
		else {																		//для файла
			strncpy(PartNumCurrentMsg, copy_ProtocolMessage + 14, 4);
			PartNumCurrentMsg[4] = '\0';
			strncpy(PartNumTotalMsg, copy_ProtocolMessage + 18, 4);
			PartNumTotalMsg[4] = '\0';
			strcat(PartProtocolType, PartNumCurrentMsg);
			strcat(PartProtocolType, PartNumTotalMsg);
		}
		strncpy(Msg, copy_ProtocolMessage + iMsgPosition, iMsgSize);
		Msg[iMsgSize] = '\0';
		strcat(PartProtocolType, Msg);
		strcpy(Msg, PartProtocolType);

		free(PartProtocolDesign); 
		free(PartProtocolType);
		free(PartNumCurrentMsg);
		free(PartNumTotalMsg);
		
		if (buffer[1] == '1') FuncForTimer();
		if (buffer[2] != '0') FileMsg = TRUE;

		head_copy = head;
		if (buffer[0] == '0') {
			while (head_copy != NULL) {																						
														//личное сообщение - текстовое
					if ((strcmp(head_copy->Login, RecepientName) == 0) || (strcmp(head_copy->Login, UserName) == 0)) {
						if (buffer[2] == '0') {
							if ((FileMsg) && (strcmp(head_copy->Login, UserName) == 0)) {
								head_copy = head_copy->next;
								continue;
							}

							hMailslot = CreateFileA(head_copy->NameOfMailslot, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
							if (hMailslot == INVALID_HANDLE_VALUE) fprintf(stdout, "GetMailslotInfo: Error %ld\n", GetLastError());

							if (!WriteFile(hMailslot, Msg, strlen(Msg), &cbWritten, NULL)) {
								CloseHandle(hMailslot);
								return -501;		
							}
							CloseHandle(hMailslot);
							//фукция записи в файл
						
							strcpy(Msg_copy, Msg);
							FuncMailStorage(head_copy->Login, Msg_copy);
						}
						else {
							if ((FileMsg) && (strcmp(head_copy->Login, UserName) == 0)) {
								head_copy = head_copy->next;
								continue;
							}

							hMailslot = CreateFileA(head_copy->NameOfMailslot, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
							if (hMailslot == INVALID_HANDLE_VALUE) fprintf(stdout, "GetMailslotInfo: Error %ld\n", GetLastError());


							char type = buffer[2];
							char fullFuleName[512] = "recepient.";
							if (buffer[2] == '1') strcat(fullFuleName, "txt");
							if (buffer[2] == '2') strcat(fullFuleName, "bmp");
							if (buffer[2] == '3') strcat(fullFuleName, "jpg");
							if (buffer[2] == '4') strcat(fullFuleName, "gif");
							if (buffer[2] == '5') strcat(fullFuleName, "png");

							FILE* inFile;
							char bufferFile[300];
							size_t bytesRead;

							
							inFile = fopen(fullFuleName, "rb");
							if (inFile == NULL) {
								fprintf(stderr, "Ошибка при открытии файла: %s\n", fullFuleName);			// ошибка - открытие файла
								CloseHandle(hMailslot);
								return 508;
							}

							fseek(inFile, 0, SEEK_END);
							int sizeFile = ftell(inFile);
							fseek(inFile, 0, SEEK_SET);

							int iCurrentMsg = 1;
							int SendPosition = 0;
							while (iNumberTotalMsg > 0) {
								int chunkSize = (SendPosition + 300 <= sizeFile) ? 300 : (sizeFile - SendPosition);

								bytesRead = fread(bufferFile, 1, chunkSize, inFile);
								char* dataWithPrefix = malloc((315 + iUserNameSize) * sizeof(char) + 1);
								memcpy(dataWithPrefix, &type, 1);
								char cCurrentMsg[5];
								sprintf(cCurrentMsg, "%04d", iCurrentMsg);
								memcpy(dataWithPrefix + 1, cCurrentMsg, 4);
								memcpy(dataWithPrefix + 5, cNumberTotalMsg, 4);	
								
								memcpy(dataWithPrefix + 9, cUserNameSize, 3);
								memcpy(dataWithPrefix + 12, UserName, iUserNameSize);

								memcpy(dataWithPrefix + 12 + iUserNameSize, bufferFile, 300);
								
						
								if (!WriteFile(hMailslot, dataWithPrefix, chunkSize + 12 + iUserNameSize, &cbWritten, NULL)) {
									CloseHandle(hMailslot);
									return -502;
								}
								iNumberTotalMsg--;
								iCurrentMsg++;
								SendPosition += 300;
							}

							fclose(inFile);
							CloseHandle(hMailslot);
						}
					}	

				head_copy = head_copy->next;
			}
		}
		else {

				while (head_copy != NULL) {																				//широковещ сообщение - текстовое
					
					if ((FileMsg) && (strcmp(head_copy->Login, UserName) == 0)) {
						head_copy = head_copy->next;
						continue;
					}
						
					
					hMailslot = CreateFileA(head_copy->NameOfMailslot, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
					if (hMailslot == INVALID_HANDLE_VALUE) fprintf(stdout, "GetMailslotInfo: Error %ld\n", GetLastError());

					if (!WriteFile(hMailslot, Msg, strlen(Msg), &cbWritten, NULL)) {
						CloseHandle(hMailslot);
						return -501;		
					}

					CloseHandle(hMailslot);
					//фукция записи в файл
					if (buffer[2] == '0') {
						strcpy(Msg_copy, Msg);
						FuncMailStorage(head_copy->Login, Msg_copy);
					}
					head_copy = head_copy->next;
				}
		}
		PrintCurrentTime();
		printf("Сообщение успешно отправлено.\n\n");
		return 200;


	}
	else {
		char Msg[] = "Некорректная отправка сообщения.\n";
		printf("%s", Msg);
		while (head_copy != NULL) {									
			if (strcmp(head_copy->Login, UserName) == 0) {
				hMailslot = CreateFileA(head_copy->NameOfMailslot, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
				if (hMailslot == INVALID_HANDLE_VALUE) fprintf(stdout, "GetMailslotInfo: Error %ld\n", GetLastError());

				if (!WriteFile(hMailslot, Msg, strlen(Msg), &cbWritten, NULL)) {
					CloseHandle(hMailslot);
					return -502;	
				}

				return 505;
			}
			head_copy = head_copy->next;
		}	
	}
}

DWORD WINAPI ClientDisconnection(LPVOID lpParam) {
	HANDLE hMailslotDisconnection;
	BOOL fReturnCode;
	DWORD cbMessages;
	DWORD cbMsgNumber;
	char szBuf[512];
	DWORD cbRead;

	hMailslotDisconnection = CreateMailslot(TEXT("\\\\.\\mailslot\\ClientDisconnectionChannel"), 0, MAILSLOT_WAIT_FOREVER, NULL);
	if (hMailslotDisconnection == INVALID_HANDLE_VALUE) return -501;

	while (1) {
		fReturnCode = GetMailslotInfo(hMailslotDisconnection, NULL, &cbMessages, &cbMsgNumber, NULL);
		if (!fReturnCode) {
			fprintf(stdout, "GetMailslotInfo: Error %ld\n", GetLastError());
		}

		if (cbMessages != MAILSLOT_NO_MESSAGE) {
			if (!ReadFile(hMailslotDisconnection, szBuf, sizeof(szBuf), &cbRead, NULL)) {
				fprintf(stdout, "ReadFileEx: Error %ld\n", GetLastError());
			}
			else {
				szBuf[cbRead] = '\0';
				
				//удаление пользователя
				head_copy = head;
				previous = NULL;
				while (head_copy != NULL) {
					if (strcmp(head_copy->Login, szBuf) == 0) {
						if (previous == NULL) {
							head = head_copy->next;
						}
						else {
							previous->next = head_copy->next;
						}
						free(head_copy);
						break;
					}
					previous = head_copy;
					head_copy = head_copy->next;
				}
				SetEvent(hEvent);
				printf("Пользователь %s вышел из сети.\n", szBuf);
			}
		}
		Sleep(20);
	}
}

DWORD WINAPI UserEnter(LPVOID lpParam) {
	HANDLE hMailslotRegistration;
	HANDLE hMailslotUnicNameUser;
	BOOL fReturnCode;
	DWORD cbMsgRegistration;
	DWORD cbMsgRegistrationNumber;
	char szBuf[512];
	DWORD cbRead;
	DWORD  cbWritten;

	char LoginError[] = "406_Not_Acceptable_Login";
	char PasswordError[] = "406_Not_Acceptable_Password";
	BOOL FlagRepeatUser = FALSE;
	char tmpLogin[64];
	char tmpPassword[64];
	char LoginFromBase[64];
	char PasswordFromBase[64];
	char UnicNameChannel[128];
	char* DataPart;
	char* szBufCopy[512];

	hMailslotRegistration = CreateMailslot(TEXT("\\\\.\\mailslot\\EnterChannel"), 0, MAILSLOT_WAIT_FOREVER, NULL);
	if (hMailslotRegistration == INVALID_HANDLE_VALUE) return -501;

	while (1) {
		fReturnCode = GetMailslotInfo(hMailslotRegistration, NULL, &cbMsgRegistration, &cbMsgRegistrationNumber, NULL);
		if (!fReturnCode) {
			fprintf(stdout, "GetMailslotInfo: Error %ld\n", GetLastError());
		}

		if (cbMsgRegistration != MAILSLOT_NO_MESSAGE) {
			if (!ReadFile(hMailslotRegistration, szBuf, sizeof(szBuf), &cbRead, NULL)) {
				fprintf(stdout, "ReadFileEx: Error %ld\n", GetLastError());
			}
			else {
				szBuf[cbRead] = '\0';

				DataPart = strtok(szBuf, " ");
				strcpy(tmpLogin, DataPart);
				DataPart = strtok(NULL, " ");
				strcpy(tmpPassword, DataPart);



				char line[128];
				FILE* file = fopen("registration.txt", "r");
				if (file == NULL) {
					printf("Ошибка открытия файла для чтения");
					return 508;
				}

				FlagRepeatUser = FALSE;								//проверка на наличие указанного пользователя
				while (fgets(line, sizeof(line), file) != NULL) {
					DataPart = strtok(line, " ");
					strcpy(LoginFromBase, DataPart);
					DataPart = strtok(NULL, " ");
					strcpy(PasswordFromBase, DataPart);
					int len = strlen(PasswordFromBase);
					PasswordFromBase[len-1] = '\0';

					/*char* space_ptr = strchr(line, ' ');
					if (space_ptr != NULL) {
						*space_ptr = '\0';
					}*/

					if (strcmp(LoginFromBase, tmpLogin) == 0) {
						if (strcmp(PasswordFromBase, tmpPassword) == 0) {
							WaitForSingleObject(hMutex, INFINITE);				// ожидание мьютекса
							node = (struct UserList*)malloc(sizeof(struct UserList));
							node->next = NULL;
							strcpy(node->Login, tmpLogin);
							strcpy(node->Password, tmpPassword);

							strcpy(UnicNameChannel, "\\\\.\\mailslot\\");
							strcat(UnicNameChannel, node->Login);
							strcpy(node->NameOfMailslot, UnicNameChannel);

							if (head == NULL) {
								head = node;
							}
							else {
								head_copy = head;
								while (head_copy->next != NULL) {
									head_copy = head_copy->next;
								}
								head_copy->next = node;
							}

							ReleaseMutex(hMutex);
							CountUser++;
							SetEvent(hEvent);
							printf("Пользователь %s появился в сети.\n", szBuf);

							break;
						}
						else {
							strcpy(UnicNameChannel, PasswordError);
							break;
						}
						
					}
					else {
						strcpy(UnicNameChannel, LoginError);
					}
				}
				fclose(file);

				do {
					hMailslotUnicNameUser = CreateFile(TEXT("\\\\.\\mailslot\\GetUnicNameFromServerChannel"), GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
				} while (hMailslotUnicNameUser == INVALID_HANDLE_VALUE);

				if (!WriteFile(hMailslotUnicNameUser, UnicNameChannel, strlen(UnicNameChannel), &cbWritten, NULL)) {
					CloseHandle(hMailslotUnicNameUser);
					fprintf(stdout, "WriteFile: Error %ld\n", GetLastError());
					return -502;
				}
				CloseHandle(hMailslotUnicNameUser);
			}
		}
		Sleep(1);
	}
}

DWORD WINAPI UserRegistration(LPVOID lpParam) {
	HANDLE hMailslotRegistration;
	HANDLE hMailslotUnicNameUser;
	BOOL fReturnCode;
	DWORD cbMsgRegistration;
	DWORD cbMsgRegistrationNumber;
	char szBuf[512];
	DWORD cbRead;
	DWORD  cbWritten;

	char LoginError[] = "406_Not_Acceptable_Login";
	BOOL FlagRepeatUser = FALSE;
	char tmpLogin[64];
	char tmpPassword[64];
	char UnicNameChannel[128]; 
	char* DataPart;
	char* szBufCopy[512];
	
	hMailslotRegistration = CreateMailslot(TEXT("\\\\.\\mailslot\\RegistrationOnServerChannel"), 0, MAILSLOT_WAIT_FOREVER, NULL);
	if (hMailslotRegistration == INVALID_HANDLE_VALUE) return -501;

	while (1) {
		fReturnCode = GetMailslotInfo(hMailslotRegistration, NULL, &cbMsgRegistration, &cbMsgRegistrationNumber, NULL);
		if (!fReturnCode) {
			fprintf(stdout, "GetMailslotInfo: Error %ld\n", GetLastError());
		}

		if (cbMsgRegistration != MAILSLOT_NO_MESSAGE) {
			if (!ReadFile(hMailslotRegistration, szBuf, sizeof(szBuf), &cbRead, NULL)) {
				fprintf(stdout, "ReadFileEx: Error %ld\n", GetLastError());
			}
			else {
				szBuf[cbRead] = '\0';

				DataPart = strtok(szBuf, " ");
				strcpy(tmpLogin, DataPart);
				DataPart = strtok(NULL, " ");
				strcpy(tmpPassword, DataPart);


				
				char line[128];
				FILE* file = fopen("registration.txt", "r");
				if (file == NULL) {
					printf("Ошибка открытия файла для чтения");
					return 508;
				}
	
				FlagRepeatUser = FALSE;								//проверка на наличие такого же пользователя
				while (fgets(line, sizeof(line), file) != NULL) {
					char* space_ptr = strchr(line, ' ');
					if (space_ptr != NULL) {
						*space_ptr = '\0'; 
					}

					if (strcmp(line, tmpLogin) == 0) {
						strcpy(UnicNameChannel, LoginError);
						FlagRepeatUser = TRUE;
						break;
					}
				}
				fclose(file);

				if(!FlagRepeatUser) {
					FILE* fileAdd = fopen("registration.txt", "a");
					fprintf(fileAdd, "%s ", tmpLogin);
					fprintf(fileAdd, "%s\n", tmpPassword);
					fclose(file);
				}
				
				if (!FlagRepeatUser) {
					WaitForSingleObject(hMutex, INFINITE);				// ожидание мьютекса
					node = (struct UserList*)malloc(sizeof(struct UserList));
					node->next = NULL;
					strcpy(node->Login, tmpLogin);
					strcpy(node->Password, tmpPassword);

					strcpy(UnicNameChannel, "\\\\.\\mailslot\\");
					strcat(UnicNameChannel, node->Login);
					strcpy(node->NameOfMailslot, UnicNameChannel);

					if (head == NULL) {
						head = node;
					}
					else {
						head_copy = head;
						while (head_copy->next != NULL) {
							head_copy = head_copy->next;
						}
						head_copy->next = node;
					}
					
					ReleaseMutex(hMutex);
					CountUser++;
					SetEvent(hEvent);
					printf("Зарегистрирован новый пользователь %s.\n", szBuf);
				}
				
				do {
					hMailslotUnicNameUser = CreateFile(TEXT("\\\\.\\mailslot\\GetUnicNameFromServerChannel"), GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
				} while (hMailslotUnicNameUser == INVALID_HANDLE_VALUE);
								
				if (!WriteFile(hMailslotUnicNameUser, UnicNameChannel, strlen(UnicNameChannel), &cbWritten, NULL)) {
					CloseHandle(hMailslotUnicNameUser);
					fprintf(stdout, "WriteFile: Error %ld\n", GetLastError());
					return -502;
				}
				CloseHandle(hMailslotUnicNameUser);
			}
		}
		Sleep(1);
	}
}


int main() {
	setlocale(LC_ALL, "rus");
	SetConsoleCP(1251);
	SetConsoleOutputCP(1251);

	hMutex = CreateMutex(NULL, FALSE, NULL);
	if (hMutex == NULL) {
		fprintf(stderr, "CreateMutex (%ld)\n", GetLastError());
		return 506;
	}

	hEvent = CreateEvent(NULL, FALSE, FALSE, "UserRegistrationEvent");
	if (hEvent == NULL) {
		fprintf(stderr, "CreateEvent (%ld)\n", GetLastError());
		return 507;
	}


	HANDLE hMailslot;
	BOOL fReturnCode;
	DWORD cbMessages;
	DWORD cbMsgNumber; 
	char szBuf[512];
	DWORD cbRead; 

	HANDLE hThreads;

	HANDLE hThreadsRegistration;
	hThreadsRegistration = CreateThread(NULL, 0, UserRegistration, NULL, 0, NULL);

	HANDLE hThreadsEnter;
	hThreadsEnter = CreateThread(NULL, 0, UserEnter, NULL, 0, NULL);

	HANDLE hThreadsDisconnection;
	hThreadsDisconnection = CreateThread(NULL, 0, ClientDisconnection, NULL, 0, NULL);

	HANDLE hThreadsOnlineUserList;
	hThreadsDisconnection = CreateThread(NULL, 0, SendOnlineUserList, NULL, 0, NULL);

	hMailslot = CreateMailslot(TEXT("\\\\.\\mailslot\\ChannelFromServer"), 0, MAILSLOT_WAIT_FOREVER, NULL);
	if (hMailslot == INVALID_HANDLE_VALUE) return -501;

	char buffer[300];

	while (1) {

		fReturnCode = GetMailslotInfo(hMailslot, NULL, &cbMessages, &cbMsgNumber, NULL);
		if (!fReturnCode) {
			fprintf(stdout, "GetMailslotInfo: Error %ld\n", GetLastError());
		}

		if (cbMessages != MAILSLOT_NO_MESSAGE) {
			if (!ReadFile(hMailslot, szBuf, sizeof(szBuf), &cbRead, NULL)) {
				fprintf(stdout, "ReadFileEx: Error %ld\n", GetLastError());
			}
			else {
				szBuf[cbRead] = '\0';
				PrintCurrentTime();
				printf("Получено новое сообщение: %s", szBuf);

				if (szBuf[2] == '0') {
					hThreads = CreateThread(NULL, 0, ThreadFuncForSending, (LPVOID)szBuf, 0, NULL);			
					WaitForSingleObject(hThreads, INFINITE);
				}	
				else {
					char type = szBuf[2];
					char fullFuleName[512] = "recepient.";
					if (type == '1') strcat(fullFuleName, "txt");
					if (type == '2') strcat(fullFuleName, "bmp");
					if (type == '3') strcat(fullFuleName, "jpg");
					if (type == '4') strcat(fullFuleName, "gif");
					if (type == '5') strcat(fullFuleName, "png");

					size_t totalSize = cbRead;
					char* dataForFile = malloc(cbRead * sizeof(char) + 1);
					char* prefix = malloc(26 * sizeof(char) + 1);
					memcpy(prefix, szBuf, 26);
					memcpy(szBuf, szBuf+26, cbRead);

					FILE* outFile = fopen(fullFuleName, "ab");  
					if (outFile == NULL) {
						fprintf(stderr, "Error opening file for writing\n");
					}

					size_t written = fwrite(szBuf, 1, cbRead-26, outFile);
					if (written != cbRead-26) {
						fprintf(stderr, "Error writing data to file\n");
						fclose(outFile);
					}

					fclose(outFile);
					printf("Received data: %lu bytes\n", cbRead-26);

					char* PartNumCurrentMsg = malloc(4 * sizeof(char) + 1);
					char* PartNumTotalMsg = malloc(4 * sizeof(char) + 1);

					strncpy(PartNumCurrentMsg, prefix + 14, 4);
					PartNumCurrentMsg[4] = '\0';
					strncpy(PartNumTotalMsg, prefix + 18, 4);
					PartNumTotalMsg[4] = '\0';
					if (strcmp(PartNumCurrentMsg, PartNumTotalMsg) == 0) {
						hThreads = CreateThread(NULL, 0, ThreadFuncForSending, (LPVOID)prefix, 0, NULL);
					}
					
				}
			}
		}
		Sleep(1);
	}

	CloseHandle(hMailslot);
	CloseHandle(hMutex);
	return 0;
}