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

void GameObject::initilize()
{
	
}

void GameObject::update(float elapsedTime)
{
}

void GameObject::draw() const
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
	VAO = vao;
	vertexCount = count;
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

	worldTransform = glm::rotate(glm::mat4(1.f), glm::radians(degrees), glm::vec3(1.f, 0.f, 0.f)) * worldTransform;

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

void GameObject::initBuffer(GLuint* VAO, GLsizei* vertexCount, std::string objFilename)
{
	glGenVertexArrays(1, VAO);		// 동적 할당....
	glBindVertexArray(*VAO);

	GLuint VBO;
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	std::vector<glm::vec3> verticies = readOBJ(objFilename);

	*vertexCount = verticies.size() / 3;

	// GPU는 하나의 컴퓨터와 같다... 내부에 gpu전용 cpu, 램, 등등 존재...

	// CPU 메모리에 있는 데이터를, GPU 메모리에 복사
	glBufferData(GL_ARRAY_BUFFER, sizeof(verticies[0]) * verticies.size(), verticies.data(), GL_STATIC_DRAW);

	// 이 데이터가 어떤 데이터인지, 우리가 정의를 했기 때문에, openGL에 알려줘야 한다!
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 9, 0);
	// location, 갯수, 타입, nomalized?, 간격(바이트), 시작오프셋
	glEnableVertexAttribArray(0);

	//// 이 데이터가 어떤 데이터인지, 우리가 정의를 했기 때문에, openGL에 알려줘야 한다!
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 9, reinterpret_cast<void*>(sizeof(float) * 3));
	// location, 갯수, 타입, nomalized?, 간격(바이트), 시작오프셋
	glEnableVertexAttribArray(1);

	//// 이 데이터가 어떤 데이터인지, 우리가 정의를 했기 때문에, openGL에 알려줘야 한다!
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 9, reinterpret_cast<void*>(sizeof(float) * 6));
	// location, 갯수, 타입, nomalized?, 간격(바이트), 시작오프셋
	glEnableVertexAttribArray(2);
}

std::vector<glm::vec3> GameObject::readOBJ(std::string filename)
{
	std::ifstream in{ filename };
	if (!in) {
		std::cout << filename << " file read failed\n";
		exit(1);
	}

	// c++ stream --> input output을 해주는 흐름?

	//srand(static_cast<unsigned int>(time(nullptr)));

	std::vector<glm::vec3> vertex;
	std::vector<glm::vec3> color;
	std::vector<glm::vec3> normal;
	std::vector<glm::ivec3> vindex;
	std::vector<glm::ivec3> nindex;
	while (in) {
		std::string line;
		std::getline(in, line);
		std::stringstream ss{ line };
		std::string str;
		ss >> str;
		if (str == "v") {
			glm::vec3 v;
			for (int i = 0; i < 3; ++i) {
				std::string subStr;
				ss >> subStr;
				v[i] = std::stof(subStr);
			}
			vertex.push_back(v);
			color.push_back(glm::vec3(rand() / float(RAND_MAX), rand() / float(RAND_MAX), rand() / float(RAND_MAX)));
		}
		else if (str == "vn") {
			glm::vec3 n;
			for (int i = 0; i < 3; ++i) {
				std::string subStr;
				ss >> subStr;
				n[i] = std::stof(subStr);
			}
			normal.push_back(n);
		}
		else if (str == "f") {
			glm::ivec3 fv;
			glm::ivec3 fn;
			for (int i = 0; i < 3; ++i) {
				std::string substr;
				ss >> substr;
				std::stringstream subss{ substr };
				std::string vIdx;
				std::getline(subss, vIdx, '/');
				fv[i] = std::stoi(vIdx) - 1;
				std::getline(subss, vIdx, '/');
				// 텍스처 건너뛰기
				std::getline(subss, vIdx, '/');
				fn[i] = std::stoi(vIdx) - 1;
			}
			vindex.push_back(fv);
			nindex.push_back(fn);
		}
	}

	std::vector<glm::vec3> data;
	for (int i = 0; i < vindex.size(); ++i) {
		data.push_back(vertex[vindex[i][0]]);
		data.push_back(color[vindex[i][0]]);
		data.push_back(normal[nindex[i][0]]);
		data.push_back(vertex[vindex[i][1]]);
		data.push_back(color[vindex[i][1]]);
		data.push_back(normal[nindex[i][1]]);
		data.push_back(vertex[vindex[i][2]]);
		data.push_back(color[vindex[i][2]]);
		data.push_back(normal[nindex[i][2]]);
	}

	std::cout << filename << " File Read, " << data.size() / 3 << " Vertices Exists." << std::endl;
	return data;
}



