#include "header.h"

#include <iostream>
#include <fstream>
#include <cstring>
#include <cstddef>
#include <string>

#include <sstream>
#include <string>
#include <iostream>

#include <fstream>
#include <vector>
#include <iterator>

HANDLE globalhMailslot;
HANDLE globalhMailslotOnline;

extern "C" __declspec(dllexport) char* __stdcall ReadingOldMessage(char* UserLogin) {
    char FileExpansion[] = ".txt";
    char fullFileName[128];
    char FilePath[256] = "d:\\ПГУ. Учебные пособия\\5 семестр\\основы ОС\\курсовая\\messenger_server\\messenger_server\\";

    char fullBuffer[2024] = "";

    snprintf(fullFileName, sizeof(fullFileName), "%s%s", UserLogin, FileExpansion);
    strcat(FilePath, fullFileName);

    FILE* file = fopen(FilePath, "r");
    if (file == NULL) {
        MessageBox(0, "Ошибка открытия файла для чтения", "Ошибка", MB_OK);
    }

    char Buffer[512];
    while (fgets(Buffer, sizeof(Buffer), file) != NULL) {
        strcat(fullBuffer, Buffer);
    }

    fclose(file);

    size_t stSize = strlen(fullBuffer) + sizeof(char);
    char* pszReturn = NULL;

    pszReturn = (char*)::CoTaskMemAlloc(stSize);
    strcpy_s(pszReturn, stSize, fullBuffer);
    return pszReturn;
}

extern "C" __declspec(dllexport) int __stdcall UserEnter(char* UserDate) {
    HANDLE hMailslot;
    DWORD  cbWritten;
    LPCSTR lpMailslotUnicNameUser;

    char* copy_UserData = UserDate;
    char prefixMailslotName[128] = "\\\\.\\mailslot\\";

    hMailslot = CreateFile(TEXT("\\\\.\\mailslot\\EnterChannel"), GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
    if (hMailslot == INVALID_HANDLE_VALUE) return 402;

    if (!WriteFile(hMailslot, UserDate, strlen(UserDate), &cbWritten, NULL)) {
        CloseHandle(hMailslot);
        return 403;
    }

    HANDLE hMailslotUnicName;
    BOOL fReturnCode;
    DWORD cbMessages = 0;
    DWORD cbMsgNumber = 0;
    char szBuf[512];
    DWORD cbRead;

    hMailslotUnicName = CreateMailslot(TEXT("\\\\.\\mailslot\\GetUnicNameFromServerChannel"), 0, MAILSLOT_WAIT_FOREVER, NULL);
    if (hMailslotUnicName == INVALID_HANDLE_VALUE) return 401;

    while (1) {
        fReturnCode = GetMailslotInfo(hMailslotUnicName, NULL, &cbMessages, &cbMsgNumber, NULL);
        if (!fReturnCode) {
            fprintf(stdout, "GetMailslotInfo: Error %ld\n", GetLastError());
        }

        if (cbMessages != MAILSLOT_NO_MESSAGE) {
            if (!ReadFile(hMailslotUnicName, szBuf, sizeof(szBuf), &cbRead, NULL)) {
                fprintf(stdout, "ReadFileEx: Error %ld\n", GetLastError());
            }
            else {
                szBuf[cbRead] = '\0';
                break;
            }
        }
        Sleep(1);
    }
    CloseHandle(hMailslotUnicName);

    char LoginError[] = "406_Not_Acceptable_Login";
    char PasswordError[] = "406_Not_Acceptable_Password";

    if (strcmp(szBuf, LoginError) == 0) {
        MessageBox(0, "Пользователь с указанным логином не найден", "Уведомление", MB_OK);
        return 406;
    }

    if (strcmp(szBuf, PasswordError) == 0) {
        MessageBox(0, "Неверный пароль", "Уведомление", MB_OK);
        return 406;
    }

    lpMailslotUnicNameUser = szBuf;

    globalhMailslot = CreateMailslot(lpMailslotUnicNameUser, 0, MAILSLOT_WAIT_FOREVER, NULL);
    if (hMailslot == INVALID_HANDLE_VALUE) MessageBox(0, "Ошибка при регистрации", "Уведомление", MB_OK);
    else MessageBox(0, "Успешеный вход в систему", "Уведомление", MB_OK);

    char postfix[7] = "Online";
    strcat(szBuf, postfix);
    LPCSTR lpMailslotUnicNameUserOnline = szBuf;

    globalhMailslotOnline = CreateMailslot(lpMailslotUnicNameUserOnline, 0, MAILSLOT_WAIT_FOREVER, NULL);;
    if (globalhMailslotOnline == INVALID_HANDLE_VALUE) MessageBox(0, "44Ошибка при регистрации", "Уведомление", MB_OK);

    return 200;
}

extern "C" __declspec(dllexport) int __stdcall UserRegistration(char* UserDate) {
    HANDLE hMailslot;
    DWORD  cbWritten;
    LPCSTR lpMailslotUnicNameUser;

    char* copy_UserData = UserDate;
    char prefixMailslotName[128] = "\\\\.\\mailslot\\";

    hMailslot = CreateFile(TEXT("\\\\.\\mailslot\\RegistrationOnServerChannel"), GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
    if (hMailslot == INVALID_HANDLE_VALUE) return 402;

    if (!WriteFile(hMailslot, UserDate, strlen(UserDate), &cbWritten, NULL)) {
        CloseHandle(hMailslot);
        return 403;
    }

    HANDLE hMailslotUnicName;
    BOOL fReturnCode;
    DWORD cbMessages =0;
    DWORD cbMsgNumber =0;
    char szBuf[512];
    DWORD cbRead;

    hMailslotUnicName = CreateMailslot(TEXT("\\\\.\\mailslot\\GetUnicNameFromServerChannel"), 0, MAILSLOT_WAIT_FOREVER, NULL);
    if (hMailslotUnicName == INVALID_HANDLE_VALUE) return 401;

    while (1) {
        fReturnCode = GetMailslotInfo(hMailslotUnicName, NULL, &cbMessages, &cbMsgNumber, NULL);
        if (!fReturnCode) {
            fprintf(stdout, "GetMailslotInfo: Error %ld\n", GetLastError());
        }

        if (cbMessages != MAILSLOT_NO_MESSAGE) {
            if (!ReadFile(hMailslotUnicName, szBuf, sizeof(szBuf), &cbRead, NULL)) {
                fprintf(stdout, "ReadFileEx: Error %ld\n", GetLastError());
            }
            else {
                szBuf[cbRead] = '\0';
                break;
            }
        }
        Sleep(1);
    }
    CloseHandle(hMailslotUnicName);

    char LoginError[] = "406_Not_Acceptable_Login";

    if (strcmp(szBuf, LoginError) == 0) {
        MessageBox(0, "Измените имя пользователя. \nПользователь с таким именем уже существует.", "Уведомление", MB_OK);
        return 406;
    }
    
    lpMailslotUnicNameUser = szBuf;

    globalhMailslot = CreateMailslot(lpMailslotUnicNameUser, 0, MAILSLOT_WAIT_FOREVER, NULL);
    if (hMailslot == INVALID_HANDLE_VALUE) MessageBox(0, "Ошибка при регистрации", "Уведомление", MB_OK);
    else MessageBox(0, "Регистрация прошла успешно", "Уведомление", MB_OK);

    char postfix[7] = "Online";
    strcat(szBuf, postfix);
    LPCSTR lpMailslotUnicNameUserOnline = szBuf;

    globalhMailslotOnline = CreateMailslot(lpMailslotUnicNameUserOnline, 0, MAILSLOT_WAIT_FOREVER, NULL);;
    if (globalhMailslotOnline == INVALID_HANDLE_VALUE) MessageBox(0, "44Ошибка при регистрации", "Уведомление", MB_OK);

    return 200; 
}

extern "C" __declspec(dllexport) int __stdcall NotifyServerAboutDisconnection(char* UserName) {
    HANDLE hMailslot;
    LPCSTR lpMailslotName = TEXT("\\\\.\\mailslot\\ClientDisconnectionChannel");
    DWORD  cbWritten;

    hMailslot = CreateFile(lpMailslotName, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
    if (hMailslot == INVALID_HANDLE_VALUE) return 402;

    if (!WriteFile(hMailslot, UserName, strlen(UserName), &cbWritten, NULL)) {
        CloseHandle(hMailslot);
        return 403;
    }

    CloseHandle(hMailslot);
    return 200;
}


extern "C" __declspec(dllexport) int __stdcall myfuncSendMessage(char* ProtocolMessage) {
    HANDLE hMailslot;
    LPCSTR lpMailslotName = TEXT("\\\\.\\mailslot\\ChannelFromServer");
    DWORD  cbWritten;

    hMailslot = CreateFile(lpMailslotName, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
    if (hMailslot == INVALID_HANDLE_VALUE) return -401;

    if (!WriteFile(hMailslot, ProtocolMessage, strlen(ProtocolMessage), &cbWritten, NULL)) {
        CloseHandle(hMailslot);
        return -502;
    }

    CloseHandle(hMailslot);
    return 200;
}

extern "C" __declspec(dllexport) int __stdcall myfuncSendFile(char* filePath, char* ProtocolMessage, int SendPosition, int chunkSize) {
    HANDLE hMailslot;
    LPCSTR lpMailslotName = TEXT("\\\\.\\mailslot\\ChannelFromServer");
    DWORD  cbWritten;
    char buffer[300];

    hMailslot = CreateFile(lpMailslotName, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
    if (hMailslot == INVALID_HANDLE_VALUE) return 401;

    std::ifstream inFile(filePath, std::ios::binary);
    if (!inFile) {
        return 402;                                               //ошибка открытия файла
    }

    inFile.seekg(SendPosition, std::ios::beg);
    inFile.read(buffer, chunkSize);
    size_t totalSize = 26 + chunkSize; 
    char* dataWithPrefix = new char[totalSize];
    memcpy(dataWithPrefix, ProtocolMessage, 26);
    memcpy(dataWithPrefix + 26, buffer, chunkSize);

    if (!WriteFile(hMailslot, dataWithPrefix, totalSize, &cbWritten, NULL)) {
        CloseHandle(hMailslot);
        return -502;
    }
    
    inFile.close();
    CloseHandle(hMailslot);

    return 200;
}

extern "C" __declspec(dllexport) char* __stdcall myfuncOnlineUserList() {
    HANDLE hMailslot;
    BOOL fReturnCode;
    DWORD cbMessages;
    DWORD cbMsgNumber;
    char szBuf[2048];
    DWORD cbRead;

    hMailslot = globalhMailslotOnline;

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
            strcpy(globalBufOnline, szBuf); 
        }
    }
        
    size_t stSize = strlen(globalBufOnline) + sizeof(char);
    char* pszReturn = NULL;

    pszReturn = (char*)::CoTaskMemAlloc(stSize);
    strcpy_s(pszReturn, stSize, globalBufOnline);
    return pszReturn;
}


extern "C" __declspec(dllexport) char* __stdcall myfuncReceiptMessage() {
    HANDLE hMailslot;
    BOOL fReturnCode;
    DWORD cbMessages;
    DWORD cbMsgNumber;
    char szBuf[2048];
    DWORD cbRead;

    char design[2];
    int NumMsg;
    int tmpNumMsg = 0;
    char globalBuf_copy[512];

    hMailslot = globalhMailslot;

    strcpy(globalBuf_copy, globalBuf);

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


            char type = szBuf[0];

            if (type == '0') {
                globalBuf[0] = '\0';
                memmove(szBuf, szBuf + 1, strlen(szBuf));           //отсечение type
                strcat(globalBuf, szBuf);

            }
            else {
                char fullFuleName[512] = "recepient.";
                if (type == '1') strcat(fullFuleName, "txt");
                if (type == '2') strcat(fullFuleName, "bmp");
                if (type == '3') strcat(fullFuleName, "jpg");
                if (type == '4') strcat(fullFuleName, "gif");
                if (type == '5') strcat(fullFuleName, "png");

                char bufferFile[300];
                char cNumberCurrentMsg[5];
                char cNumberTotalMsg[5];
                char cUserNameSize[4];
                char cUserName[128];
                int iNumberCurrentMsg;
                int iNumberTotalMsg;
                int iUserNameSize;

                strncpy(cNumberCurrentMsg, szBuf+1, 4);				//номер передаваемого сообщения
                cNumberCurrentMsg[4] = '\0';
                iNumberCurrentMsg = atoi(cNumberCurrentMsg);

                strncpy(cNumberTotalMsg, szBuf + 5, 4);				//общее количество сообщений
                cNumberTotalMsg[4] = '\0';
                iNumberTotalMsg = atoi(cNumberTotalMsg);


                strncpy(cUserNameSize, szBuf + 9, 3);				//имя отправителя
                cUserNameSize[3] = '\0';
                iUserNameSize = atoi(cUserNameSize);
                strncpy(cUserName, szBuf + 12, 3);
                cUserName[iUserNameSize] = '\0';

                memmove(bufferFile, szBuf + 12 + iUserNameSize, cbRead - 12 - iUserNameSize);



                FILE* outFile = fopen(fullFuleName, "ab");  

                if (outFile == NULL) {
                    fprintf(stderr, "Error opening file for writing\n");
                }

                size_t written = fwrite(bufferFile, 1, cbRead - 12 - iUserNameSize, outFile);
                if (written != cbRead) {
                    fprintf(stderr, "Error writing data to file\n");
                    fclose(outFile);
                }

                fclose(outFile);
                printf("Received data: %lu bytes\n", cbRead);


                if (iNumberCurrentMsg == iNumberTotalMsg) {
                    MessageBox(0, "Получен новый файл", "Уведомление", MB_OK);

                    char ProtocolAnswer[128] = "000000000000000000000000";
                    char Answer[] = "Ваш файл получен\n";
                    memmove(ProtocolAnswer+7, cUserNameSize, 3);                
                    memmove(ProtocolAnswer + 10, "0017", 4);
                    memmove(ProtocolAnswer + 22, cUserName, iUserNameSize);
                    memmove(ProtocolAnswer + 22 + iUserNameSize, Answer, strlen(Answer));
                    
                    myfuncSendMessage(ProtocolAnswer);
                }
                    
            }
        }
    }

    if (strcmp(globalBuf, globalBuf_copy) == 0) globalBuf[0] = '\0';
   
    size_t stSize = strlen(globalBuf) + sizeof(char);
    char* pszReturn = NULL;

    pszReturn = (char*)::CoTaskMemAlloc(stSize);
    strcpy_s(pszReturn, stSize, globalBuf);
    return pszReturn;
}
