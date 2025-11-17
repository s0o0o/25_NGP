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
}

void AnimalManager::SpawnAnimal(int type, float startX, float startY)
{
	// 单捞磐 积己 棺 历厘
	EnterCriticalSection(&cs_animals);

	AnimalData newAnimal;
	//newAnimal.id = GetNextAnimalID();
	newAnimal.type = type;
	newAnimal.growStep = 0;
	newAnimal.x = startX;
	newAnimal.y = startY;

	animals[newAnimal.id] = newAnimal;
	printf("[AnimalManager] 悼拱 积己 (ID: %d, Type: %d, Pos: %.1f, %.1f)\n",
		newAnimal.id, newAnimal.type, newAnimal.x, newAnimal.y);

	LeaveCriticalSection(&cs_animals);

	// 菩哦 积己
	sc_spawn_animal packet;
	packet.animalID = newAnimal.id;
	packet.animalType = newAnimal.type;
	packet.growStep = newAnimal.growStep;
	packet.x = newAnimal.x;
	packet.y = newAnimal.y;
}

