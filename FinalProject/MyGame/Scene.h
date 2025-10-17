#pragma once

#include "library.h"

#include <string>
#include <vector>
#include <random>

class GameObject;
class PlayerObject;

class Scene
{
public:
	GLuint shader;
	GLuint plainShader;
	GLuint objShader;
	GLuint bgShader;
	GLuint texShader;
	GLuint animalShader;

	GLuint sphereVAO;
	int sphereVertexCount;
	GLuint animalVAO;
	int animalVertexCount;

	GLuint fenceVAO;
	GLuint BGTexture, BGVAO;
	GLuint BGTexture2;
	GLuint GroundTexture;
	GLuint GroundTexture2;
	int fenceVertexCount;
	GameObject* townObjects[20];
	int townObjectCount;

	GLuint dogVAO;
	int dogVertexCount;

	GLuint cubeVAO;
	int cubeVertexCount;
	
	GLuint house_top_VAO, house_bottom_VAO, houseTexture[2];
	int houseTopVertexCount, houseBottomVertexCount;

	GLuint sign_top_VAO, sign_bottom_VAO, signTexture[2];
	int signTopVertexCount, signBottomVertexCount;

	GLuint treeVAO, treeVAO2;
	int treeVertexCount, treeVertexCount2;

	GLuint coinVAO, coinTexture, buySuccessTexture;
	int coinVertexCount;
	float coin_x[20];
	bool isCoin[20];
	int nowCoin;
	int maxCoin;

	GLuint foodVAO;
	int foodVertexCount;
	float feed_x[20];
	bool isFood[20];
	int nowFeed;
	int maxFeed;

	GLuint ddongVAO, ddongLogTexture, feedLogTexture;
	GLuint feedPackTexture;
	int ddongVertexCount;
	int ddongTimeCount;

	GLuint growDoneTexture;

	GLuint storeTexture;

	GLint usingLight;

	PlayerObject* player;

	GameObject* objects[10];

	GameObject* pigs[30];
	int pigCount;
	GameObject* alpacas[30];
	int alpacaCount;
	GameObject* penguins[30];
	int penguinCount;

	GameObject* chics[30];
	int chickenCount;
	GameObject* foxes[30];
	int foxCount;

	float rotateTreeY[10];

	bool isTitleAni;
	bool isTitleAniEnd;
	float cameraY;

	glm::vec3 light;
	bool isLightMove;

	int objectCount;

	int width;
	int height;

	int snowTime;
	bool isSnow;
	int snowCount;
	float randx, randy, randz, size, speed;

	int dayTime;


public:
	// 게임 로직에서, update, draw..., 키 입력
	Scene(int winWidth, int winHeight);
	~Scene();

	void initialize();
	void release();

	void update(float elapsedTime);

	void draw() const;

	void keyboard(unsigned char key, bool isPressed);
	void specialKeyboard(int key, bool isPressed);
	void mouse(int button, int state, int x, int y);
	void mouseMove(int x, int y);

	void setWindowSize(int winWidth, int winHeight);
	
	bool isDay;
	bool isAnimalSleep;

private:
	static void initBuffer(GLuint* VAO, GLsizei* vertexCount, std::string objFilename);
	static void initCubeBuffer(GLuint* VAO, GLsizei* vertexCount);

	static void initBufferWithUV(GLuint* VAO, GLsizei* vertexCount, std::string objFilename);

	static void initTexture(GLuint* texture, int size, std::string* texFilename);
	void initBackGroundBuffer();

public:
	static GLuint makeShader(std::string vertexFilename, std::string fragmentFilename);
	static std::string readFile(std::string filename);
	static std::vector<glm::vec3> readOBJ(std::string filename);

	static std::vector<glm::vec3> readOBJWithUV(std::string filename);
};

