// packetProcessor.cpp
#include "packetProcessor.h"
#include "ClientHandler.h"
#include "AnimalManager.h"


float playerLimit = 2.f;
float limitX = 22.f;
#define ANIMAL_COST 2
#define SELL_PRICE 2

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

		if (pSession->coinNum < ANIMAL_COST)
		{
			printf("[구매실패] 플레이어(%d) 코인 부족\n", pSession->playerID);

			break;
		}
		else
		{
			pSession->coinNum -= ANIMAL_COST; // 코인 차감
			printf("[구매성공] 플레이어(%d) 코인 %d개 차감\n", pSession->playerID, ANIMAL_COST);
			sc_stat_change statChangePacket;
			statChangePacket.coin = pSession->coinNum;	
			statChangePacket.feed = pSession->feedNum;
			sendPacket(client_sock, PacketType::SC_STAT_CHANGE, (char*)&(statChangePacket), sizeof(sc_stat_change));
			ANIMALS.SpawnAnimal(p->animalType, pSession->x + 2.0f, pSession->z);
		}
		break;
	}
	case PacketType::CS_REQUEST_FEED:
	{
		cs_request_feed_animal* p = (cs_request_feed_animal*)data;
		printf("[급여] 플레이어(%d)가 동물(%d)에게 사료 급여 요청\n", pSession->playerID, p->AnimalID);
		pSession->feedNum--; // 사료 개수 감소
		ANIMALS.GrowAnimal(p->AnimalID, p->AnimalType); // 동물 성장
		sc_stat_change statChangePacket;
		statChangePacket.coin = pSession->coinNum;
		statChangePacket.feed = pSession->feedNum;
		sendPacket(client_sock, PacketType::SC_STAT_CHANGE, (char*)&(statChangePacket), sizeof(sc_stat_change));
		/*sc_update_animal_state updatePacket;
		updatePacket.AnimalID = p->AnimalID;
		updatePacket.AnimalType = p->AnimalType;
		updatePacket.GrowStep =ANIMALS.GetGrowStep(p->AnimalID, p->AnimalType);
		sendPacket(client_sock, PacketType::SC_UPDATE_ANIMAL_STATE, (char*)&(updatePacket), sizeof(sc_update_animal_state));*/
		break;
	}

	case PacketType::CS_REQUEST_CLEAN_POOP:
	{
		cs_request_clean_poop* p = (cs_request_clean_poop*)data;
		printf("[청소] 플레이어(%d)가 똥(%d) 청소 요청\n", pSession->playerID, p->poopID);

		bool success = ANIMALS.RemovePoop(p->poopID);

		if (success)
		{
			pSession->feedNum += 1; 
			if (pSession->feedNum > pSession->maxFeedNum)
				pSession->feedNum = pSession->maxFeedNum; 

			printf("플레이어(%d) 사료 획득 현재: %d\n", pSession->playerID, pSession->feedNum);

			sc_stat_change statPacket;
			statPacket.coin = pSession->coinNum;
			statPacket.feed = pSession->feedNum;

			sendPacket(pSession->sock, PacketType::SC_STAT_CHANGE, (char*)&statPacket, sizeof(sc_stat_change));
		}
		else
		{
			printf("[청소실패] 잘못된 ID\n");
		}
		break;
	}
	case PacketType::CS_REQUEST_SELL:
	{
		// 동물 판매 요청 처리
		cs_request_sell_animal* p = (cs_request_sell_animal*)data;
		printf("[판매] 플레이어(%d)가 동물(%d) 판매 요청\n", pSession->playerID, p->AnimalID);
		// 동물 제거
		ANIMALS.RemoveAnimal(p->AnimalID, p->AnimalType);
		pSession->coinNum += SELL_PRICE; // 코인 추가
		printf("[판매성공] 플레이어(%d) 코인 %d개 추가\n", pSession->playerID, SELL_PRICE);
		// 플레이어 자원 패킷 전송
		sc_stat_change statChangePacket;
		statChangePacket.coin = pSession->coinNum;
		statChangePacket.feed = pSession->feedNum;
		sendPacket(client_sock, PacketType::SC_STAT_CHANGE, (char*)&(statChangePacket), sizeof(sc_stat_change));
		break;
	}
	default:
		printf("타입 정의 X 패킷 : %d\n", type);
		break;
	}
}