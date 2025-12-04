#include "PlayerObject.h"
#include "Pig.h"
#include "Client.h"
#include <iostream>


// 플레이어는 그리지 않을거임.. 바라보는 시점이라

constexpr int winWidth = 1600, winHeight = 900;
extern SOCKET g_sock;

PlayerObject::PlayerObject()
{
	isWPressed = false;
	isAPressed = false;
	isSPressed = false;
	isDPressed = false;

	moveSpeed = 6.f;

	playerX = worldTransform[3][0];
	playerZ = worldTransform[3][2];

	m_currentYaw = 0.f;
	rotateY(0.f);

}

PlayerObject::~PlayerObject()
{
}

void PlayerObject::initialize()
{
	if (shader == 0) {
		std::cerr << "Error: player shader has not been set before calling initialize!\n";
		return;
	}

	m_worldLoc = glGetUniformLocation(shader, "modelTransform");

	if (m_worldLoc < 0) std::cerr << "Error: player modelTransform uniform not found!\n";

	m_viewLoc = glGetUniformLocation(shader, "viewTransform");
	m_projLoc = glGetUniformLocation(shader, "projTransform");
	m_lightPosLoc = glGetUniformLocation(shader, "lightPos");
}

float playerLimit = 2.f;
float limitX = 22.f;

void PlayerObject::update(float elapseTime)
{

	// 이동 패킷 전송
	if (isWPressed || isAPressed || isSPressed || isDPressed)
	{ 
		cs_move move_pk = {};
		if (isWPressed) move_pk.inputDir = W;
		else if (isAPressed) move_pk.inputDir = A;
		else if (isSPressed) move_pk.inputDir = S;
		else if (isDPressed) move_pk.inputDir = D;
		move_pk.currentYaw = m_currentYaw;

		//printf("이동 패킷 보냄! dir : %d, yaw: %f \n", move_pk.inputDir, move_pk.currentYaw);
		sendPacket(g_sock, PacketType::CS_MOVE, reinterpret_cast<char*>(&move_pk), sizeof(move_pk));
	}

	glm::vec3 newPosition = worldTransform[3];
		if (newPosition.z > -8.f && newPosition.z < 10.f &&
			newPosition.x > -17.f && newPosition.x < 1.f) {
			//std::cout << "농장안에 들어옴" << std::endl;
			isInFarm = true;
		}
		else {
			isInFarm = false;
		}

	// 상점에 왔는지 체크
	{
		
		if (newPosition.x > 6.f and newPosition.x <= 12.f and
			newPosition.z > 2.f and newPosition.z < 6.f) {
			//std::cout << "상점 앞에 옴" << std::endl;
			isStoreShow = true;
		}
		else {
			isStoreShow = false;
		}
	}
}

void PlayerObject::draw(const glm::mat4& viewMatrix,
	const glm::mat4& projMatrix, const glm::vec3& lightPos) const
{

	// 일단 큐브로 표현
	glUseProgram(shader);
	GLint viewLoc = glGetUniformLocation(shader, "viewTransform");
	GLint projLoc = glGetUniformLocation(shader, "projTransform");
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(viewMatrix));
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projMatrix));

	glm::mat4 modelMatrix = worldTransform;

	modelMatrix = glm::scale(modelMatrix, glm::vec3(0.75f, 1.0f, 0.75f)); // 키가 큰 직육면체
	GLint modelLoc = glGetUniformLocation(shader, "modelTransform");
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(modelMatrix));

	glBindVertexArray(VAO);
	glDrawArrays(GL_TRIANGLES, 0, vertexCount);





}

void PlayerObject::release()
{
}


void PlayerObject::setYaw(float newYaw)
{
	m_currentYaw = newYaw;
	glm::vec3 pos = getPosition();

	worldTransform = glm::mat4(1.0f);
	worldTransform = glm::translate(worldTransform, pos);

	worldTransform = glm::rotate(worldTransform, glm::radians(-newYaw), glm::vec3(0.0f, 1.0f, 0.0f));
}

void PlayerObject::keyboard(unsigned char key, bool isPressed)
{
	if (isPressed) {			// 눌러졌을 때
		switch (key) {
		case'W':
		case'w':
			isWPressed = true;

			break;
		case'S':
		case's':
			isSPressed = true;

			break;
		case'A':
		case'a':
			isAPressed = true;

			break;
		case'D':
		case'd':
			isDPressed = true;
			break;
		case'g':
		{
			float tempX = worldTransform[3][0];
			float tempY = worldTransform[3][1];
			float tempZ = worldTransform[3][2];
			std::cout << "X :" << tempX <<
				", Y :" << tempY <<
				", Z :" << tempZ << std::endl;
		}
		break;
		case 27:
			glutSetCursor(GLUT_CURSOR_INHERIT);
			break;
		case'n':
		case 'N':
			std::cout << getName() << std::endl;
			break;
		default:
			break;
		}
	}
	else {						// 떼졌을 때
		switch (key) {
		case'W':
		case'w':
			isWPressed = false;
			break;
		case'S':
		case's':
			isSPressed = false;
			break;
		case'A':
		case'a':
			isAPressed = false;
			break;
		case'D':
		case'd':
			isDPressed = false;
			break;
		default:
			break;
		}
	}
}

void PlayerObject::mouse(int button, int state, int x, int y)
{
	if (button == GLUT_LEFT_BUTTON) {
		if (state == GLUT_DOWN) {
			//std::cout << "클릭" << std::endl;
			isLeftMousePressed = true;
			befMousePosX = x;
			befMousePosY = y;
		}
		else if (state == GLUT_UP) {
			//std::cout << "뗌" << std::endl;
			isLeftMousePressed = false;
		}
	}
}

void PlayerObject::mouseMove(int x, int y)
{
	int moveXValue = x - befMousePosX;
	int moveYValue = y - befMousePosY;
	float yawAngle = static_cast<float>(moveXValue) * MOUSE_SENSITIVITY;
	float pitchAngle = static_cast<float>(moveYValue) * MOUSE_SENSITIVITY;
	float nextPitch = m_currentPitch + pitchAngle;
	float nextYaw = m_currentYaw + yawAngle;
	if (nextPitch > MAX_PITCH) {
		pitchAngle = MAX_PITCH - m_currentPitch; 
		m_currentPitch = MAX_PITCH;          
	}
	else if (nextPitch < MIN_PITCH) {
		pitchAngle = MIN_PITCH - m_currentPitch; 
		m_currentPitch = MIN_PITCH;          
	}
	else {
		m_currentPitch = nextPitch;
	}

	m_currentYaw = nextYaw;
	if (abs(yawAngle) > 0.001f) {
		rotateY(-yawAngle);
	}
	if (abs(pitchAngle) > 0.001f) { 
		rotateX(pitchAngle);
	}

	befMousePosX = x;
	befMousePosY = y;

	int windowCenterX = glutGet(GLUT_WINDOW_WIDTH) / 2;
	int windowCenterY = glutGet(GLUT_WINDOW_HEIGHT) / 2;
	if (x != windowCenterX || y != windowCenterY) {
		glutWarpPointer(windowCenterX, windowCenterY);
		befMousePosX = windowCenterX; // 워프된 위치로 last 업데이트
		befMousePosY = windowCenterY;
	}
}

// 서버에서 받은 위치로..
void PlayerObject::movePosition(float serverX, float serverZ)
{
	glm::vec3 serverPos(serverX, 1.0f, serverZ);
	worldTransform[3] = glm::vec4(serverPos, 1.0f);

	playerX = serverX;
	playerZ = serverZ;
}