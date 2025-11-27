// ClientHandler.cpp
#include "ClientHandler.h" 
#include "packetProcessor.h"
#include "AnimalManager.h"
#include <stdio.h>        
#include <iostream>

#define BUFSIZE    4096 

//  ID 
const char* ALLOWED_IDS[] = { "1111", "2222", "3333" };
const int NUM_ALLOWED_IDS = sizeof(ALLOWED_IDS) / sizeof(ALLOWED_IDS[0]);

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
	srand(static_cast<unsigned int>(time(NULL)));

	SOCKET client_sock = (SOCKET)arg;
	PlayerSession* pSession = NULL; // 이 스레드가 맡은 세션 포인터

	PacketHeader header;
	int retval;
	char dataBuffer[BUFSIZE];
	bool bLogin = false; // 기본값 false

	struct sockaddr_in clientaddr;
	int addrlen = sizeof(clientaddr);
	getpeername(client_sock, (struct sockaddr*)&clientaddr, &addrlen);

	printf("[TCP 서버] 클라이언트 접속: IP=%s, PORT=%d (소켓: %lld)\n",
		inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port), client_sock);


	// 1. 헤더 수신 
	retval = recv(client_sock, (char*)&header, sizeof(PacketHeader), MSG_WAITALL);
	if (retval == SOCKET_ERROR || retval == 0) {
		printf("[TCP 서버] 클라이언트(%lld) 연결 끊김 (로그인 헤더 수신 전)\n", client_sock);
		// bLogin은 이미 false이므로, 바로 함수 하단의 if(bLogin) 루프를 일단 건너뜀...
	}
	else
	{
		// 2. 데이터 수신
		if (header.size > 0) {
			retval = recv(client_sock, dataBuffer, header.size, MSG_WAITALL);
			if (retval == SOCKET_ERROR || retval == 0) {
				printf("[TCP 서버] 클라이언트(%lld) 연결 끊김 (로그인 데이터 수신 전)\n", client_sock);
				// bLogin은 이미 false
			}
		}

		// 3.  헤더 타입 검사 (이제 header가 초기화됨)
		if (header.type == PacketType::CS_LOGIN_REQUEST) {
			CS_Login_Request* p = (CS_Login_Request*)dataBuffer;
			printf("클라가 로그인 요청 보냄 : %s\n", p->id);

			SC_Login_Accept packet; // 응답 패킷

			EnterCriticalSection(&cs_connections);
			// 1. 새 세션 생성
			PlayerSession newSession;
			newSession.sock = client_sock;
			newSession.playerID = (int)client_sock;			// 임시 ID (소켓 핸들)
			strcpy_s(newSession.loginID, p->id);
			newSession.x = 5.0f;	 // 초기 위치
			newSession.y = 1.f;	 // 초기 위치
			newSession.z = 5.0f;
			newSession.currentYaw = 0.0f; 
			newSession.lastInputDir = -1;				// h에서 이미 초기화 함.
			newSession.coinNum = rand() % 3 + 3;		// 초기 코인
			newSession.feedNum = rand() % 3 + 3;		// 초기 먹이
			newSession.maxCoinNum = 15;		 
			newSession.maxFeedNum = 15;					// 최대 갯수 세팅
			newSession.bActive = true;

			// 2. 맵에 삽입
			g_sessions_map[client_sock] = newSession;
			pSession = &g_sessions_map[client_sock]; // 맵에 삽입된 세션의 주소

			// 3. 응답 패킷 준비
			packet.id = pSession->playerID;
			strcpy_s(packet.name, pSession->loginID);
			packet.x = pSession->x;
			packet.y = pSession->y;
			packet.z = pSession->z;
			packet.coin = pSession->coinNum;
			packet.feed = pSession->feedNum;
			packet.maxCoin = pSession->maxCoinNum;
			packet.maxFeed = pSession->maxFeedNum;
			std::cout << "이름 : " << packet.name << std::endl;
			printf("플레이어(%d] 로그인 성공: x=%.2f, y=%.2f, z=%.2f, coin=%d, feed=%d, maxCoin=%d, maxFeed=%d\n",
				pSession->playerID, pSession->x, pSession->y, pSession->z, pSession->coinNum, pSession->feedNum, pSession->maxCoinNum, pSession->maxFeedNum);
			strcpy_s(packet.message, "[S->C] 플레이어 로그인 ㅇㅋ");

			LeaveCriticalSection(&cs_connections);

			sendPacket(client_sock, PacketType::SC_LOGIN_ACCEPT, (char*)&packet, sizeof(SC_Login_Accept));
			bLogin = true; // 디버깅용 로그인 성공

			EnterCriticalSection(&cs_connections);
			sc_login_notify myInfoPkt;
			myInfoPkt.playerID = pSession->playerID;
			myInfoPkt.x = pSession->x;
			myInfoPkt.y = pSession->y;
			myInfoPkt.z = pSession->z;
			for (auto& pair : g_sessions_map)
			{
				PlayerSession& otherSession = pair.second;
				if (otherSession.sock != client_sock && otherSession.bActive)
				{
					// 내정보 다른애들한테
					printf("내정보 다른애들한테\n");
					sendPacket(otherSession.sock, PacketType::SC_LOGIN_NOTIFY, (char*)&myInfoPkt, sizeof(sc_login_notify));
					
					// 다른애들정보 나한테
					printf("다른애들정보 나한테\n");
					sc_login_notify p;
					p.playerID = otherSession.playerID;
					p.x = otherSession.x;
					p.y = otherSession.y;
					p.z = otherSession.z;
					sendPacket(client_sock, PacketType::SC_LOGIN_NOTIFY, (char*)&p, sizeof(sc_login_notify));
				}
			}

			LeaveCriticalSection(&cs_connections);
			ANIMALS.SendExistingObjects(client_sock);
		}
		else {
			printf("[TCP 서버] 클라이언트(%lld)가 로그인 요청 대신 비정상 패킷 전송 (%d)\n", client_sock, header.type);

		}
	} // 헤더 수신 else 끝

	// --- 게임 루프 ---
	if (bLogin) {
		printf("[TCP 서버] 로그인 ㅇㅋ\n");
		while (true) {
			//printf("[TCP 서버] recv 기다림중\n");
			retval = recv(client_sock, (char*)&header, sizeof(PacketHeader), MSG_WAITALL);
			if (retval == SOCKET_ERROR || retval == 0) break;

			if (header.size > 0) {
				retval = recv(client_sock, dataBuffer, header.size, MSG_WAITALL);
				if (retval == SOCKET_ERROR || retval == 0) break;
			}

			ProcessPacket(pSession, header.type, dataBuffer);
		}
	}
	else {
		printf(" 로그인 실패 \n");
	}

	// --- 접속 종료 ---
	closesocket(client_sock);
	printf("[TCP 서버] 클라이언트(%lld) 접속 종료.\n", client_sock);

	EnterCriticalSection(&cs_connections);
	// 로그아웃 패킷
	if (pSession != NULL && pSession->bActive) {
		sc_logout p;
		p.playerID = pSession->playerID;

		for (auto& pair : g_sessions_map) {
			if (pair.second.sock != client_sock && pair.second.bActive) {
				sendPacket(pair.second.sock, PacketType::SC_LOGOUT, (char*)&p, sizeof(sc_logout));
			}
		}
	}

	if (pSession != NULL) {
		// 맵에서 세션 제거
		g_sessions_map.erase(pSession->sock);
	}

	g_connectionCount--;
	if (g_connectionCount < MAX_PLAYER) {
		SetEvent(hServerFullEvent);
	}
	LeaveCriticalSection(&cs_connections);

	return 0;
}