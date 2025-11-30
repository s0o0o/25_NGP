#include "Penguin.h"
#include <iostream>

Penguin::Penguin(int num) : penguinNum(num)
{
	setPosition(0.f, 0.f, 0.f);
	m_targetPos = glm::vec3(0.f, 0.f, 0.f);
	m_prevPos = glm::vec3(0.f, 0.f, 0.f);
	targetAngle = 0.f;
	currentAngle = 0.f;

	MOVE_SPEED = 1.0f + static_cast<float>(std::rand()) / (static_cast<float>(RAND_MAX / 0.3f));
	isBaby = true;

	rotatePenguinLeftArm = -25.f;
	rotatePenguinRightArm = 25.f;

	isMaxRotatePenguin = false;

	isNear = false;
}

Penguin::~Penguin()
{
}

void Penguin::initialize()
{
//	std::cout << "Penguin init, shader : " << shader << std::endl;
	if (shader == 0) {
		std::cerr << "Error: Pig shader has not been set before calling initialize!\n";
		return;
	}

	m_worldLoc = glGetUniformLocation(shader, "modelTransform");
	m_colorLoc = glGetUniformLocation(shader, "colorTransform");

	//std::cout << "Penguin m_worldLoc : " << m_worldLoc << std::endl;
	//std::cout << "Penguin m_colorLoc : " << m_colorLoc << std::endl;

	if (m_worldLoc < 0) std::cerr << "Error: Penguin modelTransform uniform not found!\n";
	if (m_colorLoc < 0) std::cerr << "Error: Penguin colorTransform uniform not found!\n";

	m_viewLoc = glGetUniformLocation(shader, "viewTransform");
	m_projLoc = glGetUniformLocation(shader, "projTransform");
	m_lightPosLoc = glGetUniformLocation(shader, "lightPos");
}

void Penguin::update(float elapseTime)
{
	// Æë±Ï ³¯°³ °¢µµ È¸Àü
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

	float diff = targetAngle - currentAngle;
	if (diff > 180.0f) diff -= 360.0f;
	if (diff < -180.0f) diff += 360.0f;

	if (abs(diff) > 0.1f)
	{
		float rotateStep = diff * 5.0f * elapseTime;
		rotateY(rotateStep);

		currentAngle += rotateStep;

		if (currentAngle > 360.0f) currentAngle -= 360.0f;
		if (currentAngle < 0.0f)   currentAngle += 360.0f;
	}

	glm::vec3 currentPos = getPosition();
	glm::vec3 moveDir = m_targetPos - currentPos;
	float dist = glm::length(moveDir);

	if (dist > 5.0f) {
		setPosition(m_targetPos);
	}
	else if (dist > 0.01f)
	{
		float moveStep = MOVE_SPEED * elapseTime; // ÀÌµ¿ ¼Óµµ
		if (dist <= moveStep) {
			setPosition(m_targetPos);
		}
		else {
			setPosition(currentPos + (glm::normalize(moveDir) * moveStep));
		}
	}
}

void Penguin::setTargetPosition(float x, float z)
{
	glm::vec3 newTarget(x, 0.f, z);
	glm::vec3 dir = newTarget - m_prevPos;

	if (glm::length(dir) > 0.05f)
	{
		dir = glm::normalize(dir);

		float angleRad = atan2(dir.x, dir.z);
		float angleDeg = glm::degrees(angleRad);

		targetAngle = angleDeg;
		m_prevPos = newTarget;
	}

	m_targetPos = newTarget;
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
		// Æë±Ï
		const glm::mat4 unitMat(1.f);
		float scaleFactor_Big = 1.3f;
		// ¹ß ±×¸®±â
		{
			glm::mat4 moveYMat = glm::translate(unitMat, glm::vec3(0.f, 0.5f, 0.f) * scaleFactor_Big);   // °íÁ¤°ª..
			glm::mat4 scaleMat = glm::scale(unitMat, glm::vec3(0.1f, 0.05f, 0.3f) * scaleFactor_Big);

			glm::mat4 moveleftPos = glm::translate(unitMat, glm::vec3(-0.12f, 0.02f, 0.1f) * scaleFactor_Big);
			glm::mat4 moveRightPos = glm::translate(unitMat, glm::vec3(0.12f, 0.02f, 0.1f) * scaleFactor_Big);

			// ¿ÞÂÊ
			glm::mat4 partTransform = moveleftPos * scaleMat * moveYMat;
			finalMat = baseTransform * partTransform;
			glUniformMatrix4fv(m_worldLoc, 1, GL_FALSE, glm::value_ptr(finalMat));
			glUniform3f(m_colorLoc, 1.f, 1.f, 0.3f);
			glDrawArrays(GL_TRIANGLES, 0, 36);
			// ¿À¸¥ÂÊ
			partTransform = moveRightPos * scaleMat * moveYMat;
			finalMat = baseTransform * partTransform;
			glUniformMatrix4fv(m_worldLoc, 1, GL_FALSE, glm::value_ptr(finalMat));
			glUniform3f(m_colorLoc, 1.f, 1.f, 0.3f);
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

			glm::mat4 partTransform = moveleftPos * glm::translate(unitMat, glm::vec3(-0.15f, 0.4f, 0.f)
				* scaleFactor_Big) * rotateleftarm * glm::translate(unitMat, glm::vec3(0.15f, -0.4f, 0.f) * scaleFactor_Big)
				 * scaleMat * moveYMat;
			finalMat = baseTransform * partTransform;
			glUniformMatrix4fv(m_worldLoc, 1, GL_FALSE, glm::value_ptr(finalMat));
			glUniform3f(m_colorLoc, 0.f, 0.f, 0.36f);
			glDrawArrays(GL_TRIANGLES, 0, 36);

			// ¿À¸¥ÂÊ ÆÈ
			partTransform = moveRightPos * glm::translate(unitMat, glm::vec3(0.15f, 0.4f, 0.f)
				* scaleFactor_Big) * rotaterightarm * glm::translate(unitMat, glm::vec3(-0.15f, -0.4f, 0.f) * scaleFactor_Big)
				 * scaleMat * moveYMat;
			finalMat = baseTransform * partTransform;
			glUniformMatrix4fv(m_worldLoc, 1, GL_FALSE, glm::value_ptr(finalMat));
			glUniform3f(m_colorLoc, 0.f, 0.f, 0.36f);
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}
		// ¸öÅë ±×¸®±â
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
		// ¹è ±×¸®±â
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
		// ¸Ó¸® ±×¸®±â
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
		// ºÎ¸® ±×¸®±â
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
		// ´« ±×¸®±â
		{
			glm::mat4 moveYMat = glm::translate(unitMat, glm::vec3(0.f, 0.6f, 0.f) * scaleFactor_Big);   // °íÁ¤°ª..
			glm::mat4 scaleMat = glm::scale(unitMat, glm::vec3(0.05f, 0.05f, 0.05f) * scaleFactor_Big);

			glm::mat4 moveleftPos = glm::translate(unitMat, glm::vec3(-0.07f, 0.85f, 0.15f) * scaleFactor_Big);
			glm::mat4 moveRightPos = glm::translate(unitMat, glm::vec3(0.07f, 0.85f, 0.15f) * scaleFactor_Big);

			glm::mat4 partTransform = moveleftPos * scaleMat * moveYMat;
			finalMat = baseTransform * partTransform;

			glUniformMatrix4fv(m_worldLoc, 1, GL_FALSE, glm::value_ptr(finalMat));
			glUniform3f(m_colorLoc, 0.5f, 0.5f, 0.5f);
			glDrawArrays(GL_TRIANGLES, 0, 36);
			// ¿À¸¥ÂÊ
			partTransform = moveRightPos * scaleMat * moveYMat;
			finalMat = baseTransform * partTransform;
			glUniformMatrix4fv(m_worldLoc, 1, GL_FALSE, glm::value_ptr(finalMat));
			glUniform3f(m_colorLoc, 0.5f, 0.5f, 0.5f);
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}
	}
	else if (isBaby) {
		// ¾Æ±âÆë±Ï
		const glm::mat4 unitMat(1.f);
		float scaleFactor = 0.8f;
		// ¹ß ±×¸®±â
		{
			glm::mat4 moveYMat = glm::translate(unitMat, glm::vec3(0.f, 0.5f, 0.f) * scaleFactor);   // °íÁ¤°ª..
			glm::mat4 scaleMat = glm::scale(unitMat, glm::vec3(0.1f, 0.05f, 0.3f) * scaleFactor);

			glm::mat4 moveleftPos = glm::translate(unitMat, glm::vec3(-0.12f, 0.f, 0.1f) * scaleFactor);
			glm::mat4 moveRightPos = glm::translate(unitMat, glm::vec3(0.12f, 0.f, 0.1f) * scaleFactor);;


			// ¿ÞÂÊ
			glm::mat4 partTransform = moveleftPos * scaleMat * moveYMat;
			finalMat = baseTransform * partTransform;
			glUniformMatrix4fv(m_worldLoc, 1, GL_FALSE, glm::value_ptr(finalMat));
			glUniform3f(m_colorLoc, 1.f, 1.f, 0.3f);
			glDrawArrays(GL_TRIANGLES, 0, 36);
			// ¿À¸¥ÂÊ
			partTransform = moveRightPos * scaleMat * moveYMat;
			finalMat = baseTransform * partTransform;
			glUniformMatrix4fv(m_worldLoc, 1, GL_FALSE, glm::value_ptr(finalMat));
			glUniform3f(m_colorLoc, 1.f, 1.f, 0.3f);
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

			// ¿ÞÂÊ ÆÈ
			glm::mat4 partTransform = moveleftPos * glm::translate(unitMat, glm::vec3(-0.15f, 0.4f, 0.f) * scaleFactor)
				* rotateleftarm * glm::translate(unitMat, glm::vec3(0.15f, -0.4f, 0.f) * scaleFactor) * scaleMat * moveYMat;
			finalMat = baseTransform * partTransform;
			glUniformMatrix4fv(m_worldLoc, 1, GL_FALSE, glm::value_ptr(finalMat));
			glUniform3f(m_colorLoc, 0.f, 0.f, 0.36f);
			glDrawArrays(GL_TRIANGLES, 0, 36);

			// ¿À¸¥ÂÊ ÆÈ
			partTransform = moveRightPos * glm::translate(unitMat, glm::vec3(0.15f, 0.4f, 0.f) * scaleFactor)
				* rotaterightarm * glm::translate(unitMat, glm::vec3(-0.15f, -0.4f, 0.f) * scaleFactor) * scaleMat * moveYMat;
			finalMat = baseTransform * partTransform;
			glUniformMatrix4fv(m_worldLoc, 1, GL_FALSE, glm::value_ptr(finalMat));
			glUniform3f(m_colorLoc, 0.f, 0.f, 0.36f);
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}
		// ¸öÅë ±×¸®±â
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
		// ¹è ±×¸®±â
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
		// ¸Ó¸® ±×¸®±â
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
		// ºÎ¸® ±×¸®±â
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
		// ´« ±×¸®±â
		{
			glm::mat4 moveYMat = glm::translate(unitMat, glm::vec3(0.f, 0.6f, 0.f) * scaleFactor);   // °íÁ¤°ª..
			glm::mat4 scaleMat = glm::scale(unitMat, glm::vec3(0.05f, 0.05f, 0.05f) * scaleFactor);

			glm::mat4 moveleftPos = glm::translate(unitMat, glm::vec3(-0.07f, 0.85f, 0.15f) * scaleFactor);
			glm::mat4 moveRightPos = glm::translate(unitMat, glm::vec3(0.07f, 0.85f, 0.15f) * scaleFactor);

			// ¿ÞÂÊ
			glm::mat4 partTransform = moveleftPos * scaleMat * moveYMat;
			finalMat = baseTransform * partTransform;
			glUniformMatrix4fv(m_worldLoc, 1, GL_FALSE, glm::value_ptr(finalMat));
			glUniform3f(m_colorLoc, 0.5f, 0.5f, 0.5f);
			glDrawArrays(GL_TRIANGLES, 0, 36);
			// ¿À¸¥ÂÊ
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
