#pragma once
#include "GameObject.h"
#include "PlayerObject.h"

class Pig : public GameObject
{
private:
	GLint m_worldLoc;
	GLint m_colorLoc;

	GLint m_viewLoc;     
	GLint m_projLoc;     
	GLint m_lightPosLoc; 

	const float LEG_ROTATE_SPEED = 90.0f; 
	float MOVE_SPEED;

	int pigNum;

	float rangeLimit;

	float pigXDir;
	float pigZDir;

	float rangeX;
	float rangeZ;

	bool isNear;

	float rotatePigLeftLeg;
	float rotatePigRightLeg;
	float rotateFacePig;

	float rotateBabyLeftLeg;
	float rotateBabyRightLeg;
	float rotateFaceBaby;

	bool isMaxRotateBaby;
	bool isMaxRotatePig;

	int feedNum;

public:
	Pig(int num);
	virtual ~Pig();
	
	virtual void initialize() override;
	virtual void update(float elapseTime)override;
	virtual void draw(const glm::mat4& viewMatrix, 
		const glm::mat4& projMatrix, 
		const glm::vec3& lightPos) const override;
	virtual void release() override;
};




