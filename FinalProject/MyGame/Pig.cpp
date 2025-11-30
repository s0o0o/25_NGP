#include "Pig.h"
#include "PlayerObject.h"
#include <iostream>
#include <random>


Pig::Pig(int num) : pigNum(num)
{
	setPosition(0.f, 0.f, 0.f);
	m_targetPos = glm::vec3(0.f, 0.f, 0.f);
	m_prevPos = glm::vec3(0.f, 0.f, 0.f);
	targetAngle = 0.f;
	currentAngle = 0.f;

	MOVE_SPEED = 1.0f + static_cast<float>(std::rand()) / (static_cast<float>(RAND_MAX / 0.3f)); //

	isNear = false;
	isBaby = true;

	// 돼지
	rotatePigLeftLeg = 0.f;
	rotatePigRightLeg = 0.f;

	// 아기 돼지
	rotateBabyLeftLeg = 0.f;
	rotateBabyRightLeg = 0.f;
	rotateFaceBaby = 0.f;

	isMaxRotateBaby = false; // 각도 제한
	isMaxRotatePig = false;

}

// z방향.. 카메라에서 멀어질수록 -----
// 카메라에 가까워질수록 +++++


Pig::~Pig()
{
	//	delete this;
}

void Pig::initialize()
{
	//std::cout << "돼지 init, shader : " << shader << std::endl;
	if (shader == 0) {
		std::cerr << "Error: Pig shader has not been set before calling initialize!\n";
		return;
	}

	m_worldLoc = glGetUniformLocation(shader, "modelTransform");
	m_colorLoc = glGetUniformLocation(shader, "colorTransform");

	//std::cout << "돼지 m_worldLoc : " << m_worldLoc << std::endl;
	//std::cout << "돼지 m_colorLoc : " << m_colorLoc << std::endl;

	if (m_worldLoc < 0) std::cerr << "Error: Pig modelTransform uniform not found!\n";
	if (m_colorLoc < 0) std::cerr << "Error: Pig colorTransform uniform not found!\n";

	m_viewLoc = glGetUniformLocation(shader, "viewTransform");
	m_projLoc = glGetUniformLocation(shader, "projTransform");
	m_lightPosLoc = glGetUniformLocation(shader, "lightPos");
}

// 펜스 실 범위
// x : -15.f ~ 0.f
// z : -8.f ~ 10.f

void Pig::update(float elapseTime)
{
	if (isBaby)
	{
		// 아기돼지 다리 각도 회전
		if (isMaxRotateBaby)
		{
			rotateBabyLeftLeg -= LEG_ROTATE_SPEED * elapseTime;
			rotateBabyRightLeg += LEG_ROTATE_SPEED * elapseTime;
			if (rotateBabyLeftLeg <= -10.f) {
				isMaxRotateBaby = false;
			}
		}
		else if (not isMaxRotateBaby) {
			rotateBabyLeftLeg += LEG_ROTATE_SPEED * elapseTime;
			rotateBabyRightLeg -= LEG_ROTATE_SPEED * elapseTime;
			if (rotateBabyLeftLeg >= 10.f) {
				isMaxRotateBaby = true;
			}
		}
	}
	else if (not isBaby) {
		// 돼지 다리 각도 회전
		if (isMaxRotatePig)
		{
			rotatePigLeftLeg -= LEG_ROTATE_SPEED * elapseTime;
			rotatePigRightLeg += LEG_ROTATE_SPEED * elapseTime;
			if (rotatePigLeftLeg <= -30.f) {
				isMaxRotatePig = false;
			}
		}
		else if (not isMaxRotatePig) {
			rotatePigLeftLeg += LEG_ROTATE_SPEED * elapseTime;
			rotatePigRightLeg -= LEG_ROTATE_SPEED * elapseTime;
			if (rotatePigLeftLeg >= 30.f) {
				isMaxRotatePig = true;
			}
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

void Pig::setTargetPosition(float x, float z)
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

void Pig::draw(const glm::mat4& viewMatrix,
	const glm::mat4& projMatrix, const glm::vec3& lightPos) const
{
	glUseProgram(shader);
	glBindVertexArray(VAO);

	glUniformMatrix4fv(m_viewLoc, 1, GL_FALSE, glm::value_ptr(viewMatrix));
	glUniformMatrix4fv(m_projLoc, 1, GL_FALSE, glm::value_ptr(projMatrix));
	glUniform3f(m_lightPosLoc, lightPos.x, lightPos.y, lightPos.z);

	glm::mat4 baseTransform = worldTransform;
	glm::mat4 finalMat;

	if (isBaby) { // 아기 돼지
		const glm::mat4 unitMat(1.f);
		// 다리 그리기 뒷다리
		{
			glm::mat4 moveYMat = glm::translate(unitMat, glm::vec3(0.f, 0.5f, 0.f));	// 고정값..
			glm::mat4 scaleMat = glm::scale(unitMat, glm::vec3(0.1f, 0.15f, 0.1f));

			glm::mat4 moveleftPos = glm::translate(unitMat, glm::vec3(-0.15f, 0.f, -0.15f));
			glm::mat4 moveRightPos = glm::translate(unitMat, glm::vec3(0.15f, 0.f, -0.15f));

			glm::mat4 rotateleftleg = glm::rotate(unitMat, glm::radians(rotateBabyLeftLeg), glm::vec3(1.f, 0.f, 0.f));
			glm::mat4 rotaterightleg = glm::rotate(unitMat, glm::radians(rotateBabyRightLeg), glm::vec3(1.f, 0.f, 0.f));

			// 왼쪽
			glm::mat4 partTransform = moveleftPos * rotateleftleg * scaleMat * moveYMat;
			finalMat = baseTransform * partTransform;
			glUniformMatrix4fv(m_worldLoc, 1, GL_FALSE, glm::value_ptr(finalMat));
			glUniform3f(m_colorLoc, 1.f, 0.5f, 0.7f);
			glDrawArrays(GL_TRIANGLES, 0, vertexCount);
			// 오른쪽
			partTransform = moveRightPos * rotaterightleg * scaleMat * moveYMat;
			finalMat = baseTransform * partTransform;
			glUniformMatrix4fv(m_worldLoc, 1, GL_FALSE, glm::value_ptr(finalMat));
			glUniform3f(m_colorLoc, 1.f, 0.5f, 0.7f);
			glDrawArrays(GL_TRIANGLES, 0, vertexCount);
		}
		// 몸통 그리기
		{
			glm::mat4 moveYMat = glm::translate(unitMat, glm::vec3(0.f, 0.5f, 0.f));
			glm::mat4 scaleMat = glm::scale(unitMat, glm::vec3(0.4f, 0.4f, 0.45f));
			glm::mat4 moveBottomUp = glm::translate(unitMat, glm::vec3(0.f, 0.15f, 0.f));

			glm::mat4 partTransform = moveBottomUp * scaleMat * moveYMat;
			finalMat = baseTransform * partTransform;
			//glm::mat4 finalMat = moveBottomUp * scaleMat * moveYMat;

			glUniformMatrix4fv(m_worldLoc, 1, GL_FALSE, glm::value_ptr(finalMat));
			glUniform3f(m_colorLoc, 1.f, 0.5f, 0.7f);
			glDrawArrays(GL_TRIANGLES, 0, vertexCount);
		}
		{
			// 앞다리
			glm::mat4 moveYMat = glm::translate(unitMat, glm::vec3(0.f, 0.5f, 0.f));	// 고정값..
			glm::mat4 scaleMat = glm::scale(unitMat, glm::vec3(0.1f, 0.15f, 0.1f));

			glm::mat4 moveleftPos = glm::translate(unitMat, glm::vec3(-0.15f, 0.f, 0.3f));
			glm::mat4 moveRightPos = glm::translate(unitMat, glm::vec3(0.15f, 0.f, 0.3f));

			glm::mat4 rotateleftleg = glm::rotate(unitMat, glm::radians(rotateBabyLeftLeg), glm::vec3(1.f, 0.f, 0.f));
			glm::mat4 rotaterightleg = glm::rotate(unitMat, glm::radians(rotateBabyRightLeg), glm::vec3(1.f, 0.f, 0.f));


			glm::mat4 partTransform = moveleftPos * rotateleftleg * scaleMat * moveYMat; // 원점 기준 부분 변환
			finalMat = baseTransform * partTransform;
			// 왼쪽
			/*glm::mat4 partTransform = moveleftPos * glm::translate(unitMat, glm::vec3(-0.0625f, 0.15f, 0.f))
				* rotateleftleg * glm::translate(unitMat, glm::vec3(0.0625f, -0.15f, 0.f)) **/

			glUniformMatrix4fv(m_worldLoc, 1, GL_FALSE, glm::value_ptr(finalMat));
			glUniform3f(m_colorLoc, 1.f, 0.5f, 0.7f);
			glDrawArrays(GL_TRIANGLES, 0, vertexCount);

			partTransform = moveRightPos * rotaterightleg * scaleMat * moveYMat;
			finalMat = baseTransform * partTransform;
			// 오른쪽
			//finalMat = moveFinal * rotateDir * moveRightPos * glm::translate(unitMat, glm::vec3(0.0625f, 0.15f, 0.f)) 
			// * rotaterightleg * glm::translate(unitMat, glm::vec3(-0.0625f, -0.15f, 0.f)) * scaleMat * moveYMat;
			glUniformMatrix4fv(m_worldLoc, 1, GL_FALSE, glm::value_ptr(finalMat));
			glUniform3f(m_colorLoc, 1.f, 0.5f, 0.7f);
			glDrawArrays(GL_TRIANGLES, 0, vertexCount);
		}
		// 머리 그리기
		{
			glm::mat4 moveYMat = glm::translate(unitMat, glm::vec3(0.f, 0.5f, 0.f));
			glm::mat4 scaleMat = glm::scale(unitMat, glm::vec3(0.5f, 0.5f, 0.5f));
			glm::mat4 moveBottomUp = glm::translate(unitMat, glm::vec3(0.f, 0.15f, 0.45f));

			glm::mat4 partTransform = moveBottomUp * scaleMat * moveYMat;
			finalMat = baseTransform * partTransform;
			//glm::mat4 finalMat = moveFinal * rotateDir *

			glUniformMatrix4fv(m_worldLoc, 1, GL_FALSE, glm::value_ptr(finalMat));
			glUniform3f(m_colorLoc, 1.f, 0.5f, 0.7f);
			glDrawArrays(GL_TRIANGLES, 0, vertexCount);
		}
		// 코 그리기
		{
			glm::mat4 moveYMat = glm::translate(unitMat, glm::vec3(0.f, 0.5f, 0.f));
			glm::mat4 scaleMat = glm::scale(unitMat, glm::vec3(0.2f, 0.1f, 0.1f));
			glm::mat4 moveBottomUp = glm::translate(unitMat, glm::vec3(0.f, 0.28f, 0.7f));
			glm::mat4 partTransform = moveBottomUp * scaleMat * moveYMat;
			finalMat = baseTransform * partTransform;
			glUniformMatrix4fv(m_worldLoc, 1, GL_FALSE, glm::value_ptr(finalMat));
			glUniform3f(m_colorLoc, 1.f, 0.5f, 0.7f);

			glDrawArrays(GL_TRIANGLES, 0, vertexCount);
		}
		//콧구멍 그리기
		{
			glm::mat4 moveYMat = glm::translate(unitMat, glm::vec3(0.f, 0.5f, 0.f));	// 고정값..
			glm::mat4 scaleMat = glm::scale(unitMat, glm::vec3(0.05f, 0.05f, 0.05f));
			glm::mat4 moveleftPos = glm::translate(unitMat, glm::vec3(-0.05f, 0.30f, 0.73f));
			glm::mat4 moveRightPos = glm::translate(unitMat, glm::vec3(0.05f, 0.30f, 0.73f));

			// 왼쪽
			glm::mat4 partTransform = moveleftPos * scaleMat * moveYMat;
			finalMat = baseTransform * partTransform;
			glUniformMatrix4fv(m_worldLoc, 1, GL_FALSE, glm::value_ptr(finalMat));
			glUniform3f(m_colorLoc, 0.f, 0.f, 0.f);
			glDrawArrays(GL_TRIANGLES, 0, vertexCount);

			// 오른쪽
			partTransform = moveRightPos * scaleMat * moveYMat;
			finalMat = baseTransform * partTransform;
			glUniformMatrix4fv(m_worldLoc, 1, GL_FALSE, glm::value_ptr(finalMat));
			glUniform3f(m_colorLoc, 0.f, 0.f, 0.f);
			glDrawArrays(GL_TRIANGLES, 0, vertexCount);
		}
		//눈 그리기
		{
			glm::mat4 moveYMat = glm::translate(unitMat, glm::vec3(0.f, 0.5f, 0.f));	// 고정값..
			glm::mat4 scaleMat = glm::scale(unitMat, glm::vec3(0.08f, 0.08f, 0.08f));
			glm::mat4 moveleftPos = glm::translate(unitMat, glm::vec3(-0.15f, 0.35f, 0.68f));
			glm::mat4 moveRightPos = glm::translate(unitMat, glm::vec3(0.15f, 0.35f, 0.68f));

			// 왼쪽

			glm::mat4 partTransform = moveleftPos * scaleMat * moveYMat;
			finalMat = baseTransform * partTransform;

			glUniformMatrix4fv(m_worldLoc, 1, GL_FALSE, glm::value_ptr(finalMat));
			glUniform3f(m_colorLoc, 0.f, 0.f, 0.f);
			glDrawArrays(GL_TRIANGLES, 0, vertexCount);
			// 오른쪽
			partTransform = moveRightPos * scaleMat * moveYMat;
			finalMat = baseTransform * partTransform;
			glUniformMatrix4fv(m_worldLoc, 1, GL_FALSE, glm::value_ptr(finalMat));
			glUniform3f(m_colorLoc, 0.f, 0.f, 0.f);
			glDrawArrays(GL_TRIANGLES, 0, vertexCount);
		}
		//안광 그리기
		{
			glm::mat4 moveYMat = glm::translate(unitMat, glm::vec3(0.f, 0.5f, 0.f));	// 고정값..
			glm::mat4 scaleMat = glm::scale(unitMat, glm::vec3(0.035f, 0.035f, 0.04f));
			glm::mat4 moveleftPos = glm::translate(unitMat, glm::vec3(-0.135f, 0.38f, 0.72f));
			glm::mat4 moveRightPos = glm::translate(unitMat, glm::vec3(0.165f, 0.38f, 0.72f));

			// 왼쪽
			glm::mat4 partTransform = moveleftPos * scaleMat * moveYMat;
			finalMat = baseTransform * partTransform;
			glUniformMatrix4fv(m_worldLoc, 1, GL_FALSE, glm::value_ptr(finalMat));
			glUniform3f(m_colorLoc, 1.f, 1.f, 1.f);
			glDrawArrays(GL_TRIANGLES, 0, vertexCount);
			// 오른쪽
			partTransform = moveRightPos * scaleMat * moveYMat;
			finalMat = baseTransform * partTransform;
			glUniformMatrix4fv(m_worldLoc, 1, GL_FALSE, glm::value_ptr(finalMat));
			glUniform3f(m_colorLoc, 1.f, 1.f, 1.f);
			glDrawArrays(GL_TRIANGLES, 0, vertexCount);
		}
	}

	//어른돼지
	else if (not isBaby) {
		const glm::mat4 unitMat(1.f);
		{
			glm::mat4 moveYMat = glm::translate(unitMat, glm::vec3(0.f, 0.5f, 0.f));	// 고정값..
			glm::mat4 scaleMat = glm::scale(unitMat, glm::vec3(0.18f, 0.25f, 0.18f));

			glm::mat4 moveleftPos = glm::translate(unitMat, glm::vec3(-0.18f, 0.f, -0.25f));
			glm::mat4 moveRightPos = glm::translate(unitMat, glm::vec3(0.18f, 0.f, -0.25f));

			glm::mat4 rotateleftleg = glm::rotate(unitMat, glm::radians(rotatePigLeftLeg), glm::vec3(1.f, 0.f, 0.f));
			glm::mat4 rotaterightleg = glm::rotate(unitMat, glm::radians(rotatePigRightLeg), glm::vec3(1.f, 0.f, 0.f));

			glm::mat4 partTransform = moveleftPos * rotateleftleg * scaleMat * moveYMat;
			finalMat = baseTransform * partTransform;

			//왼쪽
			/*finalMat = moveleftPos * glm::translate(unitMat, glm::vec3(-0.0625f, 0.15f, 0.f))
				* rotateleftleg * glm::translate(unitMat, glm::vec3(0.0625f, -0.15f, 0.f)) * scaleMat * moveYMat;*/
			glUniformMatrix4fv(m_worldLoc, 1, GL_FALSE, glm::value_ptr(finalMat));
			glUniform3f(m_colorLoc, 1.f, 0.5f, 0.7f);
			glDrawArrays(GL_TRIANGLES, 0, 36);

			//오른쪽
			partTransform = moveRightPos * rotaterightleg * scaleMat * moveYMat;
			finalMat = baseTransform * partTransform;
			/*finalMat = moveFinal * rotateDir * moveRightPos * glm::translate(unitMat, glm::vec3(0.0625f, 0.15f, 0.f))
				* rotaterightleg * glm::translate(unitMat, glm::vec3(-0.0625f, -0.15f, 0.f)) * scaleMat * moveYMat;*/
			glUniformMatrix4fv(m_worldLoc, 1, GL_FALSE, glm::value_ptr(finalMat));
			glUniform3f(m_colorLoc, 1.f, 0.5f, 0.7f);
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}
		//몸통 그리기
		{
			glm::mat4 moveYMat = glm::translate(unitMat, glm::vec3(0.f, 0.5f, 0.f));
			glm::mat4 scaleMat = glm::scale(unitMat, glm::vec3(0.55f, 0.55f, 0.8f));

			glm::mat4 moveBottomUp = glm::translate(unitMat, glm::vec3(0.f, 0.25f, 0.f));

			glm::mat4 partTransform = moveBottomUp * scaleMat * moveYMat;
			finalMat = baseTransform * partTransform;
			glUniformMatrix4fv(m_worldLoc, 1, GL_FALSE, glm::value_ptr(finalMat));
			glUniform3f(m_colorLoc, 1.f, 0.5f, 0.7f);
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


			glm::mat4 partTransform = moveleftPos * rotateleftleg * scaleMat * moveYMat;
			finalMat = baseTransform * partTransform;

			//왼쪽
			/*finalMat = moveFinal * rotateDir * moveleftPos * glm::translate(unitMat, glm::vec3(-0.0625f, 0.15f, 0.f))
				* rotateleftleg * glm::translate(unitMat, glm::vec3(0.0625f, -0.15f, 0.f)) * scaleMat * moveYMat;*/
			glUniformMatrix4fv(m_worldLoc, 1, GL_FALSE, glm::value_ptr(finalMat));
			glUniform3f(m_colorLoc, 1.f, 0.5f, 0.7f);
			glDrawArrays(GL_TRIANGLES, 0, 36);
			//오른쪽

			partTransform = moveRightPos * rotaterightleg * scaleMat * moveYMat;
			finalMat = baseTransform * partTransform;
			/*finalMat = moveFinal * rotateDir * moveRightPos * glm::translate(unitMat, glm::vec3(0.0625f, 0.15f, 0.f))
				* rotaterightleg * glm::translate(unitMat, glm::vec3(-0.0625f, -0.15f, 0.f)) * scaleMat * moveYMat;*/
			glUniformMatrix4fv(m_worldLoc, 1, GL_FALSE, glm::value_ptr(finalMat));
			glUniform3f(m_colorLoc, 1.f, 0.5f, 0.7f);
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}
		//머리 그리기
		{
			glm::mat4 moveYMat = glm::translate(unitMat, glm::vec3(0.f, 0.5f, 0.f));
			glm::mat4 scaleMat = glm::scale(unitMat, glm::vec3(0.5f, 0.5f, 0.5f));

			glm::mat4 moveBottomUp = glm::translate(unitMat, glm::vec3(0.f, 0.4f, 0.5f));

			glm::mat4 partTransform = moveBottomUp * scaleMat * moveYMat;
			finalMat = baseTransform * partTransform;

			glUniformMatrix4fv(m_worldLoc, 1, GL_FALSE, glm::value_ptr(finalMat));
			glUniform3f(m_colorLoc, 1.f, 0.5f, 0.7f);
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}
		//코 그리기
		{
			glm::mat4 moveYMat = glm::translate(unitMat, glm::vec3(0.f, 0.5f, 0.f));
			glm::mat4 scaleMat = glm::scale(unitMat, glm::vec3(0.2f, 0.1f, 0.1f));

			glm::mat4 moveBottomUp = glm::translate(unitMat, glm::vec3(0.f, 0.55f, 0.75f));

			glm::mat4 partTransform = moveBottomUp * scaleMat * moveYMat;
			finalMat = baseTransform * partTransform;

			glUniformMatrix4fv(m_worldLoc, 1, GL_FALSE, glm::value_ptr(finalMat));
			glUniform3f(m_colorLoc, 1.f, 0.5f, 0.7f);
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}
		//콧구멍 그리기
		{
			glm::mat4 moveYMat = glm::translate(unitMat, glm::vec3(0.f, 0.5f, 0.f));	// 고정값..
			glm::mat4 scaleMat = glm::scale(unitMat, glm::vec3(0.05f, 0.05f, 0.05f));

			glm::mat4 moveleftPos = glm::translate(unitMat, glm::vec3(-0.05f, 0.58f, 0.8f));
			glm::mat4 moveRightPos = glm::translate(unitMat, glm::vec3(0.05f, 0.58f, 0.8f));

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
		//눈 그리기
		{
			glm::mat4 moveYMat = glm::translate(unitMat, glm::vec3(0.f, 0.5f, 0.f));	// 고정값..
			glm::mat4 scaleMat = glm::scale(unitMat, glm::vec3(0.08f, 0.1f, 0.1f));

			glm::mat4 moveleftPos = glm::translate(unitMat, glm::vec3(-0.15f, 0.6f, 0.75f));
			glm::mat4 moveRightPos = glm::translate(unitMat, glm::vec3(0.15f, 0.6f, 0.75f));

			glm::mat4 partTransform = moveleftPos * scaleMat * moveYMat;
			finalMat = baseTransform * partTransform;
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
		//안광 그리기
		{
			glm::mat4 moveYMat = glm::translate(unitMat, glm::vec3(0.f, 0.5f, 0.f));	// 고정값..
			glm::mat4 scaleMat = glm::scale(unitMat, glm::vec3(0.05f, 0.1f, 0.1f));

			glm::mat4 moveleftPos = glm::translate(unitMat, glm::vec3(-0.2f, 0.6f, 0.75f));
			glm::mat4 moveRightPos = glm::translate(unitMat, glm::vec3(0.2f, 0.6f, 0.75f));
			//왼쪽
			glm::mat4 partTransform = moveleftPos * scaleMat * moveYMat;
			finalMat = baseTransform * partTransform;
			glUniformMatrix4fv(m_worldLoc, 1, GL_FALSE, glm::value_ptr(finalMat));
			glUniform3f(m_colorLoc, 1.f, 1.f, 1.f);
			glDrawArrays(GL_TRIANGLES, 0, 36);
			//오른쪽
			partTransform = moveRightPos * scaleMat * moveYMat;
			finalMat = baseTransform * partTransform;
			glUniformMatrix4fv(m_worldLoc, 1, GL_FALSE, glm::value_ptr(finalMat));
			glUniform3f(m_colorLoc, 1.f, 1.f, 1.f);
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}
	}
}

void Pig::release()
{
}

