#pragma once

#include "library.h"

#include <string>
#include <vector>


class GameObject
{
protected:		// 부모 클래스가 될 것이다..
	glm::mat4 worldTransform;

	GLuint shader;
	GLuint VAO;
	GLsizei vertexCount;

public:
	GameObject();
	virtual ~GameObject();

	virtual void initialize();
	virtual void update(float elapsedTime);
	virtual void draw(const glm::mat4& viewMatrix, 
		const glm::mat4& projMatrix, 
		const glm::vec3& lightPos) const;
	virtual void release();

	void setShader(GLuint shader);
	void setVAO(GLuint vao, GLsizei count);

	glm::vec3 getPosition() const;
	void setPosition(glm::vec3 position);
	void setPosition(float x, float y, float z);	// 오버로딩

	glm::vec3 getLook() const;
	glm::vec3 getRight() const;

	void rotateY(float degrees);
	void rotateX(float degrees);

	void move(glm::vec3 dir, float value);
	void moveForward(float value);

	bool isNear;
	int feedNum;
	bool isBaby;
	bool isExist;
};

