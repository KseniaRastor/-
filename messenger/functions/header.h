#pragma once

#define _CRT_SECURE_NO_WARNINGS
#include <windows.h>
#include <string>

extern "C" __declspec(dllexport) struct MyDataTransferProtocol
{
    //char From;
    int SizeOfUserMassage;
    char UserMassage[1024];
};

char globalBuf[512];
char globalBufOnline[512];

extern "C" __declspec(dllexport) int __stdcall myfuncSendMessage(char* ProtocolMessage);
extern "C" __declspec(dllexport) int __stdcall UserRegistration(char* UserDate);
extern "C" __declspec(dllexport) int __stdcall UserEnter(char* UserDate);
extern "C" __declspec(dllexport) int __stdcall NotifyServerAboutDisconnection(char* UserName);
extern "C" __declspec(dllexport) char* __stdcall myfuncOnlineUserList();
extern "C" __declspec(dllexport) char* __stdcall myfuncReceiptMessage();
extern "C" __declspec(dllexport) char* __stdcall ReadingOldMessage(char* UserLogin);
extern "C" __declspec(dllexport) int __stdcall myfuncSendFile(char* filePath, char* ProtocolMessage, int SendPosition, int chunkSize);




