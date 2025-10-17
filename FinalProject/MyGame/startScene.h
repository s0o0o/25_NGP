#pragma once

#include "library.h"

#include <string>
#include <vector>

class startScene
{
private:
	GLuint shader;
	GLuint objShader;
	GLuint texShader;
	GLuint bgShader;

	GLuint titleVAO;
	GLuint startSceneVAO[10];
	GLuint startSceneTexture[10];
	int titleVertexCount;
	int SSObjVertexCount[10];

	GLuint cubeVAO;
	int cubeVertexCount;

	GLint usingLight;


	int width;
	int height;

	float rotateY;
	bool isRotate;

public:
	// 게임 로직에서, update, draw..., 키 입력
	startScene(int winWidth, int winHeight);
	~startScene();

	void initialize();
	void release();

	void update(float elapsedTime);

	void draw() const;

	void keyboard(unsigned char key, bool isPressed);
	void specialKeyboard(int key, bool isPressed);
	void mouse(int button, int state, int x, int y);
	void mouseMove(int x, int y);

	void setWindowSize(int winWidth, int winHeight);

	bool isShow;
	bool isTitleAni;
	bool isTitleAniEnd;
private:
	static void initBuffer(GLuint* VAO, GLsizei* vertexCount, std::string objFilename);

	static void initBufferWithUV(GLuint* VAO, GLsizei* vertexCount, std::string objFilename);
	static void initTexture(GLuint* texture, int size, std::string* texFilename);

	static void initCubeBuffer(GLuint* VAO, GLsizei* vertexCount);


public:
	static GLuint makeShader(std::string vertexFilename, std::string fragmentFilename);
	static std::string readFile(std::string filename);
	static std::vector<glm::vec3> readOBJ(std::string filename);

	static std::vector<glm::vec3> readOBJWithUV(std::string filename);
};

