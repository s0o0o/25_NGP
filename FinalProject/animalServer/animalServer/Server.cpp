// server.cpp
#include "Common.h"      
#include "ClientHandler.h" 
#include <map> 

#define SERVERPORT 9000



HANDLE hServerFullEvent;
CRITICAL_SECTION cs_connections;
volatile long g_connectionCount = 0;

//PlayerSession의 전역 맵 
std::map<SOCKET, PlayerSession> g_sessions_map;

int main(int argc, char* argv[])
{
	int retval;

	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return 1;

	InitializeCriticalSection(&cs_connections);
	hServerFullEvent = CreateEvent(NULL, TRUE, TRUE, NULL);
	if (hServerFullEvent == NULL) err_quit("CreateEvent()");

	SOCKET listen_sock = socket(AF_INET, SOCK_STREAM, 0);
	if (listen_sock == INVALID_SOCKET) err_quit("socket()");

	struct sockaddr_in serveraddr;
	memset(&serveraddr, 0, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
	serveraddr.sin_port = htons(SERVERPORT);

	retval = bind(listen_sock, (struct sockaddr*)&serveraddr, sizeof(serveraddr));
	if (retval == SOCKET_ERROR) err_quit("bind()");

	retval = listen(listen_sock, SOMAXCONN);
	if (retval == SOCKET_ERROR) err_quit("listen()");

	printf("[TCP 서버] 게임 서버가 %d번 포트에서 시작되었습니다. (최대 %d명 접속 가능)\n", SERVERPORT, MAX_PLAYER);

	SOCKET client_sock;
	struct sockaddr_in clientaddr;
	int addrlen = sizeof(clientaddr);

	while (true) {
		WaitForSingleObject(hServerFullEvent, INFINITE);

		client_sock = accept(listen_sock, (struct sockaddr*)&clientaddr, &addrlen);
		if (client_sock == INVALID_SOCKET) {
			printf("[오류] accept() 실패: %d\n", WSAGetLastError());
			continue;
		}

		EnterCriticalSection(&cs_connections);
		g_connectionCount++;
		printf("[정보] 현재 접속자: %ld명\n", g_connectionCount);
		if (g_connectionCount >= MAX_PLAYER) {
			ResetEvent(hServerFullEvent); 
		}
		LeaveCriticalSection(&cs_connections);


		//스레드 생성
		HANDLE hThread = CreateThread(NULL, 0, ClientThread, (LPVOID)client_sock, 0, NULL);
		if (hThread == 0) {
			//  스레드 생성 실패 시 예외 처리
			printf("[오류] 스레드를 생성할 수 없습니다.\n");

			// 스레드 생성을 못했으므로 카운트 원복
			EnterCriticalSection(&cs_connections);
			g_connectionCount--;
			if (g_connectionCount < MAX_PLAYER) {
				SetEvent(hServerFullEvent); // (만약 닫혔었다면) 다시 문을 연다
			}
			LeaveCriticalSection(&cs_connections);

			closesocket(client_sock);
		}
		else {
			CloseHandle(hThread);
		}
	}

	// 서버 종료
	closesocket(listen_sock);
	CloseHandle(hServerFullEvent);
	DeleteCriticalSection(&cs_connections);
	WSACleanup();

	return 0;
}