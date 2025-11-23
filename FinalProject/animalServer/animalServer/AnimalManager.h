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
	void SpawnAnimal(int type, float startX, float startY);

	// 똥 생성
	void SpawnPoop(float x, float y);

	// 접속한 유저에게 현재 나와있는 동물/똥 목록 전송
	void SendExistingObjects(SOCKET client_sock);

	int GetNextAnimalID() { return ++nextAnimalID; }
	int GetNextPoopID() { return ++nextPoopID; }
	void GrowAnimal(int animalID);

private:
	AnimalManager() : nextAnimalID(0), nextPoopID(0) {}
	~AnimalManager() {}

	CRITICAL_SECTION cs_animals; 

	std::map<int, AnimalData> animals;
	std::map<int, PoopData> poops;

	int nextAnimalID;
	int nextPoopID;
};

#define ANIMALS AnimalManager::GetInstance()