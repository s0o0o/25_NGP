// GameLoop.cpp
#include "GameLoop.h"
#include "ClientHandler.h" 
#include "Common.h"

#include <stdio.h>
#include <chrono>
#include <cmath>
#include "AnimalManager.h"
#include "EnvironmentManager.h"	
#include "./glm/glm.hpp"
#include "./glm/ext.hpp"

// 상수 정의
const float PLAYER_LIMIT = 2.f;
const float LIMIT_X = 22.f;
const float LIMIT_Z = 20.f;
const int SERVER_TICK_RATE = 128;		 // 1초에 128번 업데이트
const float MILLISECONDS_PER_TICK = 1000.f / SERVER_TICK_RATE;

const float FENCE_MIN_X = -17.0f;
const float FENCE_MAX_X = 1.0f;
const float FENCE_MIN_Z = -8.0f;
const float FENCE_MAX_Z = 10.0f;

const float P_RADIUS = 0.3f;

DWORD WINAPI GameLoopThread(LPVOID arg)
{
	GameLoop();
	return 0;
}

void GameLoop()
{
	auto lastTickTime = std::chrono::high_resolution_clock::now();
	//printf("[Game Loop] 틱 시작 (초당 %d회)\n", SERVER_TICK_RATE);

	while (true)
	{
		auto now = std::chrono::high_resolution_clock::now();
		std::chrono::duration<float, std::milli> delta = now - lastTickTime;

		// 정해진 틱이 지났을 때만 업데이트
		if (delta.count() >= MILLISECONDS_PER_TICK)
		{
			lastTickTime = now;
			float deltaTime = delta.count() / 1000.0f;

			UpdateGameWorld(deltaTime);
		}
		else
		{
			Sleep(1);
		}
	}
}


void UpdateGameWorld(float deltaTime)
{
	if (g_connectionCount <= 0)
	{
		return;
	}

	// 동물들 업뎃
	ANIMALS.animalUpdate(deltaTime);
	EN_MANAGER.Update(deltaTime);	// 이게 날씨

	// 똥 생성 타이머
	// ---------------------------------------------------------
	static float poopSpawnTimer = 0.0f;
	const float POOP_SPAWN_INTERVAL = 5.0f; // 5초마다 생성 (원하는 시간으로 조절)

	poopSpawnTimer += deltaTime;
	if (poopSpawnTimer >= POOP_SPAWN_INTERVAL)
	{
		poopSpawnTimer = 0.0f; // 타이머 초기화

		float randX = -10.0f + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / 8.0f));
		float randZ = -5.0f + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / 15.0f));

		// 똥 생성 및 브로드캐스팅
		printf("[GameLoop] 똥 자동 생성 시도: %.2f, %.2f\n", randX, randZ);
		ANIMALS.SpawnPoop(randX, randZ);
	}

	EnterCriticalSection(&cs_connections);

	// 모든 세션을 순회하며 이동 처리
	for (auto& pair : g_sessions_map)
	{
		PlayerSession& session = pair.second;
		if (!session.bActive) continue;
		if (session.lastInputDir == -1) continue; // 입력 없으면 패스

		float yawRadians = glm::radians(session.currentYaw);
		float sinYaw = sin(yawRadians);
		float cosYaw = cos(yawRadians);

		glm::vec3 look(sinYaw, 0.f, -cosYaw);
		glm::vec3 right(-cosYaw, 0.f, -sinYaw);
		glm::vec3 dir(0.f);

		// 키 입력에 따른 이동 방향 설정
		if (session.lastInputDir == W) {
			dir += look;
			// printf("lastInputDir W : %d\n", session.lastInputDir); // 디버그용 주석
		}
		else if (session.lastInputDir == S) {
			dir -= look;
			// printf("lastInputDir S : %d\n", session.lastInputDir);
		}
		else if (session.lastInputDir == A) {
			dir += right;
			// printf("lastInputDir A: %d\n", session.lastInputDir);
		}
		else if (session.lastInputDir == D) {
			dir -= right;
			// printf("lastInputDir D: %d\n", session.lastInputDir);
		}

		// 디버깅용
		// printf("dir : %f / %f / %f \n", dir.x, dir.y, dir.z);

		glm::vec3 currentPos(session.x, session.y, session.z);
		glm::vec3 nextPos = currentPos + (dir * session.moveSpeed * deltaTime);

		//못나가게 막기 주석 해제
		if (nextPos.x > LIMIT_X - PLAYER_LIMIT) nextPos.x = LIMIT_X - PLAYER_LIMIT;
		if (nextPos.x < -LIMIT_X + PLAYER_LIMIT) nextPos.x = -LIMIT_X + PLAYER_LIMIT;
		if (nextPos.z > LIMIT_Z - PLAYER_LIMIT) nextPos.z = LIMIT_Z - PLAYER_LIMIT;
		if (nextPos.z < -LIMIT_Z + PLAYER_LIMIT) nextPos.z = -LIMIT_Z + PLAYER_LIMIT;

		if (currentPos.z >= FENCE_MIN_Z - P_RADIUS && currentPos.z <= FENCE_MAX_Z + P_RADIUS)
		{
			if (currentPos.x > FENCE_MIN_X && nextPos.x <= FENCE_MIN_X + P_RADIUS)
			{
				nextPos.x = FENCE_MIN_X + P_RADIUS; // 안쪽에 가둠
			}
			else if (currentPos.x < FENCE_MIN_X && nextPos.x >= FENCE_MIN_X - P_RADIUS)
			{
				nextPos.x = FENCE_MIN_X - P_RADIUS; // 바깥에 막음
			}

			if (currentPos.x < FENCE_MAX_X && nextPos.x >= FENCE_MAX_X - P_RADIUS)
			{
				nextPos.x = FENCE_MAX_X - P_RADIUS; // 안쪽에 가둠
			}
			else if (currentPos.x > FENCE_MAX_X && nextPos.x <= FENCE_MAX_X + P_RADIUS)
			{
				nextPos.x = FENCE_MAX_X + P_RADIUS; // 바깥에 막음
			}
		}

		if (currentPos.x >= FENCE_MIN_X - P_RADIUS && currentPos.x <= FENCE_MAX_X + P_RADIUS)
		{
			if (currentPos.z > FENCE_MIN_Z && nextPos.z <= FENCE_MIN_Z + P_RADIUS)
			{
				nextPos.z = FENCE_MIN_Z + P_RADIUS;
			}
			else if (currentPos.z < FENCE_MIN_Z && nextPos.z >= FENCE_MIN_Z - P_RADIUS)
			{
				nextPos.z = FENCE_MIN_Z - P_RADIUS;
			}

			if (currentPos.z < FENCE_MAX_Z && nextPos.z >= FENCE_MAX_Z - P_RADIUS)
			{
				nextPos.z = FENCE_MAX_Z - P_RADIUS;
			}
			else if (currentPos.z > FENCE_MAX_Z && nextPos.z <= FENCE_MAX_Z + P_RADIUS)
			{
				nextPos.z = FENCE_MAX_Z + P_RADIUS;
			}
		}

		// 세션에 최종 위치 반영
		session.x = nextPos.x;
		session.y = nextPos.y;
		session.z = nextPos.z;

		// 이동 결과 브로드캐스팅 (모든 클라이언트에게 알림)
		sc_move_update mvPkt;
		mvPkt.playerID = session.playerID;
		mvPkt.x = session.x;
		mvPkt.z = session.z;
		mvPkt.yaw = session.currentYaw;

		for (auto& send_pair : g_sessions_map)
		{
			if (send_pair.second.bActive)
			{
				sendPacket(send_pair.second.sock, PacketType::SC_MOVE_UPDATE, (char*)&mvPkt, sizeof(sc_move_update));
			}
		}

		// 입력 처리 완료 후 초기화 
		session.lastInputDir = -1;
	}

	LeaveCriticalSection(&cs_connections);
}