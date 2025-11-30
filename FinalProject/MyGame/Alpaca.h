#pragma once
#include "GameObject.h"
#include "PlayerObject.h"

class Alpaca : public GameObject
{
private:
	GLint m_worldLoc;
	GLint m_colorLoc;

	GLint m_viewLoc;
	GLint m_projLoc;
	GLint m_lightPosLoc;

	const float LEG_ROTATE_SPEED = 90.0f;
	float MOVE_SPEED;

	float rotateAlpaLeftLeg;
	float rotateAlpaRightLeg;
	float rotateFaceAlpa;

	bool isMaxRotateAlpa;

	bool isNear;
	int feedNum;

	glm::vec3 m_targetPos;
	glm::vec3 m_prevPos;

	float targetAngle;
	float currentAngle;
public:
	Alpaca();
	virtual ~Alpaca();

	virtual void initialize() override;
	virtual void update(float elapseTime)override;
	virtual void draw(const glm::mat4& viewMatrix,
		const glm::mat4& projMatrix,
		const glm::vec3& lightPos) const override;
	virtual void release() override;
	void setTargetPosition(float x, float z);

};




