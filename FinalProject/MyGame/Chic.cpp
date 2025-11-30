#include "Chic.h"
#include "PlayerObject.h"
#include <iostream>

Chic::Chic()
{
	setPosition(0.f, 0.f, 0.f);
	m_targetPos = glm::vec3(0.f, 0.f, 0.f);
	m_prevPos = glm::vec3(0.f, 0.f, 0.f);
	targetAngle = 0.f;
	currentAngle = 0.f;

	MOVE_SPEED = 1.0f + static_cast<float>(std::rand()) / (static_cast<float>(RAND_MAX / 0.3f));

	isBaby = true;

	rotateChicLeftLeg = 0.f;
	rotateChicRightLeg = 0.f;

	rotateChicLeftLeg = 0.f;
	rotateChicRightLeg = 0.f;

	isMaxRotateChic = false; // 각도 제한
	isMaxRotateAdultChic = false;
}

Chic::~Chic()
{
}

void Chic::initialize()
{
	//std::cout << "Chic init, shader : " << shader << std::endl;
	if (shader == 0) {
		std::cerr << "Error: Chic shader has not been set before calling initialize!\n";
		return;
	}
	m_worldLoc = glGetUniformLocation(shader, "modelTransform");
	m_colorLoc = glGetUniformLocation(shader, "colorTransform");
	//std::cout << "Chic m_worldLoc : " << m_worldLoc << std::endl;
	//std::cout << "Chic m_colorLoc : " << m_colorLoc << std::endl;

	if (m_worldLoc < 0) std::cerr << "Error: Chic modelTransform uniform not found!\n";
	if (m_colorLoc < 0) std::cerr << "Error: Chic colorTransform uniform not found!\n";

	m_viewLoc = glGetUniformLocation(shader, "viewTransform");
	m_projLoc = glGetUniformLocation(shader, "projTransform");
	m_lightPosLoc = glGetUniformLocation(shader, "lightPos");
}

void Chic::update(float elapseTime)
{

	if (isMaxRotateChic)
	{
		rotateChicLeftLeg -= LEG_ROTATE_SPEED * elapseTime;
		rotateChicRightLeg += LEG_ROTATE_SPEED * elapseTime;;
		if (rotateChicLeftLeg <= -30.f) {
			isMaxRotateChic = false;
		}
	}
	else if (not isMaxRotateChic) {
		rotateChicLeftLeg += LEG_ROTATE_SPEED * elapseTime;;
		rotateChicRightLeg -= LEG_ROTATE_SPEED * elapseTime;;
		if (rotateChicLeftLeg >= 30.f) {
			isMaxRotateChic = true;
		}
	}

	// 닭 다리 각도 회전
	if (isMaxRotateAdultChic)
	{
		rotateAdultChicLeftLeg -= LEG_ROTATE_SPEED * elapseTime;;
		rotateAdultChicRightLeg += LEG_ROTATE_SPEED * elapseTime;;
		if (rotateAdultChicLeftLeg <= -30.f) {
			isMaxRotateAdultChic = false;
		}
	}
	else if (not isMaxRotateAdultChic) {
		rotateAdultChicLeftLeg += LEG_ROTATE_SPEED * elapseTime;;
		rotateAdultChicRightLeg -= LEG_ROTATE_SPEED * elapseTime;;
		if (rotateAdultChicLeftLeg >= 30.f) {
			isMaxRotateAdultChic = true;
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

void Chic::setTargetPosition(float x, float z)
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


void Chic::draw(const glm::mat4& viewMatrix, 
	const glm::mat4& projMatrix, const glm::vec3& lightPos) const
{
	glUseProgram(shader);
	glBindVertexArray(VAO);

	glUniformMatrix4fv(m_viewLoc, 1, GL_FALSE, glm::value_ptr(viewMatrix));
	glUniformMatrix4fv(m_projLoc, 1, GL_FALSE, glm::value_ptr(projMatrix));
	glUniform3f(m_lightPosLoc, lightPos.x, lightPos.y, lightPos.z);

	glm::mat4 finalMat;
	glm::mat4 baseTransform = worldTransform;
	if (isBaby) {
		//병아리
		const glm::mat4 unitMat(1.f);
		//glm::mat4 moveFinal = glm::translate(unitMat, glm::vec3(moveChicX, 0.05f, moveChicZ));
		//glm::mat4 rotateDir = glm::rotate(unitMat, glm::radians(rotateFaceChic), glm::vec3(0.f, 1.f, 0.f));
		//다리 그리기
		{
			glm::mat4 moveYMat = glm::translate(unitMat, glm::vec3(0.f, 0.5f, 0.f));	// 고정값..
			glm::mat4 scaleMat = glm::scale(unitMat, glm::vec3(0.08f, 0.15f, 0.08f));

			glm::mat4 moveleftPos = glm::translate(unitMat, glm::vec3(-0.07f, 0.f, 0.f));
			glm::mat4 moveRightPos = glm::translate(unitMat, glm::vec3(0.07f, 0.f, 0.f));

			glm::mat4 rotateleftleg = glm::rotate(unitMat, glm::radians(rotateChicLeftLeg), glm::vec3(1.f, 0.f, 0.f));
			glm::mat4 rotaterightleg = glm::rotate(unitMat, glm::radians(rotateChicRightLeg), glm::vec3(1.f, 0.f, 0.f));

			//왼쪽
			glm::mat4 partTransform = moveleftPos * glm::translate(unitMat, glm::vec3(-0.0625f, 0.15f, 0.f))
				* rotateleftleg * glm::translate(unitMat, glm::vec3(0.0625f, -0.15f, 0.f)) * scaleMat * moveYMat;
			finalMat = baseTransform * partTransform;
			glUniformMatrix4fv(m_worldLoc, 1, GL_FALSE, glm::value_ptr(finalMat));
			glUniform3f(m_colorLoc, 1.f, 0.64f, 0.f);
			glDrawArrays(GL_TRIANGLES, 0, 36);
			//오른쪽
			partTransform = moveRightPos * glm::translate(unitMat, glm::vec3(0.0625f, 0.15f, 0.f))
				* rotaterightleg * glm::translate(unitMat, glm::vec3(-0.0625f, -0.15f, 0.f)) * scaleMat * moveYMat;
			finalMat = baseTransform * partTransform;
			glUniformMatrix4fv(m_worldLoc, 1, GL_FALSE, glm::value_ptr(finalMat));
			glUniform3f(m_colorLoc, 1.f, 0.64f, 0.f);
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}
		//몸통 그리기
		{
			glm::mat4 moveYMat = glm::translate(unitMat, glm::vec3(0.f, 0.5f, 0.f));
			glm::mat4 scaleMat = glm::scale(unitMat, glm::vec3(0.4f, 0.4f, 0.4f));

			glm::mat4 moveBottomUp = glm::translate(unitMat, glm::vec3(0.f, 0.15f, 0.f));

			glm::mat4 partTransform = moveBottomUp * scaleMat * moveYMat;
			finalMat = baseTransform * partTransform;
			glUniformMatrix4fv(m_worldLoc, 1, GL_FALSE, glm::value_ptr(finalMat));
			glUniform3f(m_colorLoc, 1.f, 1.0f, 0.f);
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}
		//머리 그리기
		{
			glm::mat4 moveYMat = glm::translate(unitMat, glm::vec3(0.f, 0.5f, 0.f));
			glm::mat4 scaleMat = glm::scale(unitMat, glm::vec3(0.3f, 0.3f, 0.3f));

			glm::mat4 moveBottomUp = glm::translate(unitMat, glm::vec3(0.f, 0.4f, 0.25f));
			
			glm::mat4 partTransform = moveBottomUp * scaleMat * moveYMat;
			finalMat = baseTransform * partTransform;
			glUniformMatrix4fv(m_worldLoc, 1, GL_FALSE, glm::value_ptr(finalMat));
			glUniform3f(m_colorLoc, 1.f, 1.0f, 0.f);

			glDrawArrays(GL_TRIANGLES, 0, 36);
		}
		//부리 그리기
		{
			glm::mat4 moveYMat = glm::translate(unitMat, glm::vec3(0.f, 0.5f, 0.f));
			glm::mat4 scaleMat = glm::scale(unitMat, glm::vec3(0.1f, 0.05f, 0.1f));

			glm::mat4 moveBottomUp = glm::translate(unitMat, glm::vec3(0.f, 0.48f, 0.4f));

			glm::mat4 partTransform = moveBottomUp * scaleMat * moveYMat;
			finalMat = baseTransform * partTransform;
			glUniformMatrix4fv(m_worldLoc, 1, GL_FALSE, glm::value_ptr(finalMat));
			glUniform3f(m_colorLoc, 1.f, 0.64f, 0.f);
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}
		//눈 그리기
		{
			glm::mat4 moveYMat = glm::translate(unitMat, glm::vec3(0.f, 0.6f, 0.f));	// 고정값..
			glm::mat4 scaleMat = glm::scale(unitMat, glm::vec3(0.05f, 0.05f, 0.05f));

			glm::mat4 moveleftPos = glm::translate(unitMat, glm::vec3(-0.07f, 0.5f, 0.38f));
			glm::mat4 moveRightPos = glm::translate(unitMat, glm::vec3(0.07f, 0.5f, 0.38f));

			glm::mat4 partTransform = moveleftPos * scaleMat * moveYMat;
			finalMat = baseTransform * partTransform;
			//왼쪽
			glUniformMatrix4fv(m_worldLoc, 1, GL_FALSE, glm::value_ptr(finalMat));
			glUniform3f(m_colorLoc, 0.f, 0.f, 0.f);
			glDrawArrays(GL_TRIANGLES, 0, 36);
			//오른쪽
			partTransform = moveRightPos * scaleMat * moveYMat;
			finalMat = baseTransform * partTransform;
			glUniformMatrix4fv(m_worldLoc, 1, GL_FALSE, glm::value_ptr(finalMat));
			glUniform3f(m_colorLoc, 0.f, 0.f, 0.f);
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}
	}

	if (not isBaby) {
		//닭 어른
		const glm::mat4 unitMat(1.f);
		glm::mat4 adultmoveY = glm::translate(unitMat, glm::vec3(0.f, 0.05f, 0.f));
		//다리 그리기
		{
			glm::mat4 moveYMat = glm::translate(unitMat, glm::vec3(0.f, 0.5f, 0.f));	// 고정값..
			glm::mat4 scaleMat = glm::scale(unitMat, glm::vec3(0.05f, 0.2f, 0.05f));

			glm::mat4 moveleftPos = glm::translate(unitMat, glm::vec3(-0.07f, 0.f, 0.f));
			glm::mat4 moveRightPos = glm::translate(unitMat, glm::vec3(0.07f, 0.f, 0.f));

			glm::mat4 rotateleftleg = glm::rotate(unitMat, glm::radians(rotateChicLeftLeg), glm::vec3(1.f, 0.f, 0.f));
			glm::mat4 rotaterightleg = glm::rotate(unitMat, glm::radians(rotateChicRightLeg), glm::vec3(1.f, 0.f, 0.f));

			glm::mat4 partTransform = moveleftPos * glm::translate(unitMat, glm::vec3(-0.0625f, 0.15f, 0.f))
				* rotateleftleg * glm::translate(unitMat, glm::vec3(0.0625f, -0.15f, 0.f)) * scaleMat * moveYMat;
			finalMat = baseTransform * adultmoveY * partTransform;

			//왼쪽
			
			glUniformMatrix4fv(m_worldLoc, 1, GL_FALSE, glm::value_ptr(finalMat));
			glUniform3f(m_colorLoc, 1.f, 0.64f, 0.f);
			glDrawArrays(GL_TRIANGLES, 0, 36);
			//오른쪽
			partTransform = moveRightPos * glm::translate(unitMat, glm::vec3(0.0625f, 0.15f, 0.f))
				* rotaterightleg * glm::translate(unitMat, glm::vec3(-0.0625f, -0.15f, 0.f)) * scaleMat * moveYMat;
			finalMat = baseTransform * adultmoveY * partTransform;
			glUniformMatrix4fv(m_worldLoc, 1, GL_FALSE, glm::value_ptr(finalMat));
			glUniform3f(m_colorLoc, 1.f, 0.64f, 0.f);
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}
		//몸통 그리기
		{
			glm::mat4 moveYMat = glm::translate(unitMat, glm::vec3(0.f, 0.5f, 0.f));
			glm::mat4 scaleMat = glm::scale(unitMat, glm::vec3(0.4f, 0.4f, 0.5f));

			glm::mat4 moveBottomUp = glm::translate(unitMat, glm::vec3(0.f, 0.18f, 0.f));

			glm::mat4 partTransform = moveBottomUp * scaleMat * moveYMat;
			finalMat = baseTransform * adultmoveY * partTransform;

			glUniformMatrix4fv(m_worldLoc, 1, GL_FALSE, glm::value_ptr(finalMat));
			glUniform3f(m_colorLoc, 1.f, 1.0f, 1.f);
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}
		//머리 그리기
		{
			glm::mat4 moveYMat = glm::translate(unitMat, glm::vec3(0.f, 0.5f, 0.f));
			glm::mat4 scaleMat = glm::scale(unitMat, glm::vec3(0.25f, 0.3f, 0.3f));

			glm::mat4 moveBottomUp = glm::translate(unitMat, glm::vec3(0.f, 0.5f, 0.25f));

			glm::mat4 partTransform = moveBottomUp * scaleMat * moveYMat;
			finalMat = baseTransform * adultmoveY * partTransform;
			glUniform3f(m_colorLoc, 1.f, 1.0f, 1.f);
			glUniformMatrix4fv(m_worldLoc, 1, GL_FALSE, glm::value_ptr(finalMat));

			glDrawArrays(GL_TRIANGLES, 0, 36);
		}
		//부리 그리기
		{
			glm::mat4 moveYMat = glm::translate(unitMat, glm::vec3(0.f, 0.5f, 0.f));
			glm::mat4 scaleMat = glm::scale(unitMat, glm::vec3(0.25f, 0.06f, 0.1f));

			glm::mat4 moveBottomUp = glm::translate(unitMat, glm::vec3(0.f, 0.57f, 0.4f));

			glm::mat4 partTransform = moveBottomUp * scaleMat * moveYMat;
			finalMat = baseTransform * adultmoveY * partTransform;
			glUniformMatrix4fv(m_worldLoc, 1, GL_FALSE, glm::value_ptr(finalMat));
			glUniform3f(m_colorLoc, 1.f, 0.64f, 0.f);
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}
		//닭벼슬 그리기
		{
			glm::mat4 moveYMat = glm::translate(unitMat, glm::vec3(0.f, 0.5f, 0.f));
			glm::mat4 scaleMat = glm::scale(unitMat, glm::vec3(0.08f, 0.06f, 0.12f));

			glm::mat4 moveBottomUp = glm::translate(unitMat, glm::vec3(0.f, 0.51f, 0.44f));

			glm::mat4 partTransform = moveBottomUp * scaleMat * moveYMat;
			finalMat = baseTransform * adultmoveY * partTransform;
			glUniformMatrix4fv(m_worldLoc, 1, GL_FALSE, glm::value_ptr(finalMat));
			glUniform3f(m_colorLoc, 1.f, 0.f, 0.f);
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}
		//눈 그리기
		{
			glm::mat4 moveYMat = glm::translate(unitMat, glm::vec3(0.f, 0.6f, 0.f));	// 고정값..
			glm::mat4 scaleMat = glm::scale(unitMat, glm::vec3(0.05f, 0.05f, 0.05f));

			glm::mat4 moveleftPos = glm::translate(unitMat, glm::vec3(-0.08f, 0.65f, 0.38f));
			glm::mat4 moveRightPos = glm::translate(unitMat, glm::vec3(0.08f, 0.65f, 0.38f));

			glm::mat4 partTransform = moveleftPos * scaleMat * moveYMat;
			finalMat = baseTransform * adultmoveY * partTransform;


			//왼쪽
			glUniformMatrix4fv(m_worldLoc, 1, GL_FALSE, glm::value_ptr(finalMat));
			glUniform3f(m_colorLoc, 0.f, 0.f, 0.f);
			glDrawArrays(GL_TRIANGLES, 0, 36);
			//오른쪽
			partTransform = moveRightPos * scaleMat * moveYMat;
			finalMat = baseTransform * adultmoveY * partTransform;
			glUniformMatrix4fv(m_worldLoc, 1, GL_FALSE, glm::value_ptr(finalMat));
			glUniform3f(m_colorLoc, 0.f, 0.f, 0.f);
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}
	}

}

void Chic::release()
{
}
