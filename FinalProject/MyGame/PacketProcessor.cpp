#pragma once
#include "PacketProcessor.h"
#include "Client.h"
#include "PlayerObject.h"
#include "gameScene.h"

extern int g_myPlayerID;
extern PlayerObject* g_myPlayer;
extern gameScene* g_gameScene;
LoginInfo g_loginInfo;

void ProcessPacket(PacketType type, char* data)
{
	switch (type)
	{
	case PacketType::SC_LOGIN_ACCEPT:
	{
		SC_Login_Accept* p = (SC_Login_Accept*)data;
		g_myPlayerID = p->id;
		strcpy_s(g_loginInfo.name, p->name);
		g_loginInfo.x = p->x;
		g_loginInfo.y = p->y;
		g_loginInfo.z = p->z;
		g_loginInfo.coin = p->coin;
		g_loginInfo.feed = p->feed;
		g_loginInfo.maxCoin = p->maxCoin;
		g_loginInfo.maxFeed = p->maxFeed;
		g_loginInfo.hasReceivedInfo = true;

		g_gameState = GameState::STATE_INGAME;

		printf("[클라] 로그인 성공! 초기 정보 업뎃 성공, 서버 메시지: %s\n", p->message);
		break;
	}
	case PacketType::SC_LOGIN_FAIL:
	{
		SC_Login_Fail* p = (SC_Login_Fail*)data;
		printf("[클라] 로그인 실패... 서버 메시지: %s\n", p->message);
		g_gameState = GameState::STATE_LOGIN_FAILED;
		break;
	}
	case PacketType::SC_LOGIN_NOTIFY:
	{
		sc_login_notify* p = (sc_login_notify*)data;

		// 나 무시
		if (p->playerID == g_myPlayerID) break;

		if (g_gameScene) {
			printf("다른 클라 보이게하기\n");
			g_gameScene->createOtherPlayer(p->playerID, p->name, p->x, p->y, p->z);
		}
		printf("[클라] 다른 유저(%d) 접속! 위치: %.2f, %.2f\n", p->playerID, p->x, p->z);
		break;
	}
	case PacketType::SC_LOGOUT:
	{
		sc_logout* p = (sc_logout*)data;
		if (g_gameScene) {
			g_gameScene->removeOtherPlayer(p->playerID);
		}
		printf("[클라] 유저(%d) 나감.\n", p->playerID);
		break;
	}
	case PacketType::SC_MOVE_UPDATE:
	{
		//printf("이동 패킷 옴\n");
		sc_move_update* p = (sc_move_update*)data;
		if (p->playerID == g_myPlayerID)	// 나라면, 이동 업뎃
		{
			//printf("내 이동 패킷 옴. 위치: %.2f, %.2f, yaw: %.2f\n", p->x, p->z, p->yaw);
			if (g_myPlayer != nullptr) {
				g_myPlayer->movePosition(p->x, p->z);
			}
		}
		else // 다른 사람이면, 다른 플레이어 이동 업뎃
		{
			//printf("다른 유저(%d) 이동 패킷 옴. 위치: %.2f, %.2f, yaw: %.2f\n", p->playerID, p->x, p->z, p->yaw);
			if (g_gameScene != nullptr) {
				g_gameScene->updateOtherPlayer(p->playerID, p->x, p->z, p->yaw);
			}
		}
		break;
	}
	case PacketType::SC_STAT_CHANGE:
	{
		printf("스탯 변경 패킷 옴\n");
		sc_stat_change* p = (sc_stat_change*)data;
		if (g_myPlayer != nullptr) {
			g_myPlayer->setStatus(p->coin, p->feed);
			printf("스탯 업뎃함 코인 : %d, 먹이 : %d\n", p->coin, p->feed);
		}
		break;
	}
	case PacketType::SC_SPAWN_ANIMAL:
	{
		printf("동물 생성 패킷 옴\n");
		sc_spawn_animal* p = (sc_spawn_animal*)data;
		g_gameScene->spawnList.push_back(p->animalType);
		break;
	}
	case PacketType::SC_UPDATE_ANIMAL_STATE:
	{
		printf("동물 상태 변경 패킷 옴\n");
		sc_update_animal_state* p = (sc_update_animal_state*)data;
		g_gameScene->feedAnimals(p->AnimalType, p->AnimalID, p->GrowStep);
		break;
	}
	case PacketType::SC_ANIMAL_COUNT:
	{
		printf("동물 수 변경 패킷 옴\n");
		sc_animal_count* p = (sc_animal_count*)data;
		if (g_gameScene == nullptr)
		{
			printf("게임 씬 없음\n");
			break;
		}
		g_gameScene->setAnimalCount(AnimalType::PIG, p->pigCount);
		g_gameScene->setAnimalCount(AnimalType::CHICKEN, p->chickenCount);
		g_gameScene->setAnimalCount(AnimalType::ALPACA, p->alpacaCount);
		g_gameScene->setAnimalCount(AnimalType::PENGUIN, p->penguinCount);
		g_gameScene->setAnimalCount(AnimalType::FOX, p->foxCount);
		break;
	}
	case PacketType::SC_REMOVE_ANIMAL:
	{
		printf("동물 제거 패킷 옴\n");
		sc_remove_animal* p = (sc_remove_animal*)data;
		g_gameScene->sellList.push_back(std::make_pair(p->animalType, p->animalID));
		break;
	}

	case PacketType::SC_SPAWN_POOP:
	{
		sc_spawn_poop* p = (sc_spawn_poop*)data;
		if (g_gameScene) {
			// 서버의 y-> 클라 z?
			g_gameScene->spawnPoop(p->poopID, p->x, p->y);
		}
		break;
	}

	case PacketType::SC_REMOVE_POOP:
	{
		sc_remove_poop* p = (sc_remove_poop*)data;
		if (g_gameScene) {
			g_gameScene->removePoop(p->poopID);
		}
		break;
	}
	case PacketType::SC_MOVE_ANIMAL:
	{
		//printf("이동 패킷 도착 ");
		sc_move_animal* p = (sc_move_animal*)data;
		if (g_gameScene) {
			g_gameScene->EnqueueAnimalMove(p->animalType, p->animalID, p->x, p->y);
		}
		break;
	}
	default:
	{
		printf("[클라] 알 수 없는 패킷 수신: %d\n", type);
		break;
	}
	}
}