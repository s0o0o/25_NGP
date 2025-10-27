#include "Fox.h"
#include "PlayerObject.h"
#include <iostream>

Fox::Fox()
{
	float x = -8.f + static_cast<float>(std::rand()) / (static_cast<float>(RAND_MAX / 6.f));
	float z = -3.f + static_cast<float>(std::rand()) / (static_cast<float>(RAND_MAX / 5.f));
	MOVE_SPEED = 1.0f + static_cast<float>(std::rand()) / (static_cast<float>(RAND_MAX / 0.3f));

	isBaby = true;
	rangeLimit = 1.f;

	foxXDir = (rand() % 2 == 0) ? 1.f : -1.f;
	foxZDir = (rand() % 2 == 0) ? 1.f : -1.f;

	if (foxXDir > 0 && foxZDir > 0)
	{
		rotateFaceFox = 45.f;
		std::cout << " 오른쪽가면서 카메라앞으로" << std::endl;
	}
	else if (foxXDir > 0 && foxZDir < 0) {
		rotateFaceFox = 135.f;
		std::cout << " 오른쪽가면서 뒤쪽으로" << std::endl;
	}
	else if (foxXDir < 0 && foxZDir > 0) {
		rotateFaceFox = -45.f;
		std::cout << "왼쪽으로, 카메라쪽으로" << std::endl;
	}
	else if (foxXDir < 0 && foxZDir < 0) {
		rotateFaceFox = -135.f; // 225 -> -135
		std::cout << " 왼쪽, 뒤쪽으로" << std::endl;
	}
	rotateY(rotateFaceFox);

	rotateFoxLeftLeg = 0.f;
	rotateFoxRightLeg = 0.f;
	adultY = 0.f;
	adultScale = 1.f;
	isMaxRotateFox = false; // 각도 제한

	setPosition(x, 0.f, z);
}

Fox::~Fox()
{
}

void Fox::initialize()
{
	std::cout << "Fox init, shader : " << shader << std::endl;
	if (shader == 0) {
		std::cerr << "Error: Fox shader has not been set before calling initialize!\n";
		return;
	}

	m_worldLoc = glGetUniformLocation(shader, "modelTransform");
	m_colorLoc = glGetUniformLocation(shader, "colorTransform");

	std::cout << "Fox m_worldLoc : " << m_worldLoc << std::endl;
	std::cout << "Fox m_colorLoc : " << m_colorLoc << std::endl;

	if (m_worldLoc < 0) std::cerr << "Error: Fox modelTransform uniform not found!\n";
	if (m_colorLoc < 0) std::cerr << "Error: Fox colorTransform uniform not found!\n";

	m_viewLoc = glGetUniformLocation(shader, "viewTransform");
	m_projLoc = glGetUniformLocation(shader, "projTransform");
	m_lightPosLoc = glGetUniformLocation(shader, "lightPos");
}

void Fox::update(float elapseTime)
{
	//여우 다리 각도 회전
	if (isMaxRotateFox)
	{
		rotateFoxLeftLeg -= LEG_ROTATE_SPEED * elapseTime;;
		rotateFoxRightLeg += LEG_ROTATE_SPEED * elapseTime;;
		if (rotateFoxLeftLeg <= -30.f) {
			isMaxRotateFox = false;
		}
	}
	else if (not isMaxRotateFox) {
		rotateFoxLeftLeg += LEG_ROTATE_SPEED * elapseTime;;;
		rotateFoxRightLeg -= LEG_ROTATE_SPEED * elapseTime;;;
		if (rotateFoxLeftLeg >= 30.f) {
			isMaxRotateFox = true;
		}
	}

	// 위치 변경
	glm::vec3 currentPos = getPosition();
	float deltaX = foxXDir * MOVE_SPEED * elapseTime;
	float deltaZ = foxZDir * MOVE_SPEED * elapseTime;
	glm::vec3 nextPos = currentPos + glm::vec3(deltaX, 0.f, deltaZ);

	bool directionChanged = false;
	if (nextPos.x <= -15.f + rangeLimit) { nextPos.x = -15.f + rangeLimit; foxXDir = 1.f; directionChanged = true; }	// x왼쪽끝
	if (nextPos.x >= -1.f - rangeLimit) { nextPos.x = -1.f - rangeLimit;  foxXDir = -1.f; directionChanged = true; }	// x오른쪽끝
	if (nextPos.z <= -8.f + rangeLimit) { nextPos.z = -8.f + rangeLimit;  foxZDir = 1.f; directionChanged = true; }		// z나무쪽
	if (nextPos.z >= 9.f - rangeLimit) { nextPos.z = 9.f - rangeLimit;  foxZDir = -1.f; directionChanged = true; }		// z카메라쪽

	setPosition(nextPos);
	float currentFace = rotateFaceFox;

	// 벽 부딪혓을때
	if (directionChanged)
	{
		if (foxXDir > 0 && foxZDir > 0)
		{
			rotateFaceFox = 45.f;
			std::cout << "부딪힘, 오른쪽가면서 카메라앞으로" << std::endl;
		}
		else if (foxXDir > 0 && foxZDir < 0) {
			rotateFaceFox = 135.f;
			std::cout << "부딪힘, 오른쪽가면서 뒤쪽으로" << std::endl;
		}
		else if (foxXDir < 0 && foxZDir > 0) {
			rotateFaceFox = -45.f;
			std::cout << "부딪힘 왼쪽으로, 카메라쪽으로" << std::endl;
		}
		else if (foxXDir < 0 && foxZDir < 0) {
			rotateFaceFox = -135.f;
			std::cout << "부딪힘, 왼쪽, 뒤쪽으로" << std::endl;
		}
		rotateY(-currentFace + rotateFaceFox);
	}

	if (not isBaby) {
		adultY = 0.2f;
		adultScale = 2.f;
	}

}

void Fox::draw(const glm::mat4& viewMatrix, 
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
		// 아기 여우
		const glm::mat4 unitMat(1.f);
		//다리 그리기
		{
			glm::mat4 moveYMat = glm::translate(unitMat, glm::vec3(0.f, 0.5f, 0.f));	// 고정값..
			glm::mat4 scaleMat = glm::scale(unitMat, glm::vec3(0.13f, 0.25f, 0.13f));
			glm::mat4 moveleftPos = glm::translate(unitMat, glm::vec3(-0.15f, 0.f, -0.25f));
			glm::mat4 moveRightPos = glm::translate(unitMat, glm::vec3(0.15f, 0.f, -0.25f));

			glm::mat4 rotateleftleg = glm::rotate(unitMat, glm::radians(rotateFoxLeftLeg), glm::vec3(1.f, 0.f, 0.f));
			glm::mat4 rotaterightleg = glm::rotate(unitMat, glm::radians(rotateFoxRightLeg), glm::vec3(1.f, 0.f, 0.f));

			//왼쪽
			glm::mat4 partTransform = moveleftPos * glm::translate(unitMat, glm::vec3(-0.0625f, 0.15f, 0.f))
				* rotateleftleg * glm::translate(unitMat, glm::vec3(0.0625f, -0.15f, 0.f)) * scaleMat * moveYMat;
			finalMat = baseTransform * partTransform;
			glUniformMatrix4fv(m_worldLoc, 1, GL_FALSE, glm::value_ptr(finalMat));
			glUniform3f(m_colorLoc, 0.f, 0.f, 0.f);
			glDrawArrays(GL_TRIANGLES, 0, 36);

			//오른쪽
			partTransform = moveRightPos * glm::translate(unitMat, glm::vec3(0.0625f, 0.15f, 0.f))
				* rotaterightleg * glm::translate(unitMat, glm::vec3(-0.0625f, -0.15f, 0.f)) * scaleMat * moveYMat;
			finalMat = baseTransform * partTransform;
			glUniformMatrix4fv(m_worldLoc, 1, GL_FALSE, glm::value_ptr(finalMat));
			glUniform3f(m_colorLoc, 0.f, 0.f, 0.f);
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
			glUniform3f(m_colorLoc, 0.9f, 0.7f, 0.2f);
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

			glm::mat4 partTransform = moveleftPos * glm::translate(unitMat, glm::vec3(-0.0625f, 0.15f, 0.f))
				* rotateleftleg * glm::translate(unitMat, glm::vec3(0.0625f, -0.15f, 0.f)) * scaleMat * moveYMat;
			finalMat = baseTransform * partTransform;

			//왼쪽
			glUniformMatrix4fv(m_worldLoc, 1, GL_FALSE, glm::value_ptr(finalMat));
			glUniform3f(m_colorLoc, 0.f, 0.f, 0.f);
			glDrawArrays(GL_TRIANGLES, 0, 36);
			//오른쪽
			partTransform = moveRightPos * glm::translate(unitMat, glm::vec3(0.0625f, 0.15f, 0.f))
				* rotaterightleg * glm::translate(unitMat, glm::vec3(-0.0625f, -0.15f, 0.f)) * scaleMat * moveYMat;
			finalMat = baseTransform * partTransform;
			glUniformMatrix4fv(m_worldLoc, 1, GL_FALSE, glm::value_ptr(finalMat));
			glUniform3f(m_colorLoc, 0.f, 0.f, 0.f);
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}
		//머리 그리기
		{
			glm::mat4 moveYMat = glm::translate(unitMat, glm::vec3(0.f, 0.5f, 0.f));
			glm::mat4 scaleMat = glm::scale(unitMat, glm::vec3(0.45f, 0.45f, 0.45f));
			glm::mat4 moveBottomUp = glm::translate(unitMat, glm::vec3(0.f, 0.4f, 0.5f));

			glm::mat4 partTransform = moveBottomUp * scaleMat * moveYMat;
			finalMat = baseTransform * partTransform;
			glUniformMatrix4fv(m_worldLoc, 1, GL_FALSE, glm::value_ptr(finalMat));
			glUniform3f(m_colorLoc, 0.9f, 0.7f, 0.2f);
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}
		//주둥이 그리기
		{
			glm::mat4 moveYMat = glm::translate(unitMat, glm::vec3(0.f, 0.5f, 0.f));
			glm::mat4 scaleMat = glm::scale(unitMat, glm::vec3(0.2f, 0.1f, 0.1f));
			glm::mat4 moveBottomUp = glm::translate(unitMat, glm::vec3(0.f, 0.5f, 0.75f));

			glm::mat4 partTransform = moveBottomUp * scaleMat * moveYMat;
			finalMat = baseTransform * partTransform;
			glUniformMatrix4fv(m_worldLoc, 1, GL_FALSE, glm::value_ptr(finalMat));
			glUniform3f(m_colorLoc, 0.97f, 0.8f, 0.5f);
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}
		//코
		{
			glm::mat4 moveYMat = glm::translate(unitMat, glm::vec3(0.f, 0.5f, 0.f));	// 고정값..
			glm::mat4 scaleMat = glm::scale(unitMat, glm::vec3(0.1f, 0.05f, 0.05f));
			glm::mat4 moveleftPos = glm::translate(unitMat, glm::vec3(0.f, 0.55f, 0.8f));

			glm::mat4 partTransform = moveleftPos * scaleMat * moveYMat;
			finalMat = baseTransform * partTransform;

			glUniformMatrix4fv(m_worldLoc, 1, GL_FALSE, glm::value_ptr(finalMat));
			glUniform3f(m_colorLoc, 0.f, 0.f, 0.f);
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}
		//눈 그리기
		{

			glm::mat4 moveYMat = glm::translate(unitMat, glm::vec3(0.f, 0.5f, 0.f));	// 고정값..
			glm::mat4 scaleMat = glm::scale(unitMat, glm::vec3(0.08f, 0.06f, 0.1f));
			glm::mat4 moveleftPos = glm::translate(unitMat, glm::vec3(-0.15f, 0.6f, 0.75f));
			glm::mat4 moveRightPos = glm::translate(unitMat, glm::vec3(0.15f, 0.6f, 0.75f));

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
		//안광 그리기
		{

			glm::mat4 moveYMat = glm::translate(unitMat, glm::vec3(0.f, 0.5f, 0.f));	// 고정값..
			glm::mat4 scaleMat = glm::scale(unitMat, glm::vec3(0.05f, 0.06f, 0.1f));
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
		//귀 그리기
		{
			glm::mat4 moveYMat = glm::translate(unitMat, glm::vec3(0.f, 0.5f, 0.f));	// 고정값..
			glm::mat4 scaleMat = glm::scale(unitMat, glm::vec3(0.08f, 0.09f, 0.1f));
			glm::mat4 moveleftPos = glm::translate(unitMat, glm::vec3(-0.15f, 0.85f, 0.6f));
			glm::mat4 moveRightPos = glm::translate(unitMat, glm::vec3(0.15f, 0.85f, 0.6f));

			//왼쪽
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
	}
	else if (isBaby) {
		//아기여우
		const glm::mat4 unitMat(1.f);

		glm::mat4 adultMoveMat = glm::translate(unitMat, glm::vec3(0.f,adultY, 0.f));
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

			glm::mat4 partTransform = moveleftPos * glm::translate(unitMat, glm::vec3(-0.0625f, 0.15f, 0.f)
				* scaleFactor) * rotateleftleg * glm::translate(unitMat, glm::vec3(0.0625f, -0.15f, 0.f) * scaleFactor)
				* scaleMat * moveYMat;
			finalMat = baseTransform * adultMoveMat * partTransform;
			//왼쪽
			glUniformMatrix4fv(m_worldLoc, 1, GL_FALSE, glm::value_ptr(finalMat));
			glUniform3f(m_colorLoc, 0.f, 0.f, 0.f);
			glDrawArrays(GL_TRIANGLES, 0, 36);

			//오른쪽
			partTransform = moveRightPos * glm::translate(unitMat, glm::vec3(0.0625f, 0.15f, 0.f) * scaleFactor)
				* rotaterightleg * glm::translate(unitMat, glm::vec3(-0.0625f, -0.15f, 0.f) * scaleFactor) * scaleMat * moveYMat;
			finalMat = baseTransform * adultMoveMat * partTransform;
			glUniformMatrix4fv(m_worldLoc, 1, GL_FALSE, glm::value_ptr(finalMat));
			glUniform3f(m_colorLoc, 0.f, 0.f, 0.f);
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}
		//몸통 그리기
		{
			glm::mat4 moveYMat = glm::translate(unitMat, glm::vec3(0.f, 0.5f, 0.f) * scaleFactor);
			glm::mat4 scaleMat = glm::scale(unitMat, glm::vec3(0.55f, 0.55f, 0.8f) * scaleFactor);
			glm::mat4 moveBottomUp = glm::translate(unitMat, glm::vec3(0.f, 0.25f, 0.f) * scaleFactor);

			glm::mat4 partTransform = moveBottomUp * scaleMat * moveYMat;
			finalMat = baseTransform * adultMoveMat * partTransform;
			glUniformMatrix4fv(m_worldLoc, 1, GL_FALSE, glm::value_ptr(finalMat));
			glUniform3f(m_colorLoc, 0.9f, 0.7f, 0.2f);
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

			//왼쪽
			glm::mat4 partTransform = moveleftPos * glm::translate(unitMat, glm::vec3(-0.0625f, 0.15f, 0.f) * scaleFactor)
				* rotateleftleg * glm::translate(unitMat, glm::vec3(0.0625f, -0.15f, 0.f) * scaleFactor) * scaleMat * moveYMat;
			finalMat = baseTransform * adultMoveMat * partTransform;

			glUniformMatrix4fv(m_worldLoc, 1, GL_FALSE, glm::value_ptr(finalMat));
			glUniform3f(m_colorLoc, 0.f, 0.f, 0.f);
			glDrawArrays(GL_TRIANGLES, 0, 36);
			//오른쪽
			partTransform = moveRightPos * glm::translate(unitMat, glm::vec3(0.0625f, 0.15f, 0.f) * scaleFactor)
				* rotaterightleg * glm::translate(unitMat, glm::vec3(-0.0625f, -0.15f, 0.f) * scaleFactor) * scaleMat * moveYMat;
			finalMat = baseTransform * adultMoveMat * partTransform;

			glUniformMatrix4fv(m_worldLoc, 1, GL_FALSE, glm::value_ptr(finalMat));
			glUniform3f(m_colorLoc, 0.f, 0.f, 0.f);
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}
		//머리 그리기
		{
			glm::mat4 moveYMat = glm::translate(unitMat, glm::vec3(0.f, 0.5f, 0.f) * scaleFactor);
			glm::mat4 scaleMat = glm::scale(unitMat, glm::vec3(0.45f, 0.45f, 0.45f) * scaleFactor);
			glm::mat4 moveBottomUp = glm::translate(unitMat, glm::vec3(0.f, 0.4f, 0.5f) * scaleFactor);

			glm::mat4 partTransform = moveBottomUp * scaleMat * moveYMat;
			finalMat = baseTransform * adultMoveMat * partTransform;
			glUniformMatrix4fv(m_worldLoc, 1, GL_FALSE, glm::value_ptr(finalMat));
			glUniform3f(m_colorLoc, 0.9f, 0.7f, 0.2f);
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}
		//주둥이 그리기
		{
			glm::mat4 moveYMat = glm::translate(unitMat, glm::vec3(0.f, 0.5f, 0.f) * scaleFactor);
			glm::mat4 scaleMat = glm::scale(unitMat, glm::vec3(0.2f, 0.1f, 0.1f) * scaleFactor);
			glm::mat4 moveBottomUp = glm::translate(unitMat, glm::vec3(0.f, 0.5f, 0.75f) * scaleFactor);

			glm::mat4 partTransform = moveBottomUp * scaleMat * moveYMat;
			finalMat = baseTransform * adultMoveMat * partTransform;
			glUniformMatrix4fv(m_worldLoc, 1, GL_FALSE, glm::value_ptr(finalMat));
			glUniform3f(m_colorLoc, 0.97f, 0.8f, 0.5f);
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}
		//코
		{
			glm::mat4 moveYMat = glm::translate(unitMat, glm::vec3(0.f, 0.5f, 0.f) * scaleFactor);   // 고정값..
			glm::mat4 scaleMat = glm::scale(unitMat, glm::vec3(0.1f, 0.05f, 0.05f) * scaleFactor);
			glm::mat4 moveleftPos = glm::translate(unitMat, glm::vec3(0.f, 0.55f, 0.8f) * scaleFactor);

			glm::mat4 partTransform = moveleftPos * scaleMat * moveYMat;
			finalMat = baseTransform * adultMoveMat * partTransform;

			glUniformMatrix4fv(m_worldLoc, 1, GL_FALSE, glm::value_ptr(finalMat));
			glUniform3f(m_colorLoc, 0.f, 0.f, 0.f);
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}
		//눈 그리기
		{

			glm::mat4 moveYMat = glm::translate(unitMat, glm::vec3(0.f, 0.5f, 0.f) * scaleFactor);   // 고정값..
			glm::mat4 scaleMat = glm::scale(unitMat, glm::vec3(0.08f, 0.06f, 0.1f) * scaleFactor);
			glm::mat4 moveleftPos = glm::translate(unitMat, glm::vec3(-0.15f, 0.6f, 0.75f) * scaleFactor);
			glm::mat4 moveRightPos = glm::translate(unitMat, glm::vec3(0.15f, 0.6f, 0.75f) * scaleFactor);

			glm::mat4 partTransform = moveleftPos * scaleMat * moveYMat;
			finalMat = baseTransform * adultMoveMat * partTransform;
			//왼쪽
			glUniformMatrix4fv(m_worldLoc, 1, GL_FALSE, glm::value_ptr(finalMat));
			glUniform3f(m_colorLoc, 0.f, 0.f, 0.f);
			glDrawArrays(GL_TRIANGLES, 0, 36);
			//오른쪽
			partTransform = moveRightPos * scaleMat * moveYMat;
			finalMat = baseTransform * adultMoveMat * partTransform;
			glUniformMatrix4fv(m_worldLoc, 1, GL_FALSE, glm::value_ptr(finalMat));
			glUniform3f(m_colorLoc, 0.f, 0.f, 0.f);
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}
		//안광 그리기
		{

			glm::mat4 moveYMat = glm::translate(unitMat, glm::vec3(0.f, 0.5f, 0.f) * scaleFactor);   // 고정값..
			glm::mat4 scaleMat = glm::scale(unitMat, glm::vec3(0.05f, 0.06f, 0.1f) * scaleFactor);
			glm::mat4 moveleftPos = glm::translate(unitMat, glm::vec3(-0.2f, 0.6f, 0.75f) * scaleFactor);
			glm::mat4 moveRightPos = glm::translate(unitMat, glm::vec3(0.2f, 0.6f, 0.75f) * scaleFactor);

			//왼쪽
			glm::mat4 partTransform = moveleftPos * scaleMat * moveYMat;
			finalMat = baseTransform * adultMoveMat * partTransform;
			glUniformMatrix4fv(m_worldLoc, 1, GL_FALSE, glm::value_ptr(finalMat));
			glUniform3f(m_colorLoc, 1.f, 1.f, 1.f);
			glDrawArrays(GL_TRIANGLES, 0, 36);
			//오른쪽
			partTransform = moveRightPos * scaleMat * moveYMat;
			finalMat = baseTransform * adultMoveMat * partTransform;
			glUniformMatrix4fv(m_worldLoc, 1, GL_FALSE, glm::value_ptr(finalMat));
			glUniform3f(m_colorLoc, 1.f, 1.f, 1.f);
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}
		//귀 그리기
		{
			glm::mat4 moveYMat = glm::translate(unitMat, glm::vec3(0.f, 0.5f, 0.f) * scaleFactor);   // 고정값..
			glm::mat4 scaleMat = glm::scale(unitMat, glm::vec3(0.08f, 0.09f, 0.1f) * scaleFactor);
			glm::mat4 moveleftPos = glm::translate(unitMat, glm::vec3(-0.15f, 0.8f, 0.6f) * scaleFactor);
			glm::mat4 moveRightPos = glm::translate(unitMat, glm::vec3(0.15f, 0.8f, 0.6f) * scaleFactor);

			//왼쪽
			glm::mat4 partTransform = moveleftPos * scaleMat * moveYMat;
			finalMat = baseTransform * adultMoveMat * partTransform;
			glUniformMatrix4fv(m_worldLoc, 1, GL_FALSE, glm::value_ptr(finalMat));
			glUniform3f(m_colorLoc, 0.f, 0.f, 0.f);
			glDrawArrays(GL_TRIANGLES, 0, 36);
			//오른쪽
			partTransform = moveRightPos * scaleMat * moveYMat;
			finalMat = baseTransform * adultMoveMat * partTransform;
			glUniformMatrix4fv(m_worldLoc, 1, GL_FALSE, glm::value_ptr(finalMat));
			glUniform3f(m_colorLoc, 0.f, 0.f, 0.f);
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}
	}
}

void Fox::release()
{
}
