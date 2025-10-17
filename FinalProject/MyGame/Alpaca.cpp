#include "Alpaca.h"
#include "PlayerObject.h"
#include <iostream>

Alpaca::Alpaca()
{
	float x = -6.f + static_cast<float>(std::rand()) / (static_cast<float>(RAND_MAX / 4.f));
	float z = -5.f + static_cast<float>(std::rand()) / (static_cast<float>(RAND_MAX / 5.f));

	isBaby = true;

	rangeLimit = 1.f;

	alpacaXDir = (rand() % 2 == 0) ? 1.f : -1.f;
	alpacaZDir = (rand() % 2 == 0) ? 1.f : -1.f;
	moveSpeed = 0.5f + static_cast<float>(std::rand()) / (static_cast<float>(RAND_MAX / 1.f));

	moveAlpaX = x;
	moveAlpaZ = z;

	rotateAlpaLeftLeg = 0.f;
	rotateAlpaRightLeg = 0.f;
	rotateFaceAlpa = 0.f;

	isMaxRotateAlpa = false;

	isNear = false;

	initBuffer(&alpacaVAO, &alpacaVertexCount, "./OBJ/cube.obj");

	rotateSpeed = 0.05f + static_cast<float>(std::rand()) / (static_cast<float>(RAND_MAX / 0.2f));
}

Alpaca::~Alpaca()
{
	//	delete this;
}

void Alpaca::initilize()
{
	//player = new PlayerObject;
}

// 펜스 실 범위
// x : -15.f ~ 0.f
// z : -8.f ~ 10.f

void Alpaca::update(float elapseTime)
{
	glm::vec3 dir(0.f);

	glm::vec3 newPosition = worldTransform[3]; // 현재 위치를 복사
	newPosition += getLook();

	worldTransform[3][0] = moveAlpaX;
	worldTransform[3][2] = moveAlpaZ;


	// 알파카 다리 각도 회전
	if (isMaxRotateAlpa)
	{
		rotateAlpaLeftLeg -= rotateSpeed;
		rotateAlpaRightLeg += rotateSpeed;
		if (rotateAlpaLeftLeg <= -30.f) {
			isMaxRotateAlpa = false;
		}
	}
	else if (not isMaxRotateAlpa) {
		rotateAlpaLeftLeg += rotateSpeed;
		rotateAlpaRightLeg -= rotateSpeed;
		if (rotateAlpaLeftLeg >= 30.f) {
			isMaxRotateAlpa = true;
		}
	}

	if (moveAlpaX > -15.f and moveAlpaX < -1.f
		and moveAlpaZ > -8.f and moveAlpaZ < 9.f) {


		if (alpacaXDir > 0) {
			moveAlpaX += 0.001f;
		}
		if (alpacaXDir < 0) {
			moveAlpaX -= 0.001f;
		}
		if (alpacaZDir > 0) {
			moveAlpaZ += 0.001f;
		}
		if (alpacaZDir < 0) {
			moveAlpaZ -= 0.001f;
		}

		if (moveAlpaX <= -15.f + rangeLimit) {
			alpacaXDir = 1.f;

			std::cout << alpacaXDir << std::endl;
		}
		if (moveAlpaX >= -1.f - rangeLimit) {
			alpacaXDir = -1.f;

			std::cout << alpacaXDir << std::endl;
		}
		if (moveAlpaZ <= -8.f + rangeLimit) {
			alpacaZDir = 1.f;

			std::cout << alpacaZDir << std::endl;
		}
		if (moveAlpaZ >= 9.f - rangeLimit) {
			alpacaZDir = -1.f;

			std::cout << alpacaZDir << std::endl;
		}

		if (alpacaXDir > 0 and alpacaZDir > 0) {
			rotateFaceAlpa = 45.f;
		}
		if (alpacaXDir > 0 and alpacaZDir < 0) {
			rotateFaceAlpa = 135.f;
		}
		if (alpacaXDir < 0 and alpacaZDir > 0) {
			rotateFaceAlpa = -45.f;
		}
		if (alpacaXDir < 0 and alpacaZDir < 0) {
			rotateFaceAlpa = 225.f;
		}
	}

	/*if (glm::length(dir) >= glm::epsilon<float>())
		move(dir, moveSpeed * elapseTime);*/
}

void Alpaca::draw() const
{
	glUseProgram(shader);
	glBindVertexArray(alpacaVAO);

	GLint worldLoc = glGetUniformLocation(shader, "modelTransform");
	GLint colorLoc = glGetUniformLocation(shader, "colorTransform");
	const glm::mat4 unitMat(1.f);

	if (isBaby) {
		GLint worldLoc = glGetUniformLocation(shader, "modelTransform");
		GLint colorLoc = glGetUniformLocation(shader, "colorTransform");
		const glm::mat4 unitMat(1.f);

		float scaleFactor = 0.5f;

		glm::mat4 moveFinal = glm::translate(unitMat, glm::vec3(moveAlpaX, 0.0f, moveAlpaZ));
		glm::mat4 rotateDir = glm::rotate(unitMat, glm::radians(rotateFaceAlpa), glm::vec3(0.f, 1.f, 0.f));

		// 다리 그리기
		{
			glm::mat4 moveYMat = glm::translate(unitMat, glm::vec3(0.f, 0.5f * scaleFactor, 0.f));   // 고정값..
			glm::mat4 scaleMat = glm::scale(unitMat, glm::vec3(0.18f, 0.3f, 0.18f) * scaleFactor);
			glm::mat4 moveleftPos = glm::translate(unitMat, glm::vec3(-0.18f, 0.f, -0.25f) * scaleFactor);
			glm::mat4 moveRightPos = glm::translate(unitMat, glm::vec3(0.18f, 0.f, -0.25f) * scaleFactor);

			glm::mat4 rotateleftleg = glm::rotate(unitMat, glm::radians(rotateAlpaLeftLeg), glm::vec3(1.f, 0.f, 0.f));
			glm::mat4 rotaterightleg = glm::rotate(unitMat, glm::radians(rotateAlpaRightLeg), glm::vec3(1.f, 0.f, 0.f));

			glm::mat4 finalMat;

			// 왼쪽
			finalMat = moveFinal * rotateDir * moveleftPos * glm::translate(unitMat, glm::vec3(-0.0625f, 0.15f, 0.f) * scaleFactor) * rotateleftleg * glm::translate(unitMat, glm::vec3(0.0625f, -0.15f, 0.f) * scaleFactor) * scaleMat * moveYMat;
			glUniformMatrix4fv(worldLoc, 1, GL_FALSE, glm::value_ptr(finalMat));
			glUniform3f(colorLoc, 0.92f, 0.9f, 0.8f);
			glDrawArrays(GL_TRIANGLES, 0, 36);
			// 오른쪽
			finalMat = moveFinal * rotateDir * moveRightPos * glm::translate(unitMat, glm::vec3(0.0625f, 0.15f, 0.f) * scaleFactor) * rotaterightleg * glm::translate(unitMat, glm::vec3(-0.0625f, -0.15f, 0.f) * scaleFactor) * scaleMat * moveYMat;
			glUniformMatrix4fv(worldLoc, 1, GL_FALSE, glm::value_ptr(finalMat));
			glUniform3f(colorLoc, 0.92f, 0.9f, 0.8f);
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}
		// 몸통 그리기
		{
			glm::mat4 moveYMat = glm::translate(unitMat, glm::vec3(0.f, 0.5f, 0.f) * scaleFactor);
			glm::mat4 scaleMat = glm::scale(unitMat, glm::vec3(0.55f, 0.55f, 1.f) * scaleFactor);

			glm::mat4 moveBottomUp = glm::translate(unitMat, glm::vec3(0.f, 0.3f, 0.f) * scaleFactor);

			glm::mat4 finalMat = moveFinal * moveBottomUp * rotateDir * scaleMat * moveYMat;

			glUniformMatrix4fv(worldLoc, 1, GL_FALSE, glm::value_ptr(finalMat));
			glUniform3f(colorLoc, 0.92f, 0.9f, 0.8f);
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}
		{
			// 앞다리
			glm::mat4 moveYMat = glm::translate(unitMat, glm::vec3(0.f, 0.5f, 0.f) * scaleFactor);   // 고정값..
			glm::mat4 scaleMat = glm::scale(unitMat, glm::vec3(0.18f, 0.3f, 0.18f) * scaleFactor);

			glm::mat4 moveleftPos = glm::translate(unitMat, glm::vec3(-0.18f, 0.f, 0.3f) * scaleFactor);
			glm::mat4 moveRightPos = glm::translate(unitMat, glm::vec3(0.18f, 0.f, 0.3f) * scaleFactor);

			glm::mat4 rotateleftleg = glm::rotate(unitMat, glm::radians(rotateAlpaLeftLeg), glm::vec3(1.f, 0.f, 0.f));
			glm::mat4 rotaterightleg = glm::rotate(unitMat, glm::radians(rotateAlpaRightLeg), glm::vec3(1.f, 0.f, 0.f));

			glm::mat4 finalMat;

			// 왼쪽
			finalMat = moveFinal * rotateDir * moveleftPos * glm::translate(unitMat, glm::vec3(-0.0625f, 0.15f, 0.f) * scaleFactor) * rotateleftleg * glm::translate(unitMat, glm::vec3(0.0625f, -0.15f, 0.f) * scaleFactor) * scaleMat * moveYMat;
			glUniformMatrix4fv(worldLoc, 1, GL_FALSE, glm::value_ptr(finalMat));
			glUniform3f(colorLoc, 0.92f, 0.9f, 0.8f);
			glDrawArrays(GL_TRIANGLES, 0, 36);
			// 오른쪽
			finalMat = moveFinal * rotateDir * moveRightPos * glm::translate(unitMat, glm::vec3(0.0625f, 0.15f, 0.f) * scaleFactor) * rotaterightleg * glm::translate(unitMat, glm::vec3(-0.0625f, -0.15f, 0.f) * scaleFactor) * scaleMat * moveYMat;
			glUniformMatrix4fv(worldLoc, 1, GL_FALSE, glm::value_ptr(finalMat));
			glUniform3f(colorLoc, 0.92f, 0.9f, 0.8f);
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}
		// 머리 그리기
		{
			glm::mat4 moveYMat = glm::translate(unitMat, glm::vec3(0.f, 0.5f, 0.f) * scaleFactor);
			glm::mat4 scaleMat = glm::scale(unitMat, glm::vec3(0.45f, 1.f, 0.45f) * scaleFactor);

			glm::mat4 moveBottomUp = glm::translate(unitMat, glm::vec3(0.f, 0.5f, 0.4f) * scaleFactor);

			glm::mat4 finalMat = moveFinal * rotateDir * moveBottomUp * scaleMat * moveYMat;
			glUniformMatrix4fv(worldLoc, 1, GL_FALSE, glm::value_ptr(finalMat));

			glUniform3f(colorLoc, 0.92f, 0.9f, 0.8f);
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}
		// 주둥이 그리기
		{
			glm::mat4 moveYMat = glm::translate(unitMat, glm::vec3(0.f, 0.5f, 0.f) * scaleFactor);
			glm::mat4 scaleMat = glm::scale(unitMat, glm::vec3(0.2f, 0.15f, 0.1f) * scaleFactor);

			glm::mat4 moveBottomUp = glm::translate(unitMat, glm::vec3(0.f, 0.9f, 0.65f) * scaleFactor);

			glm::mat4 finalMat = moveFinal * rotateDir * moveBottomUp * scaleMat * moveYMat;
			glUniformMatrix4fv(worldLoc, 1, GL_FALSE, glm::value_ptr(finalMat));
			glUniform3f(colorLoc, 0.8f, 0.75f, 0.6f);
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}
		// 코 그리기
		{
			glm::mat4 moveYMat = glm::translate(unitMat, glm::vec3(0.f, 0.5f, 0.f) * scaleFactor);
			glm::mat4 scaleMat = glm::scale(unitMat, glm::vec3(0.1f, 0.05f, 0.1f) * scaleFactor);

			glm::mat4 moveBottomUp = glm::translate(unitMat, glm::vec3(0.f, 0.98f, 0.7f) * scaleFactor);

			glm::mat4 finalMat = moveFinal * rotateDir * moveBottomUp * scaleMat * moveYMat;
			glUniformMatrix4fv(worldLoc, 1, GL_FALSE, glm::value_ptr(finalMat));
			glUniform3f(colorLoc, 0.8f, 0.7f, 0.6f);
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}
		// 귀 그리기
		{
			glm::mat4 moveYMat = glm::translate(unitMat, glm::vec3(0.f, 0.5f, 0.f) * scaleFactor);   // 고정값..
			glm::mat4 scaleMat = glm::scale(unitMat, glm::vec3(0.1f, 0.2f, 0.1f) * scaleFactor);

			glm::mat4 moveleftPos = glm::translate(unitMat, glm::vec3(-0.1f, 1.3f, 0.5f) * scaleFactor);
			glm::mat4 moveRightPos = glm::translate(unitMat, glm::vec3(0.1f, 1.3f, 0.5f) * scaleFactor);


			glm::mat4 finalMat;

			// 왼쪽
			finalMat = moveFinal * rotateDir * moveleftPos * scaleMat * moveYMat;
			glUniformMatrix4fv(worldLoc, 1, GL_FALSE, glm::value_ptr(finalMat));
			glUniform3f(colorLoc, 0.92f, 0.9f, 0.8f);
			glDrawArrays(GL_TRIANGLES, 0, 36);
			// 오른쪽
			finalMat = moveFinal * rotateDir * moveRightPos * scaleMat * moveYMat;
			glUniformMatrix4fv(worldLoc, 1, GL_FALSE, glm::value_ptr(finalMat));
			glUniform3f(colorLoc, 0.92f, 0.9f, 0.8f);
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}
		{
			// 눈
			glm::mat4 moveYMat = glm::translate(unitMat, glm::vec3(0.f, 0.5f, 0.f) * scaleFactor);   // 고정값..
			glm::mat4 scaleMat = glm::scale(unitMat, glm::vec3(0.07f, 0.07f, 0.07f) * scaleFactor);

			glm::mat4 moveleftPos = glm::translate(unitMat, glm::vec3(-0.15f, 1.f, 0.6f) * scaleFactor);
			glm::mat4 moveRightPos = glm::translate(unitMat, glm::vec3(0.15f, 1.f, 0.6f) * scaleFactor);

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
	}
	else if (not isBaby) {

		std::cout << " ? " << std::endl;
		glm::mat4 moveFinal = glm::translate(unitMat, glm::vec3(moveAlpaX, 0.0f, moveAlpaZ));
		glm::mat4 rotateDir = glm::rotate(unitMat, glm::radians(rotateFaceAlpa), glm::vec3(0.f, 1.f, 0.f));
		// 다리 그리기
		{
			glm::mat4 moveYMat = glm::translate(unitMat, glm::vec3(0.f, 0.5f, 0.f));	// 고정값..
			glm::mat4 scaleMat = glm::scale(unitMat, glm::vec3(0.18f, 0.3f, 0.18f));

			glm::mat4 moveleftPos = glm::translate(unitMat, glm::vec3(-0.18f, 0.f, -0.25f));
			glm::mat4 moveRightPos = glm::translate(unitMat, glm::vec3(0.18f, 0.f, -0.25f));

			glm::mat4 rotateleftleg = glm::rotate(unitMat, glm::radians(rotateAlpaLeftLeg), glm::vec3(1.f, 0.f, 0.f));
			glm::mat4 rotaterightleg = glm::rotate(unitMat, glm::radians(rotateAlpaRightLeg), glm::vec3(1.f, 0.f, 0.f));

			glm::mat4 finalMat;

			// 왼쪽
			finalMat = moveFinal * rotateDir * moveleftPos * glm::translate(unitMat, glm::vec3(-0.0625f, 0.15f, 0.f)) * rotateleftleg * glm::translate(unitMat, glm::vec3(0.0625f, -0.15f, 0.f)) * scaleMat * moveYMat;
			glUniformMatrix4fv(worldLoc, 1, GL_FALSE, glm::value_ptr(finalMat));
			glUniform3f(colorLoc, 0.92f, 0.9f, 0.8f);
			glDrawArrays(GL_TRIANGLES, 0, 36);
			// 오른쪽
			finalMat = moveFinal * rotateDir * moveRightPos * glm::translate(unitMat, glm::vec3(0.0625f, 0.15f, 0.f)) * rotaterightleg * glm::translate(unitMat, glm::vec3(-0.0625f, -0.15f, 0.f)) * scaleMat * moveYMat;
			glUniformMatrix4fv(worldLoc, 1, GL_FALSE, glm::value_ptr(finalMat));
			glUniform3f(colorLoc, 0.92f, 0.9f, 0.8f);
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}
		// 몸통 그리기
		{
			glm::mat4 moveYMat = glm::translate(unitMat, glm::vec3(0.f, 0.5f, 0.f));
			glm::mat4 scaleMat = glm::scale(unitMat, glm::vec3(0.55f, 0.55f, 1.f));

			glm::mat4 moveBottomUp = glm::translate(unitMat, glm::vec3(0.f, 0.3f, 0.f));

			glm::mat4 finalMat = moveFinal * moveBottomUp * rotateDir * scaleMat * moveYMat;

			glUniformMatrix4fv(worldLoc, 1, GL_FALSE, glm::value_ptr(finalMat));
			glUniform3f(colorLoc, 0.92f, 0.9f, 0.8f);
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}
		{
			// 앞다리
			glm::mat4 moveYMat = glm::translate(unitMat, glm::vec3(0.f, 0.5f, 0.f));	// 고정값..
			glm::mat4 scaleMat = glm::scale(unitMat, glm::vec3(0.18f, 0.3f, 0.18f));

			glm::mat4 moveleftPos = glm::translate(unitMat, glm::vec3(-0.18f, 0.f, 0.3f));
			glm::mat4 moveRightPos = glm::translate(unitMat, glm::vec3(0.18f, 0.f, 0.3f));

			glm::mat4 rotateleftleg = glm::rotate(unitMat, glm::radians(rotateAlpaLeftLeg), glm::vec3(1.f, 0.f, 0.f));
			glm::mat4 rotaterightleg = glm::rotate(unitMat, glm::radians(rotateAlpaRightLeg), glm::vec3(1.f, 0.f, 0.f));

			glm::mat4 finalMat;

			// 왼쪽
			finalMat = moveFinal * rotateDir * moveleftPos * glm::translate(unitMat, glm::vec3(-0.0625f, 0.15f, 0.f)) * rotateleftleg * glm::translate(unitMat, glm::vec3(0.0625f, -0.15f, 0.f)) * scaleMat * moveYMat;
			glUniformMatrix4fv(worldLoc, 1, GL_FALSE, glm::value_ptr(finalMat));
			glUniform3f(colorLoc, 0.92f, 0.9f, 0.8f);
			glDrawArrays(GL_TRIANGLES, 0, 36);
			// 오른쪽
			finalMat = moveFinal * rotateDir * moveRightPos * glm::translate(unitMat, glm::vec3(0.0625f, 0.15f, 0.f)) * rotaterightleg * glm::translate(unitMat, glm::vec3(-0.0625f, -0.15f, 0.f)) * scaleMat * moveYMat;
			glUniformMatrix4fv(worldLoc, 1, GL_FALSE, glm::value_ptr(finalMat));
			glUniform3f(colorLoc, 0.92f, 0.9f, 0.8f);
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}
		// 머리 그리기
		{
			glm::mat4 moveYMat = glm::translate(unitMat, glm::vec3(0.f, 0.5f, 0.f));
			glm::mat4 scaleMat = glm::scale(unitMat, glm::vec3(0.45f, 1.f, 0.45f));

			glm::mat4 moveBottomUp = glm::translate(unitMat, glm::vec3(0.f, 0.5f, 0.4f));

			glm::mat4 finalMat = moveFinal * rotateDir * moveBottomUp * scaleMat * moveYMat;
			glUniformMatrix4fv(worldLoc, 1, GL_FALSE, glm::value_ptr(finalMat));

			glUniform3f(colorLoc, 0.92f, 0.9f, 0.8f);
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}
		// 주둥이 그리기
		{
			glm::mat4 moveYMat = glm::translate(unitMat, glm::vec3(0.f, 0.5f, 0.f));
			glm::mat4 scaleMat = glm::scale(unitMat, glm::vec3(0.2f, 0.15f, 0.1f));

			glm::mat4 moveBottomUp = glm::translate(unitMat, glm::vec3(0.f, 1.05f, 0.65f));

			glm::mat4 finalMat = moveFinal * rotateDir * moveBottomUp * scaleMat * moveYMat;
			glUniformMatrix4fv(worldLoc, 1, GL_FALSE, glm::value_ptr(finalMat));
			glUniform3f(colorLoc, 0.8f, 0.75f, 0.6f);
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}
		// 코 그리기
		{
			glm::mat4 moveYMat = glm::translate(unitMat, glm::vec3(0.f, 0.5f, 0.f));
			glm::mat4 scaleMat = glm::scale(unitMat, glm::vec3(0.1f, 0.05f, 0.1f));

			glm::mat4 moveBottomUp = glm::translate(unitMat, glm::vec3(0.f, 1.15f, 0.7f));

			glm::mat4 finalMat = moveFinal * rotateDir * moveBottomUp * scaleMat * moveYMat;
			glUniformMatrix4fv(worldLoc, 1, GL_FALSE, glm::value_ptr(finalMat));
			glUniform3f(colorLoc, 0.8f, 0.7f, 0.6f);
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}
		// 귀 그리기
		{
			glm::mat4 moveYMat = glm::translate(unitMat, glm::vec3(0.f, 0.5f, 0.f));	// 고정값..
			glm::mat4 scaleMat = glm::scale(unitMat, glm::vec3(0.1f, 0.2f, 0.1f));

			glm::mat4 moveleftPos = glm::translate(unitMat, glm::vec3(-0.1f, 1.5f, 0.5f));
			glm::mat4 moveRightPos = glm::translate(unitMat, glm::vec3(0.1f, 1.5f, 0.5f));


			glm::mat4 finalMat;

			// 왼쪽
			finalMat = moveFinal * rotateDir * moveleftPos * scaleMat * moveYMat;
			glUniformMatrix4fv(worldLoc, 1, GL_FALSE, glm::value_ptr(finalMat));
			glUniform3f(colorLoc, 0.92f, 0.9f, 0.8f);
			glDrawArrays(GL_TRIANGLES, 0, 36);
			// 오른쪽
			finalMat = moveFinal * rotateDir * moveRightPos * scaleMat * moveYMat;
			glUniformMatrix4fv(worldLoc, 1, GL_FALSE, glm::value_ptr(finalMat));
			glUniform3f(colorLoc, 0.92f, 0.9f, 0.8f);
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}
		{
			// 눈
			glm::mat4 moveYMat = glm::translate(unitMat, glm::vec3(0.f, 0.5f, 0.f));	// 고정값..
			glm::mat4 scaleMat = glm::scale(unitMat, glm::vec3(0.07f, 0.07f, 0.07f));

			glm::mat4 moveleftPos = glm::translate(unitMat, glm::vec3(-0.15f, 1.2f, 0.6f));
			glm::mat4 moveRightPos = glm::translate(unitMat, glm::vec3(0.15f, 1.2f, 0.6f));

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
	}
	
	//std::cout << "뭐니.그려지니.?" << std::endl;
}

void Alpaca::release()
{
}

