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

	m_currentYaw = 0.f;
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

		{	// 얘네 없어도 움직여얗한다..
			//if (glm::length(dir) >= glm::epsilon<float>())
			//move(dir, moveSpeed * elapseTime);
			//
			//playerX = worldTransform[3][0];
			//playerZ = worldTransform[3][2];
		}
	}

		if (playerZ > -8.f && playerZ < 10.f &&
			playerX > -15.f && playerX < 1.f) {
			//std::cout << "농장안에 들어옴" << std::endl;
			isInFarm = true;
		}
		else {
			isInFarm = false;
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
	glm::vec3 serverPos(serverX, worldTransform[3].y, serverZ);
	worldTransform[3] = glm::vec4(serverPos, 1.0f);

	playerX = serverX;
	playerZ = serverZ;
}