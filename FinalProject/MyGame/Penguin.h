#pragma once
#include "GameObject.h"
#include "PlayerObject.h"

class Penguin : public GameObject
{
private:
	GLuint VAO;
	int penguinVertexCount;

	float x, y, z;
	float rangeLimit;
	float moveSpeed;

	float XDir;
	float ZDir;

	float rangeX;
	float rangeZ;

	float rotatePenguinLeftArm;
	float rotatePenguinRightArm;
	float rotateFacePenguin;

	bool isMaxRotatePenguin;
	float rotateSpeed;

	bool isNear;
	int feedNum;

public:
	Penguin();
	virtual ~Penguin();

	virtual void initilize() override;
	virtual void update(float elapseTime)override;
	virtual void draw() const override;
	virtual void release() override;

	float movePenguinX;
	float movePenguinZ;
	

};




