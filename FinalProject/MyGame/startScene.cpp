#include "startScene.h"
#include "ResourceManager.h"
#include "sceneManager.h"

#include <iostream>
#include <vector>
#include <string.h>
#include <stb_image.h>


// 1105 로그인 입력

std::string SERVERIP;

startScene::startScene(int winWidth, int winHeight)
	: width{ winWidth }, height{ winHeight }, m_activeField(0)
{
	std::cout << "startScene 생성됨. ID 필드 활성화." << std::endl;
}

startScene::~startScene()
{

}

void startScene::sceneOnEnter()
{
	width = 1600;
	height = 900;

	fontShader = m_resourceManager->getShader("font");
	fontTexture = m_resourceManager->getTexture("keyboard");
	MeshData quadMesh = m_resourceManager->getMesh("textQuad");
	fontQuadVAO = quadMesh.VAO;
	fontQuadVertCount = quadMesh.vertexCount;

	glUseProgram(fontShader);

	font_modelLoc = glGetUniformLocation(fontShader, "model");
	font_projLoc = glGetUniformLocation(fontShader, "projection");
	font_uvRectLoc = glGetUniformLocation(fontShader, "uvRect");
	font_colorLoc = glGetUniformLocation(fontShader, "textColor");

	const float TOTAL_TEXTURE_WIDTH = 450.0f;
	const float TOTAL_TEXTURE_HEIGHT = 180.0f;
	const float CELL_PX = 45.0f;	// 키 하나 45*45
	const float CELL_UV_WIDTH = CELL_PX / TOTAL_TEXTURE_WIDTH;
	const float CELL_UV_HEIGHT = CELL_PX / TOTAL_TEXTURE_HEIGHT;

	// 행(위에서부터 0123행)
	float v_row[4];
	for (int row = 0; row < 4; ++row) {
		v_row[row] = (TOTAL_TEXTURE_HEIGHT - (static_cast<float>(row) * CELL_PX) - CELL_PX) / TOTAL_TEXTURE_HEIGHT;
	}

	// 0~9
	for (int i = 0; i < 10; ++i) {
		char c = '0' + i; // '0', '1', '2', ...
		float u = (static_cast<float>(i) * CELL_PX) / TOTAL_TEXTURE_WIDTH;
		m_charUVMap[c] = { u, v_row[0], CELL_UV_WIDTH, CELL_UV_HEIGHT };
	}

	// 2행 a~
	for (int i = 0; i < 10; ++i) {
		char c = 'a' + i; // 'a', 'b', 'c', ...
		float u = (static_cast<float>(i) * CELL_PX) / TOTAL_TEXTURE_WIDTH;
		m_charUVMap[c] = { u, v_row[1], CELL_UV_WIDTH, CELL_UV_HEIGHT };
	}

	// 3행 k~
	for (int i = 0; i < 10; ++i) {
		char c = 'k' + i; // 'k', 'l', 'm', ...
		float u = (static_cast<float>(i) * CELL_PX) / TOTAL_TEXTURE_WIDTH;
		m_charUVMap[c] = { u, v_row[2], CELL_UV_WIDTH, CELL_UV_HEIGHT };
	}

	// 4행 소문자만 u~z
	for (int i = 0; i < 6; ++i) {
		char c = 'u' + i; // 'u', 'v', 'w', ...
		float u = (static_cast<float>(i) * CELL_PX) / TOTAL_TEXTURE_WIDTH;
		m_charUVMap[c] = { u, v_row[3], CELL_UV_WIDTH, CELL_UV_HEIGHT };
	}

	m_charUVMap['.'] = { (6.0f * CELL_PX) / TOTAL_TEXTURE_WIDTH, v_row[3], CELL_UV_WIDTH, CELL_UV_HEIGHT };
	m_charUVMap['!'] = { (7.0f * CELL_PX) / TOTAL_TEXTURE_WIDTH, v_row[3], CELL_UV_WIDTH, CELL_UV_HEIGHT };
	m_charUVMap['?'] = { (8.0f * CELL_PX) / TOTAL_TEXTURE_WIDTH, v_row[3], CELL_UV_WIDTH, CELL_UV_HEIGHT };
	m_charUVMap['*'] = { (9.0f * CELL_PX) / TOTAL_TEXTURE_WIDTH, v_row[3], CELL_UV_WIDTH, CELL_UV_HEIGHT };
}

void startScene::sceneOnExit()
{
}


void startScene::update(float elapsedTime)
{
}

void startScene::draw()
{
	glm::mat4 viewMatrix = glm::lookAt(glm::vec3(0.f, 2.f, 15.f), glm::vec3(0.f, 1.f, 0.f), glm::vec3(0.f, 1.f, 0.f));
	glm::mat4 projMatrix = glm::perspective(glm::radians(45.f), float(width) / float(height), 0.1f, 100.f);

	MeshData cubeMesh = m_resourceManager->getMesh("cube");
	GLuint bgShader = m_resourceManager->getShader("bg");
	GLuint texShader = m_resourceManager->getShader("tex");

	GLuint skyTex = m_resourceManager->getTexture("sky2");
	GLuint buttonTex = m_resourceManager->getTexture("pushSpaceBar");

	// login 1105
	GLuint loginTex = m_resourceManager->getTexture("login");
	GLuint titleTex = m_resourceManager->getTexture("title");

	{	// 배경  // z뒤
		glUseProgram(bgShader);
		glDisable(GL_DEPTH_TEST);
		glBindVertexArray(cubeMesh.VAO);

		glm::mat4 translateMatrix = glm::translate(glm::mat4(1.f), glm::vec3(0.0f, 0.f, 0.f));
		glm::mat4 sclaeMatrix = glm::scale(glm::mat4(1.f), glm::vec3(2.f));
		glm::mat4 matrix = translateMatrix * sclaeMatrix;

		GLint worldLoc = glGetUniformLocation(bgShader, "modelTransform");
		if (worldLoc < 0)
			std::cout << "worldLoc 찾지 못함\n";
		else
			glUniformMatrix4fv(worldLoc, 1, GL_FALSE, glm::value_ptr(matrix));


		glBindTexture(GL_TEXTURE_2D, skyTex);
		glDrawArrays(GL_TRIANGLES, 0, cubeMesh.vertexCount);

	}
	glEnable(GL_DEPTH_TEST);

	glUseProgram(texShader);

	// 로그인+title 1105
	{
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glDepthMask(GL_FALSE); // 투명도 처리를 위해 깊이 쓰기 비활성화
		GLint viewLoc = glGetUniformLocation(texShader, "viewTransform");
		GLint projLoc = glGetUniformLocation(texShader, "projTransform");
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(viewMatrix));
		glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projMatrix));

		// title 제목 (사이즈 3*1)
		{
			glBindVertexArray(cubeMesh.VAO);
			glm::mat4 translateMatrix = glm::translate(glm::mat4(1.f), glm::vec3(0.f, 3.5f, 0.0f)); // Z축을 카메라 가까이 (-5.0f)
			glm::mat4 sclaeMatrix = glm::scale(glm::mat4(1.f), glm::vec3(3.5f * 3, 3.5f, 1.f));
			glm::mat4 matrix = translateMatrix * sclaeMatrix;

			GLint worldLoc = glGetUniformLocation(texShader, "modelTransform");
			glUniformMatrix4fv(worldLoc, 1, GL_FALSE, glm::value_ptr(matrix));

			glBindTexture(GL_TEXTURE_2D, titleTex);
			glDrawArrays(GL_TRIANGLES, 0, 6);
		}

		// 로그인 창(사이즈 2*1)
		{
			glBindVertexArray(cubeMesh.VAO);
			glm::mat4 translateMatrix = glm::translate(glm::mat4(1.f), glm::vec3(0.f, -1.5f, 0.f));
			glm::mat4 sclaeMatrix = glm::scale(glm::mat4(1.f), glm::vec3(12.f, 6.f, 1.f));
			glm::mat4 matrix = translateMatrix * sclaeMatrix;

			GLint worldLoc = glGetUniformLocation(texShader, "modelTransform");
			glUniformMatrix4fv(worldLoc, 1, GL_FALSE, glm::value_ptr(matrix));

			glBindTexture(GL_TEXTURE_2D, loginTex);
			glDrawArrays(GL_TRIANGLES, 0, 6);
		}

		glDepthMask(GL_TRUE);
		glDisable(GL_BLEND);
	}

	// 폰트 출력
	{
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glDisable(GL_DEPTH_TEST);

		glUseProgram(fontShader);
		glBindVertexArray(fontQuadVAO);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, fontTexture);

		glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(width), 0.0f, static_cast<float>(height));
		glUniformMatrix4fv(font_projLoc, 1, GL_FALSE, glm::value_ptr(projection));

		// 왼쪽하단이 기준
		DrawTextWithAtlas(m_idInput, 645, 277.0f, 45.0f); // (x, y, 픽셀size)
		DrawTextWithAtlas(m_ipInput, 645.f, 207.0f, 45.0f);

		glBindVertexArray(0);
		glBindTexture(GL_TEXTURE_2D, 0);
		glEnable(GL_DEPTH_TEST);
		glDisable(GL_BLEND);
	}
}

void startScene::DrawTextWithAtlas(const std::string& text, float x, float y, float size)
{
	float inputX = x; // 입력 처질 위치
	float charWidth = size;

	for (char c : text)
	{
		UVRect uv = m_charUVMap[c];
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(inputX, y, 0.0f));
		model = glm::scale(model, glm::vec3(charWidth, charWidth, 1.0f));

		glUniformMatrix4fv(font_modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		glUniform4f(font_uvRectLoc, uv.uMin, uv.vMin, uv.uWidth, uv.vHeight);

		glDrawArrays(GL_TRIANGLES, 0, fontQuadVertCount);
		inputX += charWidth - 30.f;
	}
}

void startScene::keyboard(unsigned char key, bool isPressed)
{
	// 눌러졌을 때 아니면 뤼턴
	if (!isPressed)
		return;

	std::string* activeString = nullptr;
	if (m_activeField == 0) {		// ID 
		activeString = &m_idInput;
	}
	else if (m_activeField == 1) {	// IP주소
		activeString = &m_ipInput;
	}

	switch (key) {
	case 8: // 뒤로가기
		if (activeString && !activeString->empty()) {
			activeString->pop_back(); // 마지막 글자 삭제..
		}
		break;
	case 9:
		m_activeField = (m_activeField + 1) % 2;
		if (m_activeField == 0) std::cout << "ID 필드 활성화" << std::endl;
		else std::cout << "IP 필드 활성화" << std::endl;
		break;
	case 32:
		std::cout << " 스페이스바 누름!" << std::endl;
		SERVERIP = m_ipInput; // 서버 주소 전달
		m_sceneManager->changeScene("mainGame");
		break;
	default:
		if (key >= 32 && key <= 126) {
			if (activeString) {
				// (선택) 입력 길이 제한
				// if (activeString->length() < 20) { 
				*activeString += static_cast<char>(key); // 문자열에 추가
				// }
			}
		}
		break;
	}

	std::cout << "ID: [" << m_idInput << "], IP: [" << m_ipInput << "]" << std::endl;
	glutPostRedisplay();
}

void startScene::specialKeyboard(int key, bool isPressed)
{
	if (!isPressed) {
		return;
	}


}

void startScene::mouse(int button, int state, int x, int y)
{
}

void startScene::mouseMove(int x, int y)
{
}

void startScene::setWindowSize(int winWidth, int winHeight)
{
	width = winWidth;
	height = winHeight;
}