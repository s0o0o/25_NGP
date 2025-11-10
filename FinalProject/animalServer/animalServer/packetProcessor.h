#pragma once
#include "../../Packet.h"	
#include "Common.h"	


struct PlayerSession;


void ProcessPacket(SOCKET client_sock, PacketType type, char* data);
