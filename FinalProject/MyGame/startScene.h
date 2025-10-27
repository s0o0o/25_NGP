#pragma once

#include "library.h"
#include "MyScene.h"

#include <string>
#include <vector>

class startScene : public MyScene
{
private:
	GLint usingLight;

	int width;
	int height;

	float rotateY;
	bool isRotate;
	bool isTitleAni;
	bool isTitleAniEnd;

public:
	// 게임 로직에서, update, draw..., 키 입력
	startScene(int winWidth, int winHeight);
	~startScene();

	void sceneOnEnter() override;
	void sceneOnExit() override;	// 씬 나간거
	void update(float elapsedTime) override;
	void draw() override;

	void keyboard(unsigned char key, bool isPressed) override;
	void specialKeyboard(int key, bool isPressed) override;
	void mouse(int button, int state, int x, int y) override;
	void mouseMove(int x, int y) override;

	void setWindowSize(int winWidth, int winHeight) override;
};

