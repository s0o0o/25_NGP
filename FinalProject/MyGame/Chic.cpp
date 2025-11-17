#include "Chic.h"
#include "PlayerObject.h"
#include <iostream>

Chic::Chic()
{
	float x = -8.f + static_cast<float>(std::rand()) / (static_cast<float>(RAND_MAX / 7.f));
	float z = -5.f + static_cast<float>(std::rand()) / (static_cast<float>(RAND_MAX / 5.f));
	MOVE_SPEED = 1.0f + static_cast<float>(std::rand()) / (static_cast<float>(RAND_MAX / 0.3f));

	isBaby = true;

	rangeLimit = 1.f;

	rotateChicLeftLeg = 0.f;
	rotateChicRightLeg = 0.f;

	rotateChicLeftLeg = 0.f;
	rotateChicRightLeg = 0.f;

	isMaxRotateChic = false; // 각도 제한
	isMaxRotateAdultChic = false;

	chicXDir = (rand() % 2 == 0) ? 1.f : -1.f;
	chicZDir = (rand() % 2 == 0) ? 1.f : -1.f;

	if (chicXDir > 0 && chicZDir > 0)
	{
		rotateFaceChic = 45.f;
		//std::cout << " 오른쪽가면서 카메라앞으로" << std::endl;
	}
	else if (chicXDir > 0 && chicZDir < 0) {
		rotateFaceChic = 135.f;
		//std::cout << " 오른쪽가면서 뒤쪽으로" << std::endl;
	}
	else if (chicXDir < 0 && chicZDir > 0) {
		rotateFaceChic = -45.f;
		//std::cout << "왼쪽으로, 카메라쪽으로" << std::endl;
	}
	else if (chicXDir < 0 && chicZDir < 0) {
		rotateFaceChic = -135.f; // 225 -> -135
		//std::cout << " 왼쪽, 뒤쪽으로" << std::endl;
	}
	rotateY(rotateFaceChic);
	setPosition(x, 0.f, z);
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

	glm::vec3 currentPos = getPosition();	// 현재 위치

	float deltaX = chicXDir * MOVE_SPEED * elapseTime;
	float deltaZ = chicZDir * MOVE_SPEED * elapseTime;

	glm::vec3 nextPos = currentPos + glm::vec3(deltaX, 0.0f, deltaZ); // 다음 위치..

	bool directionChanged = false;
	if (nextPos.x <= -15.f + rangeLimit) { nextPos.x = -15.f + rangeLimit; chicXDir = 1.f; directionChanged = true; }	// x왼쪽끝
	if (nextPos.x >= -1.f - rangeLimit) { nextPos.x = -1.f - rangeLimit;  chicXDir = -1.f; directionChanged = true; }	// x오른쪽끝
	if (nextPos.z <= -8.f + rangeLimit) { nextPos.z = -8.f + rangeLimit;  chicZDir = 1.f; directionChanged = true; }		// z나무쪽
	if (nextPos.z >= 9.f - rangeLimit) { nextPos.z = 9.f - rangeLimit;   chicZDir = -1.f; directionChanged = true; }		// z카메라쪽

	setPosition(nextPos);

	float currentFace = rotateFaceChic;
	if (directionChanged) {
		if (chicXDir > 0 && chicZDir > 0)
		{
			rotateFaceChic = 45.f;
			//std::cout << "부딪힘, 오른쪽가면서 카메라앞으로" << std::endl;
		}
		else if (chicXDir > 0 && chicZDir < 0) {
			rotateFaceChic = 135.f;
			//std::cout << "부딪힘, 오른쪽가면서 뒤쪽으로" << std::endl;
		}
		else if (chicXDir < 0 && chicZDir > 0) {
			rotateFaceChic = -45.f;
			//std::cout << "부딪힘 왼쪽으로, 카메라쪽으로" << std::endl;
		}
		else if (chicXDir < 0 && chicZDir < 0) {
			rotateFaceChic = -135.f;
		//	std::cout << "부딪힘, 왼쪽, 뒤쪽으로" << std::endl;
		}
		rotateY(-currentFace + rotateFaceChic);
	}
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
