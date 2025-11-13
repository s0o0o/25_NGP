#pragma once
#include "PacketProcessor.h"
#include "Client.h"
#include "gameScene.h"
#include "main.h"

extern sceneManager* g_Manager;

void ProcessPacket(PacketType type, char* data)
{
	switch (type)
	{
	case PacketType::SC_LOGIN_ACCEPT:
	{
		SC_Login_Accept* p = (SC_Login_Accept*)data;
		printf("[클라] 로그인 성공! 서버 메시지: %s\n", p->message);
		g_gameState = GameState::STATE_INGAME;
		break;
	}
	case PacketType::SC_LOGIN_FAIL:
	{
		SC_Login_Fail* p = (SC_Login_Fail*)data;
		printf("[클라] 로그인 실패... 서버 메시지: %s\n", p->message);
		g_gameState = GameState::STATE_LOGIN_FAILED;
		break;
	}
	case PacketType::SC_MOVE_UPDATE_OWN:
	{
		sc_move_update_own* p = (sc_move_update_own*)data;
		gameScene* scene = (gameScene*)(g_Manager->getCurrentScene());
		scene->setPlayerDx(p->dx);
		scene->setPlayerDz(p->dz);
		printf("내 이동 업데이트됨!\n");
	}
	break;
	case PacketType::SC_MOVE_UPDATE:
	{
		printf("다른 클라이언트의 이동 업데이트됨!\n");
	}
	break;
	default:
		printf("[클라] 알 수 없는 패킷 수신: %d\n", type);
		break;
	}
}
