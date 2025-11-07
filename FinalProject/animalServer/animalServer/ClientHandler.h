// ClientHandler.h
#pragma once

#include "Common.h" 
#include "../../Packet.h" 

// ID 등록
extern const char* ALLOWED_IDS[];
extern const int NUM_ALLOWED_IDS;

// 클라이언트 처리 스레드 함수 선언 (본체 X)
DWORD WINAPI ClientThread(LPVOID arg);

// 동기화 객체 선언 (extern)
extern CRITICAL_SECTION cs_connections;
extern HANDLE hServerFullEvent;
extern volatile long g_connectionCount;

int sendPacket(SOCKET sock, PacketType type, const char* data, uint16_t dataSize);