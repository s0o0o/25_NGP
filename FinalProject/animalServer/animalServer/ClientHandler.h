// ClientHandler.h
#pragma once

#include "./glm/glm.hpp"
#include "./glm/ext.hpp"

#include "Common.h" 
#include "../../Packet.h" 
#include <map>

const int MAX_PLAYER = 3;

// ID 등록 잠시주석
//extern const char* ALLOWED_IDS[];
//extern const int NUM_ALLOWED_IDS;


//플레이어 세션 구조체 
struct PlayerSession {
	SOCKET sock;
	int playerID;
	char loginID[MAX_ID_LEN];

	// 플레이어의 현재 위치
	float x;
	float z;

	float moveSpeed = 6.f;	// 스피드
	int   lastInputDir; // (W,A,S,D...)
	float currentYaw;

	bool bActive; // (로그인 완료 여부)
};

extern std::map<SOCKET, PlayerSession> g_sessions_map;

//동기화 객체
extern CRITICAL_SECTION cs_connections;
extern HANDLE hServerFullEvent;
extern volatile long g_connectionCount;

//ProcessPacket 프로토타입 변경
void ProcessPacket(PlayerSession* pSession, PacketType type, char* data);


// 클라이언트 처리 스레드 함수 선언 (본체 X)
DWORD WINAPI ClientThread(LPVOID arg);


int sendPacket(SOCKET sock, PacketType type, const char* data, uint16_t dataSize);