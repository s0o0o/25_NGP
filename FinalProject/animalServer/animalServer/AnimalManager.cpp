// AnimalManager.cpp
#include "AnimalManager.h"
#include <stdio.h>

void AnimalManager::Initialize()
{
	InitializeCriticalSection(&cs_animals);
	nextAnimalID = 0;
	nextPoopID = 0;
}

void AnimalManager::Cleanup()
{
	DeleteCriticalSection(&cs_animals);
	animals.clear();
	poops.clear();
}

void AnimalManager::SpawnAnimal(int type, float startX, float startY)
{
	//데이터 생성
	EnterCriticalSection(&cs_animals);

	AnimalData newAnimal;
	newAnimal.id = GetNextAnimalID();
	newAnimal.type = type;
	newAnimal.growStep = 0;
	newAnimal.x = startX;
	newAnimal.y = startY;

	animals[newAnimal.id] = newAnimal;
	printf("[AnimalManager] 동물 생성 (ID: %d, Type: %d, Pos: %.1f, %.1f)\n",
		newAnimal.id, newAnimal.type, newAnimal.x, newAnimal.y);

	LeaveCriticalSection(&cs_animals);

	//패킷 생성
	sc_spawn_animal packet;
	packet.animalID = newAnimal.id;
	packet.animalType = newAnimal.type;
	packet.growStep = newAnimal.growStep;
	packet.x = newAnimal.x;
	packet.y = newAnimal.y;

	//브로드캐스팅
	EnterCriticalSection(&cs_connections);
	for (auto const& pair : g_sessions_map)
	{
		PlayerSession const& session = pair.second;
		if (session.bActive)
		{
			sendPacket(session.sock, PacketType::SC_SPAWN_ANIMAL, (char*)&packet, sizeof(sc_spawn_animal));
		}
	}
	LeaveCriticalSection(&cs_connections);
}


void AnimalManager::SendExistingObjects(SOCKET client_sock)
{
}
