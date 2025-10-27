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
		// 2. "속도 * 시간" 만큼만 더합니다
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
	GLint useLightColor = glGetUniformLocation(texShader, "lightColor");
	if (useLightColor < 0) {
		std::cout << " lightColor 찾을 수 없음.." << std::endl;
	}
	glUniform3f(useLightColor, 1.f, 1.f, 1.f);


	// TITLE obj 그릴거..
	GLuint objShader = m_resourceManager->getShader("obj");
	MeshData titleMesh = m_resourceManager->getMesh("title");

	glUseProgram(objShader);
	glBindVertexArray(titleMesh.VAO);
	glUniform1i(usingLight, GL_TRUE);

	GLint useGlobalColor = glGetUniformLocation(objShader, "useGlobalColor");
	{
		GLuint viewLoc = glGetUniformLocation(objShader, "viewTransform");
		if (viewLoc < 0)
			std::cout << "viewLoc 찾지 못함\n";
		GLuint projLoc = glGetUniformLocation(objShader, "projTransform");
		if (projLoc < 0)
			std::cout << "projLoc 찾지 못함\n";

		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(viewMatrix));
		glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projMatrix));

		glm::mat4 translateMatrix = glm::translate(glm::mat4(1.f), glm::vec3(0.f, -3.f, -15.f));
		glm::mat4 rotMatrixY = glm::rotate(glm::mat4(1.f), glm::radians(rotateY), glm::vec3(0.f, 1.f, 0.f));

		glm::mat4 sclaeMatrix = glm::scale(glm::mat4(1.f), glm::vec3(0.3f));
		glm::mat4 matrix = translateMatrix * rotMatrixY * sclaeMatrix;

		GLuint modelLoc = glGetUniformLocation(objShader, "modelTransform");
		if (modelLoc < 0)
			std::cout << " modelLoc 찾을수 없음!";
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(matrix));

		GLint color = glGetUniformLocation(objShader, "globalColor");
		glUniform1i(useGlobalColor, GL_TRUE);
		if (color < 0)
			std::cout << "globalColor 찾지 못함\n";
		else
			glUniform3f(color, 201 / 255.f, 133 / 255.f, 1 / 255.f);

		glUseProgram(objShader);
		useLightColor = glGetUniformLocation(objShader, "lightColor");
		if (useLightColor < 0) {
			std::cout << " lightColor 찾을 수 없음.." << std::endl;
		}
		glUniform3f(useLightColor, 1.f, 1.f, 1.f);

		glBindVertexArray(titleMesh.VAO);
		glDrawArrays(GL_TRIANGLES, 0, titleMesh.vertexCount);
	}


	glUseProgram(texShader);
	{
		glBindVertexArray(cubeMesh.VAO);
		glm::mat4 rotMatrixY = glm::rotate(glm::mat4(1.f), glm::radians(0.f), glm::vec3(0.f, 1.f, 0.f));
		glm::mat4 rotMatrixX = glm::rotate(glm::mat4(1.f), glm::radians(-10.f), glm::vec3(1.f, 0.f, 0.f));

		glm::mat4 translateMatrix = glm::translate(glm::mat4(1.f), glm::vec3(0.f, -3.f, 0.f));
		glm::mat4 sclaeMatrix = glm::scale(glm::mat4(1.f), glm::vec3(15.f, 1.5f, 0.1f));

		glm::mat4 matrix = translateMatrix * rotMatrixX * rotMatrixY * sclaeMatrix;

		GLint worldLoc = glGetUniformLocation(texShader, "modelTransform");
		if (worldLoc < 0)
			std::cout << "worldLoc 찾지 못함\n";
		else
			glUniformMatrix4fv(worldLoc, 1, GL_FALSE, glm::value_ptr(matrix));

		GLint viewLoc = glGetUniformLocation(texShader, "viewTransform");
		if (viewLoc < 0)
			std::cout << "viewLoc 찾지 못함\n";
		else
			glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(viewMatrix));

		GLint projLoc = glGetUniformLocation(texShader, "projTransform");
		if (projLoc < 0)
			std::cout << "projLoc 찾지 못함\n";
		else
			glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projMatrix));

		// 육면체 그리는거!!
		for (int i = 0; i < 6; ++i) {
			glBindTexture(GL_TEXTURE_2D, buttonTex);
			glDrawArrays(GL_TRIANGLES, 6 * i, 6);
		}
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