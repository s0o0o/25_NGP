#pragma once

#include "GameObject.h"
#include "Pig.h"

class PlayerObject : public GameObject{

	bool isWPressed;
	bool isAPressed;
	bool isSPressed;
	bool isDPressed;

	float moveSpeed;

	bool isLeftMousePressed;
	float befMousePosX;
	float befMousePosY;
	float m_currentPitch = 0.0f;
	const float MAX_PITCH = 60.0f; // 최대 올림각
	const float MIN_PITCH = -60.0f; // 최대 내림각
	const float MOUSE_SENSITIVITY = 0.15f;

	float m_currentYaw = 0.0f;
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
	
	void keyboard(unsigned char key, bool isPressed);
	void mouse(int button, int state, int x, int y);
	void mouseMove(int x, int y); 

	bool isInFarm;
	bool isStoreShow;

	int feedNum;
	void movePosition(float serverX, float serverZ);
};

