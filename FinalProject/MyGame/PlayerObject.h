#pragma once

#include "GameObject.h"
#include "Pig.h"

class PlayerObject : public GameObject{
	GLint m_worldLoc;

	GLint m_viewLoc;
	GLint m_projLoc;
	GLint m_lightPosLoc;

	bool isWPressed;
	bool isAPressed;
	bool isSPressed;
	bool isDPressed;

	float moveSpeed;

	bool isLeftMousePressed;
	float befMousePosX = glutGet(GLUT_WINDOW_WIDTH) / 2;
	float befMousePosY = glutGet(GLUT_WINDOW_HEIGHT) / 2;
	const float MAX_PITCH = 60.0f; // 최대 올림각
	const float MIN_PITCH = -60.0f; // 최대 내림각
	const float MOUSE_SENSITIVITY = 0.15f;


	int coinNum;
	int feedNum;
	int coinMax;
	int FeedMax;
	float x, z;
public :
	PlayerObject();
	virtual ~PlayerObject();
	
	virtual void initialize() override;
	virtual void update(float elapseTime)override;
	virtual void draw(const glm::mat4& viewMatrix,
		const glm::mat4& projMatrix,
		const glm::vec3& lightPos) const override;
	virtual void release() override;

	float playerX, playerZ;
	float m_currentYaw = 0.0f;
	float m_currentPitch = 0.0f;
	
	void keyboard(unsigned char key, bool isPressed);
	void mouse(int button, int state, int x, int y);
	void mouseMove(int x, int y); 

	bool isInFarm;
	bool isStoreShow;

	void movePosition(float serverX, float serverZ);
	void setStatus(int nCoin, int nFeed) {
		this->coinNum = nCoin;
		this->feedNum = nFeed;
		printf("[클라] 자산 갱신: 코인 %d, 사료 %d\n", coinNum, feedNum);
	}

	int getCoin() const { return coinNum; }
	int getFeed() const { return feedNum; }
	void setCoin(int newCoin) { this->coinNum = newCoin; }
	void setFeed(int newFeed) { this->feedNum = newFeed; }
	int getMaxCoin() const { return coinMax; }
	int getMaxFeed() const { return FeedMax; }
	void setMaxCoin(int maxCoin) { this->coinMax = maxCoin; }
	void setMaxFeed(int maxFeed) { this->FeedMax = maxFeed; }
};

