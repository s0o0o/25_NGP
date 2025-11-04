#include "startScene.h"
#include "ResourceManager.h"
#include "sceneManager.h"

#include <iostream>
#include <vector>

#include <stb_image.h>

startScene::startScene(int winWidth, int winHeight)
	: width{ winWidth }, height{ winHeight }
{
}

startScene::~startScene()
{

}

void startScene::sceneOnEnter()
{
	width = 1600; 
	height = 900;
	rotateY = 0.0f;
	isRotate = true;
	isTitleAni = true;
	isTitleAniEnd = false;

	GLuint objShader = m_resourceManager->getShader("obj");
	usingLight = glGetUniformLocation(objShader, "useLight");
}

void startScene::sceneOnExit()
{
}


void startScene::update(float elapsedTime)
{
	const float rotateSpeed = 45.0f;
	if (isRotate) {
		rotateY += rotateSpeed * elapsedTime;
	}
	else if (not isRotate) {
		rotateY -= rotateSpeed * elapsedTime;
	}

	// 3. 경계 검사는 동일
	if (rotateY >= 30.f) {
		isRotate = false;
	}
	if (rotateY <= 0.f) {
		isRotate = true;
	}
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
			glm::mat4 sclaeMatrix = glm::scale(glm::mat4(1.f), glm::vec3(3.5f*3, 3.5f, 1.f));
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
}

void startScene::keyboard(unsigned char key, bool isPressed)
{
	if (isPressed) {			// 눌러졌을 때
		switch (key) {
		case 32:
			std::cout << " 스페이스바 누름!" << std::endl;
			// 씬전환..!
			m_sceneManager->changeScene("mainGame");
			break;
		default:
			break;
		}
	}
	else {						// 떼졌을 때
		switch (key) {
		default:
			break;
		}
	}
}

void startScene::specialKeyboard(int key, bool isPressed)
{
}

void startScene::mouse(int button, int state, int x, int y)
{
	// 화면 업데이트가 된다....

	//constexpr int WHEEL_UP = 3;
	//constexpr int WHEEL_DOWN = 4;

	//switch (state) {
	//case GLUT_DOWN:
	//	switch (button) {
	//	case GLUT_LEFT_BUTTON: {
	//		float xPos = (static_cast<float>(x) / width * 2.f - 1.f);
	//		float yPos = -(static_cast<float>(y) / height * 2.f - 1.f);
	//		break;
	//	}
	//	case GLUT_MIDDLE_BUTTON:
	//		std::cout << "휠클릭 : " << x << ", " << y << std::endl;
	//		break;
	//	case GLUT_RIGHT_BUTTON:
	//		std::cout << "우클릭 : " << x << ", " << y << std::endl;
	//		break;
	//	case WHEEL_UP:
	//		std::cout << "휠  업 : " << x << ", " << y << std::endl;
	//		break;
	//	case WHEEL_DOWN:
	//		std::cout << "휠다운 : " << x << ", " << y << std::endl;
	//		break;
	//	}
	//	break;
	//case GLUT_UP:
	//	switch (button) {
	//	case GLUT_LEFT_BUTTON:
	//		break;
	//	case GLUT_RIGHT_BUTTON:
	//		break;
	//	case GLUT_MIDDLE_BUTTON:
	//		break;
	//	}
	//	break;
	//}
}

void startScene::mouseMove(int x, int y)
{

}

void startScene::setWindowSize(int winWidth, int winHeight)
{
	width = winWidth;
	height = winHeight;
}