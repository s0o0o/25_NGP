#include "Fox.h"
#include "PlayerObject.h"
#include <iostream>

Fox::Fox()
{
	float x = -8.f + static_cast<float>(std::rand()) / (static_cast<float>(RAND_MAX / 6.f));
	float z = -3.f + static_cast<float>(std::rand()) / (static_cast<float>(RAND_MAX / 5.f));
	
	isBaby = true;
	rangeLimit = 1.f;

	foxXDir = (rand() % 2 == 0) ? 1.f : -1.f;
	foxZDir = (rand() % 2 == 0) ? 1.f : -1.f;
	moveSpeed = 0.5f + static_cast<float>(std::rand()) / (static_cast<float>(RAND_MAX / 1.f));

	moveFoxX = x;
	moveFoxZ = z;

	rotateFoxLeftLeg = 0.f;
	rotateFoxRightLeg = 0.f;
	rotateFaceFox = 0.f;
	adultY = 0.f;
	adultScale = 1.f;

	/*rotateBabyLeftLeg = 0.f;
	rotateBabyRightLeg = 0.f;
	rotateFaceBaby = 0.f;*/

	isMaxRotateFox = false; // 각도 제한
	/*isMaxRotatePig = false;*/

	initBuffer(&foxVAO, &foxVertexCount, "./OBJ/cube.obj");

	rotateSpeed = 0.05f + static_cast<float>(std::rand()) / (static_cast<float>(RAND_MAX / 0.2f));
}

Fox::~Fox()
{
}

void Fox::initilize()
{
}

void Fox::update(float elapseTime)
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

	worldTransform[3][0] = moveFoxX;
	worldTransform[3][2] = moveFoxZ;

	//여우 다리 각도 회전
	if (isMaxRotateFox)
	{
		rotateFoxLeftLeg -= rotateSpeed;
		rotateFoxRightLeg += rotateSpeed;
		if (rotateFoxLeftLeg <= -30.f) {
			isMaxRotateFox = false;
		}
	}
	else if (not isMaxRotateFox) {
		rotateFoxLeftLeg += rotateSpeed;
		rotateFoxRightLeg -= rotateSpeed;
		if (rotateFoxLeftLeg >= 30.f) {
			isMaxRotateFox = true;
		}
	}

	if (moveFoxX > -15.f and moveFoxX < -1.f
		and moveFoxZ > -8.f and moveFoxZ < 9.f) {


		if (foxXDir > 0) {
			moveFoxX += 0.001f;
		}
		if (foxXDir < 0) {
			moveFoxX -= 0.001f;
		}
		if (foxZDir > 0) {
			moveFoxZ += 0.001f;
		}
		if (foxZDir < 0) {
			moveFoxZ -= 0.001f;
		}

		if (moveFoxX <= -15.f + rangeLimit) {
			foxXDir = 1.f;

			std::cout << foxXDir << std::endl;
		}
		if (moveFoxX >= -1.f - rangeLimit) {
			foxXDir = -1.f;

			std::cout << foxXDir << std::endl;
		}
		if (moveFoxZ <= -8.f + rangeLimit) {
			foxZDir = 1.f;

			std::cout << foxZDir << std::endl;
		}
		if (moveFoxZ >= 9.f - rangeLimit) {
			foxZDir = -1.f;

			std::cout << foxZDir << std::endl;
		}

		if (foxXDir > 0 and foxZDir > 0) {
			rotateFaceFox = 45.f;
		}
		if (foxXDir > 0 and foxZDir < 0) {
			rotateFaceFox = 135.f;
		}
		if (foxXDir < 0 and foxZDir > 0) {
			rotateFaceFox = -45.f;
		}
		if (foxXDir < 0 and foxZDir < 0) {
			rotateFaceFox = 225.f;
		}
	}

	if (not isBaby) {
		adultY = 0.2f;
		adultScale = 2.f;
	}
}

void Fox::draw() const
{
	glUseProgram(shader);
	glBindVertexArray(foxVAO);

	GLint worldLoc = glGetUniformLocation(shader, "modelTransform");
	GLint colorLoc = glGetUniformLocation(shader, "colorTransform");

	if (not isBaby) {


		// 아기 여우
		const glm::mat4 unitMat(1.f);
		glm::mat4 moveFinal = glm::translate(unitMat, glm::vec3(moveFoxX, 0.0f + adultY, moveFoxZ));
		glm::mat4 rotateDir = glm::rotate(unitMat, glm::radians(rotateFaceFox), glm::vec3(0.f, 1.f, 0.f));
		//glm::mat4 scaleAllMat = glm::scale(unitMat, glm::vec3(adultScale));
		//다리 그리기
		{
			glm::mat4 moveYMat = glm::translate(unitMat, glm::vec3(0.f, 0.5f, 0.f));	// 고정값..
			glm::mat4 scaleMat = glm::scale(unitMat, glm::vec3(0.13f, 0.25f, 0.13f));
			glm::mat4 moveleftPos = glm::translate(unitMat, glm::vec3(-0.15f, 0.f, -0.25f));
			glm::mat4 moveRightPos = glm::translate(unitMat, glm::vec3(0.15f, 0.f, -0.25f));

			glm::mat4 rotateleftleg = glm::rotate(unitMat, glm::radians(rotateFoxLeftLeg), glm::vec3(1.f, 0.f, 0.f));
			glm::mat4 rotaterightleg = glm::rotate(unitMat, glm::radians(rotateFoxRightLeg), glm::vec3(1.f, 0.f, 0.f));

			//scaleMat = scaleMat * scaleAllMat;

			glm::mat4 finalMat;

			//왼쪽
			finalMat = moveFinal * rotateDir * moveleftPos * glm::translate(unitMat, glm::vec3(-0.0625f, 0.15f, 0.f)) * rotateleftleg * glm::translate(unitMat, glm::vec3(0.0625f, -0.15f, 0.f)) * scaleMat * moveYMat;
			glUniformMatrix4fv(worldLoc, 1, GL_FALSE, glm::value_ptr(finalMat));
			glUniform3f(colorLoc, 0.f, 0.f, 0.f);
			glDrawArrays(GL_TRIANGLES, 0, 36);

			//오른쪽
			finalMat = moveFinal * rotateDir * moveRightPos * glm::translate(unitMat, glm::vec3(0.0625f, 0.15f, 0.f)) * rotaterightleg * glm::translate(unitMat, glm::vec3(-0.0625f, -0.15f, 0.f)) * scaleMat * moveYMat;
			glUniformMatrix4fv(worldLoc, 1, GL_FALSE, glm::value_ptr(finalMat));
			glUniform3f(colorLoc, 0.f, 0.f, 0.f);
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}
		//몸통 그리기
		{
			glm::mat4 moveYMat = glm::translate(unitMat, glm::vec3(0.f, 0.5f, 0.f));
			glm::mat4 scaleMat = glm::scale(unitMat, glm::vec3(0.55f, 0.55f, 0.8f));
			glm::mat4 moveBottomUp = glm::translate(unitMat, glm::vec3(0.f, 0.25f, 0.f));

			glm::mat4 finalMat = moveFinal * rotateDir * moveBottomUp * scaleMat * moveYMat;

			glUniformMatrix4fv(worldLoc, 1, GL_FALSE, glm::value_ptr(finalMat));
			glUniform3f(colorLoc, 0.9f, 0.7f, 0.2f);
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}
		{
			//앞다리
			glm::mat4 moveYMat = glm::translate(unitMat, glm::vec3(0.f, 0.5f, 0.f));	// 고정값..
			glm::mat4 scaleMat = glm::scale(unitMat, glm::vec3(0.13f, 0.25f, 0.13f));
			glm::mat4 moveleftPos = glm::translate(unitMat, glm::vec3(-0.15f, 0.f, 0.3f));
			glm::mat4 moveRightPos = glm::translate(unitMat, glm::vec3(0.15f, 0.f, 0.3f));

			glm::mat4 rotateleftleg = glm::rotate(unitMat, glm::radians(rotateFoxLeftLeg), glm::vec3(1.f, 0.f, 0.f));
			glm::mat4 rotaterightleg = glm::rotate(unitMat, glm::radians(rotateFoxRightLeg), glm::vec3(1.f, 0.f, 0.f));

			glm::mat4 finalMat;

			//왼쪽
			finalMat = moveFinal * rotateDir * moveleftPos * glm::translate(unitMat, glm::vec3(-0.0625f, 0.15f, 0.f)) * rotateleftleg * glm::translate(unitMat, glm::vec3(0.0625f, -0.15f, 0.f)) * scaleMat * moveYMat;
			glUniformMatrix4fv(worldLoc, 1, GL_FALSE, glm::value_ptr(finalMat));
			glUniform3f(colorLoc, 0.f, 0.f, 0.f);
			glDrawArrays(GL_TRIANGLES, 0, 36);
			//오른쪽
			finalMat = moveFinal * rotateDir * moveRightPos * glm::translate(unitMat, glm::vec3(0.0625f, 0.15f, 0.f)) * rotaterightleg * glm::translate(unitMat, glm::vec3(-0.0625f, -0.15f, 0.f)) * scaleMat * moveYMat;
			glUniformMatrix4fv(worldLoc, 1, GL_FALSE, glm::value_ptr(finalMat));
			glUniform3f(colorLoc, 0.f, 0.f, 0.f);
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}
		//머리 그리기
		{
			glm::mat4 moveYMat = glm::translate(unitMat, glm::vec3(0.f, 0.5f, 0.f));
			glm::mat4 scaleMat = glm::scale(unitMat, glm::vec3(0.45f, 0.45f, 0.45f));
			glm::mat4 moveBottomUp = glm::translate(unitMat, glm::vec3(0.f, 0.4f, 0.5f));

			glm::mat4 finalMat = moveFinal * rotateDir * moveBottomUp * scaleMat * moveYMat;
			glUniformMatrix4fv(worldLoc, 1, GL_FALSE, glm::value_ptr(finalMat));
			glUniform3f(colorLoc, 0.9f, 0.7f, 0.2f);
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}
		//주둥이 그리기
		{
			glm::mat4 moveYMat = glm::translate(unitMat, glm::vec3(0.f, 0.5f, 0.f));
			glm::mat4 scaleMat = glm::scale(unitMat, glm::vec3(0.2f, 0.1f, 0.1f));
			glm::mat4 moveBottomUp = glm::translate(unitMat, glm::vec3(0.f, 0.5f, 0.75f));

			glm::mat4 finalMat = moveFinal * rotateDir * moveBottomUp * scaleMat * moveYMat;
			glUniformMatrix4fv(worldLoc, 1, GL_FALSE, glm::value_ptr(finalMat));
			glUniform3f(colorLoc, 0.97f, 0.8f, 0.5f);
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}
		//코
		{
			glm::mat4 moveYMat = glm::translate(unitMat, glm::vec3(0.f, 0.5f, 0.f));	// 고정값..
			glm::mat4 scaleMat = glm::scale(unitMat, glm::vec3(0.1f, 0.05f, 0.05f));
			glm::mat4 moveleftPos = glm::translate(unitMat, glm::vec3(0.f, 0.55f, 0.8f));

			glm::mat4 finalMat;

			finalMat = moveFinal * rotateDir * moveleftPos * scaleMat * moveYMat;
			glUniformMatrix4fv(worldLoc, 1, GL_FALSE, glm::value_ptr(finalMat));
			glUniform3f(colorLoc, 0.f, 0.f, 0.f);
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}
		//눈 그리기
		{

			glm::mat4 moveYMat = glm::translate(unitMat, glm::vec3(0.f, 0.5f, 0.f));	// 고정값..
			glm::mat4 scaleMat = glm::scale(unitMat, glm::vec3(0.08f, 0.06f, 0.1f));
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
			glm::mat4 scaleMat = glm::scale(unitMat, glm::vec3(0.05f, 0.06f, 0.1f));
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
		//귀 그리기
		{

			glm::mat4 moveYMat = glm::translate(unitMat, glm::vec3(0.f, 0.5f, 0.f));	// 고정값..
			glm::mat4 scaleMat = glm::scale(unitMat, glm::vec3(0.08f, 0.09f, 0.1f));
			glm::mat4 moveleftPos = glm::translate(unitMat, glm::vec3(-0.15f, 0.85f, 0.6f));
			glm::mat4 moveRightPos = glm::translate(unitMat, glm::vec3(0.15f, 0.85f, 0.6f));

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
	else if (isBaby) {
		//아기여우
		const glm::mat4 unitMat(1.f);
		glm::mat4 moveFinal = glm::translate(unitMat, glm::vec3(moveFoxX, 0.0f + adultY, moveFoxZ));
		glm::mat4 rotateDir = glm::rotate(unitMat, glm::radians(rotateFaceFox), glm::vec3(0.f, 1.f, 0.f));
		
		float scaleFactor = 0.5f;
		//다리 그리기
		{
			glm::mat4 moveYMat = glm::translate(unitMat, glm::vec3(0.f, 0.5f, 0.f) * scaleFactor);   // 고정값..
			glm::mat4 scaleMat = glm::scale(unitMat, glm::vec3(0.13f, 0.25f, 0.13f) * scaleFactor);
			glm::mat4 moveleftPos = glm::translate(unitMat, glm::vec3(-0.15f, 0.f, -0.25f) * scaleFactor);
			glm::mat4 moveRightPos = glm::translate(unitMat, glm::vec3(0.15f, 0.f, -0.25f) * scaleFactor);

			glm::mat4 rotateleftleg = glm::rotate(unitMat, glm::radians(rotateFoxLeftLeg), glm::vec3(1.f, 0.f, 0.f));
			glm::mat4 rotaterightleg = glm::rotate(unitMat, glm::radians(rotateFoxRightLeg), glm::vec3(1.f, 0.f, 0.f));

			//scaleMat = scaleMat * scaleAllMat;

			glm::mat4 finalMat;

			//왼쪽
			finalMat = moveFinal * rotateDir * moveleftPos * glm::translate(unitMat, glm::vec3(-0.0625f, 0.15f, 0.f) * scaleFactor) * rotateleftleg * glm::translate(unitMat, glm::vec3(0.0625f, -0.15f, 0.f) * scaleFactor) * scaleMat * moveYMat;
			glUniformMatrix4fv(worldLoc, 1, GL_FALSE, glm::value_ptr(finalMat));
			glUniform3f(colorLoc, 0.f, 0.f, 0.f);
			glDrawArrays(GL_TRIANGLES, 0, 36);

			//오른쪽
			finalMat = moveFinal * rotateDir * moveRightPos * glm::translate(unitMat, glm::vec3(0.0625f, 0.15f, 0.f) * scaleFactor) * rotaterightleg * glm::translate(unitMat, glm::vec3(-0.0625f, -0.15f, 0.f) * scaleFactor) * scaleMat * moveYMat;
			glUniformMatrix4fv(worldLoc, 1, GL_FALSE, glm::value_ptr(finalMat));
			glUniform3f(colorLoc, 0.f, 0.f, 0.f);
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}
		//몸통 그리기
		{
			glm::mat4 moveYMat = glm::translate(unitMat, glm::vec3(0.f, 0.5f, 0.f) * scaleFactor);
			glm::mat4 scaleMat = glm::scale(unitMat, glm::vec3(0.55f, 0.55f, 0.8f) * scaleFactor);
			glm::mat4 moveBottomUp = glm::translate(unitMat, glm::vec3(0.f, 0.25f, 0.f) * scaleFactor);

			glm::mat4 finalMat = moveFinal * rotateDir * moveBottomUp * scaleMat * moveYMat;

			glUniformMatrix4fv(worldLoc, 1, GL_FALSE, glm::value_ptr(finalMat));
			glUniform3f(colorLoc, 0.9f, 0.7f, 0.2f);
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}
		{
			//앞다리
			glm::mat4 moveYMat = glm::translate(unitMat, glm::vec3(0.f, 0.5f, 0.f) * scaleFactor);   // 고정값..
			glm::mat4 scaleMat = glm::scale(unitMat, glm::vec3(0.13f, 0.25f, 0.13f) * scaleFactor);
			glm::mat4 moveleftPos = glm::translate(unitMat, glm::vec3(-0.15f, 0.f, 0.3f) * scaleFactor);
			glm::mat4 moveRightPos = glm::translate(unitMat, glm::vec3(0.15f, 0.f, 0.3f) * scaleFactor);

			glm::mat4 rotateleftleg = glm::rotate(unitMat, glm::radians(rotateFoxLeftLeg), glm::vec3(1.f, 0.f, 0.f));
			glm::mat4 rotaterightleg = glm::rotate(unitMat, glm::radians(rotateFoxRightLeg), glm::vec3(1.f, 0.f, 0.f));

			glm::mat4 finalMat;

			//왼쪽
			finalMat = moveFinal * rotateDir * moveleftPos * glm::translate(unitMat, glm::vec3(-0.0625f, 0.15f, 0.f) * scaleFactor) * rotateleftleg * glm::translate(unitMat, glm::vec3(0.0625f, -0.15f, 0.f) * scaleFactor) * scaleMat * moveYMat;
			glUniformMatrix4fv(worldLoc, 1, GL_FALSE, glm::value_ptr(finalMat));
			glUniform3f(colorLoc, 0.f, 0.f, 0.f);
			glDrawArrays(GL_TRIANGLES, 0, 36);
			//오른쪽
			finalMat = moveFinal * rotateDir * moveRightPos * glm::translate(unitMat, glm::vec3(0.0625f, 0.15f, 0.f) * scaleFactor) * rotaterightleg * glm::translate(unitMat, glm::vec3(-0.0625f, -0.15f, 0.f) * scaleFactor) * scaleMat * moveYMat;
			glUniformMatrix4fv(worldLoc, 1, GL_FALSE, glm::value_ptr(finalMat));
			glUniform3f(colorLoc, 0.f, 0.f, 0.f);
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}
		//머리 그리기
		{
			glm::mat4 moveYMat = glm::translate(unitMat, glm::vec3(0.f, 0.5f, 0.f) * scaleFactor);
			glm::mat4 scaleMat = glm::scale(unitMat, glm::vec3(0.45f, 0.45f, 0.45f) * scaleFactor);
			glm::mat4 moveBottomUp = glm::translate(unitMat, glm::vec3(0.f, 0.4f, 0.5f) * scaleFactor);

			glm::mat4 finalMat = moveFinal * rotateDir * moveBottomUp * scaleMat * moveYMat;
			glUniformMatrix4fv(worldLoc, 1, GL_FALSE, glm::value_ptr(finalMat));
			glUniform3f(colorLoc, 0.9f, 0.7f, 0.2f);
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}
		//주둥이 그리기
		{
			glm::mat4 moveYMat = glm::translate(unitMat, glm::vec3(0.f, 0.5f, 0.f) * scaleFactor);
			glm::mat4 scaleMat = glm::scale(unitMat, glm::vec3(0.2f, 0.1f, 0.1f) * scaleFactor);
			glm::mat4 moveBottomUp = glm::translate(unitMat, glm::vec3(0.f, 0.5f, 0.75f) * scaleFactor);

			glm::mat4 finalMat = moveFinal * rotateDir * moveBottomUp * scaleMat * moveYMat;
			glUniformMatrix4fv(worldLoc, 1, GL_FALSE, glm::value_ptr(finalMat));
			glUniform3f(colorLoc, 0.97f, 0.8f, 0.5f);
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}
		//코
		{
			glm::mat4 moveYMat = glm::translate(unitMat, glm::vec3(0.f, 0.5f, 0.f) * scaleFactor);   // 고정값..
			glm::mat4 scaleMat = glm::scale(unitMat, glm::vec3(0.1f, 0.05f, 0.05f) * scaleFactor);
			glm::mat4 moveleftPos = glm::translate(unitMat, glm::vec3(0.f, 0.55f, 0.8f) * scaleFactor);

			glm::mat4 finalMat;

			finalMat = moveFinal * rotateDir * moveleftPos * scaleMat * moveYMat;
			glUniformMatrix4fv(worldLoc, 1, GL_FALSE, glm::value_ptr(finalMat));
			glUniform3f(colorLoc, 0.f, 0.f, 0.f);
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}
		//눈 그리기
		{

			glm::mat4 moveYMat = glm::translate(unitMat, glm::vec3(0.f, 0.5f, 0.f) * scaleFactor);   // 고정값..
			glm::mat4 scaleMat = glm::scale(unitMat, glm::vec3(0.08f, 0.06f, 0.1f) * scaleFactor);
			glm::mat4 moveleftPos = glm::translate(unitMat, glm::vec3(-0.15f, 0.6f, 0.75f) * scaleFactor);
			glm::mat4 moveRightPos = glm::translate(unitMat, glm::vec3(0.15f, 0.6f, 0.75f) * scaleFactor);

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

			glm::mat4 moveYMat = glm::translate(unitMat, glm::vec3(0.f, 0.5f, 0.f) * scaleFactor);   // 고정값..
			glm::mat4 scaleMat = glm::scale(unitMat, glm::vec3(0.05f, 0.06f, 0.1f) * scaleFactor);
			glm::mat4 moveleftPos = glm::translate(unitMat, glm::vec3(-0.2f, 0.6f, 0.75f) * scaleFactor);
			glm::mat4 moveRightPos = glm::translate(unitMat, glm::vec3(0.2f, 0.6f, 0.75f) * scaleFactor);

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
		//귀 그리기
		{

			glm::mat4 moveYMat = glm::translate(unitMat, glm::vec3(0.f, 0.5f, 0.f) * scaleFactor);   // 고정값..
			glm::mat4 scaleMat = glm::scale(unitMat, glm::vec3(0.08f, 0.09f, 0.1f) * scaleFactor);
			glm::mat4 moveleftPos = glm::translate(unitMat, glm::vec3(-0.15f, 0.8f, 0.6f) * scaleFactor);
			glm::mat4 moveRightPos = glm::translate(unitMat, glm::vec3(0.15f, 0.8f, 0.6f) * scaleFactor);

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

void Fox::release()
{
}
