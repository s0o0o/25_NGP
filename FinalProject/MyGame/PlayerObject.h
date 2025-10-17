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
public :
	PlayerObject();
	virtual ~PlayerObject();

	virtual void initilize() override;
	virtual void update(float elapseTime)override;
	virtual void draw() const override;
	virtual void release() override;

	float playerX, playerZ;
	

	void keyboard(unsigned char key, bool isPressed);
	void mouse(int button, int state, int x, int y);
	void mouseMove(int x, int y); 

	bool isInFarm;
	bool isStoreShow;

	
	int feedNum;

};

