#pragma once
#include "GameObject.h"
#include "PlayerObject.h"

class Alpaca : public GameObject
{
private:
	GLuint alpacaVAO;
	int alpacaVertexCount;

	float x, y, z;
	float rangeLimit;
	float moveSpeed;


	float alpacaXDir;
	float alpacaZDir;

	float rangeX;
	float rangeZ;

	
	float rotateAlpaLeftLeg;
	float rotateAlpaRightLeg;
	float rotateFaceAlpa;

	bool isMaxRotateAlpa;
	float rotateSpeed;

	bool isNear;
	int feedNum;

public:
	Alpaca();
	virtual ~Alpaca();

	virtual void initilize() override;
	virtual void update(float elapseTime)override;
	virtual void draw() const override;
	virtual void release() override;

	float moveAlpaX;
	float moveAlpaZ;
	

};




