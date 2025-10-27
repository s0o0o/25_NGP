#include "Penguin.h"
#include <iostream>

Penguin::Penguin(int num) : penguinNum(num)
{
	float x = -5.f + static_cast<float>(std::rand()) / (static_cast<float>(RAND_MAX / 4.f));
	float z = -5.f + static_cast<float>(std::rand()) / (static_cast<float>(RAND_MAX / 10.f));
	MOVE_SPEED = 1.0f + static_cast<float>(std::rand()) / (static_cast<float>(RAND_MAX / 0.3f));
	isBaby = true;

	rangeLimit = 1.f;

	XDir = (rand() % 2 == 0) ? 1.f : -1.f;
	ZDir = (rand() % 2 == 0) ? 1.f : -1.f;

	if (XDir > 0 && ZDir > 0)
	{
		rotateFacePenguin = 45.f;
		//std::cout << " 오른쪽가면서 카메라앞으로" << std::endl;
	}
	else if (XDir > 0 && ZDir < 0) {
		rotateFacePenguin = 135.f;
	//	std::cout << " 오른쪽가면서 뒤쪽으로" << std::endl;
	}
	else if (XDir < 0 && ZDir > 0) {
		rotateFacePenguin = -45.f;
		//std::cout << "왼쪽으로, 카메라쪽으로" << std::endl;
	}
	else if (XDir < 0 && ZDir < 0) {
		rotateFacePenguin = -135.f; // 225 -> -135
		//std::cout << " 왼쪽, 뒤쪽으로" << std::endl;
	}
	rotateY(rotateFacePenguin);

	rotatePenguinLeftArm = -25.f;
	rotatePenguinRightArm = 25.f;

	isMaxRotatePenguin = false;

	setPosition(x, 0.f, z);
	isNear = false;
}

Penguin::~Penguin()
{
}

void Penguin::initialize()
{
	std::cout << "Penguin init, shader : " << shader << std::endl;
	if (shader == 0) {
		std::cerr << "Error: Pig shader has not been set before calling initialize!\n";
		return;
	}

	m_worldLoc = glGetUniformLocation(shader, "modelTransform");
	m_colorLoc = glGetUniformLocation(shader, "colorTransform");

	std::cout << "Penguin m_worldLoc : " << m_worldLoc << std::endl;
	std::cout << "Penguin m_colorLoc : " << m_colorLoc << std::endl;

	if (m_worldLoc < 0) std::cerr << "Error: Penguin modelTransform uniform not found!\n";
	if (m_colorLoc < 0) std::cerr << "Error: Penguin colorTransform uniform not found!\n";

	m_viewLoc = glGetUniformLocation(shader, "viewTransform");
	m_projLoc = glGetUniformLocation(shader, "projTransform");
	m_lightPosLoc = glGetUniformLocation(shader, "lightPos");
}

void Penguin::update(float elapseTime)
{
	// 펭귄 날개 각도 회전
	if (isMaxRotatePenguin)
	{
		rotatePenguinLeftArm += LEG_ROTATE_SPEED * elapseTime;
		rotatePenguinRightArm -= LEG_ROTATE_SPEED * elapseTime;
		if (rotatePenguinLeftArm >= 0.f) {
			isMaxRotatePenguin = false;
		}
	}
	else if (not isMaxRotatePenguin) {
		rotatePenguinLeftArm -= LEG_ROTATE_SPEED * elapseTime;
		rotatePenguinRightArm += LEG_ROTATE_SPEED * elapseTime;
		if (rotatePenguinLeftArm <= -25.f) {
			isMaxRotatePenguin = true;
		}
	}

	// 위치 변경
	glm::vec3 currentPos = getPosition();
	float deltaX = XDir * MOVE_SPEED * elapseTime;
	float deltaZ = ZDir * MOVE_SPEED * elapseTime;
	glm::vec3 nextPos = currentPos + glm::vec3(deltaX, 0.f, deltaZ);

	bool directionChanged = false;
	if (nextPos.x <= -15.f + rangeLimit) { nextPos.x = -15.f + rangeLimit; XDir = 1.f; directionChanged = true; }	// x왼쪽끝
	if (nextPos.x >= -1.f - rangeLimit) { nextPos.x = -1.f - rangeLimit;  XDir = -1.f; directionChanged = true; }	// x오른쪽끝
	if (nextPos.z <= -8.f + rangeLimit) { nextPos.z = -8.f + rangeLimit;  ZDir = 1.f; directionChanged = true; }		// z나무쪽
	if (nextPos.z >= 9.f - rangeLimit) { nextPos.z = 9.f - rangeLimit;  ZDir = -1.f; directionChanged = true; }		// z카메라쪽

	setPosition(nextPos);
	float currentFace = rotateFacePenguin;
	// 벽 부딪혓을때
	if (directionChanged)
	{
		if (XDir > 0 && ZDir > 0)
		{
			rotateFacePenguin = 45.f;
			std::cout << "부딪힘, 오른쪽가면서 카메라앞으로" << std::endl;
		}
		else if (XDir > 0 && ZDir < 0) {
			rotateFacePenguin = 135.f;
			std::cout << "부딪힘, 오른쪽가면서 뒤쪽으로" << std::endl;
		}
		else if (XDir < 0 && ZDir > 0) {
			rotateFacePenguin = -45.f;
			std::cout << "부딪힘 왼쪽으로, 카메라쪽으로" << std::endl;
		}
		else if (XDir < 0 && ZDir < 0) {
			rotateFacePenguin = -135.f;
			std::cout << "부딪힘, 왼쪽, 뒤쪽으로" << std::endl;
		}
		rotateY(-currentFace + rotateFacePenguin);
	}
}

void Penguin::draw(const glm::mat4& viewMatrix,
	const glm::mat4& projMatrix, const glm::vec3& lightPos) const
{
	glUseProgram(shader);
	glBindVertexArray(VAO);

	glUniformMatrix4fv(m_viewLoc, 1, GL_FALSE, glm::value_ptr(viewMatrix));
	glUniformMatrix4fv(m_projLoc, 1, GL_FALSE, glm::value_ptr(projMatrix));
	glUniform3f(m_lightPosLoc, lightPos.x, lightPos.y, lightPos.z);

	glm::mat4 finalMat;
	glm::mat4 baseTransform = worldTransform;
	if (not isBaby) {
		// 펭귄
		const glm::mat4 unitMat(1.f);
		float scaleFactor_Big = 1.3f;
		// 발 그리기
		{
			glm::mat4 moveYMat = glm::translate(unitMat, glm::vec3(0.f, 0.5f, 0.f) * scaleFactor_Big);   // 고정값..
			glm::mat4 scaleMat = glm::scale(unitMat, glm::vec3(0.1f, 0.05f, 0.3f) * scaleFactor_Big);

			glm::mat4 moveleftPos = glm::translate(unitMat, glm::vec3(-0.12f, 0.02f, 0.1f) * scaleFactor_Big);
			glm::mat4 moveRightPos = glm::translate(unitMat, glm::vec3(0.12f, 0.02f, 0.1f) * scaleFactor_Big);

			// 왼쪽
			glm::mat4 partTransform = moveleftPos * scaleMat * moveYMat;
			finalMat = baseTransform * partTransform;
			glUniformMatrix4fv(m_worldLoc, 1, GL_FALSE, glm::value_ptr(finalMat));
			glUniform3f(m_colorLoc, 1.f, 1.f, 0.3f);
			glDrawArrays(GL_TRIANGLES, 0, 36);
			// 오른쪽
			partTransform = moveRightPos * scaleMat * moveYMat;
			finalMat = baseTransform * partTransform;
			glUniformMatrix4fv(m_worldLoc, 1, GL_FALSE, glm::value_ptr(finalMat));
			glUniform3f(m_colorLoc, 1.f, 1.f, 0.3f);
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}
		// 팔 그리기
		{
			glm::mat4 moveYMat = glm::translate(unitMat, glm::vec3(0.f, 0.5f, 0.f) * scaleFactor_Big);
			glm::mat4 scaleMat = glm::scale(unitMat, glm::vec3(0.0625f, 0.5f, 0.2f) * scaleFactor_Big); // 팔 크기

			// 어깨 위치 설정 (몸통의 윗부분 양옆)
			glm::mat4 moveleftPos = glm::translate(unitMat, glm::vec3(-0.2f, 0.2f, 0.f) * scaleFactor_Big); // 왼쪽 어깨
			glm::mat4 moveRightPos = glm::translate(unitMat, glm::vec3(0.2f, 0.2f, 0.f) * scaleFactor_Big); // 오른쪽 어깨

			glm::mat4 rotateleftarm = glm::rotate(unitMat, glm::radians(rotatePenguinLeftArm), glm::vec3(0.f, 0.f, 1.f));
			glm::mat4 rotaterightarm = glm::rotate(unitMat, glm::radians(rotatePenguinRightArm), glm::vec3(0.f, 0.f, 1.f));

			glm::mat4 partTransform = moveleftPos * glm::translate(unitMat, glm::vec3(-0.15f, 0.4f, 0.f)
				* scaleFactor_Big) * rotateleftarm * glm::translate(unitMat, glm::vec3(0.15f, -0.4f, 0.f) * scaleFactor_Big)
				 * scaleMat * moveYMat;
			finalMat = baseTransform * partTransform;
			glUniformMatrix4fv(m_worldLoc, 1, GL_FALSE, glm::value_ptr(finalMat));
			glUniform3f(m_colorLoc, 0.f, 0.f, 0.36f);
			glDrawArrays(GL_TRIANGLES, 0, 36);

			// 오른쪽 팔
			partTransform = moveRightPos * glm::translate(unitMat, glm::vec3(0.15f, 0.4f, 0.f)
				* scaleFactor_Big) * rotaterightarm * glm::translate(unitMat, glm::vec3(-0.15f, -0.4f, 0.f) * scaleFactor_Big)
				 * scaleMat * moveYMat;
			finalMat = baseTransform * partTransform;
			glUniformMatrix4fv(m_worldLoc, 1, GL_FALSE, glm::value_ptr(finalMat));
			glUniform3f(m_colorLoc, 0.f, 0.f, 0.36f);
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}
		// 몸통 그리기
		{
			glm::mat4 moveYMat = glm::translate(unitMat, glm::vec3(0.f, 0.5f, 0.f) * scaleFactor_Big);
			glm::mat4 scaleMat = glm::scale(unitMat, glm::vec3(0.4f, 0.65f, 0.4f) * scaleFactor_Big);

			glm::mat4 moveBottomUp = glm::translate(unitMat, glm::vec3(0.f, 0.0f, 0.f) * scaleFactor_Big);

			glm::mat4 partTransform = moveBottomUp * scaleMat * moveYMat;
			finalMat = baseTransform * partTransform;
			glUniformMatrix4fv(m_worldLoc, 1, GL_FALSE, glm::value_ptr(finalMat));
			glUniform3f(m_colorLoc, 0.f, 0.f, 0.36f);
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}
		// 배 그리기
		{
			glm::mat4 moveYMat = glm::translate(unitMat, glm::vec3(0.f, 0.5f, 0.f) * scaleFactor_Big);
			glm::mat4 scaleMat = glm::scale(unitMat, glm::vec3(0.3f, 0.5f, 0.1f) * scaleFactor_Big);

			glm::mat4 moveBottomUp = glm::translate(unitMat, glm::vec3(0.f, 0.09f, 0.18f) * scaleFactor_Big);

			glm::mat4 partTransform = moveBottomUp * scaleMat * moveYMat;
			finalMat = baseTransform * partTransform;
			glUniformMatrix4fv(m_worldLoc, 1, GL_FALSE, glm::value_ptr(finalMat));
			glUniform3f(m_colorLoc, 1.f, 1.f, 1.f);
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}
		// 머리 그리기
		{
			glm::mat4 moveYMat = glm::translate(unitMat, glm::vec3(0.f, 0.5f, 0.f) * scaleFactor_Big);
			glm::mat4 scaleMat = glm::scale(unitMat, glm::vec3(0.3f, 0.3f, 0.3f) * scaleFactor_Big);

			glm::mat4 moveBottomUp = glm::translate(unitMat, glm::vec3(0.f, 0.7f, 0.f) * scaleFactor_Big);

			glm::mat4 partTransform = moveBottomUp * scaleMat * moveYMat;
			finalMat = baseTransform * partTransform;
			glUniformMatrix4fv(m_worldLoc, 1, GL_FALSE, glm::value_ptr(finalMat));
			glUniform3f(m_colorLoc, 0.f, 0.f, 0.36f);
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}
		// 부리 그리기
		{
			glm::mat4 moveYMat = glm::translate(unitMat, glm::vec3(0.f, 0.5f, 0.f) * scaleFactor_Big);
			glm::mat4 scaleMat = glm::scale(unitMat, glm::vec3(0.1f, 0.05f, 0.1f) * scaleFactor_Big);

			glm::mat4 moveBottomUp = glm::translate(unitMat, glm::vec3(0.f, 0.8f, 0.2f) * scaleFactor_Big);

			glm::mat4 partTransform = moveBottomUp * scaleMat * moveYMat;
			finalMat = baseTransform * partTransform;
			glUniformMatrix4fv(m_worldLoc, 1, GL_FALSE, glm::value_ptr(finalMat));
			glUniform3f(m_colorLoc, 1.f, 0.4f, 0.2f);
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}
		// 눈 그리기
		{
			glm::mat4 moveYMat = glm::translate(unitMat, glm::vec3(0.f, 0.6f, 0.f) * scaleFactor_Big);   // 고정값..
			glm::mat4 scaleMat = glm::scale(unitMat, glm::vec3(0.05f, 0.05f, 0.05f) * scaleFactor_Big);

			glm::mat4 moveleftPos = glm::translate(unitMat, glm::vec3(-0.07f, 0.85f, 0.15f) * scaleFactor_Big);
			glm::mat4 moveRightPos = glm::translate(unitMat, glm::vec3(0.07f, 0.85f, 0.15f) * scaleFactor_Big);

			glm::mat4 partTransform = moveleftPos * scaleMat * moveYMat;
			finalMat = baseTransform * partTransform;

			glUniformMatrix4fv(m_worldLoc, 1, GL_FALSE, glm::value_ptr(finalMat));
			glUniform3f(m_colorLoc, 0.5f, 0.5f, 0.5f);
			glDrawArrays(GL_TRIANGLES, 0, 36);
			// 오른쪽
			partTransform = moveRightPos * scaleMat * moveYMat;
			finalMat = baseTransform * partTransform;
			glUniformMatrix4fv(m_worldLoc, 1, GL_FALSE, glm::value_ptr(finalMat));
			glUniform3f(m_colorLoc, 0.5f, 0.5f, 0.5f);
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}
	}
	else if (isBaby) {
		// 아기펭귄
		const glm::mat4 unitMat(1.f);
		float scaleFactor = 0.8f;
		// 발 그리기
		{
			glm::mat4 moveYMat = glm::translate(unitMat, glm::vec3(0.f, 0.5f, 0.f) * scaleFactor);   // 고정값..
			glm::mat4 scaleMat = glm::scale(unitMat, glm::vec3(0.1f, 0.05f, 0.3f) * scaleFactor);

			glm::mat4 moveleftPos = glm::translate(unitMat, glm::vec3(-0.12f, 0.f, 0.1f) * scaleFactor);
			glm::mat4 moveRightPos = glm::translate(unitMat, glm::vec3(0.12f, 0.f, 0.1f) * scaleFactor);;


			// 왼쪽
			glm::mat4 partTransform = moveleftPos * scaleMat * moveYMat;
			finalMat = baseTransform * partTransform;
			glUniformMatrix4fv(m_worldLoc, 1, GL_FALSE, glm::value_ptr(finalMat));
			glUniform3f(m_colorLoc, 1.f, 1.f, 0.3f);
			glDrawArrays(GL_TRIANGLES, 0, 36);
			// 오른쪽
			partTransform = moveRightPos * scaleMat * moveYMat;
			finalMat = baseTransform * partTransform;
			glUniformMatrix4fv(m_worldLoc, 1, GL_FALSE, glm::value_ptr(finalMat));
			glUniform3f(m_colorLoc, 1.f, 1.f, 0.3f);
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}
		// 팔 그리기
		{
			glm::mat4 moveYMat = glm::translate(unitMat, glm::vec3(0.f, 0.5f, 0.f) * scaleFactor);
			glm::mat4 scaleMat = glm::scale(unitMat, glm::vec3(0.0625f, 0.5f, 0.2f) * scaleFactor); // 팔 크기

			// 어깨 위치 설정 (몸통의 윗부분 양옆)
			glm::mat4 moveleftPos = glm::translate(unitMat, glm::vec3(-0.2f, 0.2f, 0.f) * scaleFactor); // 왼쪽 어깨
			glm::mat4 moveRightPos = glm::translate(unitMat, glm::vec3(0.2f, 0.2f, 0.f) * scaleFactor); // 오른쪽 어깨

			glm::mat4 rotateleftarm = glm::rotate(unitMat, glm::radians(rotatePenguinLeftArm), glm::vec3(0.f, 0.f, 1.f));
			glm::mat4 rotaterightarm = glm::rotate(unitMat, glm::radians(rotatePenguinRightArm), glm::vec3(0.f, 0.f, 1.f));

			// 왼쪽 팔
			glm::mat4 partTransform = moveleftPos * glm::translate(unitMat, glm::vec3(-0.15f, 0.4f, 0.f) * scaleFactor)
				* rotateleftarm * glm::translate(unitMat, glm::vec3(0.15f, -0.4f, 0.f) * scaleFactor) * scaleMat * moveYMat;
			finalMat = baseTransform * partTransform;
			glUniformMatrix4fv(m_worldLoc, 1, GL_FALSE, glm::value_ptr(finalMat));
			glUniform3f(m_colorLoc, 0.f, 0.f, 0.36f);
			glDrawArrays(GL_TRIANGLES, 0, 36);

			// 오른쪽 팔
			partTransform = moveRightPos * glm::translate(unitMat, glm::vec3(0.15f, 0.4f, 0.f) * scaleFactor)
				* rotaterightarm * glm::translate(unitMat, glm::vec3(-0.15f, -0.4f, 0.f) * scaleFactor) * scaleMat * moveYMat;
			finalMat = baseTransform * partTransform;
			glUniformMatrix4fv(m_worldLoc, 1, GL_FALSE, glm::value_ptr(finalMat));
			glUniform3f(m_colorLoc, 0.f, 0.f, 0.36f);
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}
		// 몸통 그리기
		{
			glm::mat4 moveYMat = glm::translate(unitMat, glm::vec3(0.f, 0.5f, 0.f) * scaleFactor);
			glm::mat4 scaleMat = glm::scale(unitMat, glm::vec3(0.4f, 0.65f, 0.4f) * scaleFactor);

			glm::mat4 moveBottomUp = glm::translate(unitMat, glm::vec3(0.f, 0.05f, 0.f) * scaleFactor);

			glm::mat4 partTransform = moveBottomUp * scaleMat * moveYMat;
			finalMat = baseTransform * partTransform;
			glUniformMatrix4fv(m_worldLoc, 1, GL_FALSE, glm::value_ptr(finalMat));
			glUniform3f(m_colorLoc, 0.f, 0.f, 0.36f);
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}
		// 배 그리기
		{
			glm::mat4 moveYMat = glm::translate(unitMat, glm::vec3(0.f, 0.5f, 0.f) * scaleFactor);
			glm::mat4 scaleMat = glm::scale(unitMat, glm::vec3(0.3f, 0.5f, 0.1f) * scaleFactor);

			glm::mat4 moveBottomUp = glm::translate(unitMat, glm::vec3(0.f, 0.09f, 0.18f) * scaleFactor);


			glm::mat4 partTransform = moveBottomUp * scaleMat * moveYMat;
			finalMat = baseTransform * partTransform;
			glUniformMatrix4fv(m_worldLoc, 1, GL_FALSE, glm::value_ptr(finalMat));
			glUniform3f(m_colorLoc, 1.f, 1.f, 1.f);
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}
		// 머리 그리기
		{
			glm::mat4 moveYMat = glm::translate(unitMat, glm::vec3(0.f, 0.5f, 0.f) * scaleFactor);
			glm::mat4 scaleMat = glm::scale(unitMat, glm::vec3(0.3f, 0.3f, 0.3f) * scaleFactor);

			glm::mat4 moveBottomUp = glm::translate(unitMat, glm::vec3(0.f, 0.68f, 0.f) * scaleFactor);

			glm::mat4 partTransform = moveBottomUp * scaleMat * moveYMat;
			finalMat = baseTransform * partTransform;
			glUniformMatrix4fv(m_worldLoc, 1, GL_FALSE, glm::value_ptr(finalMat));

			glUniform3f(m_colorLoc, 0.f, 0.f, 0.36f);
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}
		// 부리 그리기
		{
			glm::mat4 moveYMat = glm::translate(unitMat, glm::vec3(0.f, 0.5f, 0.f) * scaleFactor);
			glm::mat4 scaleMat = glm::scale(unitMat, glm::vec3(0.1f, 0.05f, 0.1f) * scaleFactor);

			glm::mat4 moveBottomUp = glm::translate(unitMat, glm::vec3(0.f, 0.8f, 0.2f) * scaleFactor);

			glm::mat4 partTransform = moveBottomUp * scaleMat * moveYMat;
			finalMat = baseTransform * partTransform;
			glUniformMatrix4fv(m_worldLoc, 1, GL_FALSE, glm::value_ptr(finalMat));
			glUniform3f(m_colorLoc, 1.f, 0.4f, 0.2f);
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}
		// 눈 그리기
		{
			glm::mat4 moveYMat = glm::translate(unitMat, glm::vec3(0.f, 0.6f, 0.f) * scaleFactor);   // 고정값..
			glm::mat4 scaleMat = glm::scale(unitMat, glm::vec3(0.05f, 0.05f, 0.05f) * scaleFactor);

			glm::mat4 moveleftPos = glm::translate(unitMat, glm::vec3(-0.07f, 0.85f, 0.15f) * scaleFactor);
			glm::mat4 moveRightPos = glm::translate(unitMat, glm::vec3(0.07f, 0.85f, 0.15f) * scaleFactor);

			// 왼쪽
			glm::mat4 partTransform = moveleftPos * scaleMat * moveYMat;
			finalMat = baseTransform * partTransform;
			glUniformMatrix4fv(m_worldLoc, 1, GL_FALSE, glm::value_ptr(finalMat));
			glUniform3f(m_colorLoc, 0.5f, 0.5f, 0.5f);
			glDrawArrays(GL_TRIANGLES, 0, 36);
			// 오른쪽
			partTransform = moveRightPos * scaleMat * moveYMat;
			finalMat = baseTransform * partTransform;
			glUniformMatrix4fv(m_worldLoc, 1, GL_FALSE, glm::value_ptr(finalMat));
			glUniform3f(m_colorLoc, 0.5f, 0.5f, 0.5f);
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}
	}
}

void Penguin::release()
{
}
