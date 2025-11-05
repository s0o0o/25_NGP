// ClientHandler.cpp

#include "ClientHandler.h" 
#include <stdio.h>        

#define BUFSIZE    4096 

// ID 등록
const char* ALLOWED_IDS[] = { "1111", "2222", "3333" };
const int NUM_ALLOWED_IDS = sizeof(ALLOWED_IDS) / sizeof(ALLOWED_IDS[0]);

// ClientThread 
DWORD WINAPI ClientThread(LPVOID arg)
{
	SOCKET client_sock = (SOCKET)arg;
	int retval;

	struct sockaddr_in clientaddr;
	int addrlen = sizeof(clientaddr);
	getpeername(client_sock, (struct sockaddr*)&clientaddr, &addrlen);

	printf("[TCP 서버] 클라이언트 접속: IP=%s, PORT=%d (소켓: %lld)\n",
		inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port), client_sock);

	// --- 로그인 처리 ---

	// 1. 로그인 패킷 수신 (ID)
	CS_Login_Request loginPkt;
	retval = recv(client_sock, (char*)&loginPkt, sizeof(CS_Login_Request), MSG_WAITALL);
	if (retval == SOCKET_ERROR || retval == 0) {
		printf("[TCP 서버] 클라이언트(%lld) 연결 끊김 (로그인 전)\n", client_sock);
		closesocket(client_sock);

		// 접속 슬롯 반환 (Event 방식)
		EnterCriticalSection(&cs_connections);
		g_connectionCount--;
		if (g_connectionCount < 3) {
			SetEvent(hServerFullEvent); // 자리가 났다고 알림 (문을 연다)
		}
		LeaveCriticalSection(&cs_connections);

		return 1;
	}

	printf("[TCP 서버] 클라이언트(%lld) 로그인 시도: ID=%s\n",
		client_sock, loginPkt.id);

	// 2. ID 기반 로그인 로직
	SC_Login_Result resultPkt;
	bool id_found = false;
	for (int i = 0; i < NUM_ALLOWED_IDS; ++i) {
		if (strcmp(loginPkt.id, ALLOWED_IDS[i]) == 0) {
			id_found = true;
			break;
		}
	}
	if (id_found) {
		resultPkt.success = true;
		strcpy_s(resultPkt.message, "[서버] 로그인 성공! 게임 서버에 접속합니다.");
	}
	else {
		resultPkt.success = false;
		strcpy_s(resultPkt.message, "[서버] 등록되지 않은 ID입니다.");
	}

	// 3. 로그인 결과 전송
	retval = send(client_sock, (char*)&resultPkt, sizeof(SC_Login_Result), 0);
	if (retval == SOCKET_ERROR) {
		printf("[TCP 서버] 클라이언트(%lld) 연결 오류 (결과 전송 실패)\n", client_sock);
		closesocket(client_sock);

		//  접속 슬롯 반환 (Event 방식)
		EnterCriticalSection(&cs_connections);
		g_connectionCount--;
		if (g_connectionCount < 3) {
			SetEvent(hServerFullEvent); // 자리가 났다고 알림 (문을 연다)
		}
		LeaveCriticalSection(&cs_connections);

		return 1;
	}

	// 4. 로그인 성공 시 -> 게임 로직 루프
	if (resultPkt.success) {
		printf("[TCP 서버] 클라이언트(%lld) 로그인 성공 (ID: %s).\n", client_sock, loginPkt.id);
		//  유저 데이터 로드
		// ...
		//
		// ...
		char buf[BUFSIZE];
		while (true) {
			retval = recv(client_sock, buf, BUFSIZE, 0);
			if (retval == SOCKET_ERROR || retval == 0) {
				printf("[TCP 서버] 클라이언트(%lld) 연결 끊김 (게임 중)\n", client_sock);
				break;
			}
		}
	}
	else {
		printf("[TCP 서버] 클라이언트(%lld) 로그인 실패. 접속을 종료합니다.\n", client_sock);
	}

	// 스레드 종료 및 소켓 닫기
	closesocket(client_sock);
	printf("[TCP 서버] 클라이언트(%lld) 접속 종료.\n", client_sock);

	//  접속 슬롯 반환 (Event 방식)
	EnterCriticalSection(&cs_connections);
	g_connectionCount--;
	if (g_connectionCount < 3) {
		SetEvent(hServerFullEvent); // 자리가 났다고 알림 (문을 연다)
	}
	LeaveCriticalSection(&cs_connections);

	return 0;
}