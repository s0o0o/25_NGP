#pragma once
// GameLoop.h
#pragma once

#include "Common.h"

extern const float PLAYER_LIMIT;
extern const float LIMIT_X;
extern const float LIMIT_Z;
extern const int SERVER_TICK_RATE;

// 게임 루프 스레드 함수
DWORD WINAPI GameLoopThread(LPVOID arg);

// 게임 로직
void GameLoop();

// 게임 월드 상태 업데이트 (이동, 물리)
void UpdateGameWorld(float deltaTime);

