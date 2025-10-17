#pragma once
#include "GameObject.h"
#include "PlayerObject.h"

class Fox : public GameObject
{
private:
	GLuint foxVAO;
	int foxVertexCount;

	float x, y, z;
	float rangeLimit;
	float moveSpeed;

	

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
	/*bool isMaxRotateAdultChic;*/

	float rotateSpeed;
	int feedNum;

public:
	Fox();
	virtual ~Fox();

	virtual void initilize() override;
	virtual void update(float elapseTime)override;
	virtual void draw() const override;
	virtual void release() override;

	float moveFoxX;
	float moveFoxZ;
};

