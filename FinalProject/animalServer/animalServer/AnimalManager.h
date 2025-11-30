// AnimalManager.h
#pragma once
#include "Common.h"
#include "ClientHandler.h" 
#include "../../Packet.h"
#include <map>

struct AnimalData {
	int id;
	int type;
	int growStep;
	float x, y;

	float moveSpeed = 1.f;
	float xDir = 0.f; // -1, 0, 1
	float yDir = 0.f; // -1, 0, 1 
	float moveTimer = 4.9f; // 시작하자마자 움직이게.. (5초가 기준)

	float lastSentX, lastSentY;
};


struct PoopData {
	int id;
	float x, y;
};

class AnimalManager
{
public:
	static AnimalManager& GetInstance() {
		static AnimalManager instance;
		return instance;
	}

	void Initialize();
	void Cleanup();

	// 동물 생성 
	void SpawnAnimal(int type, float startX, float startY, bool broadcast = true);
	void RemoveAnimal(int animalID, int animalType);

	// 똥 생성
	void SpawnPoop(float x, float y);

	bool RemovePoop(int poopID);

	// 접속한 유저에게 현재 나와있는 동물/똥 목록 전송
	void SendExistingObjects(SOCKET client_sock);

	int GetNextAnimalID(int type);
	int GetNextPoopID() { return ++nextPoopID; }
	void GrowAnimal(int animalID, int animalType);

	// 동물 상태 브로드캐스팅
	void BroadcastAnimalState(int animalID);

	// 동물 움직임 업뎃
	void animalUpdate(float deltaTime);
	void broadcastMovement();
private:
	AnimalManager() : nextPoopID(0) {}
	~AnimalManager() {}

	CRITICAL_SECTION cs_animals; 

	std::map<int, AnimalData> animals;
	std::map<int, PoopData> poops;

	int nextPoopID;

	// 동물 카운트
	int pigCount;
	int alpacaCount;
	int penguinCount;
	int chickenCount;
	int foxCount;

	float broadcastTimer = 0.1f; // 패킷 전송 주기 체크용 타이머
	const float BROADCAST_INTERVAL = 0.1f; // 0.1초마다 전송
};

#define ANIMALS AnimalManager::GetInstance()