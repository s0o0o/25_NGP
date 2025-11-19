// packetProcessor.cpp
#include "packetProcessor.h"
#include "ClientHandler.h"
#include "AnimalManager.h"


float playerLimit = 2.f;
float limitX = 22.f;

void ProcessPacket(PlayerSession* pSession, PacketType type, char* data)
{
	if (pSession == NULL || !pSession->bActive) return; // 리턴

	SOCKET client_sock = pSession->sock;

	switch (type)
	{
	case PacketType::CS_MOVE:
	{
		cs_move* p = (cs_move*)data;

		pSession->lastInputDir = p->inputDir;
		pSession->currentYaw = p->currentYaw;

		break;
	}
	case PacketType::CS_REQUEST_BUY_ANIMAL:
	{
		// 동물 구매 요청 처리
		cs_request_buy_animal* p = (cs_request_buy_animal*)data;

		printf("[구매] 플레이어(%d)가 동물(%d) 구매 요청\n", pSession->playerID, p->animalType);

		// 코인 차감하는 로직 
		// if (pSession->coin >= 가격) { ... }

		ANIMALS.SpawnAnimal(p->animalType, pSession->x + 2.0f, pSession->z);
		break;
	}
	case PacketType::CS_REQUEST_FEED:
	{
		// 먹이처리
		break;
	}

	default:
		printf("타입 정의 X 패킷 : %d\n", type);
		break;
	}
}