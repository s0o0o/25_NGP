#include "Penguin.h"
#include <iostream>

Penguin::Penguin()
{

	float x = -5.f + static_cast<float>(std::rand()) / (static_cast<float>(RAND_MAX / 4.f));
	float z = -5.f + static_cast<float>(std::rand()) / (static_cast<float>(RAND_MAX / 10.f));

	isBaby = true;

	rangeLimit = 1.f;

	XDir = (rand() % 2 == 0) ? 1.f : -1.f;
	ZDir = (rand() % 2 == 0) ? 1.f : -1.f;
	moveSpeed = 0.5f + static_cast<float>(std::rand()) / (static_cast<float>(RAND_MAX / 1.f));

	movePenguinX = x;
	movePenguinZ = z;

	rotatePenguinLeftArm = -25.f;
	rotatePenguinRightArm = 25.f;
	rotateFacePenguin = 0.f;

	isMaxRotatePenguin = false;

	isNear = false;

	initBuffer(&VAO, &penguinVertexCount, "./OBJ/cube.obj");

	rotateSpeed = 0.05f + static_cast<float>(std::rand()) / (static_cast<float>(RAND_MAX / 0.1f));
}

Penguin::~Penguin()
{
}

void Penguin::initilize()
{

}

void Penguin::update(float elapseTime)
{
	glm::vec3 dir(0.f);

	glm::vec3 newPosition = worldTransform[3]; // ÇöÀç À§Ä¡¸¦ º¹»ç
	newPosition += getLook();
	worldTransform[3][0] = movePenguinX;
	worldTransform[3][2] = movePenguinZ;



	// Æë±Ï ³¯°³ °¢µµ È¸Àü
	if (isMaxRotatePenguin)
	{
		rotatePenguinLeftArm += rotateSpeed;
		rotatePenguinRightArm -= rotateSpeed;
		if (rotatePenguinLeftArm >= 0.f) {
			isMaxRotatePenguin = false;
		}
	}
	else if (not isMaxRotatePenguin) {
		rotatePenguinLeftArm -= rotateSpeed;
		rotatePenguinRightArm += rotateSpeed;
		if (rotatePenguinLeftArm <= -25.f) {
			isMaxRotatePenguin = true;
		}
	}

	if (movePenguinX > -15.f and movePenguinX < -1.f
		and movePenguinZ > -8.f and movePenguinZ < 9.f) {


		if (XDir > 0) {
			movePenguinX += 0.001f;
		}
		if (XDir < 0) {
			movePenguinX -= 0.001f;
		}
		if (ZDir > 0) {
			movePenguinZ += 0.001f;
		}
		if (ZDir < 0) {
			movePenguinZ -= 0.001f;
		}

		if (movePenguinX <= -15.f + rangeLimit) {
			XDir = 1.f;

			std::cout << XDir << std::endl;
		}
		if (movePenguinX >= -1.f - rangeLimit) {
			XDir = -1.f;

			std::cout << XDir << std::endl;
		}
		if (movePenguinZ <= -8.f + rangeLimit) {
			ZDir = 1.f;

			std::cout << ZDir << std::endl;
		}
		if (movePenguinZ >= 9.f - rangeLimit) {
			ZDir = -1.f;

			std::cout << ZDir << std::endl;
		}

		if (XDir > 0 and ZDir > 0) {
			rotateFacePenguin = 45.f;
		}
		if (XDir > 0 and ZDir < 0) {
			rotateFacePenguin = 135.f;
		}
		if (XDir < 0 and ZDir > 0) {
			rotateFacePenguin = -45.f;
		}
		if (XDir < 0 and ZDir < 0) {
			rotateFacePenguin = 225.f;
		}
	}

}

void Penguin::draw() const
{
	glUseProgram(shader);
	glBindVertexArray(VAO);

	GLint worldLoc = glGetUniformLocation(shader, "modelTransform");
	GLint colorLoc = glGetUniformLocation(shader, "colorTransform");

	if (not isBaby) {
		// Æë±Ï
	// Æë±Ï
		const glm::mat4 unitMat(1.f);
		glm::mat4 moveFinal = glm::translate(unitMat, glm::vec3(movePenguinX, 0.0f, movePenguinZ));
		glm::mat4 rotateDir = glm::rotate(unitMat, glm::radians(rotateFacePenguin), glm::vec3(0.f, 1.f, 0.f));
		float scaleFactor_Big = 1.3f;
		// ¹ß ±×¸®±â
		{
			glm::mat4 moveYMat = glm::translate(unitMat, glm::vec3(0.f, 0.5f, 0.f) * scaleFactor_Big);   // °íÁ¤°ª..
			glm::mat4 scaleMat = glm::scale(unitMat, glm::vec3(0.1f, 0.05f, 0.3f) * scaleFactor_Big);

			glm::mat4 moveleftPos = glm::translate(unitMat, glm::vec3(-0.12f, 0.02f, 0.1f) * scaleFactor_Big);
			glm::mat4 moveRightPos = glm::translate(unitMat, glm::vec3(0.12f, 0.02f, 0.1f) * scaleFactor_Big);

			glm::mat4 finalMat;

			// ¿ÞÂÊ
			finalMat = moveFinal * rotateDir * moveleftPos * scaleMat * moveYMat;
			glUniformMatrix4fv(worldLoc, 1, GL_FALSE, glm::value_ptr(finalMat));
			glUniform3f(colorLoc, 1.f, 1.f, 0.3f);
			glDrawArrays(GL_TRIANGLES, 0, 36);
			// ¿À¸¥ÂÊ
			finalMat = moveFinal * rotateDir * moveRightPos * scaleMat * moveYMat;
			glUniformMatrix4fv(worldLoc, 1, GL_FALSE, glm::value_ptr(finalMat));
			glUniform3f(colorLoc, 1.f, 1.f, 0.3f);
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}
		// ÆÈ ±×¸®±â
		{
			glm::mat4 moveYMat = glm::translate(unitMat, glm::vec3(0.f, 0.5f, 0.f) * scaleFactor_Big);
			glm::mat4 scaleMat = glm::scale(unitMat, glm::vec3(0.0625f, 0.5f, 0.2f) * scaleFactor_Big); // ÆÈ Å©±â

			// ¾î±ú À§Ä¡ ¼³Á¤ (¸öÅëÀÇ À­ºÎºÐ ¾ç¿·)
			glm::mat4 moveleftPos = glm::translate(unitMat, glm::vec3(-0.2f, 0.2f, 0.f) * scaleFactor_Big); // ¿ÞÂÊ ¾î±ú
			glm::mat4 moveRightPos = glm::translate(unitMat, glm::vec3(0.2f, 0.2f, 0.f) * scaleFactor_Big); // ¿À¸¥ÂÊ ¾î±ú

			glm::mat4 rotateleftarm = glm::rotate(unitMat, glm::radians(rotatePenguinLeftArm), glm::vec3(0.f, 0.f, 1.f));
			glm::mat4 rotaterightarm = glm::rotate(unitMat, glm::radians(rotatePenguinRightArm), glm::vec3(0.f, 0.f, 1.f));

			glm::mat4 finalMat;

			// ¿ÞÂÊ ÆÈ
			finalMat = moveFinal * rotateDir * moveleftPos * glm::translate(unitMat, glm::vec3(-0.15f, 0.4f, 0.f) * scaleFactor_Big) * rotateleftarm * glm::translate(unitMat, glm::vec3(0.15f, -0.4f, 0.f) * scaleFactor_Big) * scaleMat * moveYMat;
			glUniformMatrix4fv(worldLoc, 1, GL_FALSE, glm::value_ptr(finalMat));
			glUniform3f(colorLoc, 0.f, 0.f, 0.36f);
			glDrawArrays(GL_TRIANGLES, 0, 36);

			// ¿À¸¥ÂÊ ÆÈ
			finalMat = moveFinal * rotateDir * moveRightPos * glm::translate(unitMat, glm::vec3(0.15f, 0.4f, 0.f) * scaleFactor_Big) * rotaterightarm * glm::translate(unitMat, glm::vec3(-0.15f, -0.4f, 0.f) * scaleFactor_Big) * scaleMat * moveYMat;
			glUniformMatrix4fv(worldLoc, 1, GL_FALSE, glm::value_ptr(finalMat));
			glUniform3f(colorLoc, 0.f, 0.f, 0.36f);
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}
		// ¸öÅë ±×¸®±â
		{
			glm::mat4 moveYMat = glm::translate(unitMat, glm::vec3(0.f, 0.5f, 0.f) * scaleFactor_Big);
			glm::mat4 scaleMat = glm::scale(unitMat, glm::vec3(0.4f, 0.65f, 0.4f) * scaleFactor_Big);

			glm::mat4 moveBottomUp = glm::translate(unitMat, glm::vec3(0.f, 0.0f, 0.f) * scaleFactor_Big);

			glm::mat4 finalMat = moveFinal * rotateDir * moveBottomUp * scaleMat * moveYMat;

			glUniformMatrix4fv(worldLoc, 1, GL_FALSE, glm::value_ptr(finalMat));
			glUniform3f(colorLoc, 0.f, 0.f, 0.36f);
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}
		// ¹è ±×¸®±â
		{
			glm::mat4 moveYMat = glm::translate(unitMat, glm::vec3(0.f, 0.5f, 0.f) * scaleFactor_Big);
			glm::mat4 scaleMat = glm::scale(unitMat, glm::vec3(0.3f, 0.5f, 0.1f) * scaleFactor_Big);

			glm::mat4 moveBottomUp = glm::translate(unitMat, glm::vec3(0.f, 0.09f, 0.18f) * scaleFactor_Big);

			glm::mat4 finalMat = moveFinal * rotateDir * moveBottomUp * scaleMat * moveYMat;

			glUniformMatrix4fv(worldLoc, 1, GL_FALSE, glm::value_ptr(finalMat));
			glUniform3f(colorLoc, 1.f, 1.f, 1.f);
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}
		// ¸Ó¸® ±×¸®±â
		{
			glm::mat4 moveYMat = glm::translate(unitMat, glm::vec3(0.f, 0.5f, 0.f) * scaleFactor_Big);
			glm::mat4 scaleMat = glm::scale(unitMat, glm::vec3(0.3f, 0.3f, 0.3f) * scaleFactor_Big);

			glm::mat4 moveBottomUp = glm::translate(unitMat, glm::vec3(0.f, 0.7f, 0.f) * scaleFactor_Big);

			glm::mat4 finalMat = moveFinal * rotateDir * moveBottomUp * scaleMat * moveYMat;
			glUniformMatrix4fv(worldLoc, 1, GL_FALSE, glm::value_ptr(finalMat));

			glUniform3f(colorLoc, 0.f, 0.f, 0.36f);
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}
		// ºÎ¸® ±×¸®±â
		{
			glm::mat4 moveYMat = glm::translate(unitMat, glm::vec3(0.f, 0.5f, 0.f) * scaleFactor_Big);
			glm::mat4 scaleMat = glm::scale(unitMat, glm::vec3(0.1f, 0.05f, 0.1f) * scaleFactor_Big);

			glm::mat4 moveBottomUp = glm::translate(unitMat, glm::vec3(0.f, 0.8f, 0.2f) * scaleFactor_Big);

			glm::mat4 finalMat = moveFinal * rotateDir * moveBottomUp * scaleMat * moveYMat;
			glUniformMatrix4fv(worldLoc, 1, GL_FALSE, glm::value_ptr(finalMat));
			glUniform3f(colorLoc, 1.f, 0.4f, 0.2f);
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}
		// ´« ±×¸®±â
		{
			glm::mat4 moveYMat = glm::translate(unitMat, glm::vec3(0.f, 0.6f, 0.f) * scaleFactor_Big);   // °íÁ¤°ª..
			glm::mat4 scaleMat = glm::scale(unitMat, glm::vec3(0.05f, 0.05f, 0.05f) * scaleFactor_Big);

			glm::mat4 moveleftPos = glm::translate(unitMat, glm::vec3(-0.07f, 0.85f, 0.15f) * scaleFactor_Big);
			glm::mat4 moveRightPos = glm::translate(unitMat, glm::vec3(0.07f, 0.85f, 0.15f) * scaleFactor_Big);

			glm::mat4 finalMat;

			// ¿ÞÂÊ
			finalMat = moveFinal * rotateDir * moveleftPos * scaleMat * moveYMat;
			glUniformMatrix4fv(worldLoc, 1, GL_FALSE, glm::value_ptr(finalMat));
			glUniform3f(colorLoc, 0.5f, 0.5f, 0.5f);
			glDrawArrays(GL_TRIANGLES, 0, 36);
			// ¿À¸¥ÂÊ
			finalMat = moveFinal * rotateDir * moveRightPos * scaleMat * moveYMat;
			glUniformMatrix4fv(worldLoc, 1, GL_FALSE, glm::value_ptr(finalMat));
			glUniform3f(colorLoc, 0.5f, 0.5f, 0.5f);
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}
	}
	else if (isBaby) {
		// ¾Æ±âÆë±Ï
		const glm::mat4 unitMat(1.f);
		glm::mat4 moveFinal = glm::translate(unitMat, glm::vec3(movePenguinX, 0.0f, movePenguinZ));
		glm::mat4 rotateDir = glm::rotate(unitMat, glm::radians(rotateFacePenguin), glm::vec3(0.f, 1.f, 0.f));
		float scaleFactor = 0.8f;
		// ¹ß ±×¸®±â
		{
			glm::mat4 moveYMat = glm::translate(unitMat, glm::vec3(0.f, 0.5f, 0.f) * scaleFactor);   // °íÁ¤°ª..
			glm::mat4 scaleMat = glm::scale(unitMat, glm::vec3(0.1f, 0.05f, 0.3f) * scaleFactor);

			glm::mat4 moveleftPos = glm::translate(unitMat, glm::vec3(-0.12f, 0.f, 0.1f) * scaleFactor);
			glm::mat4 moveRightPos = glm::translate(unitMat, glm::vec3(0.12f, 0.f, 0.1f) * scaleFactor);;

			glm::mat4 finalMat;

			// ¿ÞÂÊ
			finalMat = moveFinal * rotateDir * moveleftPos * scaleMat * moveYMat;
			glUniformMatrix4fv(worldLoc, 1, GL_FALSE, glm::value_ptr(finalMat));
			glUniform3f(colorLoc, 1.f, 1.f, 0.3f);
			glDrawArrays(GL_TRIANGLES, 0, 36);
			// ¿À¸¥ÂÊ
			finalMat = moveFinal * rotateDir * moveRightPos * scaleMat * moveYMat;
			glUniformMatrix4fv(worldLoc, 1, GL_FALSE, glm::value_ptr(finalMat));
			glUniform3f(colorLoc, 1.f, 1.f, 0.3f);
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}
		// ÆÈ ±×¸®±â
		{
			glm::mat4 moveYMat = glm::translate(unitMat, glm::vec3(0.f, 0.5f, 0.f) * scaleFactor);
			glm::mat4 scaleMat = glm::scale(unitMat, glm::vec3(0.0625f, 0.5f, 0.2f) * scaleFactor); // ÆÈ Å©±â

			// ¾î±ú À§Ä¡ ¼³Á¤ (¸öÅëÀÇ À­ºÎºÐ ¾ç¿·)
			glm::mat4 moveleftPos = glm::translate(unitMat, glm::vec3(-0.2f, 0.2f, 0.f) * scaleFactor); // ¿ÞÂÊ ¾î±ú
			glm::mat4 moveRightPos = glm::translate(unitMat, glm::vec3(0.2f, 0.2f, 0.f) * scaleFactor); // ¿À¸¥ÂÊ ¾î±ú

			glm::mat4 rotateleftarm = glm::rotate(unitMat, glm::radians(rotatePenguinLeftArm), glm::vec3(0.f, 0.f, 1.f));
			glm::mat4 rotaterightarm = glm::rotate(unitMat, glm::radians(rotatePenguinRightArm), glm::vec3(0.f, 0.f, 1.f));

			glm::mat4 finalMat;

			// ¿ÞÂÊ ÆÈ
			finalMat = moveFinal * rotateDir * moveleftPos * glm::translate(unitMat, glm::vec3(-0.15f, 0.4f, 0.f) * scaleFactor) * rotateleftarm * glm::translate(unitMat, glm::vec3(0.15f, -0.4f, 0.f) * scaleFactor) * scaleMat * moveYMat;
			glUniformMatrix4fv(worldLoc, 1, GL_FALSE, glm::value_ptr(finalMat));
			glUniform3f(colorLoc, 0.f, 0.f, 0.36f);
			glDrawArrays(GL_TRIANGLES, 0, 36);

			// ¿À¸¥ÂÊ ÆÈ
			finalMat = moveFinal * rotateDir * moveRightPos * glm::translate(unitMat, glm::vec3(0.15f, 0.4f, 0.f) * scaleFactor) * rotaterightarm * glm::translate(unitMat, glm::vec3(-0.15f, -0.4f, 0.f) * scaleFactor) * scaleMat * moveYMat;
			glUniformMatrix4fv(worldLoc, 1, GL_FALSE, glm::value_ptr(finalMat));
			glUniform3f(colorLoc, 0.f, 0.f, 0.36f);
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}
		// ¸öÅë ±×¸®±â
		{
			glm::mat4 moveYMat = glm::translate(unitMat, glm::vec3(0.f, 0.5f, 0.f) * scaleFactor);
			glm::mat4 scaleMat = glm::scale(unitMat, glm::vec3(0.4f, 0.65f, 0.4f) * scaleFactor);

			glm::mat4 moveBottomUp = glm::translate(unitMat, glm::vec3(0.f, 0.05f, 0.f) * scaleFactor);

			glm::mat4 finalMat = moveFinal * rotateDir * moveBottomUp * scaleMat * moveYMat;

			glUniformMatrix4fv(worldLoc, 1, GL_FALSE, glm::value_ptr(finalMat));
			glUniform3f(colorLoc, 0.f, 0.f, 0.36f);
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}
		// ¹è ±×¸®±â
		{
			glm::mat4 moveYMat = glm::translate(unitMat, glm::vec3(0.f, 0.5f, 0.f) * scaleFactor);
			glm::mat4 scaleMat = glm::scale(unitMat, glm::vec3(0.3f, 0.5f, 0.1f) * scaleFactor);

			glm::mat4 moveBottomUp = glm::translate(unitMat, glm::vec3(0.f, 0.09f, 0.18f) * scaleFactor);

			glm::mat4 finalMat = moveFinal * rotateDir * moveBottomUp * scaleMat * moveYMat;

			glUniformMatrix4fv(worldLoc, 1, GL_FALSE, glm::value_ptr(finalMat));
			glUniform3f(colorLoc, 1.f, 1.f, 1.f);
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}
		// ¸Ó¸® ±×¸®±â
		{
			glm::mat4 moveYMat = glm::translate(unitMat, glm::vec3(0.f, 0.5f, 0.f) * scaleFactor);
			glm::mat4 scaleMat = glm::scale(unitMat, glm::vec3(0.3f, 0.3f, 0.3f) * scaleFactor);

			glm::mat4 moveBottomUp = glm::translate(unitMat, glm::vec3(0.f, 0.68f, 0.f) * scaleFactor);

			glm::mat4 finalMat = moveFinal * rotateDir * moveBottomUp * scaleMat * moveYMat;
			glUniformMatrix4fv(worldLoc, 1, GL_FALSE, glm::value_ptr(finalMat));

			glUniform3f(colorLoc, 0.f, 0.f, 0.36f);
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}
		// ºÎ¸® ±×¸®±â
		{
			glm::mat4 moveYMat = glm::translate(unitMat, glm::vec3(0.f, 0.5f, 0.f) * scaleFactor);
			glm::mat4 scaleMat = glm::scale(unitMat, glm::vec3(0.1f, 0.05f, 0.1f) * scaleFactor);

			glm::mat4 moveBottomUp = glm::translate(unitMat, glm::vec3(0.f, 0.8f, 0.2f) * scaleFactor);

			glm::mat4 finalMat = moveFinal * rotateDir * moveBottomUp * scaleMat * moveYMat;
			glUniformMatrix4fv(worldLoc, 1, GL_FALSE, glm::value_ptr(finalMat));
			glUniform3f(colorLoc, 1.f, 0.4f, 0.2f);
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}
		// ´« ±×¸®±â
		{
			glm::mat4 moveYMat = glm::translate(unitMat, glm::vec3(0.f, 0.6f, 0.f) * scaleFactor);   // °íÁ¤°ª..
			glm::mat4 scaleMat = glm::scale(unitMat, glm::vec3(0.05f, 0.05f, 0.05f) * scaleFactor);

			glm::mat4 moveleftPos = glm::translate(unitMat, glm::vec3(-0.07f, 0.85f, 0.15f) * scaleFactor);
			glm::mat4 moveRightPos = glm::translate(unitMat, glm::vec3(0.07f, 0.85f, 0.15f) * scaleFactor);

			glm::mat4 finalMat;

			// ¿ÞÂÊ
			finalMat = moveFinal * rotateDir * moveleftPos * scaleMat * moveYMat;
			glUniformMatrix4fv(worldLoc, 1, GL_FALSE, glm::value_ptr(finalMat));
			glUniform3f(colorLoc, 0.5f, 0.5f, 0.5f);
			glDrawArrays(GL_TRIANGLES, 0, 36);
			// ¿À¸¥ÂÊ
			finalMat = moveFinal * rotateDir * moveRightPos * scaleMat * moveYMat;
			glUniformMatrix4fv(worldLoc, 1, GL_FALSE, glm::value_ptr(finalMat));
			glUniform3f(colorLoc, 0.5f, 0.5f, 0.5f);
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}
	}
}

void Penguin::release()
{
}
