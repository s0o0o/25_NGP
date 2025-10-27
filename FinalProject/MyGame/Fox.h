#pragma once
#include "GameObject.h"
#include "PlayerObject.h"

class Fox : public GameObject
{
private:
	GLint m_worldLoc;
	GLint m_colorLoc;

	GLint m_viewLoc;
	GLint m_projLoc;
	GLint m_lightPosLoc;

	float x, y, z;
	float rangeLimit;

	const float LEG_ROTATE_SPEED = 90.0f;
	float MOVE_SPEED;

	float foxXDir;
	float foxZDir;

	float adultY;
	float adultScale;
	float rangeX;
	float rangeZ;


	float rotateFoxLeftLeg;
	float rotateFoxRightLeg;
	float rotateFaceFox;

	// 어른버전은 일단 주석처리
	/*float rotateAdultChicLeftLeg;
	float rotateAdultChicRightLeg;
	float rotateFaceAdultChic;*/

	bool isMaxRotateFox;

	int feedNum;
public:
	Fox();
	virtual ~Fox();

	virtual void initialize() override;
	virtual void update(float elapseTime)override;
	virtual void draw(const glm::mat4& viewMatrix,
		const glm::mat4& projMatrix,
		const glm::vec3& lightPos) const override;	
	virtual void release() override;

};

