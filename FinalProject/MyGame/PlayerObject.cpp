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

	initialize();

	playerX = worldTransform[3][0];
	playerZ = worldTransform[3][2];

	rotateY(0.f);
}

PlayerObject::~PlayerObject()
{
}

void PlayerObject::initialize()
{
	setVAO(0, 0);
	setShader(0);
}

float playerLimit = 2.f;
float limitX = 22.f;

void PlayerObject::update(float elapseTime)
{
	glm::vec3 dir(0.f);

	// 이동 패킷 전송
	if (isWPressed || isAPressed || isSPressed || isDPressed)
	{
		cs_move move_pk;
		cs_move result_pk;
		move_pk.x = playerX;
		move_pk.y = playerZ;
		if (isWPressed) move_pk.dir = W;
		else if (isAPressed) move_pk.dir = A;
		else if (isSPressed) move_pk.dir = S;
		else if (isDPressed) move_pk.dir = D;
		sendPacket(g_sock, PacketType::CS_MOVE, reinterpret_cast<char*>(&move_pk), sizeof(move_pk));
		//recv(g_sock, reinterpret_cast<char*>(&result_pk), sizeof(result_pk), MSG_WAITALL);
	}

	if (isWPressed) {
		glm::vec3 newPosition = worldTransform[3]; // 현재 위치를 복사
		newPosition += getLook(); // 이동 방향 추가

		// X축과 Z축 범위를 체크
		if (newPosition.z > -20.f + playerLimit && newPosition.z < 20.f - playerLimit &&
			newPosition.x > -limitX + playerLimit && newPosition.x < limitX - playerLimit) {
			dir += getLook(); // 범위를 초과하지 않으면 이동
		}
		else {
			if (newPosition.z <= -20.f + playerLimit) dir.z = (-20.f + playerLimit) - worldTransform[3][2];
			if (newPosition.z >= 20.f - playerLimit) dir.z = (20.f - playerLimit) - worldTransform[3][2];
			if (newPosition.x <= -limitX + playerLimit) dir.x = (-limitX + playerLimit) - worldTransform[3][0];
			if (newPosition.x >= limitX - playerLimit) dir.x = (limitX - playerLimit) - worldTransform[3][0];
		}

		if (newPosition.z > -8.f && newPosition.z < 10.f &&
			newPosition.x > -15.f && newPosition.x < 1.f) {
			//std::cout << "농장안에 들어옴" << std::endl;
			isInFarm = true;
		}
		else {
			isInFarm = false;
		}
	}
	if (isAPressed) {
		glm::vec3 newPosition = worldTransform[3]; // 현재 위치를 복사
		newPosition += getRight(); // 이동 방향 추가

		// X축과 Z축 범위를 체크
		if (newPosition.z > -20.f + playerLimit && newPosition.z < 20.f - playerLimit &&
			newPosition.x > -limitX + playerLimit && newPosition.x < limitX - playerLimit) {
			dir += getRight(); // 범위를 초과하지 않으면 이동
		}
		else {
			if (newPosition.z <= -20.f + playerLimit) dir.z = (-20.f + playerLimit) - worldTransform[3][2];
			if (newPosition.z >= 20.f - playerLimit) dir.z = (20.f - playerLimit) - worldTransform[3][2];
			if (newPosition.x <= -limitX + playerLimit) dir.x = (-limitX + playerLimit) - worldTransform[3][0];
			if (newPosition.x >= limitX - playerLimit) dir.x = (limitX - playerLimit) - worldTransform[3][0];
		}

		if (newPosition.z > -8.f && newPosition.z < 10.f &&
			newPosition.x > -15.f && newPosition.x < 1.f) {
			//std::cout << "농장안에 들어옴" << std::endl;
			isInFarm = true;

		}
		else {
			isInFarm = false;
		}
	}

	if (isSPressed) {
		glm::vec3 newPosition = worldTransform[3]; // 현재 위치를 복사
		newPosition -= getLook(); // 이동 방향 추가

		// X축과 Z축 범위를 체크
		if (newPosition.z > -20.f + playerLimit && newPosition.z < 20.f - playerLimit &&
			newPosition.x > -limitX + playerLimit && newPosition.x < limitX - playerLimit) {
			dir -= getLook(); // 범위를 초과하지 않으면 이동
		}
		else {
			if (newPosition.z <= -20.f + playerLimit) dir.z = (-20.f + playerLimit) - worldTransform[3][2];
			if (newPosition.z >= 20.f - playerLimit) dir.z = (20.f - playerLimit) - worldTransform[3][2];
			if (newPosition.x <= -limitX + playerLimit) dir.x = (-limitX + playerLimit) - worldTransform[3][0];
			if (newPosition.x >= limitX - playerLimit) dir.x = (limitX - playerLimit) - worldTransform[3][0];
		}

		if (newPosition.z > -8.f && newPosition.z < 10.f &&
			newPosition.x > -15.f && newPosition.x < 1.f) {
			//std::cout << "농장안에 들어옴" << std::endl;
			isInFarm = true;

		}
		else {
			isInFarm = false;
		}
	}

	if (isDPressed) {
		glm::vec3 newPosition = worldTransform[3]; // 현재 위치를 복사
		newPosition -= getRight(); // 이동 방향 추가

		// X축과 Z축 범위를 체크
		if (newPosition.z > -20.f + playerLimit && newPosition.z < 20.f - playerLimit &&
			newPosition.x > -limitX + playerLimit && newPosition.x < limitX - playerLimit) {
			dir -= getRight(); // 범위를 초과하지 않으면 이동
		}
		else {
			if (newPosition.z <= -20.f + playerLimit) dir.z = (-20.f + playerLimit) - worldTransform[3][2];
			if (newPosition.z >= 20.f - playerLimit) dir.z = (20.f - playerLimit) - worldTransform[3][2];
			if (newPosition.x <= -limitX + playerLimit) dir.x = (-limitX + playerLimit) - worldTransform[3][0];
			if (newPosition.x >= limitX - playerLimit) dir.x = (limitX - playerLimit) - worldTransform[3][0];
		}

		if (newPosition.z > -8.f && newPosition.z < 10.f &&
			newPosition.x > -15.f && newPosition.x < 1.f) {
			//std::cout << "농장안에 들어옴" << std::endl;
			isInFarm = true;
		}
		else {
			isInFarm = false;
		}
	}

	// 상점에 왔는지 체크
	{
		glm::vec3 newPosition = worldTransform[3];
		if (newPosition.x > 6.f and newPosition.x <= 12.f and
			newPosition.z > 2.f and newPosition.z < 6.f) {
			//std::cout << "상점 앞에 옴" << std::endl;
			isStoreShow = true;
		}
		else {
			isStoreShow = false;
		}
	}

	playerX = worldTransform[3][0];
	playerZ = worldTransform[3][2];

	//std::cout << "playerX :" << playerX << ", worldTransform[3][0] : " << worldTransform[3][0] << std::endl;
	//std::cout << "playerZ :" << playerZ << ", worldTransform[3][2] : " << worldTransform[3][2] << std::endl;

	if (glm::length(dir) >= glm::epsilon<float>())
		move(dir, moveSpeed * elapseTime);
}

void PlayerObject::draw(const glm::mat4& viewMatrix,
	const glm::mat4& projMatrix, const glm::vec3& lightPos) const
{
}

void PlayerObject::release()
{
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
			std::cout << "클릭" << std::endl;
			isLeftMousePressed = true;
			befMousePosX = x;
			befMousePosY = y;
		}
		else if (state == GLUT_UP) {
			std::cout << "뗌" << std::endl;
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
