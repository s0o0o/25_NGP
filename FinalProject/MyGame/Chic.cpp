#include "Chic.h"
#include "PlayerObject.h"
#include <iostream>

Chic::Chic()
{
	float x = -8.f + static_cast<float>(std::rand()) / (static_cast<float>(RAND_MAX / 7.f));
	float z = -5.f + static_cast<float>(std::rand()) / (static_cast<float>(RAND_MAX / 5.f));

	//setPosition(x, 0.f, z);
	isBaby = true;

	rangeLimit = 1.f;

	chicXDir = (rand() % 2 == 0) ? 1.f : -1.f;
	chicZDir = (rand() % 2 == 0) ? 1.f : -1.f;
	moveSpeed = 0.5f + static_cast<float>(std::rand()) / (static_cast<float>(RAND_MAX / 1.f));

	moveChicX = x;
	moveChicZ = z;

	rotateChicLeftLeg = 0.f;
	rotateChicRightLeg = 0.f;
	rotateFaceChic = 0.f;


	rotateChicLeftLeg = 0.f;
	rotateChicRightLeg = 0.f;
	rotateFaceChic = 0.f;

	isMaxRotateChic = false; // 각도 제한
	isMaxRotateAdultChic = false;

	initBuffer(&chicVAO, &chicVertexCount, "./OBJ/cube.obj");

	rotateSpeed = 0.05f + static_cast<float>(std::rand()) / (static_cast<float>(RAND_MAX / 0.2f));
}

Chic::~Chic()
{
}

void Chic::initilize()
{
}

void Chic::update(float elapseTime)
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

	worldTransform[3][0] = moveChicX;
	worldTransform[3][2] = moveChicZ;




	if (isMaxRotateChic)
	{
		rotateChicLeftLeg -= rotateSpeed;
		rotateChicRightLeg += rotateSpeed;
		if (rotateChicLeftLeg <= -30.f) {
			isMaxRotateChic = false;
		}
	}
	else if (not isMaxRotateChic) {
		rotateChicLeftLeg += rotateSpeed;
		rotateChicRightLeg -= rotateSpeed;
		if (rotateChicLeftLeg >= 30.f) {
			isMaxRotateChic = true;
		}
	}

	// 닭 다리 각도 회전
	if (isMaxRotateAdultChic)
	{
		rotateAdultChicLeftLeg -= rotateSpeed;
		rotateAdultChicRightLeg += rotateSpeed;
		if (rotateAdultChicLeftLeg <= -30.f) {
			isMaxRotateAdultChic = false;
		}
	}
	else if (not isMaxRotateAdultChic) {
		rotateAdultChicLeftLeg += rotateSpeed;
		rotateAdultChicRightLeg -= rotateSpeed;
		if (rotateAdultChicLeftLeg >= 30.f) {
			isMaxRotateAdultChic = true;
		}
	}




	if (moveChicX > -15.f and moveChicX < -1.f
		and moveChicZ > -8.f and moveChicZ < 9.f) {


		if (chicXDir > 0) {
			moveChicX += 0.001f;
		}
		if (chicXDir < 0) {
			moveChicX -= 0.001f;
		}
		if (chicZDir > 0) {
			moveChicZ += 0.001f;
		}
		if (chicZDir < 0) {
			moveChicZ -= 0.001f;
		}

		if (moveChicX <= -15.f + rangeLimit) {
			chicXDir = 1.f;

			std::cout << chicXDir << std::endl;
		}
		if (moveChicX >= -1.f - rangeLimit) {
			chicXDir = -1.f;

			std::cout << chicXDir << std::endl;
		}
		if (moveChicZ <= -8.f + rangeLimit) {
			chicZDir = 1.f;

			std::cout << chicZDir << std::endl;
		}
		if (moveChicZ >= 9.f - rangeLimit) {
			chicZDir = -1.f;

			std::cout << chicZDir << std::endl;
		}

		if (chicXDir > 0 and chicZDir > 0) {
			rotateFaceChic = 45.f;
		}
		if (chicXDir > 0 and chicZDir < 0) {
			rotateFaceChic = 135.f;
		}
		if (chicXDir < 0 and chicZDir > 0) {
			rotateFaceChic = -45.f;
		}
		if (chicXDir < 0 and chicZDir < 0) {
			rotateFaceChic = 225.f;
		}
	}

}

void Chic::draw() const
{
	glUseProgram(shader);
	glBindVertexArray(chicVAO);

	GLint worldLoc = glGetUniformLocation(shader, "modelTransform");
	GLint colorLoc = glGetUniformLocation(shader, "colorTransform");

	if (isBaby) {
		//병아리
		const glm::mat4 unitMat(1.f);
		glm::mat4 moveFinal = glm::translate(unitMat, glm::vec3(moveChicX, 0.05f, moveChicZ));
		glm::mat4 rotateDir = glm::rotate(unitMat, glm::radians(rotateFaceChic), glm::vec3(0.f, 1.f, 0.f));
		//다리 그리기
		{
			glm::mat4 moveYMat = glm::translate(unitMat, glm::vec3(0.f, 0.5f, 0.f));	// 고정값..
			glm::mat4 scaleMat = glm::scale(unitMat, glm::vec3(0.08f, 0.15f, 0.08f));

			glm::mat4 moveleftPos = glm::translate(unitMat, glm::vec3(-0.07f, 0.f, 0.f));
			glm::mat4 moveRightPos = glm::translate(unitMat, glm::vec3(0.07f, 0.f, 0.f));

			glm::mat4 rotateleftleg = glm::rotate(unitMat, glm::radians(rotateChicLeftLeg), glm::vec3(1.f, 0.f, 0.f));
			glm::mat4 rotaterightleg = glm::rotate(unitMat, glm::radians(rotateChicRightLeg), glm::vec3(1.f, 0.f, 0.f));

			glm::mat4 finalMat;

			//왼쪽
			finalMat = moveFinal * rotateDir * moveleftPos * glm::translate(unitMat, glm::vec3(-0.0625f, 0.15f, 0.f)) * rotateleftleg * glm::translate(unitMat, glm::vec3(0.0625f, -0.15f, 0.f)) * scaleMat * moveYMat;
			glUniformMatrix4fv(worldLoc, 1, GL_FALSE, glm::value_ptr(finalMat));
			glUniform3f(colorLoc, 1.f, 0.64f, 0.f);
			glDrawArrays(GL_TRIANGLES, 0, 36);
			//오른쪽
			finalMat = moveFinal * rotateDir * moveRightPos * glm::translate(unitMat, glm::vec3(0.0625f, 0.15f, 0.f)) * rotaterightleg * glm::translate(unitMat, glm::vec3(-0.0625f, -0.15f, 0.f)) * scaleMat * moveYMat;
			glUniformMatrix4fv(worldLoc, 1, GL_FALSE, glm::value_ptr(finalMat));
			glUniform3f(colorLoc, 1.f, 0.64f, 0.f);
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}
		//몸통 그리기
		{
			glm::mat4 moveYMat = glm::translate(unitMat, glm::vec3(0.f, 0.5f, 0.f));
			glm::mat4 scaleMat = glm::scale(unitMat, glm::vec3(0.4f, 0.4f, 0.4f));

			glm::mat4 moveBottomUp = glm::translate(unitMat, glm::vec3(0.f, 0.15f, 0.f));

			glm::mat4 finalMat = moveFinal * rotateDir * moveBottomUp * scaleMat * moveYMat;

			glUniformMatrix4fv(worldLoc, 1, GL_FALSE, glm::value_ptr(finalMat));
			glUniform3f(colorLoc, 1.f, 1.0f, 0.f);
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}
		//머리 그리기
		{
			glm::mat4 moveYMat = glm::translate(unitMat, glm::vec3(0.f, 0.5f, 0.f));
			glm::mat4 scaleMat = glm::scale(unitMat, glm::vec3(0.3f, 0.3f, 0.3f));

			glm::mat4 moveBottomUp = glm::translate(unitMat, glm::vec3(0.f, 0.4f, 0.25f));

			glm::mat4 finalMat = moveFinal * rotateDir * moveBottomUp * scaleMat * moveYMat;
			glUniform3f(colorLoc, 1.f, 1.0f, 0.f);
			glUniformMatrix4fv(worldLoc, 1, GL_FALSE, glm::value_ptr(finalMat));

			glDrawArrays(GL_TRIANGLES, 0, 36);
		}
		//부리 그리기
		{
			glm::mat4 moveYMat = glm::translate(unitMat, glm::vec3(0.f, 0.5f, 0.f));
			glm::mat4 scaleMat = glm::scale(unitMat, glm::vec3(0.1f, 0.05f, 0.1f));

			glm::mat4 moveBottomUp = glm::translate(unitMat, glm::vec3(0.f, 0.48f, 0.4f));

			glm::mat4 finalMat = moveFinal * rotateDir * moveBottomUp * scaleMat * moveYMat;
			glUniformMatrix4fv(worldLoc, 1, GL_FALSE, glm::value_ptr(finalMat));
			glUniform3f(colorLoc, 1.f, 0.64f, 0.f);
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}
		//눈 그리기
		{
			glm::mat4 moveYMat = glm::translate(unitMat, glm::vec3(0.f, 0.6f, 0.f));	// 고정값..
			glm::mat4 scaleMat = glm::scale(unitMat, glm::vec3(0.05f, 0.05f, 0.05f));

			glm::mat4 moveleftPos = glm::translate(unitMat, glm::vec3(-0.07f, 0.5f, 0.38f));
			glm::mat4 moveRightPos = glm::translate(unitMat, glm::vec3(0.07f, 0.5f, 0.38f));

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
	}

	if (not isBaby) {
		//닭
		const glm::mat4 unitMat(1.f);
		glm::mat4 moveFinal = glm::translate(unitMat, glm::vec3(moveChicX, 0.05f, moveChicZ));
		glm::mat4 rotateDir = glm::rotate(unitMat, glm::radians(rotateFaceChic), glm::vec3(0.f, 1.f, 0.f));
		//다리 그리기
		{
			glm::mat4 moveYMat = glm::translate(unitMat, glm::vec3(0.f, 0.5f, 0.f));	// 고정값..
			glm::mat4 scaleMat = glm::scale(unitMat, glm::vec3(0.05f, 0.2f, 0.05f));

			glm::mat4 moveleftPos = glm::translate(unitMat, glm::vec3(-0.07f, 0.f, 0.f));
			glm::mat4 moveRightPos = glm::translate(unitMat, glm::vec3(0.07f, 0.f, 0.f));

			glm::mat4 rotateleftleg = glm::rotate(unitMat, glm::radians(rotateChicLeftLeg), glm::vec3(1.f, 0.f, 0.f));
			glm::mat4 rotaterightleg = glm::rotate(unitMat, glm::radians(rotateChicRightLeg), glm::vec3(1.f, 0.f, 0.f));

			glm::mat4 finalMat;

			//왼쪽
			finalMat = moveFinal * rotateDir * moveleftPos * glm::translate(unitMat, glm::vec3(-0.0625f, 0.15f, 0.f)) * rotateleftleg * glm::translate(unitMat, glm::vec3(0.0625f, -0.15f, 0.f)) * scaleMat * moveYMat;
			glUniformMatrix4fv(worldLoc, 1, GL_FALSE, glm::value_ptr(finalMat));
			glUniform3f(colorLoc, 1.f, 0.64f, 0.f);
			glDrawArrays(GL_TRIANGLES, 0, 36);
			//오른쪽
			finalMat = moveFinal * rotateDir * moveRightPos * glm::translate(unitMat, glm::vec3(0.0625f, 0.15f, 0.f)) * rotaterightleg * glm::translate(unitMat, glm::vec3(-0.0625f, -0.15f, 0.f)) * scaleMat * moveYMat;
			glUniformMatrix4fv(worldLoc, 1, GL_FALSE, glm::value_ptr(finalMat));
			glUniform3f(colorLoc, 1.f, 0.64f, 0.f);
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}
		//몸통 그리기
		{
			glm::mat4 moveYMat = glm::translate(unitMat, glm::vec3(0.f, 0.5f, 0.f));
			glm::mat4 scaleMat = glm::scale(unitMat, glm::vec3(0.4f, 0.4f, 0.5f));

			glm::mat4 moveBottomUp = glm::translate(unitMat, glm::vec3(0.f, 0.18f, 0.f));

			glm::mat4 finalMat = moveFinal * rotateDir * moveBottomUp * scaleMat * moveYMat;

			glUniformMatrix4fv(worldLoc, 1, GL_FALSE, glm::value_ptr(finalMat));
			glUniform3f(colorLoc, 1.f, 1.0f, 1.f);
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}
		//머리 그리기
		{
			glm::mat4 moveYMat = glm::translate(unitMat, glm::vec3(0.f, 0.5f, 0.f));
			glm::mat4 scaleMat = glm::scale(unitMat, glm::vec3(0.25f, 0.3f, 0.3f));

			glm::mat4 moveBottomUp = glm::translate(unitMat, glm::vec3(0.f, 0.5f, 0.25f));

			glm::mat4 finalMat = moveFinal * rotateDir * moveBottomUp * scaleMat * moveYMat;
			glUniform3f(colorLoc, 1.f, 1.0f, 1.f);
			glUniformMatrix4fv(worldLoc, 1, GL_FALSE, glm::value_ptr(finalMat));

			glDrawArrays(GL_TRIANGLES, 0, 36);
		}
		//부리 그리기
		{
			glm::mat4 moveYMat = glm::translate(unitMat, glm::vec3(0.f, 0.5f, 0.f));
			glm::mat4 scaleMat = glm::scale(unitMat, glm::vec3(0.25f, 0.06f, 0.1f));

			glm::mat4 moveBottomUp = glm::translate(unitMat, glm::vec3(0.f, 0.57f, 0.4f));

			glm::mat4 finalMat = moveFinal * rotateDir * moveBottomUp * scaleMat * moveYMat;
			glUniformMatrix4fv(worldLoc, 1, GL_FALSE, glm::value_ptr(finalMat));
			glUniform3f(colorLoc, 1.f, 0.64f, 0.f);
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}
		//닭벼슬 그리기
		{
			glm::mat4 moveYMat = glm::translate(unitMat, glm::vec3(0.f, 0.5f, 0.f));
			glm::mat4 scaleMat = glm::scale(unitMat, glm::vec3(0.08f, 0.06f, 0.12f));

			glm::mat4 moveBottomUp = glm::translate(unitMat, glm::vec3(0.f, 0.51f, 0.44f));

			glm::mat4 finalMat = moveFinal * rotateDir * moveBottomUp * scaleMat * moveYMat;
			glUniformMatrix4fv(worldLoc, 1, GL_FALSE, glm::value_ptr(finalMat));
			glUniform3f(colorLoc, 1.f, 0.f, 0.f);
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}
		//눈 그리기
		{
			glm::mat4 moveYMat = glm::translate(unitMat, glm::vec3(0.f, 0.6f, 0.f));	// 고정값..
			glm::mat4 scaleMat = glm::scale(unitMat, glm::vec3(0.05f, 0.05f, 0.05f));

			glm::mat4 moveleftPos = glm::translate(unitMat, glm::vec3(-0.08f, 0.65f, 0.38f));
			glm::mat4 moveRightPos = glm::translate(unitMat, glm::vec3(0.08f, 0.65f, 0.38f));

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
	}

}

void Chic::release()
{
}
