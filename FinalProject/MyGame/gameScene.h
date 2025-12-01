#pragma once

#include "MyScene.h"
#include "library.h"

#include <string>
#include <vector>
#include <random>
#include <map>

#include <queue>
#include <mutex>

class GameObject;
class PlayerObject;
class Pig;
class Alpaca;
class Penguin;
class Chic;
class Fox;

struct AnimalMoveInfo {
	int type; // 동물 종류
	int id;   // 동물 번호
	float x;
	float y;
};

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

enum class EInteractType {
	NONE = 1,
	DDONG,
	ANIMAL,
};


class gameScene : public MyScene
{
private:
	PlayerObject* player;
	PlayerObject* otherPlayer[2] = { nullptr, nullptr };

	GameObject* objects[10];
	GameObject* pigs[30];
	GameObject* alpacas[30];
	GameObject* penguins[30];
	GameObject* chics[30];
	GameObject* foxes[30];

	GameObject* townObjects[20];	// 원래 집도 짓게 하려고 한듯..

	int otherPlayerIDs[2] = { -1, -1 };
	int pigCount = 0;
	int alpacaCount = 0;
	int penguinCount = 0;
	int chickenCount = 0;
	int foxCount = 0;
	int townObjectCount;

	std::vector<Ddong> ddongs;
	std::vector<Snow> snows;

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
	bool isSettingFinished = false;

	//dd
	std::map<int, GameObject*> poopMap;
	GLuint ddongVAO = 0;
	GLsizei ddongVertexCount = 0;

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

	void createOtherPlayer(int id, float x, float y, float z);
	void removeOtherPlayer(int id);
	void updateOtherPlayer(int id, float x, float z, float yaw);

	void setAnimalCount(int animalType, int count);

	// 동물 관련
	void spawnAnimal(const int animalType);
	void sellAnimal(const int animalType, const int animalID);
	void feedAnimals(const int animalType, const int animalID, const int growStep);
	std::vector<int> spawnList = {};
	std::vector<std::pair<int, int>> sellList = {}; // <Type, ID>

	//dd
	void spawnPoop(int id, float x, float z);
	void removePoop(int id);

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
		m_defaultShader_cameraPosLoc;

public:
	void checkInteraction(glm::vec3 playerPos);	// 여기서 동물+똥 상호작용
	void updateAnimalPos(int type, int id, float x, float y);
private:
	EInteractType m_currentInteractType;

	std::string m_myPlayerID;	// 머리위에 띄울.. 변수저장용 

	std::queue<AnimalMoveInfo> m_animalMoveQueue;
	std::mutex m_queueLock;

public:
	// 수신스레드에서 큐에 넣을때
	void EnqueueAnimalMove(int type, int id, float x, float y);

	// 메인스레드에서 큐에있는거 처리할때
	void ProcessAnimalMoveQueue();
};

extern gameScene* g_gameScene;
