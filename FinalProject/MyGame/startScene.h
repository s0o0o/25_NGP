#pragma once

#include "library.h"
#include "MyScene.h"

#include <string>
#include <vector>
#include <map>

struct UVRect {
	float uMin, vMin; 
	float uWidth, vHeight; 
};	;

class startScene : public MyScene
{
private:
	GLint usingLight;

	int width;
	int height;

	std::string m_idInput;
	std::string m_ipInput;
	int m_activeField; // 0: ID, 1: IP -> 탭누르면 전환됨

	GLuint fontShader;
	GLuint fontTexture;
	GLuint fontQuadVAO;
	GLsizei fontQuadVertCount;

	GLint font_modelLoc;
	GLint font_projLoc;
	GLint font_uvRectLoc;
	GLint font_colorLoc;

	std::map<char, UVRect> m_charUVMap;
	void DrawTextWithAtlas(const std::string& text, float x, float y, float size);
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

