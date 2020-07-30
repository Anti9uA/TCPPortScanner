// TCPPortScanner.cpp : 이 파일에는 'main' 함수가 포함됩니다. 거기서 프로그램 실행이 시작되고 종료됩니다.
// 나만의 포트스캐너
#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <stdio.h>
#include <WinSock2.h>
#include <Windows.h>
#include <iostream>
#pragma comment(lib, "ws2_32.lib") 
#pragma comment(lib,"user32")
#pragma comment(lib,"gdi32")
#pragma comment(lib,"Advapi32")
hostent* host;
sockaddr_in sai;

int startport, endport;
WSADATA firstsock;
char hostname[100];		// ip address
struct timeval tv;

DWORD WINAPI thread(PVOID param);

int main(int argc, char* argv[])
{
	strncpy((char*)&sai, "", sizeof(sai));
	sai.sin_family = AF_INET; 

	if (WSAStartup(MAKEWORD(2, 0), &firstsock) != 0) {	// checking WINSOCK VERSION 2.0
		fprintf(stderr, "WSAStartup() failed..");
		exit(1);
	}
	printf("Enter hostname or ip address to scan ==> ex) 15.165.112.41");
	memcpy(hostname, argv[1], strlen(argv[1]));
	// strcpy(hostname, "15.165.112.41"); //211.43.203.44");
	// scanf_s("%s",&hostname, sizeof(hostname));

	if (isdigit(hostname[0])) {
		printf("\nHold on a sec..");
		sai.sin_addr.s_addr = inet_addr(hostname);
		printf("\nDone!!");
	}

	else if ((host = gethostbyname(hostname)) != 0) {
		printf("\nHold on a sec..");
		strncpy((char*)&sai.sin_addr, (char*)host->h_addr_list[0], sizeof sai.sin_addr);
		printf("\nDone!!\n");
	}

	else {
		printf("ERROR : Did u type correct hostname WTF?? \n");
		exit(1);
	}

	CreateThread(NULL, 0, thread, NULL, 0, NULL);

	getchar();
	return 0;
}

DWORD WINAPI thread(PVOID param) {
	SOCKET s;
	int newSockStat;
	ULONG NonBlk = 1;
	int Ret;
	DWORD err;

	printf("\nScanning Start...\n");
	while (1) {
		startport = 1;
		endport = 49151;
		tv.tv_sec = 0;
		tv.tv_usec = 5000;
		for (int i = startport; i <= endport; i++) {
			s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);	// producing TCP socket handle :)
			if (s < 0) {	// if not a socket
				perror("\nERROR: Failed to creat socket..\n");
				exit(0);
			}
			sai.sin_port = htons(i);
			ioctlsocket(s, FIONBIO, &NonBlk);

			err = connect(s, (struct sockaddr*)&sai, sizeof(sai));	// connecing server with socket

			if (err == SOCKET_ERROR) { // if connection denied
				err = WSAGetLastError();		// errorhandle
				fd_set Write, Err;	// to check if it's writed or making error
				FD_ZERO(&Write);
				FD_ZERO(&Err);
				FD_SET(s, &Write);
				FD_SET(s, &Err);

				Ret = select(0, NULL, &Write, &Err, &tv);
				if (Ret != 0) {
					printf("\nPort %d Connected..", i);
				}
			}
			else {		// connection success
				if (shutdown(s, SD_BOTH) == SOCKET_ERROR) {
					perror("\nshutdown");	
					exit(1);
				}
			}
			closesocket(s); // closing socket
		}	// end for (i)
	}	// end while(1)
	fflush(stdout);		// clearing buffer / flushing stream;
	return 0;
}