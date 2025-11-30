#include "Alpaca.h"
#include "PlayerObject.h"
#include <iostream>

Alpaca::Alpaca()
{
	setPosition(0.f, 0.f, 0.f);
	m_targetPos = glm::vec3(0.f, 0.f, 0.f);
	m_prevPos = glm::vec3(0.f, 0.f, 0.f);
	targetAngle = 0.f;
	currentAngle = 0.f;

	MOVE_SPEED = 1.0f + static_cast<float>(std::rand()) / (static_cast<float>(RAND_MAX / 0.3f));

	isBaby = true;


	rotateAlpaLeftLeg = 0.f;
	rotateAlpaRightLeg = 0.f;

	isMaxRotateAlpa = false;

	isNear = false;
}

Alpaca::~Alpaca()
{
	//	delete this;
}

void Alpaca::initialize()
{
	//std::cout << "Alpaca init, shader : " << shader << std::endl;
	if (shader == 0) {
		std::cerr << "Error: Chic shader has not been set before calling initialize!\n";
		return;
	}
	m_worldLoc = glGetUniformLocation(shader, "modelTransform");
	m_colorLoc = glGetUniformLocation(shader, "colorTransform");
	//std::cout << "Alpaca m_worldLoc : " << m_worldLoc << std::endl;
	//std::cout << "Alpaca m_colorLoc : " << m_colorLoc << std::endl;

	if (m_worldLoc < 0) std::cerr << "Error: Alpaca modelTransform uniform not found!\n";
	if (m_colorLoc < 0) std::cerr << "Error: Alpaca colorTransform uniform not found!\n";

	m_viewLoc = glGetUniformLocation(shader, "viewTransform");
	m_projLoc = glGetUniformLocation(shader, "projTransform");
	m_lightPosLoc = glGetUniformLocation(shader, "lightPos");
}

// 펜스 실 범위
// x : -15.f ~ 0.f
// z : -8.f ~ 10.f

void Alpaca::update(float elapseTime)
{

	// 알파카 다리 각도 회전
	if (isMaxRotateAlpa)
	{
		rotateAlpaLeftLeg -= LEG_ROTATE_SPEED * elapseTime;;
		rotateAlpaRightLeg += LEG_ROTATE_SPEED * elapseTime;;
		if (rotateAlpaLeftLeg <= -30.f) {
			isMaxRotateAlpa = false;
		}
	}
	else if (not isMaxRotateAlpa) {
		rotateAlpaLeftLeg += LEG_ROTATE_SPEED * elapseTime;;
		rotateAlpaRightLeg -= LEG_ROTATE_SPEED * elapseTime;;
		if (rotateAlpaLeftLeg >= 30.f) {
			isMaxRotateAlpa = true;
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
		float moveStep = MOVE_SPEED * elapseTime; // 이동 속도

		if (dist <= moveStep) {
			setPosition(m_targetPos);
		}
		else {
			setPosition(currentPos + (glm::normalize(moveDir) * moveStep));
		}
	}
}

void Alpaca::setTargetPosition(float x, float z)
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


void Alpaca::draw(const glm::mat4& viewMatrix, 
	const glm::mat4& projMatrix, const glm::vec3& lightPos) const
{
	glUseProgram(shader);
	glBindVertexArray(VAO);

	glUniformMatrix4fv(m_viewLoc, 1, GL_FALSE, glm::value_ptr(viewMatrix));
	glUniformMatrix4fv(m_projLoc, 1, GL_FALSE, glm::value_ptr(projMatrix));
	glUniform3f(m_lightPosLoc, lightPos.x, lightPos.y, lightPos.z);

	const glm::mat4 unitMat(1.f);
	glm::mat4 finalMat;
	glm::mat4 baseTransform = worldTransform;

	if (isBaby) {

		float scaleFactor = 0.5f;

		// 다리 그리기
		{
			glm::mat4 moveYMat = glm::translate(unitMat, glm::vec3(0.f, 0.5f * scaleFactor, 0.f));   // 고정값..
			glm::mat4 scaleMat = glm::scale(unitMat, glm::vec3(0.18f, 0.3f, 0.18f) * scaleFactor);
			glm::mat4 moveleftPos = glm::translate(unitMat, glm::vec3(-0.18f, 0.f, -0.25f) * scaleFactor);
			glm::mat4 moveRightPos = glm::translate(unitMat, glm::vec3(0.18f, 0.f, -0.25f) * scaleFactor);

			glm::mat4 rotateleftleg = glm::rotate(unitMat, glm::radians(rotateAlpaLeftLeg), glm::vec3(1.f, 0.f, 0.f));
			glm::mat4 rotaterightleg = glm::rotate(unitMat, glm::radians(rotateAlpaRightLeg), glm::vec3(1.f, 0.f, 0.f));

			// 왼쪽
			glm::mat4 partTransform = moveleftPos * glm::translate(unitMat, glm::vec3(-0.0625f, 0.15f, 0.f)
				* scaleFactor) * rotateleftleg * glm::translate(unitMat, glm::vec3(0.0625f, -0.15f, 0.f) * scaleFactor)
				* scaleMat * moveYMat;
			finalMat = baseTransform * partTransform;
			glUniformMatrix4fv(m_worldLoc, 1, GL_FALSE, glm::value_ptr(finalMat));
			glUniform3f(m_colorLoc, 0.92f, 0.9f, 0.8f);
			glDrawArrays(GL_TRIANGLES, 0, 36);
			// 오른쪽
			partTransform = moveRightPos * glm::translate(unitMat, glm::vec3(0.0625f, 0.15f, 0.f)
				* scaleFactor) * rotaterightleg * glm::translate(unitMat, glm::vec3(-0.0625f, -0.15f, 0.f)
					* scaleFactor) * scaleMat * moveYMat;
			finalMat = baseTransform * partTransform;
			glUniformMatrix4fv(m_worldLoc, 1, GL_FALSE, glm::value_ptr(finalMat));
			glUniform3f(m_colorLoc, 0.92f, 0.9f, 0.8f);
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}
		// 몸통 그리기
		{
			glm::mat4 moveYMat = glm::translate(unitMat, glm::vec3(0.f, 0.5f, 0.f) * scaleFactor);
			glm::mat4 scaleMat = glm::scale(unitMat, glm::vec3(0.55f, 0.55f, 1.f) * scaleFactor);

			glm::mat4 moveBottomUp = glm::translate(unitMat, glm::vec3(0.f, 0.3f, 0.f) * scaleFactor);

			glm::mat4 partTransform = moveBottomUp * scaleMat * moveYMat;
			finalMat = baseTransform * partTransform;
			glUniformMatrix4fv(m_worldLoc, 1, GL_FALSE, glm::value_ptr(finalMat));
			glUniform3f(m_colorLoc, 0.92f, 0.9f, 0.8f);
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

			// 왼쪽
			glm::mat4 partTransform = moveleftPos * glm::translate(unitMat, glm::vec3(-0.0625f, 0.15f, 0.f)
				* scaleFactor) * rotateleftleg * glm::translate(unitMat, glm::vec3(0.0625f, -0.15f, 0.f)
					* scaleFactor) * scaleMat * moveYMat;
			finalMat = baseTransform * partTransform;
			glUniformMatrix4fv(m_worldLoc, 1, GL_FALSE, glm::value_ptr(finalMat));
			glUniform3f(m_colorLoc, 0.92f, 0.9f, 0.8f);
			glDrawArrays(GL_TRIANGLES, 0, 36);
			// 오른쪽
			partTransform = moveRightPos * glm::translate(unitMat, glm::vec3(0.0625f, 0.15f, 0.f)
				* scaleFactor) * rotaterightleg * glm::translate(unitMat, glm::vec3(-0.0625f, -0.15f, 0.f)
					* scaleFactor) * scaleMat * moveYMat;
			finalMat = baseTransform * partTransform;
			glUniformMatrix4fv(m_worldLoc, 1, GL_FALSE, glm::value_ptr(finalMat));
			glUniform3f(m_colorLoc, 0.92f, 0.9f, 0.8f);
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}
		// 머리 그리기
		{
			glm::mat4 moveYMat = glm::translate(unitMat, glm::vec3(0.f, 0.5f, 0.f) * scaleFactor);
			glm::mat4 scaleMat = glm::scale(unitMat, glm::vec3(0.45f, 1.f, 0.45f) * scaleFactor);

			glm::mat4 moveBottomUp = glm::translate(unitMat, glm::vec3(0.f, 0.5f, 0.4f) * scaleFactor);
			glm::mat4 partTransform = moveBottomUp * scaleMat * moveYMat;
			finalMat = baseTransform * partTransform;
			glUniformMatrix4fv(m_worldLoc, 1, GL_FALSE, glm::value_ptr(finalMat));

			glUniform3f(m_colorLoc, 0.92f, 0.9f, 0.8f);
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}
		// 주둥이 그리기
		{
			glm::mat4 moveYMat = glm::translate(unitMat, glm::vec3(0.f, 0.5f, 0.f) * scaleFactor);
			glm::mat4 scaleMat = glm::scale(unitMat, glm::vec3(0.2f, 0.15f, 0.1f) * scaleFactor);

			glm::mat4 moveBottomUp = glm::translate(unitMat, glm::vec3(0.f, 0.9f, 0.65f) * scaleFactor);

			glm::mat4 partTransform = moveBottomUp * scaleMat * moveYMat;
			finalMat = baseTransform * partTransform;
			glUniformMatrix4fv(m_worldLoc, 1, GL_FALSE, glm::value_ptr(finalMat));
			glUniform3f(m_colorLoc, 0.8f, 0.75f, 0.6f);
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}
		// 코 그리기
		{
			glm::mat4 moveYMat = glm::translate(unitMat, glm::vec3(0.f, 0.5f, 0.f) * scaleFactor);
			glm::mat4 scaleMat = glm::scale(unitMat, glm::vec3(0.1f, 0.05f, 0.1f) * scaleFactor);
			glm::mat4 moveBottomUp = glm::translate(unitMat, glm::vec3(0.f, 0.98f, 0.7f) * scaleFactor);
			glm::mat4 partTransform = moveBottomUp * scaleMat * moveYMat;
			finalMat = baseTransform * partTransform;
			glUniformMatrix4fv(m_worldLoc, 1, GL_FALSE, glm::value_ptr(finalMat));
			glUniform3f(m_colorLoc, 0.8f, 0.7f, 0.6f);
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}
		// 귀 그리기
		{
			glm::mat4 moveYMat = glm::translate(unitMat, glm::vec3(0.f, 0.5f, 0.f) * scaleFactor);   // 고정값..
			glm::mat4 scaleMat = glm::scale(unitMat, glm::vec3(0.1f, 0.2f, 0.1f) * scaleFactor);

			glm::mat4 moveleftPos = glm::translate(unitMat, glm::vec3(-0.1f, 1.3f, 0.5f) * scaleFactor);
			glm::mat4 moveRightPos = glm::translate(unitMat, glm::vec3(0.1f, 1.3f, 0.5f) * scaleFactor);


			// 왼쪽
			glm::mat4 partTransform = moveleftPos * scaleMat * moveYMat;
			finalMat = baseTransform * partTransform;
			glUniformMatrix4fv(m_worldLoc, 1, GL_FALSE, glm::value_ptr(finalMat));
			glUniform3f(m_colorLoc, 0.92f, 0.9f, 0.8f);
			glDrawArrays(GL_TRIANGLES, 0, 36);
			// 오른쪽
			partTransform = moveRightPos * scaleMat * moveYMat;
			finalMat = baseTransform * partTransform;
			glUniformMatrix4fv(m_worldLoc, 1, GL_FALSE, glm::value_ptr(finalMat));
			glUniform3f(m_colorLoc, 0.92f, 0.9f, 0.8f);
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}
		{
			// 눈
			glm::mat4 moveYMat = glm::translate(unitMat, glm::vec3(0.f, 0.5f, 0.f) * scaleFactor);   // 고정값..
			glm::mat4 scaleMat = glm::scale(unitMat, glm::vec3(0.07f, 0.07f, 0.07f) * scaleFactor);

			glm::mat4 moveleftPos = glm::translate(unitMat, glm::vec3(-0.15f, 1.f, 0.6f) * scaleFactor);
			glm::mat4 moveRightPos = glm::translate(unitMat, glm::vec3(0.15f, 1.f, 0.6f) * scaleFactor);

			// 왼쪽
			glm::mat4 partTransform = moveleftPos * scaleMat * moveYMat;
			finalMat = baseTransform * partTransform;
			glUniformMatrix4fv(m_worldLoc, 1, GL_FALSE, glm::value_ptr(finalMat));
			glUniform3f(m_colorLoc, 0.f, 0.f, 0.f);
			glDrawArrays(GL_TRIANGLES, 0, 36);
			// 오른쪽
			partTransform = moveRightPos * scaleMat * moveYMat;
			finalMat = baseTransform * partTransform;
			glUniformMatrix4fv(m_worldLoc, 1, GL_FALSE, glm::value_ptr(finalMat));
			glUniform3f(m_colorLoc, 0.f, 0.f, 0.f);
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}
	}
	else if (not isBaby) {
		//std::cout << " ? " << std::endl;
		// 다리 그리기
		{
			glm::mat4 moveYMat = glm::translate(unitMat, glm::vec3(0.f, 0.5f, 0.f));	// 고정값..
			glm::mat4 scaleMat = glm::scale(unitMat, glm::vec3(0.18f, 0.3f, 0.18f));

			glm::mat4 moveleftPos = glm::translate(unitMat, glm::vec3(-0.18f, 0.f, -0.25f));
			glm::mat4 moveRightPos = glm::translate(unitMat, glm::vec3(0.18f, 0.f, -0.25f));

			glm::mat4 rotateleftleg = glm::rotate(unitMat, glm::radians(rotateAlpaLeftLeg), glm::vec3(1.f, 0.f, 0.f));
			glm::mat4 rotaterightleg = glm::rotate(unitMat, glm::radians(rotateAlpaRightLeg), glm::vec3(1.f, 0.f, 0.f));

			// 왼쪽
			glm::mat4 partTransform = moveleftPos * glm::translate(unitMat, glm::vec3(-0.0625f, 0.15f, 0.f))
				* rotateleftleg * glm::translate(unitMat, glm::vec3(0.0625f, -0.15f, 0.f)) * scaleMat * moveYMat;
			finalMat = baseTransform * partTransform;
			glUniformMatrix4fv(m_worldLoc, 1, GL_FALSE, glm::value_ptr(finalMat));
			glUniform3f(m_colorLoc, 0.92f, 0.9f, 0.8f);
			glDrawArrays(GL_TRIANGLES, 0, 36);
			// 오른쪽
			partTransform = moveRightPos * glm::translate(unitMat, glm::vec3(0.0625f, 0.15f, 0.f))
				* rotaterightleg * glm::translate(unitMat, glm::vec3(-0.0625f, -0.15f, 0.f)) * scaleMat * moveYMat;
			finalMat = baseTransform * partTransform;
			glUniformMatrix4fv(m_worldLoc, 1, GL_FALSE, glm::value_ptr(finalMat));
			glUniform3f(m_colorLoc, 0.92f, 0.9f, 0.8f);
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}
		// 몸통 그리기
		{
			glm::mat4 moveYMat = glm::translate(unitMat, glm::vec3(0.f, 0.5f, 0.f));
			glm::mat4 scaleMat = glm::scale(unitMat, glm::vec3(0.55f, 0.55f, 1.f));

			glm::mat4 moveBottomUp = glm::translate(unitMat, glm::vec3(0.f, 0.3f, 0.f));

			glm::mat4 partTransform = moveBottomUp * scaleMat * moveYMat;
			finalMat = baseTransform * partTransform;
			glUniformMatrix4fv(m_worldLoc, 1, GL_FALSE, glm::value_ptr(finalMat));
			glUniform3f(m_colorLoc, 0.92f, 0.9f, 0.8f);
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

			glm::mat4 partTransform = moveleftPos * glm::translate(unitMat, glm::vec3(-0.0625f, 0.15f, 0.f))
				* rotateleftleg * glm::translate(unitMat, glm::vec3(0.0625f, -0.15f, 0.f)) * scaleMat * moveYMat;
			finalMat = baseTransform * partTransform;

			// 왼쪽
			glUniformMatrix4fv(m_worldLoc, 1, GL_FALSE, glm::value_ptr(finalMat));
			glUniform3f(m_colorLoc, 0.92f, 0.9f, 0.8f);
			glDrawArrays(GL_TRIANGLES, 0, 36);
			// 오른쪽
			partTransform = moveRightPos * glm::translate(unitMat, glm::vec3(0.0625f, 0.15f, 0.f))
				* rotaterightleg * glm::translate(unitMat, glm::vec3(-0.0625f, -0.15f, 0.f)) * scaleMat * moveYMat;
			finalMat = baseTransform * partTransform;
			glUniformMatrix4fv(m_worldLoc, 1, GL_FALSE, glm::value_ptr(finalMat));
			glUniform3f(m_colorLoc, 0.92f, 0.9f, 0.8f);
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}
		// 머리 그리기
		{
			glm::mat4 moveYMat = glm::translate(unitMat, glm::vec3(0.f, 0.5f, 0.f));
			glm::mat4 scaleMat = glm::scale(unitMat, glm::vec3(0.45f, 1.f, 0.45f));

			glm::mat4 moveBottomUp = glm::translate(unitMat, glm::vec3(0.f, 0.5f, 0.4f));

			glm::mat4 partTransform = moveBottomUp * scaleMat * moveYMat;
			finalMat = baseTransform * partTransform;
			glUniformMatrix4fv(m_worldLoc, 1, GL_FALSE, glm::value_ptr(finalMat));
			glUniform3f(m_colorLoc, 0.92f, 0.9f, 0.8f);
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}
		// 주둥이 그리기
		{
			glm::mat4 moveYMat = glm::translate(unitMat, glm::vec3(0.f, 0.5f, 0.f));
			glm::mat4 scaleMat = glm::scale(unitMat, glm::vec3(0.2f, 0.15f, 0.1f));

			glm::mat4 moveBottomUp = glm::translate(unitMat, glm::vec3(0.f, 1.05f, 0.65f));

			glm::mat4 partTransform = moveBottomUp * scaleMat * moveYMat;
			finalMat = baseTransform * partTransform;
			glUniformMatrix4fv(m_worldLoc, 1, GL_FALSE, glm::value_ptr(finalMat));
			glUniform3f(m_colorLoc, 0.8f, 0.75f, 0.6f);
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}
		// 코 그리기
		{
			glm::mat4 moveYMat = glm::translate(unitMat, glm::vec3(0.f, 0.5f, 0.f));
			glm::mat4 scaleMat = glm::scale(unitMat, glm::vec3(0.1f, 0.05f, 0.1f));

			glm::mat4 moveBottomUp = glm::translate(unitMat, glm::vec3(0.f, 1.15f, 0.7f));

			glm::mat4 partTransform = moveBottomUp * scaleMat * moveYMat;
			finalMat = baseTransform * partTransform;
			glUniformMatrix4fv(m_worldLoc, 1, GL_FALSE, glm::value_ptr(finalMat));
			glUniform3f(m_colorLoc, 0.8f, 0.7f, 0.6f);
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}
		// 귀 그리기
		{
			glm::mat4 moveYMat = glm::translate(unitMat, glm::vec3(0.f, 0.5f, 0.f));	// 고정값..
			glm::mat4 scaleMat = glm::scale(unitMat, glm::vec3(0.1f, 0.2f, 0.1f));

			glm::mat4 moveleftPos = glm::translate(unitMat, glm::vec3(-0.1f, 1.5f, 0.5f));
			glm::mat4 moveRightPos = glm::translate(unitMat, glm::vec3(0.1f, 1.5f, 0.5f));


			// 왼쪽
			glm::mat4 partTransform = moveleftPos * scaleMat * moveYMat;
			finalMat = baseTransform * partTransform;
			glUniformMatrix4fv(m_worldLoc, 1, GL_FALSE, glm::value_ptr(finalMat));
			glUniform3f(m_colorLoc, 0.92f, 0.9f, 0.8f);
			glDrawArrays(GL_TRIANGLES, 0, 36);
			// 오른쪽
			partTransform = moveRightPos * scaleMat * moveYMat;
			finalMat = baseTransform * partTransform;
			glUniformMatrix4fv(m_worldLoc, 1, GL_FALSE, glm::value_ptr(finalMat));
			glUniform3f(m_colorLoc, 0.92f, 0.9f, 0.8f);
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}
		{
			// 눈
			glm::mat4 moveYMat = glm::translate(unitMat, glm::vec3(0.f, 0.5f, 0.f));	// 고정값..
			glm::mat4 scaleMat = glm::scale(unitMat, glm::vec3(0.07f, 0.07f, 0.07f));

			glm::mat4 moveleftPos = glm::translate(unitMat, glm::vec3(-0.15f, 1.2f, 0.6f));
			glm::mat4 moveRightPos = glm::translate(unitMat, glm::vec3(0.15f, 1.2f, 0.6f));

			// 왼쪽
			glm::mat4 partTransform = moveleftPos * scaleMat * moveYMat;
			finalMat = baseTransform * partTransform;
			glUniformMatrix4fv(m_worldLoc, 1, GL_FALSE, glm::value_ptr(finalMat));
			glUniform3f(m_colorLoc, 0.f, 0.f, 0.f);
			glDrawArrays(GL_TRIANGLES, 0, 36);
			// 오른쪽
			partTransform = moveRightPos * scaleMat * moveYMat;
			finalMat = baseTransform * partTransform;
			glUniformMatrix4fv(m_worldLoc, 1, GL_FALSE, glm::value_ptr(finalMat));
			glUniform3f(m_colorLoc, 0.f, 0.f, 0.f);
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}
	}
}

void Alpaca::release()
{
}

