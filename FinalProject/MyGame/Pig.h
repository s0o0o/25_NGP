#pragma once
#include "GameObject.h"
#include "PlayerObject.h"

class Pig : public GameObject
{
private:
	GLuint pigVAO;
	int pigVertexCount;

	float x, y, z;
	float rangeLimit;
	float moveSpeed;


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

	float rotateSpeed;
	int feedNum;

	
public:
	Pig();
	virtual ~Pig();

	virtual void initilize() override;
	virtual void update(float elapseTime)override;
	virtual void draw() const override;
	virtual void release() override;

	float movePigX;
	float movePigZ;
	

};




