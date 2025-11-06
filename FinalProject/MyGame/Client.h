#pragma once
#define _CRT_SECURE_NO_WARNINGS
//===================================================
// 클라이언트 추가 - 2025.10.31
//===================================================
#include "Common.h"
#include "../Packet.h"
#include <string>

extern std::string SERVERIP;
extern std::string ID;
#define SERVERPORT 9000
#define BUFSIZE    512

enum class GameState {
	STATE_LOGIN_SCENE,  //  로그인 화면
	STATE_INGAME,          // 로그인 성공해서 인게임
	STATE_LOGIN_FAILED   // 로그인 실패
};

extern volatile GameState g_gameState;
extern SOCKET g_sock;


//GameState g_gameState = GameState::STATE_LOGIN_SCENE;

int sendPacket(SOCKET sock, PacketType type, const char* data, uint16_t dataSize);

void InitClient(); // 클라이언트 초기화
void CloseClient(); // 클라이언트 종료
