#pragma once

#include "library.h"
#include <string>
#include <map>
#include <vector>

struct MeshData {
	GLuint VAO;
	GLuint VBO;
	int vertexCount;
};

class ResourceManager
{
public:
	ResourceManager();
	~ResourceManager();

	void loadAllResource();

	// 리소스 가져오는 함수들
	GLuint getShader(const std::string& name);
	GLuint getTexture(const std::string& name);
	MeshData getMesh(const std::string& name);

private:
	std::map<std::string, GLuint>   m_Shaders;
	std::map<std::string, GLuint>   m_Textures;
	std::map<std::string, MeshData> m_Meshes; // (VAO + VertexCount)

	static MeshData initBuffer(std::string objFilename);
	static MeshData initBufferWithUV(std::string objFilename);
	static GLuint initTexture(std::string texFilename);
	static MeshData initCubeBuffer();
	static MeshData initBackGroundBuffer(); // 아마 배경용
	static MeshData initTextQuad(); // 2D 텍스트 출력용

	static GLuint makeShader(std::string vertexFilename, std::string fragmentFilename);
	static std::string readFile(std::string filename);
	static std::vector<glm::vec3> readOBJ(std::string filename);
	static std::vector<glm::vec3> readOBJWithUV(std::string filename);
};

