#pragma once

#include "MyScene.h"
#include "library.h"

#include <string>
#include <vector>
#include <random>

class GameObject;
class PlayerObject;
class Pig;
class Alpaca;
class Penguin;
class Chic;
class Fox;

struct Ddong {
	float x, y, z;
	bool isDraw;
	bool isNear;
};

struct Snow {
	float x, y, z;
	float size;
	float speed;
	bool isDraw;
};


class gameScene : public MyScene
{
private:
	PlayerObject* player;
	GameObject* objects[10];
	GameObject* pigs[30];
	GameObject* alpacas[30];
	GameObject* penguins[30];
	GameObject* chics[30];
	GameObject* foxes[30];
	GameObject* townObjects[20];	// 원래 집도 짓게 하려고 한듯..

	int pigCount;
	int alpacaCount;
	int penguinCount;
	int chickenCount;
	int foxCount;
	int townObjectCount;

	std::vector<Ddong> ddongs;
	std::vector<Snow> snows;

	float coin_x[20];
	bool isCoin[20];
	int nowCoin;
	int maxCoin;

	float feed_x[20];
	bool isFood[20];
	int nowFeed;
	int maxFeed;

	float ddongSpawnTimer;
	const float DDONG_SPAWN_TIME = 10.f; // 10초에 하나 스폰
	float rotateTreeY[10];

	bool isTitleAni;
	bool isTitleAniEnd;
	float cameraY;
	
	// 환경 상태
	glm::vec3 light;
	bool isLightMove;

	int objectCount;

	int width;
	int height;

	float snowTimer;
	const float SNOW_TIME = 200.f; // 200초마다 눈옴
	bool isSnow;

	float snowSpawnTimer;
	const float SNOW_SPAWN_TIME = 0.025f; // 눈 생기게 하는 타이머

	float dayTimer;
	const float DAY_TIME = 300.f; // 200초마다 눈옴

	bool isDay;
	bool isAnimalSleep;
public:
	// 게임 로직에서, update, draw..., 키 입력
	gameScene(int winWidth, int winHeight);
	~gameScene();

	void sceneOnEnter() override;	// 씬 입장
	void sceneOnExit() override;	// 씬 나간거

	void update(float elapsedTime) override;

	void draw() override;

	void keyboard(unsigned char key, bool isPressed) override;
	void specialKeyboard(int key, bool isPressed) override;
	void mouse(int button, int state, int x, int y) override;
	void mouseMove(int x, int y) override;

	void setWindowSize(int winWidth, int winHeight) override;
	void setPlayerDx(float n);
	void setPlayerDz(float n);

private:
	GLint m_texShader_modelLoc, m_texShader_viewLoc, m_texShader_projLoc, 
		m_texShader_lightColorLoc, m_texShader_useLightLoc;
	GLint m_bgShader_modelLoc;
	GLint m_objShader_modelLoc, m_objShader_viewLoc, m_objShader_projLoc, 
		m_objShader_globalColorLoc, m_objShader_useGlobalColorLoc, m_objShader_lightColorLoc,
		m_objShader_useLightLoc;
	GLint m_animalShader_modelLoc, m_animalShader_viewLoc, m_animalShader_projLoc, 
		m_animalShader_lightPosLoc, m_animalShader_lightColorLoc;
	GLint m_defaultShader_viewLoc, m_defaultShader_projLoc, 
		m_defaultShader_cameraPosLoc; // (이름이 'shader'였던 것)
};

