#pragma once

class sceneManager;
class ResourceManager;

// 각 씬들은 이 MyScene 상속받아서 생성하기

class MyScene {

public:
	virtual ~MyScene() {}

	// 전부 순수가상함수로... 상속받아서 전부 재정의해서 쓰게 만든다
	virtual void sceneOnEnter() = 0;	// 씬 처음 입장할때
	virtual void sceneOnExit() = 0;		// 해당 씬 나갈때..

	// 게임 로직.. 매 프레임 호출
	virtual void update(float elapsedTime) = 0;
	virtual void draw() = 0;	// 그리기.. 매프레임 호출
	virtual void setWindowSize(int winWidth, int winHeight) = 0;

	// 입력 처리 관련
	virtual void keyboard(unsigned char key, bool isPressed) = 0;
	virtual void specialKeyboard(int key, bool isPressed) = 0;
	virtual void mouse(int button, int state, int x, int y) = 0;
	virtual void mouseMove(int x, int y) = 0;

	virtual void setSceneManager(sceneManager* mgr) { m_sceneManager = mgr; }
	virtual void setResourceManager(ResourceManager* res) { m_resourceManager = res; }

protected:
	sceneManager* m_sceneManager;
	ResourceManager* m_resourceManager;
};