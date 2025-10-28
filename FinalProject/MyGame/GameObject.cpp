#include "GameObject.h"

#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <sstream>

GameObject::GameObject()
	: worldTransform(1.f)
{
	isNear = false;
	feedNum = 0;

	/*isBaby = true;
	isAdult = false;*/
}

GameObject::~GameObject()
{
}

void GameObject::initialize()
{
	
}

void GameObject::update(float elapsedTime)
{
}

void GameObject::draw(const glm::mat4& viewMatrix, 
	const glm::mat4& projMatrix, 
	const glm::vec3& lightPos) const
{
	glUseProgram(shader);

	// 모델변환행렬
	GLint modelLoc = glGetUniformLocation(shader, "modelTransform");
	if (modelLoc < 0)
		std::cout << "modelLoc 찾지 못함\n";
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(glm::mat4(1.f)));

	glBindVertexArray(VAO);
	glDrawArrays(GL_TRIANGLES, 0, vertexCount);
}

void GameObject::release()
{
}

void GameObject::setShader(GLuint my_shader)
{
	this->shader = my_shader;
}

void GameObject::setVAO(GLuint vao, GLsizei count)
{
	this->VAO = vao;
	this->vertexCount = count;
}

glm::vec3 GameObject::getPosition() const
{
	return glm::vec3(worldTransform[3]); 
	// 월드 변환.. 4*4행렬에서.. 3열에는 항상 이동정보가 저장되어있다!!
	// translate한 값이 저장되어있음..
	// opengl기준으로는 3열, 아마 다렉기준으로는 3행에 저장되어있으므로 객체가 어디에 
	// 위치해있는지 알고싶으면.. translate한 값만! 저장되어있는. 이동정보가 저장되어있는
	// 월드변환행렬 3열을 가져오면됨..									
}

void GameObject::setPosition(glm::vec3 position)
{
	worldTransform[3] = glm::vec4(position, 1.f);
}

void GameObject::setPosition(float x, float y, float z)
{
	setPosition(glm::vec3(x, y, z));
}

glm::vec3 GameObject::getLook() const
{
	return glm::vec3(worldTransform[2]);
}

glm::vec3 GameObject::getRight() const
{
	return glm::vec3(worldTransform[0]);
}


void GameObject::rotateY(float degrees)
{
	glm::vec3 originPos = getPosition();
	setPosition(0.f, 0.f, 0.f);
	worldTransform = glm::rotate(glm::mat4(1.f), glm::radians(degrees), glm::vec3(0.f, 1.f, 0.f)) * worldTransform;
	setPosition(originPos);
}

void GameObject::rotateX(float degrees)
{
	glm::vec3 originPos = getPosition();
	setPosition(0.f, 0.f, 0.f);
	glm::vec3 right = glm::vec3(worldTransform[0]); // 로컬 right 벡터
	worldTransform = glm::rotate(glm::mat4(1.f), glm::radians(degrees), right) * worldTransform;
	setPosition(originPos);
}


void GameObject::move(glm::vec3 dir, float value)
{
	glm::vec3 normalDir = glm::normalize(dir);
	worldTransform = glm::translate(glm::mat4(1.f), normalDir * value) * worldTransform;
}

void GameObject::moveForward(float value)
{
	move(getLook(), value);
}
