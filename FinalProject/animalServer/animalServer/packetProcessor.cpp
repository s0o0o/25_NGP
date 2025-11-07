#include "packetProcessor.h"
#include "ClientHandler.h"

void ProcessPacket(SOCKET client_sock, PacketType type, char* data)
{
	switch (type)
	{
	 //	로긘 제외 타입들...
	case PacketType::CS_MOVE:
		printf("CS_MOVE 도착\n");
		// 움직였을 때 .. 정보 업뎃하고
		// 패킷보내고
		// 브로드 캐스팅 후..
		sendPacket(client_sock, PacketType::SC_MOVE_UPDATE, reinterpret_cast<char*>(&data), sizeof(data)); // 전송
		// 또 다시 받을준비하기
		break;
	default:
		printf("타입 정의 X 패킷 : %d\n", type);
		break;
	}
}


