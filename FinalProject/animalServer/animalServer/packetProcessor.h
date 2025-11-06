#pragma once
#include "../../Packet.h"	
#include "Common.h"	

void ProcessPacket(SOCKET client_sock, PacketType type, char* data);
