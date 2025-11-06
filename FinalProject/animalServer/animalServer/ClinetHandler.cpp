// ClientHandler.cpp
#include "ClientHandler.h" 
#include "packetProcessor.h"

#include <stdio.h>        

#define BUFSIZE    4096 

//const char* ALLOWED_IDS[] = { "1111", "2222", "3333" };
//const int NUM_ALLOWED_IDS = sizeof(ALLOWED_IDS) / sizeof(ALLOWED_IDS[0]);

int sendPacket(SOCKET sock, PacketType type, const char* data, uint16_t dataSize)
{
	PacketHeader header;
	header.type = type;
	header.size = dataSize;

	// 헤더 전송 (4바이트)
	int retval = send(sock, (const char*)&header, sizeof(PacketHeader), 0);
	if (retval == SOCKET_ERROR) {
		printf("[서버] 패킷 헤더 전송 실패");
		return SOCKET_ERROR;
	}

	// 데이터 전송 (size 만큼)
	if (dataSize > 0) {
		retval = send(sock, data, dataSize, 0);
		if (retval == SOCKET_ERROR) {
			printf("[서버] 패킷 데이터 전송 실패");
			return SOCKET_ERROR;
		}
	}

	return (sizeof(PacketHeader) + dataSize); // 총 보낸 바이트 수 반환
}

// 접속 -> 로그인 처리 후 + packetprocess
// ClientThread 
DWORD WINAPI ClientThread(LPVOID arg)
{
	SOCKET client_sock = (SOCKET)arg;
	PacketHeader header;
	int retval;
	char dataBuffer[BUFSIZE];
	bool bLogin = false;

	struct sockaddr_in clientaddr;
	int addrlen = sizeof(clientaddr);
	getpeername(client_sock, (struct sockaddr*)&clientaddr, &addrlen);

	printf("[TCP 서버] 클라이언트 접속: IP=%s, PORT=%d (소켓: %lld)\n",
		inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port), client_sock);

	// 로그인만 먼저..
	// 먼저 헤더 수신
	retval = recv(client_sock, (char*)&header, sizeof(PacketHeader), MSG_WAITALL);
	if (retval == SOCKET_ERROR || retval == 0) {
		printf("[TCP 서버] 클라이언트연결 끊김 (로그인 헤더 수신 전)\n");
		return 1;
	}

	// 패킷(데이터) 수신
	if (header.size > 0) {
		retval = recv(client_sock, dataBuffer, header.size, MSG_WAITALL);
		if (retval == SOCKET_ERROR || retval == 0) {
			printf("[TCP 서버] 클라이언트 연결 끊김 (로그인 데이터 수신 전)\n");
			return 1;
		}
	}

	// 온 패킷이 로그인 요청이 맞는지 확인...
	if (header.type == PacketType::CS_LOGIN_REQUEST) {
		CS_Login_Request* p = (CS_Login_Request*)dataBuffer;
		printf("클라가 로그인 요청 보냄 : %s\n", p->id);

		SC_Login_Accept packet;
		packet.id = 100;
		packet.coin = 5;
		packet.feed = 5;
		strcpy_s(packet.message, "[S->C] 플레이어 로그인 ㅇㅋ 패킷 전송함\n");
		// 필요한 초기 정보 로딩하고 패킷 보내기

		sendPacket(client_sock, PacketType::SC_LOGIN_ACCEPT, (char*)&packet, sizeof(SC_Login_Accept));
		bLogin = true;	// 이제 로그인 ㅇㅋ

		printf("[서버] 플레이어 정보 (id: %d, coin: %d, feed: %d)\n", packet.id, packet.coin, packet.feed);

		// 접속자 수 증가 및 상태 확인 (CS 동기화)
		EnterCriticalSection(&cs_connections);
		g_connectionCount++;
		printf("[정보] 현재 접속자: %ld명\n", g_connectionCount);
		LeaveCriticalSection(&cs_connections);

		if (g_connectionCount >= 3) {
			SC_Login_Fail packet;
			strcpy_s(packet.message, "[서버] 등록되지 않은 ID");
			sendPacket(client_sock, PacketType::SC_LOGIN_FAIL, (char*)&packet, sizeof(SC_Login_Fail));
			bLogin = false;
			ResetEvent(hServerFullEvent); // 3명이 찼으므로 문을 닫는다 (Non-Signaled)
		}
	}
	else {
		printf("[TCP 서버] 클라이언트(%lld)가 로그인 요청 대신 비정상 패킷 전송 (%d)\n", client_sock, header.type);
	}


	//{	// id로 접속 들어오게하는거
	//	// 요청온거 처리하고 응답도 보내야함..
	//	bool id_found = false;
	//	for (int i = 0; i < NUM_ALLOWED_IDS; ++i) {
	//		if (strcmp(p->id, ALLOWED_IDS[i]) == 0) {
	//			id_found = true;
	//			break;
	//		}
	//	}
	//	if (id_found) {
	//		SC_Login_Accept packet;
	//		packet.id = 1; // 임시
	//		strcpy_s(packet.message, "[서버] 로그인 성공! 게임 서버에 접속");
	//		sendPacket(client_sock, PacketType::SC_LOGIN_ACCEPT, (char*)&packet, sizeof(SC_Login_Accept));
	//		bLogin = true;
	//		// 접속자 수 증가 및 상태 확인 (CS 동기화)
	//		EnterCriticalSection(&cs_connections);
	//		g_connectionCount++;
	//		printf("[정보] 현재 접속자: %ld명\n", g_connectionCount);
	//		if (g_connectionCount >= 3) {
	//			ResetEvent(hServerFullEvent); // 3명이 찼으므로 문을 닫는다 (Non-Signaled)
	//		}
	//		LeaveCriticalSection(&cs_connections);
	//	}
	//	else {
	//		SC_Login_Fail packet;
	//		strcpy_s(packet.message, "[서버] 등록되지 않은 ID");
	//		sendPacket(client_sock, PacketType::SC_LOGIN_FAIL, (char*)&packet, sizeof(SC_Login_Fail));
	//		bLogin = false;
	//	}
	//}

	// 이제 로그인 했으면 게임 루프 돌기
	if (bLogin) {
		printf("[TCP 서버] 로그인 ㅇㅋ\n");
		while (true) {
			printf("[TCP 서버] recv 기다림중\n");
			retval = recv(client_sock, (char*)&header, sizeof(PacketHeader), MSG_WAITALL);	// 헤더먼저
			if (retval == SOCKET_ERROR || retval == 0) break;

			if (header.size > 0) {	// 그다음 데이터
				retval = recv(client_sock, dataBuffer, header.size, MSG_WAITALL);
				if (retval == SOCKET_ERROR || retval == 0) break;
			}

			// 클라가 보낸 패킷 다 여기서 처리..
			ProcessPacket(client_sock, header.type, dataBuffer);
		}
	}
	else {
		printf(" 로그인 실패 \n");
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

	// 이전 코드
	////////////////////////////////////////////////
	//// --- 로그인 처리 ---
	//// 1. 로그인 패킷 수신 (ID)
	//CS_Login_Request loginPkt;
	//retval = recv(client_sock, (char*)&loginPkt, sizeof(CS_Login_Request), MSG_WAITALL);
	//if (retval == SOCKET_ERROR || retval == 0) {
	//	printf("[TCP 서버] 클라이언트(%lld) 연결 끊김 (로그인 전)\n", client_sock);
	//	closesocket(client_sock);
	//	// 접속 슬롯 반환 (Event 방식)
	//	EnterCriticalSection(&cs_connections);
	//	g_connectionCount--;
	//	if (g_connectionCount < 3) {
	//		SetEvent(hServerFullEvent); // 자리가 났다고 알림 (문을 연다)
	//	}
	//	LeaveCriticalSection(&cs_connections);
	//	return 1;
	//}
	//printf("[TCP 서버] 클라이언트(%lld) 로그인 시도: ID=%s\n",
	//	client_sock, loginPkt.id);
	// 2. ID 기반 로그인 로직
	// 
	//// 3. 로그인 결과 전송
	//retval = send(client_sock, (char*)&resultPkt, sizeof(SC_Login_Accept), 0);
	//if (retval == SOCKET_ERROR) {
	//	printf("[TCP 서버] 클라이언트(%lld) 연결 오류 (결과 전송 실패)\n", client_sock);
	//	closesocket(client_sock);
	//
	//	//  접속 슬롯 반환 (Event 방식)
	//	EnterCriticalSection(&cs_connections);
	//	g_connectionCount--;	// 공유변수니까 임계영역보호
	//	if (g_connectionCount < 3) {
	//		SetEvent(hServerFullEvent); // 자리가 났다고 알림 (문을 연다)
	//	}
	//	LeaveCriticalSection(&cs_connections);
	//	return 1;
	//}
	//// 4. 로그인 성공 시 -> 게임 로직 루프
	//if (resultPkt.success) {
	//	printf("[TCP 서버] 클라이언트(%lld) 로그인 성공 (ID: %s).\n", client_sock, loginPkt.id);
	//	//  유저 데이터 로드
	//	// ...
	//	//
	//	// ...
	//	char buf[BUFSIZE];
	//	while (true) {
	//		retval = recv(client_sock, buf, BUFSIZE, 0);
	//		if (retval == SOCKET_ERROR || retval == 0) {
	//			printf("[TCP 서버] 클라이언트(%lld) 연결 끊김 (게임 중)\n", client_sock);
	//			break;
	//		}
	//	}
	//}
}

