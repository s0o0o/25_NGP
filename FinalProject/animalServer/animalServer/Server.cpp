// server.cpp
#include "Common.h"      
#include "../../Packet.h"    
#include "ClientHandler.h" 

// Client.h 에 정의된 포트와 버퍼 크기 사용
#define SERVERPORT 9000

HANDLE hServerFullEvent;
CRITICAL_SECTION cs_connections;
volatile long g_connectionCount = 0;

int main(int argc, char* argv[])
{
	int retval;

	// 1. 윈속 초기화
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return 1;

	// 크리티컬 섹션 및 이벤트 초기화
	InitializeCriticalSection(&cs_connections);
	hServerFullEvent = CreateEvent(NULL, TRUE, TRUE, NULL);
	if (hServerFullEvent == NULL) err_quit("CreateEvent()");

	// 2. 리슨 소켓 생성
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
	// ---

	printf("[TCP 서버] 게임 서버가 %d번 포트에서 시작되었습니다. (최대 3명 접속 가능)\n", SERVERPORT);

	SOCKET client_sock;
	struct sockaddr_in clientaddr;
	int addrlen = sizeof(clientaddr);

	// 5. accept()
	while (true) {
		// 이벤트 대기 (hServerFullEvent가 Signaled(자리 있음) 상태가 될 때까지 대기)
		WaitForSingleObject(hServerFullEvent, INFINITE);

		client_sock = accept(listen_sock, (struct sockaddr*)&clientaddr, &addrlen);
		if (client_sock == INVALID_SOCKET) {
			printf("[오류] accept() 실패: %d\n", WSAGetLastError());
			continue;
		}

		// 6. 클라이언트 접속 시 CreateThread로 스레드 생성
		//    (ClientThread 함수는 ClientHandler.h 에 선언되어 있음)
		HANDLE hThread = CreateThread(NULL, 0, ClientThread, (LPVOID)client_sock, 0, NULL);
		if (hThread == 0) {
			// [수정] 빠져있던 스레드 생성 실패 시 예외 처리
			printf("[오류] 스레드를 생성할 수 없습니다.\n");
			closesocket(client_sock);
		}
		else {
			CloseHandle(hThread);
		}
	}

	closesocket(listen_sock);
	CloseHandle(hServerFullEvent);
	DeleteCriticalSection(&cs_connections);
	WSACleanup();

	return 0;
}