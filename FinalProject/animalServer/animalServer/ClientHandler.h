// ClientHandler.h
#pragma once

#include "Common.h" 
#include "Packet.h" 

// 클라이언트 처리 스레드 함수 선언 (본체 X)
DWORD WINAPI ClientThread(LPVOID arg);

// 동기화 객체 선언 (extern)
extern CRITICAL_SECTION cs_connections;
extern HANDLE hServerFullEvent;
extern volatile long g_connectionCount;