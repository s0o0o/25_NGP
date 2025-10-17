#include "PlayerObject.h"
#include "Pig.h"
#include <iostream>


// �÷��̾�� �׸��� ��������.. �ٶ󺸴� �����̶�

constexpr int winWidth = 1600, winHeight = 900;


PlayerObject::PlayerObject()
{
	isWPressed = false;
	isAPressed = false;
	isSPressed = false;
	isDPressed = false;

	moveSpeed = 6.f;

	initilize();

	playerX = worldTransform[3][0];
	playerZ = worldTransform[3][2];
}

PlayerObject::~PlayerObject()
{
}

void PlayerObject::initilize()
{
	setVAO(0, 0);
	setShader(0);
}

float playerLimit = 2.f;
float limitX = 22.f;

void PlayerObject::update(float elapseTime)
{

	glm::vec3 dir(0.f);
	if (isWPressed) {

		glm::vec3 newPosition = worldTransform[3]; // ���� ��ġ�� ����
		newPosition += getLook(); // �̵� ���� �߰�

		// X��� Z�� ������ üũ
		if (newPosition.z > -20.f + playerLimit && newPosition.z < 20.f - playerLimit &&
			newPosition.x > -limitX + playerLimit && newPosition.x < limitX - playerLimit) {
			dir += getLook(); // ������ �ʰ����� ������ �̵�
		}
		else {
			if (newPosition.z <= -20.f + playerLimit) dir.z = (-20.f + playerLimit) - worldTransform[3][2];
			if (newPosition.z >= 20.f - playerLimit) dir.z = (20.f - playerLimit) - worldTransform[3][2];
			if (newPosition.x <= -limitX + playerLimit) dir.x = (-limitX + playerLimit) - worldTransform[3][0];
			if (newPosition.x >= limitX - playerLimit) dir.x = (limitX - playerLimit) - worldTransform[3][0];
		}

		if (newPosition.z > -8.f && newPosition.z < 10.f &&
			newPosition.x > -15.f && newPosition.x < 1.f) {
			//std::cout << "����ȿ� ����" << std::endl;
			isInFarm = true;

		}
		else {
			isInFarm = false;
		}
	}
	if (isAPressed) {
		glm::vec3 newPosition = worldTransform[3]; // ���� ��ġ�� ����
		newPosition += getRight(); // �̵� ���� �߰�

		// X��� Z�� ������ üũ
		if (newPosition.z > -20.f + playerLimit && newPosition.z < 20.f - playerLimit &&
			newPosition.x > -limitX + playerLimit && newPosition.x < limitX - playerLimit) {
			dir += getRight(); // ������ �ʰ����� ������ �̵�
		}
		else {
			if (newPosition.z <= -20.f + playerLimit) dir.z = (-20.f + playerLimit) - worldTransform[3][2];
			if (newPosition.z >= 20.f - playerLimit) dir.z = (20.f - playerLimit) - worldTransform[3][2];
			if (newPosition.x <= -limitX + playerLimit) dir.x = (-limitX + playerLimit) - worldTransform[3][0];
			if (newPosition.x >= limitX - playerLimit) dir.x = (limitX - playerLimit) - worldTransform[3][0];
		}

		if (newPosition.z > -8.f && newPosition.z < 10.f &&
			newPosition.x > -15.f && newPosition.x < 1.f) {
			//std::cout << "����ȿ� ����" << std::endl;
			isInFarm = true;

		}
		else {
			isInFarm = false;
		}
	}

	if (isSPressed) {
		glm::vec3 newPosition = worldTransform[3]; // ���� ��ġ�� ����
		newPosition -= getLook(); // �̵� ���� �߰�

		// X��� Z�� ������ üũ
		if (newPosition.z > -20.f + playerLimit && newPosition.z < 20.f - playerLimit &&
			newPosition.x > -limitX + playerLimit && newPosition.x < limitX - playerLimit) {
			dir -= getLook(); // ������ �ʰ����� ������ �̵�
		}
		else {
			if (newPosition.z <= -20.f + playerLimit) dir.z = (-20.f + playerLimit) - worldTransform[3][2];
			if (newPosition.z >= 20.f - playerLimit) dir.z = (20.f - playerLimit) - worldTransform[3][2];
			if (newPosition.x <= -limitX + playerLimit) dir.x = (-limitX + playerLimit) - worldTransform[3][0];
			if (newPosition.x >= limitX - playerLimit) dir.x = (limitX - playerLimit) - worldTransform[3][0];
		}

		if (newPosition.z > -8.f && newPosition.z < 10.f &&
			newPosition.x > -15.f && newPosition.x < 1.f) {
			//std::cout << "����ȿ� ����" << std::endl;
			isInFarm = true;

		}
		else {
			isInFarm = false;
		}
	}

	if (isDPressed) {
		glm::vec3 newPosition = worldTransform[3]; // ���� ��ġ�� ����
		newPosition -= getRight(); // �̵� ���� �߰�

		// X��� Z�� ������ üũ
		if (newPosition.z > -20.f + playerLimit && newPosition.z < 20.f - playerLimit &&
			newPosition.x > -limitX + playerLimit && newPosition.x < limitX - playerLimit) {
			dir -= getRight(); // ������ �ʰ����� ������ �̵�
		}
		else {
			if (newPosition.z <= -20.f + playerLimit) dir.z = (-20.f + playerLimit) - worldTransform[3][2];
			if (newPosition.z >= 20.f - playerLimit) dir.z = (20.f - playerLimit) - worldTransform[3][2];
			if (newPosition.x <= -limitX + playerLimit) dir.x = (-limitX + playerLimit) - worldTransform[3][0];
			if (newPosition.x >= limitX - playerLimit) dir.x = (limitX - playerLimit) - worldTransform[3][0];
		}

		if (newPosition.z > -8.f && newPosition.z < 10.f &&
			newPosition.x > -15.f && newPosition.x < 1.f) {
			//std::cout << "����ȿ� ����" << std::endl;
			isInFarm = true;
		}
		else {
			isInFarm = false;
		}
	}


	// ������ �Դ��� üũ

	{
		glm::vec3 newPosition = worldTransform[3];

		if (newPosition.x > 6.f and newPosition.x <= 12.f and
			newPosition.z > 3.f and newPosition.z < 5.f) {
			//std::cout << "���� �տ� ��" << std::endl;
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

void PlayerObject::draw() const
{
}

void PlayerObject::release()
{
}


void PlayerObject::keyboard(unsigned char key, bool isPressed)
{
	if (isPressed) {			// �������� ��
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
		default:
			break;
		}
	}
	else {						// ������ ��
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
			std::cout << "Ŭ��" << std::endl;
			isLeftMousePressed = true;
			befMousePosX = x;
			befMousePosY = y;
		}
		else if (state == GLUT_UP) {
			std::cout << "��" << std::endl;
			isLeftMousePressed = false;
		}
	}

}

void PlayerObject::mouseMove(int x, int y)
{
	int moveXValue = x - befMousePosX;
	int moveYValue = y - befMousePosY;

	int limitRange = 20;
	if (moveXValue > limitRange) {
		moveXValue = limitRange;
	}
	else if (moveXValue < -limitRange) {
		moveXValue = -limitRange;
	}

	if (moveYValue > limitRange) {
		moveYValue = limitRange;
	}
	else if (moveYValue < -limitRange) {
		moveYValue = -limitRange;
	}


	rotateX(float(moveYValue / 160.f));
	rotateY(float(moveXValue / 80.f));


	std::cout << "moveXValue : " << moveXValue << std::endl;
	std::cout << "moveYValue : " << moveYValue << std::endl;


	moveXValue = x;
	moveYValue = y;
}
