// EnvironmentManager.cpp
#include "EnvironmentManager.h"
#include <stdio.h>

void EnvironmentManager::Initialize()
{
	isDay = true;
	isSnow = true;
	dayTimer = 0.f;
	snowTimer = 0.f;
}

void EnvironmentManager::Update(float deltaTime)
{
	bool stateChanged = false;

	// ³·/¹ã Å¸ÀÌ¸Ó
	dayTimer += deltaTime;
	if (dayTimer >= DAY_TIME) {
		dayTimer -= DAY_TIME;
		isDay = !isDay;
		stateChanged = true;
		printf("[Environment] ³·/¹ã º¯°æ -> %s\n", isDay ? "³·" : "¹ã");
	}

	//´« Å¸ÀÌ¸Ó
	snowTimer += deltaTime;
	if (snowTimer >= SNOW_TIME) {
		snowTimer -= SNOW_TIME;
		isSnow = !isSnow;
		stateChanged = true;
		printf("[Environment] ³¯¾¾ º¯°æ -> %s\n", isSnow ? "´«" : "¸¼À½");
	}

	if (stateChanged) {
		BroadcastEnvironment();
	}
}

void EnvironmentManager::BroadcastEnvironment()
{
	sc_environment_update packet;
	packet.isDay = isDay;
	packet.isSnow = isSnow;

	EnterCriticalSection(&cs_connections);
	for (auto const& pair : g_sessions_map)
	{
		PlayerSession const& session = pair.second;
		if (session.bActive)
		{
			printf("È¯°æ ºê·ÎµåÄ³½ºÆÃ\n");
			sendPacket(session.sock, PacketType::SC_ENVIRONMENT_UPDATE, (char*)&packet, sizeof(sc_environment_update));
		}
	}
	LeaveCriticalSection(&cs_connections);
}

void EnvironmentManager::SendCurrentEnvironment(SOCKET client_sock)
{
	sc_environment_update packet;
	packet.isDay = isDay;
	packet.isSnow = isSnow;

	sendPacket(client_sock, PacketType::SC_ENVIRONMENT_UPDATE, (char*)&packet, sizeof(sc_environment_update));
	printf("ÇöÀç È¯°æ Á¤º¸ Àü¼Û (³·?: %d, ´«?: %d)\n", isDay, isSnow);
}

