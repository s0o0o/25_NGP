// AnimalManager.h
#pragma once
#include "Common.h"
#include "ClientHandler.h" 
#include "../../Packet.h"
#include <map>
#include <vector>


struct AnimalData {
	int id;
	int type;
	int growStep;
	float x, y;
	// float moveTimer; 
};

struct PoopData {
	int id;
	float x, y;
};

class AnimalManager
{
public:
	void Initialize();
	void Cleanup();

	void SpawnAnimal(int type, float startX, float startY);
	void SpawnPoop(float x, float y);

	// 접속한 유저에게 현재 나와있는 동물/똥 목록 전송
	void SendExistingObjects(SOCKET client_sock);

	int GetNextAnimalID() { return ++nextAnimalID; }
	int GetNextPoopID() { return ++nextPoopID; }

private:
	AnimalManager() : nextAnimalID(0), nextPoopID(0) {}
	~AnimalManager() {}

	// 공유 자원 보호를 위한 크리티컬 섹션 
	CRITICAL_SECTION cs_animals;

	// 동물 및 똥 관리 컨테이너 
	std::map<int, AnimalData> animals;
	std::map<int, PoopData> poops;

	int nextAnimalID;
	int nextPoopID;
};
