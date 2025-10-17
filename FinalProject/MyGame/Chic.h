#pragma once
#include "GameObject.h"
#include "PlayerObject.h"

class Chic : public GameObject
{
private:
	GLuint chicVAO;
	int chicVertexCount;

	float x, y, z;
	float rangeLimit;
	float moveSpeed;

	float chicXDir;
	float chicZDir;

	float rangeX;
	float rangeZ;


	float rotateChicLeftLeg;
	float rotateChicRightLeg;
	float rotateFaceChic;


	float rotateAdultChicLeftLeg;
	float rotateAdultChicRightLeg;
	float rotateFaceAdultChic;

	bool isMaxRotateChic;
	bool isMaxRotateAdultChic;

	float rotateSpeed;
	int feedNum;

public:
	Chic();
	virtual ~Chic();

	virtual void initilize() override;
	virtual void update(float elapseTime)override;
	virtual void draw() const override;
	virtual void release() override;

	float moveChicX;
	float moveChicZ;
};

