#include "PacketProcessor.h"
#include "Client.h"

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
	default:
		printf("[클라] 알 수 없는 패킷 수신: %d\n", type);
		break;
	}
}
