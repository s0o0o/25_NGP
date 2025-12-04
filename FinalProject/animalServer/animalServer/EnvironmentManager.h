#pragma once

#pragma once
#include "Common.h"
#include "ClientHandler.h"
#include "../../Packet.h"

class EnvironmentManager
{
public:
    static EnvironmentManager& GetInstance() {
        static EnvironmentManager instance;
        return instance;
    }

    void Initialize();

    void Update(float deltaTime);

    void SendCurrentEnvironment(SOCKET client_sock);

    // ÇöÀç »óÅÂ Getter
    bool IsDay() const { return isDay; }
    bool IsSnow() const { return isSnow; }

private:
    EnvironmentManager() : isDay(true), isSnow(true), dayTimer(0.f), snowTimer(0.f) {}
    ~EnvironmentManager() {}

    bool isDay;
    bool isSnow;

    float dayTimer;
    float snowTimer;

    //===============³¯¾¾ Å¸ÀÌ¸Ó ===============
    const float DAY_TIME = 10.f;        // ³·¹ã 10ÃÊ
	const float SNOW_TIME = 30.f;       // ³¯¾¾ 30ÃÊ
    ///=========================================


    void BroadcastEnvironment();
};

#define EN_MANAGER EnvironmentManager::GetInstance()

