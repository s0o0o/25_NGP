// server.cpp
#include "Common.h"      
#include "ClientHandler.h" 
#include <map> 

#include <thread> 
#include <chrono>   
#include <cmath>
#include "./glm/glm.hpp"
#include "./glm/ext.hpp"

#define SERVERPORT 9000

HANDLE hServerFullEvent;
CRITICAL_SECTION cs_connections;
volatile long g_connectionCount = 0;

//PlayerSession의 전역 맵 
std::map<SOCKET, PlayerSession> g_sessions_map;

// 게임 루프 스레드 관련..
DWORD WINAPI GameLoopThread(LPVOID arg);
void GameLoop();
void UpdateGameWorld(float deltaTime);
const float PLAYER_LIMIT = 2.f;
const float LIMIT_X = 22.f;
const float LIMIT_Z = 20.f;
const int SERVER_TICK_RATE = 128; // 1초에 몇번 틱불림
const float MILLISECONDS_PER_TICK = 1000.f / SERVER_TICK_RATE;

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

	HANDLE hGameLoopThread = CreateThread(NULL, 0, GameLoopThread, NULL, 0, NULL);
	if (hGameLoopThread == 0) {
		err_quit("게임루프 실패..\n");
	}
	CloseHandle(hGameLoopThread);
	printf("[정보] 게임루프 시작\n");
	
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

		//워커 스레드 생성
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


// 게임 월드 계산하는거.. 일단은 이동만 처리
void UpdateGameWorld(float deltaTime)
{
	EnterCriticalSection(&cs_connections);
	for (auto& pair : g_sessions_map)
	{
		PlayerSession& session = pair.second;
		if (!session.bActive) continue;
		if (session.moveSpeed < 0.1f) session.moveSpeed = 6.0f;
		// 입력 없음 계산 넘기기
		if (session.lastInputDir == -1) continue;

		printf("[서버] 입력 받음! ID:%d, Dir:%d, Yaw:%.1f\n",
			session.playerID, session.lastInputDir, session.currentYaw);

		float yawRadians = glm::radians(session.currentYaw);
		float sinYaw = sin(yawRadians);
		float cosYaw = cos(yawRadians);

		glm::vec3 look(sinYaw, 0.f, -cosYaw);
		glm::vec3 right(-cosYaw, 0.f, -sinYaw);
		glm::vec3 dir(0.f);

		if (session.lastInputDir == W) {
			dir += look;
			printf("lastInputDir W : %d\n", session.lastInputDir);
		}
		else if (session.lastInputDir == S) {
			dir -= look;
			printf("lastInputDir S : %d\n", session.lastInputDir);
		}
		else if (session.lastInputDir == A) {
			dir += right;
			printf("lastInputDir A: %d\n", session.lastInputDir);
		}
		else if (session.lastInputDir == D) {
			dir -= right;
			printf("lastInputDir D: %d\n", session.lastInputDir);
		}

		printf("dir : %f / %f / %f \n", dir.x, dir.y, dir.z);
		glm::vec3 currentPos(session.x, 0.f, session.z);
		glm::vec3 nextPos = currentPos + (dir * session.moveSpeed * deltaTime);

		// 이건 그 맵 경계 
		/*if (nextPos.x > LIMIT_X - PLAYER_LIMIT) nextPos.x = LIMIT_X - PLAYER_LIMIT;
		if (nextPos.x < -LIMIT_X + PLAYER_LIMIT) nextPos.x = -LIMIT_X + PLAYER_LIMIT;
		if (nextPos.z > LIMIT_Z - PLAYER_LIMIT) nextPos.z = LIMIT_Z - PLAYER_LIMIT;
		if (nextPos.z < -LIMIT_Z + PLAYER_LIMIT) nextPos.z = -LIMIT_Z + PLAYER_LIMIT;*/

		session.x = nextPos.x;
		session.z = nextPos.z;

		sc_move_update resPkt;
		resPkt.playerID = session.playerID;
		resPkt.x = session.x;
		resPkt.y = session.z;
		for (auto& pair_send : g_sessions_map)
		{
			if (pair_send.second.bActive)
			{	// 로긘 상태인애들한테만 보내기
				printf("SC_MOVE_UPDATE 보냄 \n");
				sendPacket(pair_send.second.sock, PacketType::SC_MOVE_UPDATE, (char*)&resPkt, sizeof(sc_move_update));
			}
		}
		session.lastInputDir = -1;	// 입력초기화
	}

	LeaveCriticalSection(&cs_connections);
}

void GameLoop()
{
	auto lastTickTime = std::chrono::high_resolution_clock::now();
	printf("[Game Loop] 틱 시작 (초당 %d회 목표)\n", SERVER_TICK_RATE);

	while (true)
	{
		auto now = std::chrono::high_resolution_clock::now();
		// 지난 틱 이후 경과 시간... 지낫다면 겜월드 update해주기 
		std::chrono::duration<float, std::milli> elapsed = now - lastTickTime;
		float deltaTime_ms = elapsed.count();

		if (deltaTime_ms >= MILLISECONDS_PER_TICK)
		{
			lastTickTime = now;
			float deltaTime_sec = deltaTime_ms / 1000.f;

			UpdateGameWorld(deltaTime_sec);
		}

		auto nextTickTime = lastTickTime + std::chrono::milliseconds((long long)MILLISECONDS_PER_TICK);
		auto timeToWait = nextTickTime - std::chrono::high_resolution_clock::now();

		if (timeToWait > std::chrono::milliseconds(1))
		{
			Sleep(1);
		}
	}
}

DWORD WINAPI GameLoopThread(LPVOID arg)
{
	GameLoop();
	return 0;
}