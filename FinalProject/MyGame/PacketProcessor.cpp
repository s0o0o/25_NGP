#pragma once
#include "PacketProcessor.h"
#include "Client.h"
#include "PlayerObject.h"

extern int g_myPlayerID;
extern PlayerObject* g_myPlayer;
LoginInfo g_loginInfo;

void ProcessPacket(PacketType type, char* data)
{
	switch (type)
	{
	case PacketType::SC_LOGIN_ACCEPT:
	{
		SC_Login_Accept* p = (SC_Login_Accept*)data;
		g_myPlayerID = p->id;
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
	case PacketType::SC_MOVE_UPDATE:
	{
		//printf("이동 패킷 옴\n");
		sc_move_update* p = (sc_move_update*)data;
		if (p->playerID == g_myPlayerID)	// 나라면, 이동 업뎃
		{
			if (g_myPlayer != nullptr) {
				g_myPlayer->movePosition(p->x, p->z);
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
	//case PacketType::SC_MOVE_UPDATE:
	//{
	//	printf("다른 클라이언트의 이동 업데이트됨!\n");
	//}
	break;
	default:
		printf("[클라] 알 수 없는 패킷 수신: %d\n", type);
		break;
	}
}
