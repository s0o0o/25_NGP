#define STB_IMAGE_IMPLEMENTATION		// 단 하나의 .cpp 에만 define 해줘야 한다.. 중복 include 주의!
#include "ResourceManager.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>

#include <stb_image.h>

ResourceManager::ResourceManager()
{
}

ResourceManager::~ResourceManager()
{
	std::cout << " 모든 리소스 제거 중..." << std::endl;
	for (auto const& [name, id] : m_Shaders) {
		glDeleteProgram(id);	// 셰이더 제거
	}

	std::vector<GLuint> textureIDs;
	for (auto const& [name, id] : m_Textures) {
		textureIDs.push_back(id);
	}
	if (!textureIDs.empty()) {	// 텍스처들 제거
		glDeleteTextures(textureIDs.size(), textureIDs.data());
	}

	// vao, vbo 제거
	std::vector<GLuint> vboIDs;
	std::vector<GLuint> vaoIDs;
	for (auto const& [name, mesh] : m_Meshes) {
		vboIDs.push_back(mesh.VBO); 
		vaoIDs.push_back(mesh.VAO);
	}
	if (!vboIDs.empty()) {
		glDeleteBuffers(vboIDs.size(), vboIDs.data());
	}
	if (!vaoIDs.empty()) {
		glDeleteVertexArrays(vaoIDs.size(), vaoIDs.data());
	}
	std::cout << " 모든 리소스 제거 완료!" << std::endl;
}

void ResourceManager::loadAllResource()
{
	stbi_set_flip_vertically_on_load(true);
	std::cout << "Loading Shaders..." << std::endl;

	m_Shaders["default"] = makeShader("./Shader/vertex.glsl", "./Shader/fragment.glsl");
	m_Shaders["obj"] = makeShader("./Shader/obj_vertex.glsl", "./Shader/obj_fragment.glsl");
	m_Shaders["bg"] = makeShader("./Shader/bg_vertex.glsl", "./Shader/bg_fragment.glsl");
	m_Shaders["plain"] = makeShader("./Shader/plainVert.glsl", "./Shader/plainFrag.glsl");
	m_Shaders["tex"] = makeShader("./Shader/tex_vertex.glsl", "./Shader/tex_fragement.glsl");
	m_Shaders["animal"] = makeShader("./Shader/Animalvertex.glsl", "./Shader/Animalfragment.glsl");

	// 시작화면에 필요한거
	m_Shaders["font"] = makeShader("./Shader/fontvertex.glsl", "./Shader/fontfrag.glsl");

	std::cout << "Loading Meshes..." << std::endl;

	m_Meshes["sphere"] = initBuffer("./OBJ/sphere.obj");
	m_Meshes["cube"] = initCubeBuffer();
	m_Meshes["fence"] = initBuffer("./OBJ/fence.obj");
	m_Meshes["tree_up"] = initBuffer("./OBJ/tree_up.obj");
	m_Meshes["tree_bottom"] = initBuffer("./OBJ/tree_bottom.obj");
	m_Meshes["animal_cube"] = initBuffer("./OBJ/cube.obj"); // 동물 그리는.. 큐브!!!!
	m_Meshes["house_top"] = initBuffer("./OBJ/house_top.obj");
	m_Meshes["house_bottom"] = initBufferWithUV("./OBJ/house_bottom.obj");
	m_Meshes["sign_up"] = initBufferWithUV("./OBJ/sign_up.obj");	// 이거 농장 표시판 위아래
	m_Meshes["sign_bottom"] = initBufferWithUV("./OBJ/sign_bottom.obj");
	m_Meshes["ddong"] = initBuffer("./OBJ/ddong.obj");
	m_Meshes["coin"] = initBufferWithUV("./OBJ/coin.obj");
	m_Meshes["feed"] = initBufferWithUV("./OBJ/feed.obj");
	m_Meshes["background"] = initBackGroundBuffer(); // (배경 VAO)
	m_Meshes["textQuad"] = initTextQuad(); // 폰트그리는 vao

	// 시작화면에 필요한거
	m_Meshes["titleobj"] = initBufferWithUV("./OBJ/title.obj");

	m_Textures["farmsign_rest"] = initTexture("./Img/farmsign_rest.png");
	m_Textures["farmsign"] = initTexture("./Img/farmsign.png");
	m_Textures["grass"] = initTexture("./Img/grass.jpg");
	m_Textures["snowGrass"] = initTexture("./Img/snowGrass2.png");
	m_Textures["sky1"] = initTexture("./Img/sky1.png");
	m_Textures["sky2"] = initTexture("./Img/sky2.png");
	m_Textures["houseWood"] = initTexture("./Img/houseWood.png");
	m_Textures["store"] = initTexture("./Img/storesign.png");
	m_Textures["storeScene"] = initTexture("./Img/storeUI.png");
	m_Textures["ddongButton"] = initTexture("./Img/ddongButton1.png");
	m_Textures["feedButton"] = initTexture("./Img/feedButton1.png");
	m_Textures["coin"] = initTexture("./Img/coin.png");
	m_Textures["buy"] = initTexture("./Img/buy.png");
	m_Textures["feedpack"] = initTexture("./Img/feed.png");
	m_Textures["growNsell"] = initTexture("./Img/growNsell2.png");
	m_Textures["tino"] = initTexture("./Img/tino.png");
	m_Textures["inFarm"] = initTexture("./Img/inFarm.png");
	m_Textures["outFarm"] = initTexture("./Img/outFarm.png");

	// 시작화면에 필요한거
	m_Textures["pushSpaceBar"] = initTexture("./Img/enterSpaceBar.png");
	m_Textures["login"] = initTexture("./Img/loginUI.png");
	m_Textures["title"] = initTexture("./Img/textAnimalFarm.png");
	m_Textures["keyboard"] = initTexture("./Img/keyboard.png");
	m_Textures["keyboard_noAlpha"] = initTexture("./Img/keyboard_noalpha.png");

}

GLuint ResourceManager::getShader(const std::string& name)
{
	auto it = m_Shaders.find(name);
	if (it != m_Shaders.end()) {	// end가 아니라면 찾은거지
		return it->second; // it->second가 int값.. (키)
	}

	// 끝까지 갔는데 name 못찾은 경우는 오류..
	std::cerr << "\n\nError: Shader '" << name << "' not found in ResourceManager!" << std::endl;
	return 0;
}

GLuint ResourceManager::getTexture(const std::string& name)
{
	auto it = m_Textures.find(name);
	if (it != m_Textures.end()){
		return it->second;
	}
	// 끝까지 갔는데 name 못찾은 경우는 오류..
	std::cerr << "\n\nError: Textures '" << name << "' not found in ResourceManager!" << std::endl;
	return 0;
}

MeshData ResourceManager::getMesh(const std::string& name)
{
	auto it = m_Meshes.find(name);

	if (it != m_Meshes.end()) {
		return it->second; // it->second가 meshData..
	}
	
	std::cerr << "Error: Mesh '" << name << "' not found in ResourceManager!" << std::endl;
	return { 0, 0, 0 }; // 비어있는 MeshData 반환
}


MeshData ResourceManager::initBuffer(std::string objFilename)
{
	GLuint VAO;		// VAO는 어떻게 읽을지 설명..
	GLsizei vertexCount;
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	GLuint VBO;
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	std::vector<glm::vec3> verticies = readOBJ(objFilename);

	vertexCount = verticies.size() / 3;

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

	return { VAO, VBO, vertexCount };
}

MeshData ResourceManager::initBufferWithUV(std::string objFilename)
{
	GLuint VAO;
	GLsizei vertexCount;
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	GLuint VBO;
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	std::vector<glm::vec3> verticies = readOBJWithUV(objFilename);

	vertexCount = verticies.size() / 3;

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

	return { VAO, VBO, vertexCount };
}

MeshData ResourceManager::initCubeBuffer()
{
	GLuint VAO;
	GLsizei vertexCount;
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	GLuint VBO;
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	vertexCount = 36;

	float size = 0.5f;
	float vertexData[6 * 2 * 3 * 9]{ // 6면, 면당 2삼각형, 삼각형당 점3개, 점당 9개값 --> 324
		// px, py, pz,		tx, ty, tz=0,	 nx, ny, nz

		//Front					// 여기가 텍스처 좌표들 (지금 여기선 텍스처좌표는 왼쪽아래를 0,0 기준으로 삼음!!)
		-size, -size, size,		0, 0, 0,	1, 0, 0,
		size, -size, size,		1, 0, 0,	1, 0, 0,
		size, size, size,		1, 1, 0,	1, 0, 0,
		size, size, size,		1, 1, 0,	1, 0, 0,
		-size, size, size,		0, 1, 0,	1, 0, 0,
		-size, -size, size,		0, 0, 0,	1, 0, 0,
		//Back
		-size, -size, -size,	1, 0, 0,	0, 1, 0,
		-size, size, -size,		1, 1, 0,	0, 1, 0,
		size, size, -size,		0, 1, 0,	0, 1, 0,
		size, size, -size,		0, 1, 0,	0, 1, 0,
		size, -size, -size,		0, 0, 0,	0, 1, 0,
		-size, -size, -size,	1, 0, 0,	0, 1, 0,
		//Left
		-size, -size, size,		1, 0, 0,	0, 0, 1,
		-size, size, size,		1, 1, 0,	0, 0, 1,
		-size, size, -size,		0, 1, 0,	0, 0, 1,
		-size, size, -size,		0, 1, 0,	0, 0, 1,
		-size, -size, -size,	0, 0, 0,	0, 0, 1,
		-size, -size, size,		1, 0, 0,	0, 0, 1,
		//Right
		size, -size, size,		0, 0, 0,	1, 1, 0,
		size, -size, -size,		1, 0, 0,	1, 1, 0,
		size, size, -size,		1, 1, 0,	1, 1, 0,
		size, size, -size,		1, 1, 0,	1, 1, 0,
		size, size, size,		0, 1, 0,	1, 1, 0,
		size, -size, size,		0, 0, 0,	1, 1, 0,
		//Top
		-size, size, size,		0, 0, 0,	1, 0, 1,
		size, size, size,		1, 0, 0,	1, 0, 1,
		size, size, -size,		1, 1, 0,	1, 0, 1,
		size, size, -size,		1, 1, 0,	1, 0, 1,
		-size, size, -size,		0, 1, 0,	1, 0, 1,
		-size, size, size,		0, 0, 0,	1, 0, 1,
		//Bottom
		-size, -size, size,		0, 0, 0,	0, 1, 1,
		-size, -size, -size,	1, 0, 0,	0, 1, 1,
		size, -size, -size,		1, 1, 0,	0, 1, 1,
		size, -size, -size,		1, 1, 0,	0, 1, 1,
		size, -size, size,		0, 1, 0,	0, 1, 1,
		-size, -size, size,		0, 0, 0,	0, 1, 1
	};

	// CPU 메모리에 있는 데이터를, GPU 메모리에 복사
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 324, vertexData, GL_STATIC_DRAW);

	// 이 데이터가 어떤 데이터인지, 우리가 정의를 했기 때문에, openGL에 알려줘야 한다!
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 9, 0);
	// location, 갯수, 타입, nomalized?, 간격(바이트), 시작오프셋
	glEnableVertexAttribArray(0);

	// 이 데이터가 어떤 데이터인지, 우리가 정의를 했기 때문에, openGL에 알려줘야 한다!
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 9, reinterpret_cast<void*>(sizeof(float) * 3));
	// location, 갯수, 타입, nomalized?, 간격(바이트), 시작오프셋
	glEnableVertexAttribArray(1);

	// 이 데이터가 어떤 데이터인지, 우리가 정의를 했기 때문에, openGL에 알려줘야 한다!
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 9, reinterpret_cast<void*>(sizeof(float) * 6));
	// location, 갯수, 타입, nomalized?, 간격(바이트), 시작오프셋
	glEnableVertexAttribArray(2);

	return { VAO, VBO, vertexCount };
}

MeshData ResourceManager::initBackGroundBuffer() {
	GLuint VAO;
	GLsizei vertexCount = 6;
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	GLuint VBO;
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	float size = 1.0f;
	float vertexData[54]{ // 6면, 면당 2삼각형, 삼각형당 점3개, 점당 9개값 --> 324
		// px, py, pz,		tx, ty, tz=0,	 nx, ny, nz

		//Front					// 여기가 텍스처 좌표들 (지금 여기선 텍스처좌표는 왼쪽아래를 0,0 기준으로 삼음!!)
		-size * 2, -size,0,			0, 0, 0,	1, 0, 0,
		size * 2, -size, 0,			1, 0, 0,	1, 0, 0,
		size * 2, size, 0,			1, 1, 0,	1, 0, 0,
		size * 2, size, 0,			1, 1, 0,	1, 0, 0,
		-size * 2, size, 0,			0, 1, 0,	1, 0, 0,
		-size * 2, -size,0,			0, 0, 0,	1, 0, 0

	};

	// CPU 메모리에 있는 데이터를, GPU 메모리에 복사
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 54, vertexData, GL_STATIC_DRAW);
	// 이 데이터가 어떤 데이터인지, 우리가 정의를 했기 때문에, openGL에 알려줘야 한다!
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 9, 0);
	// location, 갯수, 타입, nomalized?, 간격(바이트), 시작오프셋
	glEnableVertexAttribArray(0);
	// 이 데이터가 어떤 데이터인지, 우리가 정의를 했기 때문에, openGL에 알려줘야 한다!
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 9, reinterpret_cast<void*>(sizeof(float) * 3));
	// location, 갯수, 타입, nomalized?, 간격(바이트), 시작오프셋
	glEnableVertexAttribArray(1);
	// 이 데이터가 어떤 데이터인지, 우리가 정의를 했기 때문에, openGL에 알려줘야 한다!
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 9, reinterpret_cast<void*>(sizeof(float) * 6));
	// location, 갯수, 타입, nomalized?, 간격(바이트), 시작오프셋
	glEnableVertexAttribArray(2);

	return { VAO, VBO, vertexCount };
}

MeshData ResourceManager::initTextQuad()
{
	float vertices[] = {
		// pos      // uv
		0.0f, 1.0f,   0.0f, 1.0f,
		0.0f, 0.0f,   0.0f, 0.0f,
		1.0f, 0.0f,   1.0f, 0.0f,

		0.0f, 1.0f,   0.0f, 1.0f,
		1.0f, 0.0f,   1.0f, 0.0f,
		1.0f, 1.0f,   1.0f, 1.0f
	};

	GLuint VAO, VBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// pos (location = 0)
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// uv (location = 1)
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glBindVertexArray(0);

	return { VAO, VBO, 6 };
}

GLuint ResourceManager::initTexture(std::string texFilename)
{
	GLuint textureID;
	glGenTextures(1, &textureID); // 3. 텍스처를 "1개"만 생성합니다.

	glBindTexture(GL_TEXTURE_2D, textureID); // 4. 방금 만든 ID를 바인딩합니다.

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	int width, height, nrChannels; // (변수명 'level'보다 'nrChannels'가 명확합니다)
	unsigned char* data = stbi_load(texFilename.c_str(), &width, &height, &nrChannels, 0);
	if (data) // data가 null이 아닐 때 (로드 성공)
	{
		GLenum format;
		if (nrChannels == 3)
			format = GL_RGB;
		else if (nrChannels == 4)
			format = GL_RGBA;
		else {
			std::cerr << "Texture load Error: " << texFilename << " (Unknown format)\n";
			stbi_image_free(data);
			return 0; // 오류 시 0 반환
		}
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D); // (선택 사항: 밉맵 생성)
	}
	else // 로드 실패
	{
		std::cerr << "Texture load Error: Failed to load texture at " << texFilename << "\n";
		return 0; // 오류 시 0 반환
	}

	stbi_image_free(data); // 5. stbi_load로 할당된 메모리 해제
	return textureID;
}

GLuint ResourceManager::makeShader(std::string vertexFilename, std::string fragmentFilename)
{
	// 쉐이더 만들자...

	GLint result;
	GLchar errorLog[512];
	GLuint vertexShader, fragmentShader;

	std::string vertexStr = readFile(vertexFilename);	// 이 변수는 지역변수라서
	const char* vertexSource{ vertexStr.c_str() };		// 이 변수에 저장되려면 위 변수가 소멸되면 안된다..!

	vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexSource, NULL);
	glCompileShader(vertexShader);

	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &result);
	if (!result)
	{
		glGetShaderInfoLog(vertexShader, 512, NULL, errorLog);
		std::cerr << vertexFilename << " ERROR: vertex shader error\n" << errorLog << std::endl;
	}
	else {
		std::cout << vertexFilename << " 컴파일 성공!" << std::endl;
	}

	std::string fragmentStr = readFile(fragmentFilename);
	const char* fragmentSource{ fragmentStr.c_str() };

	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentSource, NULL);
	glCompileShader(fragmentShader);

	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &result);
	if (!result)
	{
		glGetShaderInfoLog(fragmentShader, 512, NULL, errorLog);
		std::cerr << fragmentFilename << " ERROR: fragment shader error\n" << errorLog << std::endl;
	}
	else {
		std::cout << fragmentFilename << " 컴파일 성공!" << std::endl;
	}

	GLuint shaderID = glCreateProgram();
	glAttachShader(shaderID, vertexShader);		// 만든 vertex Shader를 쉐이더 프로그램에 추가해 준다
	glAttachShader(shaderID, fragmentShader);	// 만든 fragment Shader를 쉐이더 프로그램에 추가해 준다
	glLinkProgram(shaderID);					// 추가한 쉐이더들을 하나의 프로그램으로 생성한다!

	glDeleteShader(vertexShader);				// 이제 더이상 vertex Shaer는 필요없다...
	glDeleteShader(fragmentShader);				// 이제 더이상 fragment Shaer는 필요없다...

	glGetProgramiv(shaderID, GL_LINK_STATUS, &result);
	if (!result) {
		glGetProgramInfoLog(shaderID, 512, NULL, errorLog);
		std::cerr << vertexFilename << ", " << fragmentFilename << " ERROR: shader program 연결 실패\n" << errorLog << std::endl;
		//return;
	}
	else {
		std::cout << vertexFilename << ", " << fragmentFilename << " shader program 생성 성공!" << std::endl;
	}

	return shaderID;
}

std::string ResourceManager::readFile(std::string filename)
{
	std::ifstream in{ filename };
	if (!in) {
		std::cout << filename << "파일 존재하지 않음!" << std::endl;
		exit(1);
	}

	std::string str;
	std::string temp;
	while (std::getline(in, temp)) {
		str += temp;
		str.append(1, '\n');
	}

	return str;
}

std::vector<glm::vec3> ResourceManager::readOBJ(std::string filename)
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

std::vector<glm::vec3> ResourceManager::readOBJWithUV(std::string filename)
{
	std::ifstream in{ filename };
	if (!in) {
		std::cout << filename << " file read failed\n";
		exit(1);
	}

	std::vector<glm::vec3> vertex;
	std::vector<glm::vec3> tex;
	std::vector<glm::vec3> normal;
	std::vector<glm::ivec3> vindex;
	std::vector<glm::ivec3> tindex;
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
		}
		else if (str == "vt") {
			glm::vec3 t;
			for (int i = 0; i < 3; ++i) {
				std::string subStr;
				ss >> subStr;
				t[i] = std::stof(subStr);
			}
			tex.push_back(t);
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
			glm::ivec3 ft;
			glm::ivec3 fn;
			for (int i = 0; i < 3; ++i) {
				std::string substr;
				ss >> substr;
				std::stringstream subss{ substr };
				std::string vIdx;
				std::getline(subss, vIdx, '/');
				fv[i] = std::stoi(vIdx) - 1;
				std::getline(subss, vIdx, '/');
				ft[i] = std::stoi(vIdx) - 1;
				std::getline(subss, vIdx, '/');
				fn[i] = std::stoi(vIdx) - 1;
			}
			vindex.push_back(fv);
			tindex.push_back(ft);
			nindex.push_back(fn);
		}
	}

	std::vector<glm::vec3> data;
	for (int i = 0; i < vindex.size(); ++i) {
		data.push_back(vertex[vindex[i][0]]);
		data.push_back(tex[tindex[i][0]]);
		data.push_back(normal[nindex[i][0]]);
		data.push_back(vertex[vindex[i][1]]);
		data.push_back(tex[tindex[i][1]]);
		data.push_back(normal[nindex[i][1]]);
		data.push_back(vertex[vindex[i][2]]);
		data.push_back(tex[tindex[i][2]]);
		data.push_back(normal[nindex[i][2]]);
	}

	std::cout << filename << " File Read, " << data.size() / 3 << " Vertices Exists." << std::endl;
	return data;
}


