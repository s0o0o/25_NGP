#include "Pig.h"
#include "PlayerObject.h"
#include <iostream>
#include <random>

Pig::Pig()
{
	//srand(static_cast<unsigned int>(time(NULL)));

	float x = -10.f + static_cast<float>(std::rand()) / (static_cast<float>(RAND_MAX / 5.f));
	float z = -5.f + static_cast<float>(std::rand()) / (static_cast<float>(RAND_MAX / 10.f));

	rangeLimit = 1.f;

	pigXDir = (rand() % 2 == 0) ? 1.f : -1.f;
	pigZDir = (rand() % 2 == 0) ? 1.f : -1.f;
	moveSpeed = 0.5f + static_cast<float>(std::rand()) / (static_cast<float>(RAND_MAX / 1.f));

	movePigX = x;
	movePigZ = z;

	isNear = false;
	isBaby = true;

	// 돼지
	rotatePigLeftLeg = 0.f;
	rotatePigRightLeg = 0.f;
	rotateFacePig = 0.f;

	// 아기 돼지
	rotateBabyLeftLeg = 0.f;
	rotateBabyRightLeg = 0.f;
	rotateFaceBaby = 0.f;

	isMaxRotateBaby = false; // 각도 제한
	isMaxRotatePig = false;

	initBuffer(&pigVAO, &pigVertexCount, "./OBJ/cube.obj");

	rotateSpeed = 0.05f + static_cast<float>(std::rand()) / (static_cast<float>(RAND_MAX / 0.2f));
}

Pig::~Pig()
{
	//	delete this;
}

void Pig::initilize()
{
	//player = new PlayerObject;
}

// 펜스 실 범위
// x : -15.f ~ 0.f
// z : -8.f ~ 10.f

void Pig::update(float elapseTime)
{
	glm::vec3 dir(0.f);

	glm::vec3 newPosition = worldTransform[3]; // 현재 위치를 복사
	newPosition += getLook();

	//const glm::vec3 playerPosition = player->getPosition();

	//if (player->playerX >= worldTransform[3][0] - 1.f and
	//	player->playerX <= worldTransform[3][0] + 1.f and
	//	player->playerZ >= worldTransform[3][2] - 1.f and
	//	player->playerZ <= worldTransform[3][2] + 1.f) {
	//	std::cout << " 가까이에 있다!!!! " << std::endl;
	//}


	//rotateY(90.f);

	worldTransform[3][0] = movePigX;
	worldTransform[3][2] = movePigZ;



	if (isBaby)
	{
		// 아기돼지 다리 각도 회전
		if (isMaxRotateBaby)
		{
			rotateBabyLeftLeg -= rotateSpeed;
			rotateBabyRightLeg += rotateSpeed;
			if (rotateBabyLeftLeg <= -10.f) {
				isMaxRotateBaby = false;
			}
		}
		else if (not isMaxRotateBaby) {
			rotateBabyLeftLeg += rotateSpeed;
			rotateBabyRightLeg -= rotateSpeed;
			if (rotateBabyLeftLeg >= 10.f) {
				isMaxRotateBaby = true;
			}
		}
	}
	else if (not isBaby) {
		// 돼지 다리 각도 회전
		if (isMaxRotatePig)
		{
			rotatePigLeftLeg -= rotateSpeed;
			rotatePigRightLeg += rotateSpeed;
			if (rotatePigLeftLeg <= -30.f) {
				isMaxRotatePig = false;
			}
		}
		else if (not isMaxRotatePig) {
			rotatePigLeftLeg += rotateSpeed;
			rotatePigRightLeg -= rotateSpeed;
			if (rotatePigLeftLeg >= 30.f) {
				isMaxRotatePig = true;
			}
		}

	}

	if (movePigX > -15.f and movePigX < -1.f
		and movePigZ > -8.f and movePigZ < 9.f) {


		if (pigXDir > 0) {
			movePigX += 0.001f;
		}
		if (pigXDir < 0) {
			movePigX -= 0.001f;
		}
		if (pigZDir > 0) {
			movePigZ += 0.001f;
		}
		if (pigZDir < 0) {
			movePigZ -= 0.001f;
		}

		if (movePigX <= -15.f + rangeLimit) {
			pigXDir = 1.f;

			std::cout << pigXDir << std::endl;
		}
		if (movePigX >= -1.f - rangeLimit) {
			pigXDir = -1.f;

			std::cout << pigXDir << std::endl;
		}
		if (movePigZ <= -8.f + rangeLimit) {
			pigZDir = 1.f;

			std::cout << pigZDir << std::endl;
		}
		if (movePigZ >= 9.f - rangeLimit) {
			pigZDir = -1.f;

			std::cout << pigZDir << std::endl;
		}

		if (pigXDir > 0 and pigZDir > 0) {
			rotateFacePig = 45.f;
		}
		if (pigXDir > 0 and pigZDir < 0) {
			rotateFacePig = 135.f;
		}
		if (pigXDir < 0 and pigZDir > 0) {
			rotateFacePig = -45.f;
		}
		if (pigXDir < 0 and pigZDir < 0) {
			rotateFacePig = 225.f;
		}
	}

	/*if (glm::length(dir) >= glm::epsilon<float>())
		move(dir, moveSpeed * elapseTime);*/
}

void Pig::draw() const
{
	glUseProgram(shader);
	glBindVertexArray(pigVAO);

	GLint worldLoc = glGetUniformLocation(shader, "modelTransform");
	GLint colorLoc = glGetUniformLocation(shader, "colorTransform");

	if (isBaby) { // 아기 돼지

		const glm::mat4 unitMat(1.f);
		glm::mat4 moveFinal = glm::translate(unitMat, glm::vec3(movePigX, 0.0f, movePigZ));
		glm::mat4 rotateDir = glm::rotate(unitMat, glm::radians(rotateFacePig), glm::vec3(0.f, 1.f, 0.f));
		// 다리 그리기 뒷다리
		{
			glm::mat4 moveYMat = glm::translate(unitMat, glm::vec3(0.f, 0.5f, 0.f));	// 고정값..
			glm::mat4 scaleMat = glm::scale(unitMat, glm::vec3(0.1f, 0.15f, 0.1f));

			glm::mat4 moveleftPos = glm::translate(unitMat, glm::vec3(-0.15f, 0.f, -0.15f));
			glm::mat4 moveRightPos = glm::translate(unitMat, glm::vec3(0.15f, 0.f, -0.15f));

			glm::mat4 rotateleftleg = glm::rotate(unitMat, glm::radians(rotateBabyLeftLeg), glm::vec3(1.f, 0.f, 0.f));
			glm::mat4 rotaterightleg = glm::rotate(unitMat, glm::radians(rotateBabyRightLeg), glm::vec3(1.f, 0.f, 0.f));
			glm::mat4 finalMat;

			//왼쪽
			finalMat = moveFinal * rotateDir * moveleftPos * glm::translate(unitMat, glm::vec3(-0.0625f, 0.15f, 0.f)) * rotateleftleg * glm::translate(unitMat, glm::vec3(0.0625f, -0.15f, 0.f)) * scaleMat * moveYMat;
			glUniformMatrix4fv(worldLoc, 1, GL_FALSE, glm::value_ptr(finalMat));
			glUniform3f(colorLoc, 1.f, 0.5f, 0.7f);
			glDrawArrays(GL_TRIANGLES, 0, 36);
			//오른쪽
			finalMat = moveFinal * rotateDir * moveRightPos * glm::translate(unitMat, glm::vec3(0.0625f, 0.15f, 0.f)) * rotaterightleg * glm::translate(unitMat, glm::vec3(-0.0625f, -0.15f, 0.f)) * scaleMat * moveYMat;
			glUniformMatrix4fv(worldLoc, 1, GL_FALSE, glm::value_ptr(finalMat));
			glUniform3f(colorLoc, 1.f, 0.5f, 0.7f);
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}
		// 몸통 그리기
		{
			glm::mat4 moveYMat = glm::translate(unitMat, glm::vec3(0.f, 0.5f, 0.f));
			glm::mat4 scaleMat = glm::scale(unitMat, glm::vec3(0.4f, 0.4f, 0.45f));

			glm::mat4 moveBottomUp = glm::translate(unitMat, glm::vec3(0.f, 0.15f, 0.f));

			glm::mat4 finalMat = moveFinal * rotateDir * moveBottomUp * scaleMat * moveYMat;

			glUniformMatrix4fv(worldLoc, 1, GL_FALSE, glm::value_ptr(finalMat));
			glUniform3f(colorLoc, 1.f, 0.5f, 0.7f);
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}
		{
			// 앞다리
			glm::mat4 moveYMat = glm::translate(unitMat, glm::vec3(0.f, 0.5f, 0.f));	// 고정값..
			glm::mat4 scaleMat = glm::scale(unitMat, glm::vec3(0.1f, 0.15f, 0.1f));

			glm::mat4 moveleftPos = glm::translate(unitMat, glm::vec3(-0.15f, 0.f, 0.3f));
			glm::mat4 moveRightPos = glm::translate(unitMat, glm::vec3(0.15f, 0.f, 0.3f));

			glm::mat4 rotateleftleg = glm::rotate(unitMat, glm::radians(rotateBabyLeftLeg), glm::vec3(1.f, 0.f, 0.f));
			glm::mat4 rotaterightleg = glm::rotate(unitMat, glm::radians(rotateBabyRightLeg), glm::vec3(1.f, 0.f, 0.f));
			glm::mat4 finalMat;

			// 왼쪽
			finalMat = moveFinal * rotateDir * moveleftPos * glm::translate(unitMat, glm::vec3(-0.0625f, 0.15f, 0.f)) * rotateleftleg * glm::translate(unitMat, glm::vec3(0.0625f, -0.15f, 0.f)) * scaleMat * moveYMat;
			glUniformMatrix4fv(worldLoc, 1, GL_FALSE, glm::value_ptr(finalMat));
			glUniform3f(colorLoc, 1.f, 0.5f, 0.7f);
			glDrawArrays(GL_TRIANGLES, 0, 36);
			// 오른쪽
			finalMat = moveFinal * rotateDir * moveRightPos * glm::translate(unitMat, glm::vec3(0.0625f, 0.15f, 0.f)) * rotaterightleg * glm::translate(unitMat, glm::vec3(-0.0625f, -0.15f, 0.f)) * scaleMat * moveYMat;
			glUniformMatrix4fv(worldLoc, 1, GL_FALSE, glm::value_ptr(finalMat));
			glUniform3f(colorLoc, 1.f, 0.5f, 0.7f);
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}
		// 머리 그리기
		{
			glm::mat4 moveYMat = glm::translate(unitMat, glm::vec3(0.f, 0.5f, 0.f));
			glm::mat4 scaleMat = glm::scale(unitMat, glm::vec3(0.5f, 0.5f, 0.5f));

			glm::mat4 moveBottomUp = glm::translate(unitMat, glm::vec3(0.f, 0.15f, 0.45f));

			glm::mat4 finalMat = moveFinal * rotateDir * moveBottomUp * scaleMat * moveYMat;

			glUniformMatrix4fv(worldLoc, 1, GL_FALSE, glm::value_ptr(finalMat));
			glUniform3f(colorLoc, 1.f, 0.5f, 0.7f);
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}
		// 코 그리기
		{
			glm::mat4 moveYMat = glm::translate(unitMat, glm::vec3(0.f, 0.5f, 0.f));
			glm::mat4 scaleMat = glm::scale(unitMat, glm::vec3(0.2f, 0.1f, 0.1f));

			glm::mat4 moveBottomUp = glm::translate(unitMat, glm::vec3(0.f, 0.28f, 0.7f));

			glm::mat4 finalMat = moveFinal * rotateDir * moveBottomUp * scaleMat * moveYMat;
			glUniformMatrix4fv(worldLoc, 1, GL_FALSE, glm::value_ptr(finalMat));
			glUniform3f(colorLoc, 1.f, 0.5f, 0.7f);

			glDrawArrays(GL_TRIANGLES, 0, 36);
		}
		//콧구멍 그리기
		{
			glm::mat4 moveYMat = glm::translate(unitMat, glm::vec3(0.f, 0.5f, 0.f));	// 고정값..
			glm::mat4 scaleMat = glm::scale(unitMat, glm::vec3(0.05f, 0.05f, 0.05f));

			glm::mat4 moveleftPos = glm::translate(unitMat, glm::vec3(-0.05f, 0.30f, 0.73f));
			glm::mat4 moveRightPos = glm::translate(unitMat, glm::vec3(0.05f, 0.30f, 0.73f));

			glm::mat4 finalMat;
			// 왼쪽
			finalMat = moveFinal * rotateDir * moveleftPos * scaleMat * moveYMat;
			glUniformMatrix4fv(worldLoc, 1, GL_FALSE, glm::value_ptr(finalMat));
			glUniform3f(colorLoc, 0.f, 0.f, 0.f);
			glDrawArrays(GL_TRIANGLES, 0, 36);
			// 오른쪽
			finalMat = moveFinal * rotateDir * moveRightPos * scaleMat * moveYMat;
			glUniformMatrix4fv(worldLoc, 1, GL_FALSE, glm::value_ptr(finalMat));
			glUniform3f(colorLoc, 0.f, 0.f, 0.f);
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}
		//눈 그리기
		{
			glm::mat4 moveYMat = glm::translate(unitMat, glm::vec3(0.f, 0.5f, 0.f));	// 고정값..
			glm::mat4 scaleMat = glm::scale(unitMat, glm::vec3(0.08f, 0.08f, 0.08f));

			glm::mat4 moveleftPos = glm::translate(unitMat, glm::vec3(-0.15f, 0.35f, 0.68f));
			glm::mat4 moveRightPos = glm::translate(unitMat, glm::vec3(0.15f, 0.35f, 0.68f));

			glm::mat4 finalMat;
			// 왼쪽
			finalMat = moveFinal * rotateDir * moveleftPos * scaleMat * moveYMat;
			glUniformMatrix4fv(worldLoc, 1, GL_FALSE, glm::value_ptr(finalMat));
			glUniform3f(colorLoc, 0.f, 0.f, 0.f);
			glDrawArrays(GL_TRIANGLES, 0, 36);
			// 오른쪽
			finalMat = moveFinal * rotateDir * moveRightPos * scaleMat * moveYMat;
			glUniformMatrix4fv(worldLoc, 1, GL_FALSE, glm::value_ptr(finalMat));
			glUniform3f(colorLoc, 0.f, 0.f, 0.f);
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}
		//안광 그리기
		{
			glm::mat4 moveYMat = glm::translate(unitMat, glm::vec3(0.f, 0.5f, 0.f));	// 고정값..
			glm::mat4 scaleMat = glm::scale(unitMat, glm::vec3(0.035f, 0.035f, 0.04f));

			glm::mat4 moveleftPos = glm::translate(unitMat, glm::vec3(-0.135f, 0.38f, 0.72f));
			glm::mat4 moveRightPos = glm::translate(unitMat, glm::vec3(0.165f, 0.38f, 0.72f));

			glm::mat4 finalMat;
			// 왼쪽
			finalMat = moveFinal * rotateDir * moveleftPos * scaleMat * moveYMat;
			glUniformMatrix4fv(worldLoc, 1, GL_FALSE, glm::value_ptr(finalMat));
			glUniform3f(colorLoc, 1.f, 1.f, 1.f);
			glDrawArrays(GL_TRIANGLES, 0, 36);
			// 오른쪽
			finalMat = moveFinal * rotateDir * moveRightPos * scaleMat * moveYMat;
			glUniformMatrix4fv(worldLoc, 1, GL_FALSE, glm::value_ptr(finalMat));
			glUniform3f(colorLoc, 1.f, 1.f, 1.f);
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}
	}

	//어른돼지
	else if (not isBaby) {
		const glm::mat4 unitMat(1.f);
		glm::mat4 moveFinal = glm::translate(unitMat, glm::vec3(movePigX, 0.f, movePigZ));
		glm::mat4 rotateDir = glm::rotate(unitMat, glm::radians(rotateFacePig), glm::vec3(0.f, 1.f, 0.f));
		//다리 그리기
		{
			glm::mat4 moveYMat = glm::translate(unitMat, glm::vec3(0.f, 0.5f, 0.f));	// 고정값..
			glm::mat4 scaleMat = glm::scale(unitMat, glm::vec3(0.18f, 0.25f, 0.18f));

			glm::mat4 moveleftPos = glm::translate(unitMat, glm::vec3(-0.18f, 0.f, -0.25f));
			glm::mat4 moveRightPos = glm::translate(unitMat, glm::vec3(0.18f, 0.f, -0.25f));

			glm::mat4 rotateleftleg = glm::rotate(unitMat, glm::radians(rotatePigLeftLeg), glm::vec3(1.f, 0.f, 0.f));
			glm::mat4 rotaterightleg = glm::rotate(unitMat, glm::radians(rotatePigRightLeg), glm::vec3(1.f, 0.f, 0.f));

			glm::mat4 finalMat;

			//왼쪽
			finalMat = moveFinal * rotateDir * moveleftPos * glm::translate(unitMat, glm::vec3(-0.0625f, 0.15f, 0.f)) * rotateleftleg * glm::translate(unitMat, glm::vec3(0.0625f, -0.15f, 0.f)) * scaleMat * moveYMat;
			glUniformMatrix4fv(worldLoc, 1, GL_FALSE, glm::value_ptr(finalMat));
			glUniform3f(colorLoc, 1.f, 0.5f, 0.7f);
			glDrawArrays(GL_TRIANGLES, 0, 36);

			//오른쪽
			finalMat = moveFinal * rotateDir * moveRightPos * glm::translate(unitMat, glm::vec3(0.0625f, 0.15f, 0.f)) * rotaterightleg * glm::translate(unitMat, glm::vec3(-0.0625f, -0.15f, 0.f)) * scaleMat * moveYMat;
			glUniformMatrix4fv(worldLoc, 1, GL_FALSE, glm::value_ptr(finalMat));
			glUniform3f(colorLoc, 1.f, 0.5f, 0.7f);
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}
		//몸통 그리기
		{
			glm::mat4 moveYMat = glm::translate(unitMat, glm::vec3(0.f, 0.5f, 0.f));
			glm::mat4 scaleMat = glm::scale(unitMat, glm::vec3(0.55f, 0.55f, 0.8f));

			glm::mat4 moveBottomUp = glm::translate(unitMat, glm::vec3(0.f, 0.25f, 0.f));

			glm::mat4 finalMat = moveFinal * rotateDir * moveBottomUp * scaleMat * moveYMat;

			glUniformMatrix4fv(worldLoc, 1, GL_FALSE, glm::value_ptr(finalMat));
			glUniform3f(colorLoc, 1.f, 0.5f, 0.7f);
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}
		{
			//앞다리
			glm::mat4 moveYMat = glm::translate(unitMat, glm::vec3(0.f, 0.5f, 0.f));	// 고정값..
			glm::mat4 scaleMat = glm::scale(unitMat, glm::vec3(0.18f, 0.25f, 0.18f));

			glm::mat4 moveleftPos = glm::translate(unitMat, glm::vec3(-0.18f, 0.f, 0.3f));
			glm::mat4 moveRightPos = glm::translate(unitMat, glm::vec3(0.18f, 0.f, 0.3f));

			glm::mat4 rotateleftleg = glm::rotate(unitMat, glm::radians(rotatePigLeftLeg), glm::vec3(1.f, 0.f, 0.f));
			glm::mat4 rotaterightleg = glm::rotate(unitMat, glm::radians(rotatePigRightLeg), glm::vec3(1.f, 0.f, 0.f));

			glm::mat4 finalMat;

			//왼쪽
			finalMat = moveFinal * rotateDir * moveleftPos * glm::translate(unitMat, glm::vec3(-0.0625f, 0.15f, 0.f)) * rotateleftleg * glm::translate(unitMat, glm::vec3(0.0625f, -0.15f, 0.f)) * scaleMat * moveYMat;
			glUniformMatrix4fv(worldLoc, 1, GL_FALSE, glm::value_ptr(finalMat));
			glUniform3f(colorLoc, 1.f, 0.5f, 0.7f);
			glDrawArrays(GL_TRIANGLES, 0, 36);
			//오른쪽
			finalMat = moveFinal * rotateDir * moveRightPos * glm::translate(unitMat, glm::vec3(0.0625f, 0.15f, 0.f)) * rotaterightleg * glm::translate(unitMat, glm::vec3(-0.0625f, -0.15f, 0.f)) * scaleMat * moveYMat;
			glUniformMatrix4fv(worldLoc, 1, GL_FALSE, glm::value_ptr(finalMat));
			glUniform3f(colorLoc, 1.f, 0.5f, 0.7f);
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}
		//머리 그리기
		{
			glm::mat4 moveYMat = glm::translate(unitMat, glm::vec3(0.f, 0.5f, 0.f));
			glm::mat4 scaleMat = glm::scale(unitMat, glm::vec3(0.5f, 0.5f, 0.5f));

			glm::mat4 moveBottomUp = glm::translate(unitMat, glm::vec3(0.f, 0.4f, 0.5f));

			glm::mat4 finalMat = moveFinal * rotateDir * moveBottomUp * scaleMat * moveYMat;
			glUniformMatrix4fv(worldLoc, 1, GL_FALSE, glm::value_ptr(finalMat));
			glUniform3f(colorLoc, 1.f, 0.5f, 0.7f);
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}
		//코 그리기
		{
			glm::mat4 moveYMat = glm::translate(unitMat, glm::vec3(0.f, 0.5f, 0.f));
			glm::mat4 scaleMat = glm::scale(unitMat, glm::vec3(0.2f, 0.1f, 0.1f));

			glm::mat4 moveBottomUp = glm::translate(unitMat, glm::vec3(0.f, 0.55f, 0.75f));

			glm::mat4 finalMat = moveFinal * rotateDir * moveBottomUp * scaleMat * moveYMat;
			glUniformMatrix4fv(worldLoc, 1, GL_FALSE, glm::value_ptr(finalMat));
			glUniform3f(colorLoc, 1.f, 0.5f, 0.7f);
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}
		//콧구멍 그리기
		{
			glm::mat4 moveYMat = glm::translate(unitMat, glm::vec3(0.f, 0.5f, 0.f));	// 고정값..
			glm::mat4 scaleMat = glm::scale(unitMat, glm::vec3(0.05f, 0.05f, 0.05f));

			glm::mat4 moveleftPos = glm::translate(unitMat, glm::vec3(-0.05f, 0.58f, 0.8f));
			glm::mat4 moveRightPos = glm::translate(unitMat, glm::vec3(0.05f, 0.58f, 0.8f));

			glm::mat4 finalMat;

			//왼쪽
			finalMat = moveFinal * rotateDir * moveleftPos * scaleMat * moveYMat;
			glUniformMatrix4fv(worldLoc, 1, GL_FALSE, glm::value_ptr(finalMat));
			glUniform3f(colorLoc, 0.f, 0.f, 0.f);
			glDrawArrays(GL_TRIANGLES, 0, 36);
			//오른쪽
			finalMat = moveFinal * rotateDir * moveRightPos * scaleMat * moveYMat;
			glUniformMatrix4fv(worldLoc, 1, GL_FALSE, glm::value_ptr(finalMat));
			glUniform3f(colorLoc, 0.f, 0.f, 0.f);
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}
		//눈 그리기
		{

			glm::mat4 moveYMat = glm::translate(unitMat, glm::vec3(0.f, 0.5f, 0.f));	// 고정값..
			glm::mat4 scaleMat = glm::scale(unitMat, glm::vec3(0.08f, 0.1f, 0.1f));

			glm::mat4 moveleftPos = glm::translate(unitMat, glm::vec3(-0.15f, 0.6f, 0.75f));
			glm::mat4 moveRightPos = glm::translate(unitMat, glm::vec3(0.15f, 0.6f, 0.75f));

			glm::mat4 finalMat;

			//왼쪽
			finalMat = moveFinal * rotateDir * moveleftPos * scaleMat * moveYMat;
			glUniformMatrix4fv(worldLoc, 1, GL_FALSE, glm::value_ptr(finalMat));
			glUniform3f(colorLoc, 0.f, 0.f, 0.f);
			glDrawArrays(GL_TRIANGLES, 0, 36);
			//오른쪽
			finalMat = moveFinal * rotateDir * moveRightPos * scaleMat * moveYMat;
			glUniformMatrix4fv(worldLoc, 1, GL_FALSE, glm::value_ptr(finalMat));
			glUniform3f(colorLoc, 0.f, 0.f, 0.f);
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}
		//안광 그리기
		{

			glm::mat4 moveYMat = glm::translate(unitMat, glm::vec3(0.f, 0.5f, 0.f));	// 고정값..
			glm::mat4 scaleMat = glm::scale(unitMat, glm::vec3(0.05f, 0.1f, 0.1f));

			glm::mat4 moveleftPos = glm::translate(unitMat, glm::vec3(-0.2f, 0.6f, 0.75f));
			glm::mat4 moveRightPos = glm::translate(unitMat, glm::vec3(0.2f, 0.6f, 0.75f));

			glm::mat4 finalMat;

			//왼쪽
			finalMat = moveFinal * rotateDir * moveleftPos * scaleMat * moveYMat;
			glUniformMatrix4fv(worldLoc, 1, GL_FALSE, glm::value_ptr(finalMat));
			glUniform3f(colorLoc, 1.f, 1.f, 1.f);
			glDrawArrays(GL_TRIANGLES, 0, 36);
			//오른쪽
			finalMat = moveFinal * rotateDir * moveRightPos * scaleMat * moveYMat;
			glUniformMatrix4fv(worldLoc, 1, GL_FALSE, glm::value_ptr(finalMat));
			glUniform3f(colorLoc, 1.f, 1.f, 1.f);
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}
	}


	//std::cout << "뭐니.그려지니.?" << std::endl;
}

void Pig::release()
{
}

