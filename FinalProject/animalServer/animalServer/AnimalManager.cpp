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
	// 데이터 생성 
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

	//패킷
	sc_spawn_animal packet;
	packet.animalID = newAnimal.id;
	packet.animalType = newAnimal.type;
	packet.growStep = newAnimal.growStep;
	packet.x = newAnimal.x;
	packet.y = newAnimal.y;

	// 브로드캐스팅
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

void AnimalManager::SpawnPoop(float x, float y)
{
	EnterCriticalSection(&cs_animals);

	PoopData newPoop;
	newPoop.id = GetNextPoopID();
	newPoop.x = x;
	newPoop.y = y;

	poops[newPoop.id] = newPoop;
	printf("[AnimalManager] 똥 생성 (ID: %d)\n", newPoop.id);

	LeaveCriticalSection(&cs_animals);

	sc_spawn_poop packet;
	packet.poopID = newPoop.id;
	packet.x = newPoop.x;
	packet.y = newPoop.y;

	EnterCriticalSection(&cs_connections);
	for (auto const& pair : g_sessions_map)
	{
		PlayerSession const& session = pair.second;
		if (session.bActive)
		{
			sendPacket(session.sock, PacketType::SC_SPAWN_POOP, (char*)&packet, sizeof(sc_spawn_poop));
		}
	}
	LeaveCriticalSection(&cs_connections);
}

void AnimalManager::SendExistingObjects(SOCKET client_sock)
{
	EnterCriticalSection(&cs_animals);

	// 기존 동물들 전송
	for (auto const& pair : animals)
	{
		const AnimalData& a = pair.second;
		sc_spawn_animal packet;
		packet.animalID = a.id;
		packet.animalType = a.type;
		packet.growStep = a.growStep;
		packet.x = a.x;
		packet.y = a.y;
		sendPacket(client_sock, PacketType::SC_SPAWN_ANIMAL, (char*)&packet, sizeof(sc_spawn_animal));
	}

	// 기존 똥들 전송
	for (auto const& pair : poops)
	{
		const PoopData& p = pair.second;
		sc_spawn_poop packet;
		packet.poopID = p.id;
		packet.x = p.x;
		packet.y = p.y;
		sendPacket(client_sock, PacketType::SC_SPAWN_POOP, (char*)&packet, sizeof(sc_spawn_poop));
	}

	LeaveCriticalSection(&cs_animals);
}