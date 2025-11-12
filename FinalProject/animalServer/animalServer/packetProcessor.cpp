// packetProcessor.cpp
#include "packetProcessor.h"
#include "ClientHandler.h" // sendPacket, g_sessions_map, cs_connections


void ProcessPacket(PlayerSession* pSession, PacketType type, char* data)
{
	if (pSession == NULL || !pSession->bActive) return; // 리턴

	SOCKET client_sock = pSession->sock;

	switch (type)
	{
	case PacketType::CS_MOVE:
	{
		// 1. 클라이언트가 보낸 데이터를 cs_move 패킷으로 해석 
		cs_move* p = (cs_move*)data;

		// 2. 응답 패킷 생성 (다른 사람에게 뿌릴 용도)
		sc_move_update resPkt;
		resPkt.playerID = pSession->playerID; // ID확인
		resPkt.x = p->x;                       
		resPkt.y = p->y;                      

		// 3. 자기 위치 갱신 + 타인에게 전파
		EnterCriticalSection(&cs_connections);

		// 3-1. (내 위치 갱신)
		pSession->x = p->x;
		pSession->y = p->y;
		printf("플레이어(%d) 이동: x=%.2f, y=%.2f\n", pSession->playerID, pSession->x, pSession->y);

		for (auto const& pair : g_sessions_map)
		{
			// pair.first 는 키 (SOCKET)
			// pair.second 는 값 (PlayerSession)
			PlayerSession const& session = pair.second; // 값(PlayerSession)을 가져옴

			// (session.bActive 는 맵에 있으므로 항상 true)
			if (session.sock != client_sock) // 나 빼고
			{
				sendPacket(session.sock, PacketType::SC_MOVE_UPDATE, (char*)&resPkt, sizeof(sc_move_update));
			}
		}

		LeaveCriticalSection(&cs_connections);

		break;
	}
	default:
		printf("타입 정의 X 패킷 : %d\n", type);
		break;
	}
}