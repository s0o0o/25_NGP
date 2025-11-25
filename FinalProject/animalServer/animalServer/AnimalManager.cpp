// AnimalManager.cpp
#include "AnimalManager.h"
#include <stdio.h>

void AnimalManager::Initialize()
{
	InitializeCriticalSection(&cs_animals);
	nextPoopID = 0;

	pigCount = 1;
	chickenCount = 1;
	alpacaCount = 1;
	penguinCount = 1;
	foxCount = 1;

	// 초기 동물 생성 ( 돼지만 2마리 , 나머지는 1마리씩)
	// 무한 루프 방지를 위해 변수 이름 대신 숫자 사용
	EnterCriticalSection(&cs_animals);
	for (int i = 0; i < 2; ++i)
	{
		SpawnAnimal(AnimalType::PIG, 0.5f, 0.5f, false);
	}

	for(int i = 0; i < 1; ++i)
	{
		SpawnAnimal(AnimalType::CHICKEN, 0.5f, 0.5f, false);
	}

	for (int i = 0; i < 1; ++i)
	{
		SpawnAnimal(AnimalType::ALPACA, 0.5f, 0.5f, false);
	}

	for (int i = 0; i < 1; ++i)
	{
		SpawnAnimal(AnimalType::PENGUIN, 0.5f, 0.5f, false);
	}

	for (int i = 0; i < 1; ++i)
	{
		SpawnAnimal(AnimalType::FOX, 0.5f, 0.5f, false);
	}

	LeaveCriticalSection(&cs_animals);
}

void AnimalManager::Cleanup()
{
	DeleteCriticalSection(&cs_animals);
	animals.clear();
	poops.clear();
}

void AnimalManager::SpawnAnimal(int type, float startX, float startY, bool broadcast)
{
	// 데이터 생성 
	EnterCriticalSection(&cs_animals);

	AnimalData newAnimal;
	newAnimal.id = GetNextAnimalID(type);
	newAnimal.type = type;
	newAnimal.growStep = 0;
	newAnimal.x = startX;
	newAnimal.y = startY;

	int id = (newAnimal.id << 8) | (newAnimal.type & 0xFF);

	animals[id] = newAnimal;
	printf("[AnimalManager] 동물 생성 (ID: %d, Type: %d, Pos: %.1f, %.1f)\n",
		newAnimal.id, newAnimal.type, newAnimal.x, newAnimal.y);

	LeaveCriticalSection(&cs_animals);

	if (broadcast) 
	{
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

	// 동물 변수 설정
	sc_animal_count animalCountPacket;
	animalCountPacket.pigCount = pigCount;
	animalCountPacket.chickenCount = chickenCount;
	animalCountPacket.alpacaCount = alpacaCount;
	animalCountPacket.penguinCount = penguinCount;
	animalCountPacket.foxCount = foxCount;
	sendPacket(client_sock, PacketType::SC_ANIMAL_COUNT, (char*)&animalCountPacket, sizeof(sc_animal_count));

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

int AnimalManager::GetNextAnimalID(int type)
{
	EnterCriticalSection(&cs_animals);

	int returnCount = -1;
	switch (type)
	{
	case AnimalType::PIG:
	{
		returnCount = pigCount - 1;
		pigCount += 1;
	}
	break;
	case AnimalType::CHICKEN:
	{
		returnCount = chickenCount - 1;
		chickenCount += 1;
	}
	break;
	case AnimalType::ALPACA:
	{
		returnCount = alpacaCount - 1;
		alpacaCount += 1;
	}
	break;
	case AnimalType::PENGUIN:
	{
		returnCount = penguinCount - 1;
		penguinCount += 1;
	}
	break;
	case AnimalType::FOX:
	{
		returnCount = foxCount - 1;
		foxCount++;
	}
	break;
	}
	LeaveCriticalSection(&cs_animals);

	return returnCount;
}

void AnimalManager::GrowAnimal(int animalID, int animalType)
{
	EnterCriticalSection(&cs_animals);
	int id = (animalID << 8) | (animalType & 0xFF);

	for(auto& pair : animals)
	{
		AnimalData& a = pair.second;
		if (a.id == animalID)
		{
			a.growStep += 1;
			printf("[AnimalManager] 동물 성장 (ID: %d, New GrowStep: %d)\n", a.id, a.growStep);
			break;
		}
	}
	LeaveCriticalSection(&cs_animals);

	BroadcastAnimalState(id); // 상태 업데이트 브로드 캐스트
}

void AnimalManager::BroadcastAnimalState(int animalID)
{
	// 안전하게 동물 상태 취득
	AnimalData target;
	bool found = false;

	EnterCriticalSection(&cs_animals);
	auto it = animals.find(animalID);
	if (it != animals.end())
	{
		target = it->second; // 복사
		found = true;
	}
	LeaveCriticalSection(&cs_animals);

	if (!found) return;

	// 업데이트 패킷 구성
	sc_update_animal_state updatePacket;
	updatePacket.AnimalID = target.id;
	updatePacket.AnimalType = target.type;
	updatePacket.GrowStep = target.growStep;
	printf("[AnimalManager] 동물 상태 브로드캐스트 (ID: %d, Type: %d, GrowStep: %d)\n",
		target.id, target.type, target.growStep);

	// 브로드캐스트
	EnterCriticalSection(&cs_connections);
	for (auto const& pair : g_sessions_map)
	{
		PlayerSession const& session = pair.second;
		if (session.bActive)
		{
			sendPacket(session.sock, PacketType::SC_UPDATE_ANIMAL_STATE, (char*)&updatePacket, sizeof(sc_update_animal_state));
		}
	}
	LeaveCriticalSection(&cs_connections);
}
