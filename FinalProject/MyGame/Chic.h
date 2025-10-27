#pragma once
#include "GameObject.h"
#include "PlayerObject.h"

class Chic : public GameObject
{
private:
	GLint m_worldLoc;
	GLint m_colorLoc;

	GLint m_viewLoc;
	GLint m_projLoc;
	GLint m_lightPosLoc;

	float rangeLimit;

	const float LEG_ROTATE_SPEED = 90.0f;
	float MOVE_SPEED;

	float chicXDir;
	float chicZDir;

	float rangeX;
	float rangeZ;

	float rotateChicLeftLeg;
	float rotateChicRightLeg;
	float rotateFaceChic;

	float rotateAdultChicLeftLeg;
	float rotateAdultChicRightLeg;

	bool isMaxRotateChic;
	bool isMaxRotateAdultChic;

	int feedNum;
public:
	Chic();
	virtual ~Chic();

	virtual void initialize() override;
	virtual void update(float elapseTime)override;
	virtual void draw(const glm::mat4& viewMatrix,
		const glm::mat4& projMatrix,
		const glm::vec3& lightPos) const override;
	virtual void release() override;
};

