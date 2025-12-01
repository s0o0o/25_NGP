#pragma once
#include "gameScene.h"
#include "Client.h"

#include "ResourceManager.h"
#include "sceneManager.h"

#include "PlayerObject.h"
#include "Pig.h"
#include "Alpaca.h"
#include "Penguin.h"
#include "Chic.h"
#include "Fox.h"
#include <random>
#include <iostream>

#include <stb_image.h>	// 얘 추가할말?

const float LIGHT_SPEED = 0.5f;
const float CAMERA_ANIM_SPEED = 10.0f;
const float SNOW_SPEED = 5.0f;
gameScene* g_gameScene = nullptr;
extern int g_myPlayerID;

gameScene::gameScene(int winWidth, int winHeight)
{
	player = nullptr;
	g_gameScene = this;
}

gameScene::~gameScene()
{
}

void gameScene::sceneOnEnter()	// 이게 init역할
{
	glutSetCursor(GLUT_CURSOR_NONE);	// gamescene입장할때 마우스 숨기기..
	width = 1600;
	height = 900;

	light.x = 0.f;
	light.y = 0.f;
	light.z = 0.f;
	isLightMove = false;


	objectCount = 2;
	townObjectCount = 15;	// 얘 안쓰는 듯..?
	spawnList.clear();

	// 나무 생성
	for (int i = 0; i < 10; ++i) {
		rotateTreeY[i] = static_cast<float>(std::rand()) / RAND_MAX * 360.0f;
	}

	// 서버에서 받은 값으로 초기 동물 셋팅
	GLuint animalShader = m_resourceManager->getShader("animal");
	MeshData animalMesh = m_resourceManager->getMesh("animal_cube");

	for (int i = 0; i < pigCount; ++i)
	{
		pigs[i] = new Pig(i);	// pig는 게임객체... 업캐스팅........
		pigs[i]->setShader(animalShader);
		pigs[i]->setVAO(animalMesh.VAO, animalMesh.vertexCount);
		pigs[i]->initialize();
	}

	// 알파카
	for (int i = 0; i < alpacaCount; ++i)
	{
		alpacas[i] = new Alpaca;	// pig는 게임객체... 업캐스팅........
		alpacas[i]->setShader(animalShader);
		alpacas[i]->setVAO(animalMesh.VAO, animalMesh.vertexCount);
		alpacas[i]->initialize();
	}

	// 닭
	for (int i = 0; i < chickenCount; ++i)
	{
		chics[i] = new Chic;	// pig는 게임객체... 업캐스팅........
		chics[i]->setShader(animalShader);
		chics[i]->setVAO(animalMesh.VAO, animalMesh.vertexCount);
		chics[i]->initialize();
	}

	// 여우
	for (int i = 0; i < foxCount; ++i)
	{
		foxes[i] = new Fox;	// pig는 게임객체... 업캐스팅........
		foxes[i]->setShader(animalShader);
		foxes[i]->setVAO(animalMesh.VAO, animalMesh.vertexCount);
		foxes[i]->initialize();
	}

	// 펭귄
	for (int i = 0; i < penguinCount; ++i)
	{
		penguins[i] = new Penguin(i);	// pig는 게임객체... 업캐스팅........
		penguins[i]->setShader(animalShader);
		penguins[i]->setVAO(animalMesh.VAO, animalMesh.vertexCount);
		penguins[i]->initialize();
	}

	//printf("pgiCount after init: %d\n", pigCount);

	cameraY = 25.5f;
	isTitleAniEnd = false;
	isTitleAni = true;

	isSnow = true;
	isDay = true;

	ddongSpawnTimer = 0.f;

	GLuint texShader = m_resourceManager->getShader("tex");
	GLuint objShader = m_resourceManager->getShader("obj");
	GLuint defaultShader = m_resourceManager->getShader("default");
	GLuint bgShader = m_resourceManager->getShader("bg");

	m_texShader_modelLoc = glGetUniformLocation(texShader, "modelTransform");
	m_texShader_viewLoc = glGetUniformLocation(texShader, "viewTransform");
	m_texShader_projLoc = glGetUniformLocation(texShader, "projTransform");
	m_texShader_lightColorLoc = glGetUniformLocation(texShader, "lightColor");
	m_texShader_useLightLoc = glGetUniformLocation(texShader, "useLight");

	m_bgShader_modelLoc = glGetUniformLocation(bgShader, "modelTransform");

	m_objShader_modelLoc = glGetUniformLocation(objShader, "modelTransform");
	m_objShader_viewLoc = glGetUniformLocation(objShader, "viewTransform");
	m_objShader_projLoc = glGetUniformLocation(objShader, "projTransform");
	m_objShader_globalColorLoc = glGetUniformLocation(objShader, "globalColor");
	m_objShader_useGlobalColorLoc = glGetUniformLocation(objShader, "useGlobalColor");
	m_objShader_lightColorLoc = glGetUniformLocation(objShader, "lightColor");
	m_objShader_useLightLoc = glGetUniformLocation(objShader, "useLight");

	m_animalShader_lightPosLoc = glGetUniformLocation(animalShader, "lightPos");
	m_animalShader_lightColorLoc = glGetUniformLocation(animalShader, "lightColor");

	m_defaultShader_viewLoc = glGetUniformLocation(defaultShader, "viewTransform");
	m_defaultShader_projLoc = glGetUniformLocation(defaultShader, "projTransform");
	m_defaultShader_cameraPosLoc = glGetUniformLocation(defaultShader, "cameraPos");

	// 플레이어 셋팅
	MeshData cubeMesh = m_resourceManager->getMesh("cube");
	player = new PlayerObject();
	player->setShader(texShader); // 또는 colorShader
	player->setVAO(cubeMesh.VAO, cubeMesh.vertexCount);
	player->initialize();
	player->rotateY(180.f);
	g_myPlayer = player;
	if (g_loginInfo.hasReceivedInfo)
	{
		player->setPosition((float)g_loginInfo.x, (float)g_loginInfo.y, (float)g_loginInfo.z);
		g_myPlayer->setName(g_loginInfo.name);
		g_myPlayer->setCoin(g_loginInfo.coin);
		g_myPlayer->setFeed(g_loginInfo.feed);
		g_myPlayer->setMaxCoin(g_loginInfo.maxCoin);
		g_myPlayer->setMaxFeed(g_loginInfo.maxFeed);
		printf("[클라] 내 플레이어 ,x=%.2f, y=%.2f, z= %.2f, coin=%d, feed=%d, maxCoin=%d, maxFeed=%d\n",
			(float)g_loginInfo.x, (float)g_loginInfo.y, (float)g_loginInfo.z, player->getCoin(), player->getFeed(), player->getMaxCoin(), player->getMaxFeed());
		std::cout << "이름 : " << g_myPlayer->getName() << std::endl;
	}

	MeshData ddongMesh = m_resourceManager->getMesh("ddong");
	ddongVAO = ddongMesh.VAO;
	ddongVertexCount = ddongMesh.vertexCount;
}

void gameScene::sceneOnExit()
{
	for (int i = 0; i < objectCount; ++i)
		delete objects[i];

	for (int i = 0; i < pigCount; ++i)
		delete pigs[i];

	for (int i = 0; i < alpacaCount; ++i)
		delete alpacas[i];

	for (int i = 0; i < penguinCount; ++i)
		delete penguins[i];

	for (int i = 0; i < chickenCount; ++i)
		delete chics[i];

	for (int i = 0; i < foxCount; ++i)
		delete foxes[i];

	delete player;

	//CloseClient(); // 클라이언트 종료
}

void gameScene::createOtherPlayer(int id, float x, float y, float z)
{
	GLuint texShader = m_resourceManager->getShader("tex");
	MeshData cubeMesh = m_resourceManager->getMesh("cube"); // 기존 코드 참고

	for (int i = 0; i < 2; ++i) {
		if (otherPlayerIDs[i] == id) return;	// 이미 있는 플레이어면 ㅐ스
	}

	for (int i = 0; i < 2; ++i)
	{
		if (otherPlayer[i] == nullptr) // 빈 자리 발견
		{
			otherPlayer[i] = new PlayerObject();

			MeshData cubeMesh = m_resourceManager->getMesh("cube");
			otherPlayer[i]->setVAO(cubeMesh.VAO, cubeMesh.vertexCount);
			otherPlayer[i]->setShader(texShader);
			otherPlayer[i]->initialize();
			otherPlayer[i]->setPosition(x, y, z);
			otherPlayerIDs[i] = id;

			return;
		}
	}
	printf("[오류] 최대 인원 초과\n");
}

void gameScene::removeOtherPlayer(int id)
{
	for (int i = 0; i < 2; ++i)
	{
		if (otherPlayerIDs[i] == id)
		{
			if (otherPlayer[i] != nullptr) {
				delete otherPlayer[i];
				otherPlayer[i] = nullptr;
			}
			otherPlayerIDs[i] = -1; // id 초기화

			printf("유저(%d) 로그아웃\n", id);
			return;
		}
	}
}

void gameScene::updateOtherPlayer(int id, float x, float z, float yaw)
{
	for (int i = 0; i < 2; ++i)
	{
		if (otherPlayerIDs[i] == id && otherPlayer[i] != nullptr)
		{
			otherPlayer[i]->movePosition(x, z); // (이전에 만든 함수 사용)
			otherPlayer[i]->setYaw(yaw);
			return;
		}
	}

	printf("모르는 유저(%d)의 이동 패킷 수신\n", id);
	createOtherPlayer(id, x, 1.5f, z);
}

void gameScene::setAnimalCount(int animalType, int count)
{
	switch (animalType)
	{
	case AnimalType::PIG:
		pigCount = count;
		printf("pigCount %d\n", pigCount);
		break;
	case AnimalType::CHICKEN:
		chickenCount = count;
		printf("chickCount %d\n", chickenCount);
		break;
	case AnimalType::ALPACA:
		alpacaCount = count;
		printf("alpacaCount %d\n", alpacaCount);
		break;
	case AnimalType::PENGUIN:
		penguinCount = count;
		printf("pCount %d\n", penguinCount);
		break;
	case AnimalType::FOX:
		foxCount = count;
		break;
	}
}

void gameScene::spawnAnimal(const int animalType)
{
	GLuint animalShader = m_resourceManager->getShader("animal");
	MeshData animalMesh = m_resourceManager->getMesh("animal_cube");

	switch (animalType)
	{
	case AnimalType::PIG: // 돼지 구매
	{
		pigs[pigCount] = new Pig(pigCount);
		pigs[pigCount]->setShader(animalShader);
		pigs[pigCount]->setVAO(animalMesh.VAO, animalMesh.vertexCount);
		pigs[pigCount]->initialize();
		++pigCount;
	}
	break;
	case AnimalType::CHICKEN: // 병아리 구매
	{
		chics[chickenCount] = new Chic;
		chics[chickenCount]->setShader(animalShader);
		chics[chickenCount]->setVAO(animalMesh.VAO, animalMesh.vertexCount);
		chics[chickenCount]->initialize();
		++chickenCount;
	}
	break;
	case AnimalType::ALPACA: // 알파카 구매
	{
		alpacas[alpacaCount] = new Alpaca;
		alpacas[alpacaCount]->setShader(animalShader);
		alpacas[alpacaCount]->setVAO(animalMesh.VAO, animalMesh.vertexCount);
		alpacas[alpacaCount]->initialize();
		++alpacaCount;
	}
	break;
	case AnimalType::PENGUIN: // 펭귄 구매
	{
		penguins[penguinCount] = new Penguin(penguinCount);
		penguins[penguinCount]->setShader(animalShader);
		penguins[penguinCount]->setVAO(animalMesh.VAO, animalMesh.vertexCount);
		penguins[penguinCount]->initialize();
		++penguinCount;
	}
	break;
	case AnimalType::FOX: // 여우 구매
	{
		foxes[foxCount] = new Fox;
		foxes[foxCount]->setShader(animalShader);
		foxes[foxCount]->setVAO(animalMesh.VAO, animalMesh.vertexCount);
		foxes[foxCount]->initialize();
		++foxCount;
	}
	break;
	}
	printf("[게임씬] 동물 구매 및 생성 완료! 종류: %d\n", animalType);
}

void gameScene::sellAnimal(const int animalType, const int animalID)
{
	switch (animalType)
	{
	case AnimalType::PIG: // 돼지 판매
	{
		if (animalID < 0 || animalID >= pigCount) return;
		delete pigs[animalID];
		if (animalID != pigCount - 1) {
			pigs[animalID] = pigs[pigCount - 1];
		}
		pigs[pigCount - 1] = nullptr;
		--pigCount;
	}
	break;
	case AnimalType::CHICKEN: // 병아리 판매
	{
		if (animalID < 0 || animalID >= chickenCount) return;
		delete chics[animalID];
		if (animalID != chickenCount - 1) {
			chics[animalID] = chics[chickenCount - 1];
		}
		chics[chickenCount - 1] = nullptr;
		--chickenCount;
	}
	break;
	case AnimalType::ALPACA: // 알파카 판매
	{
		if (animalID < 0 || animalID >= alpacaCount) return;
		delete alpacas[animalID];
		if (animalID != alpacaCount - 1) {
			alpacas[animalID] = alpacas[alpacaCount - 1];
		}
		alpacas[alpacaCount - 1] = nullptr;
		--alpacaCount;
	}
	break;
	case AnimalType::PENGUIN: // 펭귄 판매
	{
		if (animalID < 0 || animalID >= penguinCount) return;
		delete penguins[animalID];
		if (animalID != penguinCount - 1) {
			penguins[animalID] = penguins[penguinCount - 1];
		}
		penguins[penguinCount - 1] = nullptr;
		--penguinCount;
	}
	break;
	case AnimalType::FOX: // 여우 판매
	{
		if (animalID < 0 || animalID >= foxCount) return;
		delete foxes[animalID];
		if (animalID != foxCount - 1) {
			foxes[animalID] = foxes[foxCount - 1];
		}
		foxes[foxCount - 1] = nullptr;
		--foxCount;
	}
	break;
	default:
		break;
	}
}

void gameScene::feedAnimals(const int animalType, const int animalID, const int growStep)
{
	switch (animalType)
	{
	case AnimalType::PIG: // 돼지 먹이주기
	{
		if (animalID < 0 || animalID >= pigCount) return;
		pigs[animalID]->feedNum = growStep;
		if (growStep >= 5) pigs[animalID]->isBaby = false;
	}
	break;
	case AnimalType::CHICKEN: // 병아리 먹이주기
	{
		if (animalID < 0 || animalID >= chickenCount) return;
		chics[animalID]->feedNum = growStep;
		if (growStep >= 5) chics[animalID]->isBaby = false;
	}
	break;
	case AnimalType::ALPACA: // 알파카 먹이주기
	{
		if (animalID < 0 || animalID >= alpacaCount) return;
		alpacas[animalID]->feedNum = growStep;
		if (growStep >= 5) alpacas[animalID]->isBaby = false;
	}
	break;
	case AnimalType::PENGUIN: // 펭귄 먹이주기
	{
		if (animalID < 0 || animalID >= penguinCount) return;
		penguins[animalID]->feedNum = growStep;
		if (growStep >= 5) penguins[animalID]->isBaby = false;
	}
	break;
	case AnimalType::FOX: // 여우 먹이주기
	{
		if (animalID < 0 || animalID >= foxCount) return;
		foxes[animalID]->feedNum = growStep;
		if (growStep >= 5) foxes[animalID]->isBaby = false;
	}
	break;
	}

	printf("[게임씬] 동물 먹이주기 완료! 종류: %d, ID: %d, 성장단계: %d\n", animalType, animalID, growStep);
}

//
void gameScene::spawnPoop(int id, float x, float z)
{
	if (poopMap.find(id) != poopMap.end()) return;

	GameObject* newPoop = new GameObject();

	GLuint objShader = m_resourceManager->getShader("obj");
	newPoop->setShader(objShader);

	// sceneOnEnter에서 저장해둔 VAO 사용
	newPoop->setVAO(ddongVAO, ddongVertexCount);

	newPoop->setPosition(x, 0.0f, z);

	poopMap[id] = newPoop;

	printf("똥 생성 완료! ID: %d, Pos: %.1f, %.1f\n", id, x, z);
}


void gameScene::removePoop(int id)
{
	auto it = poopMap.find(id);
	if (it != poopMap.end()) {
		delete it->second;
		poopMap.erase(it);
		printf("똥 삭제 완료! ID: %d\n", id);
	}
}

//
void gameScene::update(float elapsedTime)
{
	ProcessAnimalMoveQueue();	// 큐에서 꺼내서 동물들 위치 동기화

	player->update(elapsedTime);
	const glm::vec3 playerPosition = player->getPosition();

	// 이건 시작했을 때.. 카메라 착지하는듯한 뷰 보여주는거
	if (isTitleAni) {
		cameraY -= CAMERA_ANIM_SPEED * elapsedTime;
		if (cameraY <= 2.f) {
			isTitleAni = false;
			isTitleAniEnd = true;
			player->rotateY(0.f);
		}
	}

	if (spawnList.empty() == false)
	{
		for (int animalType : spawnList)
		{
			spawnAnimal(animalType);
		}
		spawnList.clear();
	}

	if (sellList.empty() == false)
	{
		for (auto animal : sellList)
		{
			int animalType = animal.first;
			int animalID = animal.second;
			sellAnimal(animalType, animalID);
		}
		sellList.clear();
	}

	// 동물 거리 체크
	//isAnimalNear(playerPosition);
	checkInteraction(playerPosition);

	// 해 움직이는거.. 
	if (isLightMove) {
		light.x += LIGHT_SPEED * elapsedTime;
		if (light.x >= 5.f) {
			isLightMove = false;
		}
	}
	else {
		light.x -= LIGHT_SPEED * elapsedTime;
		if (light.x <= -5.f) {
			isLightMove = true;
		}
	}

	//ddongSpawnTimer += elapsedTime;
	//if (ddongSpawnTimer >= DDONG_SPAWN_TIME) {
	//	float x = -10.f + static_cast<float>(std::rand()) / (static_cast<float>(RAND_MAX / 8.f));
	//	float z = -5.f + static_cast<float>(std::rand()) / (static_cast<float>(RAND_MAX / 15.f));

	//	ddongs.emplace_back(Ddong{ x, 0.f, z , true ,false });
	//	ddongSpawnTimer -= DDONG_SPAWN_TIME;	// 타이머 리셋..
	//}



	//std::cout << "elapsedTime : " << elapsedTime << std::endl;
	if (not isAnimalSleep)	// 먹이 일정 갯수 이상 주면 어른됨.. -> 어른이 되면 팔수있당..
	{
		for (int i = 0; i < pigCount; ++i) {
			pigs[i]->update(elapsedTime);
			if (pigs[i]->feedNum >= 5) {
				if (pigs[i]->isBaby) {
					pigs[i]->isBaby = false;
				}
				if (not pigs[i]->isBaby) {
				}
			}
		}

		for (int i = 0; i < alpacaCount; ++i)
		{
			alpacas[i]->update(elapsedTime);
			if (alpacas[i]->feedNum >= 7) {
				if (alpacas[i]->isBaby) {
					alpacas[i]->isBaby = false;
				}
				if (not alpacas[i]->isBaby) {
				}
			}
		}

		for (int i = 0; i < penguinCount; ++i)
		{
			penguins[i]->update(elapsedTime);
			if (penguins[i]->feedNum >= 5) {
				if (penguins[i]->isBaby) {
					penguins[i]->isBaby = false;
				}
				if (not penguins[i]->isBaby) {
				}
			}
		}

		for (int i = 0; i < chickenCount; ++i)
		{
			chics[i]->update(elapsedTime);
			if (chics[i]->feedNum >= 5) {
				if (chics[i]->isBaby) {
					chics[i]->isBaby = false;
				}
				if (not chics[i]->isBaby) {
				}
			}

		}

		for (int i = 0; i < foxCount; ++i) {
			foxes[i]->update(elapsedTime);
			if (foxes[i]->feedNum >= 5) {
				if (foxes[i]->isBaby) {
					foxes[i]->isBaby = false;
				}
				if (not foxes[i]->isBaby) {
					//std::cout << " 이제 돼지 어른" << std::endl;
				}
			}
		}
	}


	// 눈내리는거
	snowTimer += elapsedTime;
	if (snowTimer >= SNOW_TIME) {
		isSnow = not isSnow;
		snowTimer -= SNOW_TIME;
		snows.clear();
	}

	if (isSnow) {
		snowSpawnTimer += elapsedTime;
		if (snowSpawnTimer >= SNOW_SPAWN_TIME) {
			float randx = -20.f + static_cast<float>(std::rand()) / (static_cast<float>(RAND_MAX / 40.f));
			float randz = -10.f + static_cast<float>(std::rand()) / (static_cast<float>(RAND_MAX / 20.f));
			float size = 0.08f + (rand() / (float)RAND_MAX) * (0.03f - 0.01f);
			float speed =/* 0.005f + (rand() / (float)RAND_MAX) * (0.000075f - 0.00005f);*/
				0.8f + (rand() / (float)RAND_MAX) * 0.4f;;

			snows.emplace_back(Snow{ randx, 20.5f, randz, size, speed, true });
			snowSpawnTimer -= SNOW_SPAWN_TIME;
		}

		for (auto& snow : snows) {
			snow.y -= (snow.speed * SNOW_SPEED) * elapsedTime;
			if (snow.y < 0.f) {
				snow.isDraw = false;
			}
		}
	}

	dayTimer += elapsedTime;
	if (dayTimer >= DAY_TIME) {
		dayTimer -= DAY_TIME;
		isDay = not isDay;
	}

	if (isDay) {
		isAnimalSleep = false;
		//std::cout << "낮" << std::endl;
	}
	else if (not isDay) {
		isAnimalSleep = true;
		//std::cout << "밤" << std::endl;
	}
}


void gameScene::draw()
{
	glm::mat4 projMatrix = glm::mat4(1.0f);
	glm::mat4 viewMatrix = glm::lookAt(glm::vec3(0.f, cameraY, 15.f), glm::vec3(0.f, 0.5f, 0.f), glm::vec3(0.f, 1.f, 0.f));
	glm::vec3 cameraPos = glm::vec3(1.f);
	//if (isTitleAniEnd) {
	//	cameraPos = player->getPosition();		// 플레이어 위치에서
	//	cameraPos.y = 1.5f;
	//	glm::vec3 targetPos = cameraPos + player->getLook(); // 플레이어 앞을 바라본다
	//	viewMatrix = glm::lookAt(cameraPos, targetPos, glm::vec3(0.f, 1.f, 0.f));
	//}
	if (not isTitleAniEnd) {
		viewMatrix = glm::lookAt(glm::vec3(0.f, cameraY, 25.f), glm::vec3(0.f, 2.5f, 10.f), glm::vec3(0.f, 1.f, 0.f));
	}
	if (g_myPlayer != nullptr and isTitleAniEnd)
	{
		glm::vec3 playerPos = g_myPlayer->getPosition();
		glm::vec3 playerLook = g_myPlayer->getLook();

		float cameraDistance = 5.0f; // 등 뒤로 5만큼
		float cameraHeight = 3.0f;   // 위로 3만큼

		glm::vec3 cameraPos = playerPos - (playerLook * cameraDistance) + glm::vec3(0.0f, cameraHeight, 0.0f);
		glm::vec3 targetPos = playerPos + glm::vec3(0.0f, 1.0f, 0.0f);

		viewMatrix = glm::lookAt(cameraPos, targetPos, glm::vec3(0.f, 1.f, 0.f));
	}
	projMatrix = glm::perspective(glm::radians(45.f), float(width) / float(height), 0.1f, 100.f);



	GLuint texShader = m_resourceManager->getShader("tex");
	GLuint objShader = m_resourceManager->getShader("obj");
	GLuint animalShader = m_resourceManager->getShader("animal");
	GLuint defaultShader = m_resourceManager->getShader("default");
	GLuint bgShader = m_resourceManager->getShader("bg");

	MeshData bgMesh = m_resourceManager->getMesh("background");
	GLuint skyTexture = m_resourceManager->getTexture("sky2");

	// 배경 먼저
	glUseProgram(texShader);
	{
		glDisable(GL_DEPTH_TEST);
		glBindVertexArray(bgMesh.VAO);
		glUniformMatrix4fv(m_texShader_viewLoc, 1, GL_FALSE, glm::value_ptr(viewMatrix));
		glUniformMatrix4fv(m_texShader_projLoc, 1, GL_FALSE, glm::value_ptr(projMatrix));
		{
			glm::mat4 translateMatrix = glm::translate(glm::mat4(1.f), glm::vec3(-40.0f, 15.f, -3.f));
			glm::mat4 rotMatrixY = glm::rotate(glm::mat4(1.f), glm::radians(90.f), glm::vec3(0.f, 1.f, 0.f));
			glm::mat4 sclaeMatrix = glm::scale(glm::mat4(1.f), glm::vec3(23.f));
			glm::mat4 matrix = translateMatrix * rotMatrixY * sclaeMatrix;

			glUniformMatrix4fv(m_texShader_modelLoc, 1, GL_FALSE, glm::value_ptr(matrix));
			glBindTexture(GL_TEXTURE_2D, skyTexture);
			glDrawArrays(GL_TRIANGLES, 0, bgMesh.vertexCount);
		}
		{
			glm::mat4 translateMatrix = glm::translate(glm::mat4(1.f), glm::vec3(40.0f, 15.f, -3.f));
			glm::mat4 rotMatrixY = glm::rotate(glm::mat4(1.f), glm::radians(-90.f), glm::vec3(0.f, 1.f, 0.f));
			glm::mat4 sclaeMatrix = glm::scale(glm::mat4(1.f), glm::vec3(23.f));
			glm::mat4 matrix = translateMatrix * rotMatrixY * sclaeMatrix;

			glUniformMatrix4fv(m_texShader_modelLoc, 1, GL_FALSE, glm::value_ptr(matrix));
			glDrawArrays(GL_TRIANGLES, 0, bgMesh.vertexCount);
		}
		{
			glm::mat4 translateMatrix = glm::translate(glm::mat4(1.f), glm::vec3(0.0f, 18.f, -45.f));
			glm::mat4 sclaeMatrix = glm::scale(glm::mat4(1.f), glm::vec3(20.f));
			glm::mat4 matrix = translateMatrix * sclaeMatrix;

			glUniformMatrix4fv(m_texShader_modelLoc, 1, GL_FALSE, glm::value_ptr(matrix));
			glDrawArrays(GL_TRIANGLES, 0, bgMesh.vertexCount);
		}
		{	// z앞
			glDisable(GL_CULL_FACE);
			glm::mat4 translateMatrix = glm::translate(glm::mat4(1.f), glm::vec3(0.0f, 18.f, 50.f));
			glm::mat4 rotMatrixY = glm::rotate(glm::mat4(1.f), glm::radians(180.f), glm::vec3(0.f, 1.f, 0.f));
			glm::mat4 sclaeMatrix = glm::scale(glm::mat4(1.f), glm::vec3(30.f));
			glm::mat4 matrix = translateMatrix * rotMatrixY * sclaeMatrix;

			glUniformMatrix4fv(m_texShader_modelLoc, 1, GL_FALSE, glm::value_ptr(matrix));
			glDrawArrays(GL_TRIANGLES, 0, bgMesh.vertexCount);
			glEnable(GL_CULL_FACE);
		}
		glEnable(GL_DEPTH_TEST);
	}

	// 바닥
	{
		glUseProgram(texShader);
		glBindVertexArray(bgMesh.VAO);
		glm::mat4 translateMatrix = glm::translate(glm::mat4(1.f), glm::vec3(0.0f, 0.f, 0.f));
		glm::mat4 rotMatrixX = glm::rotate(glm::mat4(1.f), glm::radians(-90.f), glm::vec3(1.f, 0.f, 0.f));

		glm::mat4 sclaeMatrix = glm::scale(glm::mat4(1.f), glm::vec3(25.f));
		glm::mat4 matrix = translateMatrix * rotMatrixX * sclaeMatrix;

		glUniformMatrix4fv(m_texShader_modelLoc, 1, GL_FALSE, glm::value_ptr(matrix));
		glUniformMatrix4fv(m_texShader_viewLoc, 1, GL_FALSE, glm::value_ptr(viewMatrix));
		glUniformMatrix4fv(m_texShader_projLoc, 1, GL_FALSE, glm::value_ptr(projMatrix));

		GLuint groundTexture;
		if (isSnow) {
			groundTexture = m_resourceManager->getTexture("snowGrass");
		}
		else groundTexture = m_resourceManager->getTexture("grass");

		glBindTexture(GL_TEXTURE_2D, groundTexture);
		glDrawArrays(GL_TRIANGLES, 0, bgMesh.vertexCount);
	}

	{
		glUseProgram(animalShader);
		glUniform3f(m_animalShader_lightPosLoc, light.x, light.y, light.z);
		glUniformMatrix4fv(m_animalShader_viewLoc, 1, GL_FALSE, glm::value_ptr(viewMatrix));
		glUniformMatrix4fv(m_animalShader_projLoc, 1, GL_FALSE, glm::value_ptr(projMatrix));

		glUseProgram(defaultShader);
		glUniform3f(m_defaultShader_cameraPosLoc, cameraPos.x, cameraPos.y, cameraPos.z);
		glUniformMatrix4fv(m_defaultShader_viewLoc, 1, GL_FALSE, glm::value_ptr(viewMatrix));
		glUniformMatrix4fv(m_defaultShader_projLoc, 1, GL_FALSE, glm::value_ptr(projMatrix));
	}



	// 펜스
	MeshData fenceMesh = m_resourceManager->getMesh("fence");
	{
		glUseProgram(objShader);
		glBindVertexArray(fenceMesh.VAO);

		glUniformMatrix4fv(m_objShader_viewLoc, 1, GL_FALSE, glm::value_ptr(viewMatrix));
		glUniformMatrix4fv(m_objShader_projLoc, 1, GL_FALSE, glm::value_ptr(projMatrix));
		glUniform1i(m_objShader_useGlobalColorLoc, GL_TRUE); // 단색 사용 ON
		glUniform3f(m_objShader_globalColorLoc, 138 / 255.f, 73 / 255.f, 36 / 255.f); // 울타리 색상 설정

		glm::mat4 scaleMatrix = glm::scale(glm::mat4(1.f), glm::vec3(1.f)); // 크기는 동일하므로 루프 밖에서 정의

		// 첫 번째 그룹 (가로+세로)
		for (int i = 0; i < 6; ++i) {
			// 울타리 조각 1 (가로 위쪽)
			glm::mat4 translateMatrix = glm::translate(glm::mat4(1.f), glm::vec3(-15.f + i * 3.f, 0.f, -8.f));
			glm::mat4 rotMatrixY = glm::rotate(glm::mat4(1.f), glm::radians(90.f), glm::vec3(0.f, 1.f, 0.f));
			glm::mat4 matrix = translateMatrix * rotMatrixY * scaleMatrix;
			glUniformMatrix4fv(m_objShader_modelLoc, 1, GL_FALSE, glm::value_ptr(matrix));
			glDrawArrays(GL_TRIANGLES, 0, fenceMesh.vertexCount);

			// 울타리 조각 2 (세로 왼쪽)
			translateMatrix = glm::translate(glm::mat4(1.f), glm::vec3(-17.f, 0.f, -6.f + i * 3.f));
			rotMatrixY = glm::rotate(glm::mat4(1.f), glm::radians(0.f), glm::vec3(0.f, 1.f, 0.f)); // 회전 없음
			matrix = translateMatrix * rotMatrixY * scaleMatrix;
			glUniformMatrix4fv(m_objShader_modelLoc, 1, GL_FALSE, glm::value_ptr(matrix));
			glDrawArrays(GL_TRIANGLES, 0, fenceMesh.vertexCount);

			// 울타리 조각 3 (세로 오른쪽)
			translateMatrix = glm::translate(glm::mat4(1.f), glm::vec3(1.f, 0.f, -6.f + i * 3.f));
			matrix = translateMatrix * rotMatrixY * scaleMatrix;
			glUniformMatrix4fv(m_objShader_modelLoc, 1, GL_FALSE, glm::value_ptr(matrix));
			glDrawArrays(GL_TRIANGLES, 0, fenceMesh.vertexCount);
		}

		for (int i = 0; i < 6; ++i) {
			glm::mat4 translateMatrix = glm::translate(glm::mat4(1.f), glm::vec3(-15.f + i * 3.f, 0.f, 10.f));
			glm::mat4 rotMatrixY = glm::rotate(glm::mat4(1.f), glm::radians(90.f), glm::vec3(0.f, 1.f, 0.f));
			glm::mat4 matrix = translateMatrix * rotMatrixY * scaleMatrix;
			glUniformMatrix4fv(m_objShader_modelLoc, 1, GL_FALSE, glm::value_ptr(matrix));
			glDrawArrays(GL_TRIANGLES, 0, fenceMesh.vertexCount);
		}
	}

	/////////////////////////
	// 농장 표지판
	MeshData signBottomMesh = m_resourceManager->getMesh("sign_bottom");
	MeshData signTopMesh = m_resourceManager->getMesh("sign_up");	// 표지판 obj
	MeshData cubeMesh = m_resourceManager->getMesh("cube");			// 글씨 부분용 큐브..
	GLuint woodTexture = m_resourceManager->getTexture("farmsign_rest");
	GLuint textTexture = m_resourceManager->getTexture("farmsign");
	{
		glUseProgram(texShader);

		// 조명 설정.. 낮밤
		if (not isDay) {
			glUniform3f(m_texShader_lightColorLoc, 0.f / 255.f, 40.f / 255.f, 145.f / 255.f);
		}
		else { // isDay
			glUniform3f(m_texShader_lightColorLoc, 255.f / 255.f, 242.f / 255.f, 230.f / 255.f);
		}
		glUniform1i(m_texShader_useLightLoc, GL_TRUE);

		glm::mat4 translateMatrix = glm::translate(glm::mat4(1.f), glm::vec3(0.f, 0.f, 11.5f));
		glm::mat4 rotMatrixY = glm::rotate(glm::mat4(1.f), glm::radians(20.f), glm::vec3(0.f, 1.f, 0.f));
		glm::mat4 sclaeMatrix = glm::scale(glm::mat4(1.f), glm::vec3(0.2f));
		glm::mat4 matrix = translateMatrix * rotMatrixY * sclaeMatrix;
		glUniformMatrix4fv(m_texShader_modelLoc, 1, GL_FALSE, glm::value_ptr(matrix));

		// 이건 하단
		glBindVertexArray(signBottomMesh.VAO);
		glBindTexture(GL_TEXTURE_2D, woodTexture); // 나무 텍스처
		glDrawArrays(GL_TRIANGLES, 0, signBottomMesh.vertexCount);

		// 상단 그리기
		glBindVertexArray(signTopMesh.VAO);
		glDrawArrays(GL_TRIANGLES, 0, signTopMesh.vertexCount);

		// 글씨
		glUniform1i(m_texShader_useLightLoc, GL_FALSE); // 글씨는 조명안받게.. 꺼준다
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		translateMatrix = glm::translate(glm::mat4(1.f), glm::vec3(0.f, 1.55f, 11.6f));
		rotMatrixY = glm::rotate(glm::mat4(1.f), glm::radians(20.f), glm::vec3(0.f, 1.f, 0.f));
		sclaeMatrix = glm::scale(glm::mat4(1.f), glm::vec3(1.5f, 1.5f, 0.001f));
		matrix = translateMatrix * rotMatrixY * sclaeMatrix;
		glUniformMatrix4fv(m_texShader_modelLoc, 1, GL_FALSE, glm::value_ptr(matrix));
		glBindVertexArray(cubeMesh.VAO); // 큐브 메시 사용
		glBindTexture(GL_TEXTURE_2D, textTexture); // 글씨 텍스처
		glDrawArrays(GL_TRIANGLES, 0, cubeMesh.vertexCount);

		glUniform1i(m_texShader_useLightLoc, GL_TRUE);
		glDisable(GL_BLEND);
	}

	// 나무
	MeshData treeTopMesh = m_resourceManager->getMesh("tree_up");
	MeshData treeBottomMesh = m_resourceManager->getMesh("tree_bottom");
	{
		glUseProgram(objShader);
		glUniformMatrix4fv(m_objShader_viewLoc, 1, GL_FALSE, glm::value_ptr(viewMatrix));
		glUniformMatrix4fv(m_objShader_projLoc, 1, GL_FALSE, glm::value_ptr(projMatrix));
		glUniform1i(m_objShader_useLightLoc, GL_TRUE);			// 조명 사용 ON
		glUniform1i(m_objShader_useGlobalColorLoc, GL_TRUE);	// 단색 사용 ON

		glm::mat4 scaleMatrix = glm::scale(glm::mat4(1.f), glm::vec3(1.f));
		for (int i = 0; i < 10; ++i)
		{
			// --- 나무 1 (Z 뒷부분 첫 번째 줄) ---
			glm::mat4 translateMatrix = glm::translate(glm::mat4(1.f), glm::vec3(-20.f + i * 4.f, 0.f, -10.f));
			glm::mat4 rotMatrixY = glm::rotate(glm::mat4(1.f), glm::radians(rotateTreeY[i]), glm::vec3(0.f, 1.f, 0.f));
			glm::mat4 matrix = translateMatrix * rotMatrixY * scaleMatrix;
			glUniformMatrix4fv(m_objShader_modelLoc, 1, GL_FALSE, glm::value_ptr(matrix));

			// 윗부분 (초록색)
			glUniform3f(m_objShader_globalColorLoc, 98 / 255.f, 193 / 255.f, 91 / 255.f);
			glBindVertexArray(treeTopMesh.VAO);
			glDrawArrays(GL_TRIANGLES, 0, treeTopMesh.vertexCount);

			// 아랫부분 (갈색)
			glUniform3f(m_objShader_globalColorLoc, 138 / 255.f, 73 / 255.f, 36 / 255.f);
			glBindVertexArray(treeBottomMesh.VAO);
			glDrawArrays(GL_TRIANGLES, 0, treeBottomMesh.vertexCount);

			// --- 나무 2 (Z 뒷부분 두 번째 줄) ---
			translateMatrix = glm::translate(glm::mat4(1.f), glm::vec3(-18.f + i * 4.f, 0.f, -15.f));
			// rotMatrixY는 위와 동일
			matrix = translateMatrix * rotMatrixY * scaleMatrix;
			glUniformMatrix4fv(m_objShader_modelLoc, 1, GL_FALSE, glm::value_ptr(matrix));

			// 윗부분 (초록색)
			glUniform3f(m_objShader_globalColorLoc, 98 / 255.f, 193 / 255.f, 91 / 255.f);
			glBindVertexArray(treeTopMesh.VAO);
			glDrawArrays(GL_TRIANGLES, 0, treeTopMesh.vertexCount);

			// 아랫부분 (갈색)
			glUniform3f(m_objShader_globalColorLoc, 138 / 255.f, 73 / 255.f, 36 / 255.f);
			glBindVertexArray(treeBottomMesh.VAO);
			glDrawArrays(GL_TRIANGLES, 0, treeBottomMesh.vertexCount);

			// --- 나무 3 (오른쪽 옆) ---
			translateMatrix = glm::translate(glm::mat4(1.f), glm::vec3(20.f, 0.f, -18.f + i * 4.f));
			// rotMatrixY는 위와 동일
			matrix = translateMatrix * rotMatrixY * scaleMatrix;
			glUniformMatrix4fv(m_objShader_modelLoc, 1, GL_FALSE, glm::value_ptr(matrix));

			// 윗부분 (초록색)
			glUniform3f(m_objShader_globalColorLoc, 98 / 255.f, 193 / 255.f, 91 / 255.f);
			glBindVertexArray(treeTopMesh.VAO);
			glDrawArrays(GL_TRIANGLES, 0, treeTopMesh.vertexCount);

			// 아랫부분 (갈색)
			glUniform3f(m_objShader_globalColorLoc, 138 / 255.f, 73 / 255.f, 36 / 255.f);
			glBindVertexArray(treeBottomMesh.VAO);
			glDrawArrays(GL_TRIANGLES, 0, treeBottomMesh.vertexCount);

			// --- 나무 4 (왼쪽 옆) ---
			translateMatrix = glm::translate(glm::mat4(1.f), glm::vec3(-20.f, 0.f, -18.f + i * 4.f));
			// rotMatrixY는 위와 동일
			matrix = translateMatrix * rotMatrixY * scaleMatrix;
			glUniformMatrix4fv(m_objShader_modelLoc, 1, GL_FALSE, glm::value_ptr(matrix));

			// 윗부분 (초록색)
			glUniform3f(m_objShader_globalColorLoc, 98 / 255.f, 193 / 255.f, 91 / 255.f);
			glBindVertexArray(treeTopMesh.VAO);
			glDrawArrays(GL_TRIANGLES, 0, treeTopMesh.vertexCount);

			// 아랫부분 (갈색)
			glUniform3f(m_objShader_globalColorLoc, 138 / 255.f, 73 / 255.f, 36 / 255.f);
			glBindVertexArray(treeBottomMesh.VAO);
			glDrawArrays(GL_TRIANGLES, 0, treeBottomMesh.vertexCount);
		}
	}


	// 상점
	MeshData houseBottomMesh = m_resourceManager->getMesh("house_bottom");
	MeshData houseTopMesh = m_resourceManager->getMesh("house_top");
	GLuint houseWoodTexture = m_resourceManager->getTexture("houseWood");
	{
		glUseProgram(texShader);
		glBindVertexArray(houseBottomMesh.VAO);
		glUniformMatrix4fv(m_texShader_viewLoc, 1, GL_FALSE, glm::value_ptr(viewMatrix));
		glUniformMatrix4fv(m_texShader_projLoc, 1, GL_FALSE, glm::value_ptr(projMatrix));
		if (not isDay) {
			glUniform3f(m_texShader_lightColorLoc, 0.f / 255.f, 40.f / 255.f, 145.f / 255.f);
		}
		else {
			glUniform3f(m_texShader_lightColorLoc, 255.f / 255.f, 242.f / 255.f, 230.f / 255.f);
		}

		glm::mat4 translateMatrix = glm::translate(glm::mat4(1.f), glm::vec3(10.f, 0.f, 0.f));
		glm::mat4 rotMatrixY = glm::rotate(glm::mat4(1.f), glm::radians(90.f), glm::vec3(0.f, 1.f, 0.f));
		glm::mat4 scaleMatrix = glm::scale(glm::mat4(1.f), glm::vec3(0.3f)); // 오타 수정
		glm::mat4 matrix = translateMatrix * rotMatrixY * scaleMatrix;
		glUniformMatrix4fv(m_texShader_modelLoc, 1, GL_FALSE, glm::value_ptr(matrix));

		glBindTexture(GL_TEXTURE_2D, houseWoodTexture);
		glDrawArrays(GL_TRIANGLES, 0, houseBottomMesh.vertexCount);

		glUseProgram(objShader);	// 지붕은 그냥 색칠되어있어서.. objShader
		glBindVertexArray(houseTopMesh.VAO);
		glUniformMatrix4fv(m_objShader_viewLoc, 1, GL_FALSE, glm::value_ptr(viewMatrix));
		glUniformMatrix4fv(m_objShader_projLoc, 1, GL_FALSE, glm::value_ptr(projMatrix));
		glUniform1i(m_objShader_useGlobalColorLoc, GL_TRUE);

		translateMatrix = glm::translate(glm::mat4(1.f), glm::vec3(10.f, 0.f, 0.f));
		rotMatrixY = glm::rotate(glm::mat4(1.f), glm::radians(90.f), glm::vec3(0.f, 1.f, 0.f));
		scaleMatrix = glm::scale(glm::mat4(1.f), glm::vec3(0.3f)); // 오타 수정
		matrix = translateMatrix * rotMatrixY * scaleMatrix;
		glUniformMatrix4fv(m_objShader_modelLoc, 1, GL_FALSE, glm::value_ptr(matrix));

		glUniform3f(m_objShader_globalColorLoc, 255 / 255.f, 178 / 255.f, 217 / 255.f); // 지붕 색상
		glDrawArrays(GL_TRIANGLES, 0, houseTopMesh.vertexCount);
	}

	// 상점 간판
	GLuint storeSignTexture = m_resourceManager->getTexture("store");
	{
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glUseProgram(texShader);
		glBindVertexArray(cubeMesh.VAO);

		glUniformMatrix4fv(m_texShader_viewLoc, 1, GL_FALSE, glm::value_ptr(viewMatrix));
		glUniformMatrix4fv(m_texShader_projLoc, 1, GL_FALSE, glm::value_ptr(projMatrix));
		glUniform1i(m_texShader_useLightLoc, GL_FALSE); // 간판은 조명 안 받게....

		glm::mat4 translateMatrix = glm::translate(glm::mat4(1.f), glm::vec3(10.f, 3.f, 1.8f));
		glm::mat4 rotMatrixY = glm::rotate(glm::mat4(1.f), glm::radians(0.f), glm::vec3(0.f, 1.f, 0.f));
		glm::mat4 scaleMatrix = glm::scale(glm::mat4(1.f), glm::vec3(3.f, 1.5f, 0.001f)); // 오타 수정
		glm::mat4 matrix = translateMatrix * rotMatrixY * scaleMatrix;
		glUniformMatrix4fv(m_texShader_modelLoc, 1, GL_FALSE, glm::value_ptr(matrix));

		glBindTexture(GL_TEXTURE_2D, storeSignTexture);
		glDrawArrays(GL_TRIANGLES, 0, cubeMesh.vertexCount);

		glUniform1i(m_texShader_useLightLoc, GL_TRUE);

		glDisable(GL_BLEND);
	}





	// 동물 그리기
	{
		for (int i = 0; i < pigCount; ++i) pigs[i]->draw(viewMatrix, projMatrix, light);
		// 알파카
		for (int i = 0; i < alpacaCount; ++i) alpacas[i]->draw(viewMatrix, projMatrix, light);
		// 펭귄
		for (int i = 0; i < penguinCount; ++i) penguins[i]->draw(viewMatrix, projMatrix, light);
		// 치킨
		for (int i = 0; i < chickenCount; ++i) chics[i]->draw(viewMatrix, projMatrix, light);
		// 여우
		for (int i = 0; i < foxCount; ++i) 	foxes[i]->draw(viewMatrix, projMatrix, light);

	}	// 동물 ㄱ리기 끝..

	MeshData ddongMesh = m_resourceManager->getMesh("ddong");
	GLuint ddongButtonTexture = m_resourceManager->getTexture("ddongButton");

	glUseProgram(objShader);
	glUniformMatrix4fv(m_objShader_viewLoc, 1, GL_FALSE, glm::value_ptr(viewMatrix));
	glUniformMatrix4fv(m_objShader_projLoc, 1, GL_FALSE, glm::value_ptr(projMatrix));
	glUniform1i(m_objShader_useGlobalColorLoc, GL_TRUE); // 단색 사용 ON
	glUniform3f(m_objShader_globalColorLoc, 93 / 255.f, 44 / 255.f, 11 / 255.f); // 똥 색상
	glBindVertexArray(ddongMesh.VAO);

	for (auto& pair : poopMap) {
		GameObject* pObj = pair.second; // 맵에 저장된 GameObject 포인터

		// GameObject에서 현재 위치 가져오기
		glm::vec3 pos = pObj->getPosition();

		glm::mat4 translateMatrix = glm::translate(glm::mat4(1.f), pos);
		glm::mat4 rotMatrixY = glm::rotate(glm::mat4(1.f), glm::radians(90.f), glm::vec3(0.f, 1.f, 0.f));
		glm::mat4 scaleMatrix = glm::scale(glm::mat4(1.f), glm::vec3(0.3f));

		glm::mat4 matrix = translateMatrix * rotMatrixY * scaleMatrix;
		glUniformMatrix4fv(m_objShader_modelLoc, 1, GL_FALSE, glm::value_ptr(matrix));

		glDrawArrays(GL_TRIANGLES, 0, ddongMesh.vertexCount);
	}

	// 플레이어 그리기
	glEnable(GL_DEPTH_TEST);
	glm::vec3 lightPos(50.0f, 100.0f, 50.0f);
	GLuint playerTexture = m_resourceManager->getTexture("tino");
	glBindTexture(GL_TEXTURE_2D, playerTexture);
	if (g_myPlayer != nullptr)
	{
		g_myPlayer->draw(viewMatrix, projMatrix, lightPos);
	}

	for (int i = 0; i < 2; ++i)
	{
		if (otherPlayer[i] != nullptr)
		{
			otherPlayer[i]->draw(viewMatrix, projMatrix, lightPos);
		}
	}
	glDisable(GL_DEPTH_TEST);

	//// 플레이어 그리고 그 위에 id추가하기
	//// 폰트 출력
	//{
	//	glEnable(GL_BLEND);
	//	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	//	glDisable(GL_DEPTH_TEST);

	//	glUseProgram(fontShader);
	//	glBindVertexArray(fontQuadVAO);
	//	glActiveTexture(GL_TEXTURE0);
	//	glBindTexture(GL_TEXTURE_2D, fontTexture);

	//	glm::mat4 textProjection = glm::ortho(0.0f, static_cast<float>(width), 0.0f, static_cast<float>(height));
	//	glUniformMatrix4fv(font_projLoc, 1, GL_FALSE, glm::value_ptr(textProjection));

	//	// id
	//	DrawTextWithAtlas(m_myPlayerID, screenPos.x + textOffsetX, screenPos.y + idOffsetY, currentFontSize);

	//	glBindVertexArray(0);
	//	glBindTexture(GL_TEXTURE_2D, 0);
	//	glEnable(GL_DEPTH_TEST);
	//	glDisable(GL_BLEND);
	//}



	// ui 설정용
	glm::mat4 uiViewMatrix = glm::mat4(1.0f);
	glm::mat4 uiProjMatrix = glm::ortho(0.0f, static_cast<float>(width), 0.0f, static_cast<float>(height));
	glUniformMatrix4fv(m_texShader_viewLoc, 1, GL_FALSE, glm::value_ptr(uiViewMatrix));
	glUniformMatrix4fv(m_texShader_projLoc, 1, GL_FALSE, glm::value_ptr(uiProjMatrix));

	// 코인.. + 사료
	cubeMesh = m_resourceManager->getMesh("cube");
	GLuint coinTexture = m_resourceManager->getTexture("coin");
	GLuint feedTexture = m_resourceManager->getTexture("feedpack");
	GLuint sellTexture = m_resourceManager->getTexture("growNsell");
	float uiStartX = 50.0f;           // ui 첫번째 x위치
	float uiGap = 75.0f;              // 간격

	glUseProgram(texShader);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_DEPTH_TEST);

	glBindVertexArray(cubeMesh.VAO);
	glUniform1i(m_texShader_useLightLoc, GL_FALSE); // UI는 조명 영향 안 받음..

	// 코인
	glBindTexture(GL_TEXTURE_2D, coinTexture);
	for (int i = 0; i < player->getCoin(); ++i) {
		float xPos = uiStartX + (i * uiGap);
		glm::mat4 translateMatrix = glm::translate(glm::mat4(1.f), glm::vec3(xPos, 840.f, 0.f));
		glm::mat4 sclaeMatrix = glm::scale(glm::mat4(1.f), glm::vec3(70.f, 70.f, 0.1f));
		glm::mat4 matrix = translateMatrix * sclaeMatrix;
		glUniformMatrix4fv(m_texShader_modelLoc, 1, GL_FALSE, glm::value_ptr(matrix));
		glDrawArrays(GL_TRIANGLES, 0, 6);
	}
	// 사료
	glBindTexture(GL_TEXTURE_2D, feedTexture);
	for (int i = 0; i < player->getFeed(); ++i) {
		float xPos = uiStartX + (i * uiGap);
		glm::mat4 translateMatrix = glm::translate(glm::mat4(1.f), glm::vec3(xPos, 50.f, 0.f));
		glm::mat4 scaleMatrix = glm::scale(glm::mat4(1.f), glm::vec3(70.f, 70.f, 0.1f)); // 직접 계산된 값 사용
		glm::mat4 matrix = translateMatrix * scaleMatrix; // 회전 없음
		glUniformMatrix4fv(m_texShader_modelLoc, 1, GL_FALSE, glm::value_ptr(matrix));
		glDrawArrays(GL_TRIANGLES, 0, 6);
	}

	glUniformMatrix4fv(m_texShader_viewLoc, 1, GL_FALSE, glm::value_ptr(uiViewMatrix));
	glUniformMatrix4fv(m_texShader_projLoc, 1, GL_FALSE, glm::value_ptr(uiProjMatrix));
	glm::mat4 translateMatrixUI = glm::translate(glm::mat4(1.f), glm::vec3(width / 2, height / 2, 0.f)); // 화면 중앙 기준으로 가정
	glUniform1i(m_texShader_useLightLoc, GL_FALSE); // UI는 조명 영향 안 받음..


	if (m_currentInteractType == EInteractType::DDONG)
	{
		for (auto& pair : poopMap) {
			GameObject* poop = pair.second;

			if (poop->isNear) {
				glUseProgram(texShader);
				glEnable(GL_BLEND);
				glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
				glDisable(GL_DEPTH_TEST);
				glBindVertexArray(cubeMesh.VAO);
				glBindTexture(GL_TEXTURE_2D, ddongButtonTexture);

				glm::mat4 scaleMatrixUI = glm::scale(glm::mat4(1.f), glm::vec3(1000.f, 500.f, 0.001f));
				glm::mat4 matrixUI = translateMatrixUI * scaleMatrixUI; // 회전 없음
				glUniformMatrix4fv(m_texShader_modelLoc, 1, GL_FALSE, glm::value_ptr(matrixUI));

				glDrawArrays(GL_TRIANGLES, 0, cubeMesh.vertexCount);

				glEnable(GL_DEPTH_TEST);
				glDisable(GL_BLEND);

				break;
			}
		}
	}


	// 동물 근처일때 UI
	if (m_currentInteractType == EInteractType::ANIMAL) {
		glUseProgram(texShader);
		glActiveTexture(GL_TEXTURE0);
		bool isDrawn = false;

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glDisable(GL_DEPTH_TEST);
		glBindVertexArray(cubeMesh.VAO);

		GLuint feedButtonTexture = m_resourceManager->getTexture("feedButton");
		if (not isDrawn) {
			for (int i = 0; i < pigCount; ++i) {
				if (pigs[i]->isNear) {
					GLuint targetTex = pigs[i]->isBaby ? feedButtonTexture : sellTexture;
					glBindTexture(GL_TEXTURE_2D, targetTex);

					glm::mat4 scaleMatrixUI = glm::scale(glm::mat4(1.f), glm::vec3(1000.f, 500.f, 0.001f));
					glm::mat4 matrixUI = translateMatrixUI * scaleMatrixUI;
					glUniformMatrix4fv(m_texShader_modelLoc, 1, GL_FALSE, glm::value_ptr(matrixUI));

					glDrawArrays(GL_TRIANGLES, 0, cubeMesh.vertexCount);

					isDrawn = true;
					break;
				}
			}
		}
		if (not isDrawn) {
			for (int i = 0; i < alpacaCount; ++i) {
				if (alpacas[i]->isNear) {
					GLuint targetTex = alpacas[i]->isBaby ? feedButtonTexture : sellTexture;
					glBindTexture(GL_TEXTURE_2D, targetTex);

					glm::mat4 scaleMatrixUI = glm::scale(glm::mat4(1.f), glm::vec3(1000.f, 500.f, 0.001f));
					glm::mat4 matrixUI = translateMatrixUI * scaleMatrixUI;
					glUniformMatrix4fv(m_texShader_modelLoc, 1, GL_FALSE, glm::value_ptr(matrixUI));

					glDrawArrays(GL_TRIANGLES, 0, cubeMesh.vertexCount);
					isDrawn = true;
					break;
				}
			}
		}
		if (not isDrawn) {
			for (int i = 0; i < alpacaCount; ++i) {
				if (penguins[i]->isNear) {
					GLuint targetTex = penguins[i]->isBaby ? feedButtonTexture : sellTexture;
					glBindTexture(GL_TEXTURE_2D, targetTex);

					glm::mat4 scaleMatrixUI = glm::scale(glm::mat4(1.f), glm::vec3(1000.f, 500.f, 0.001f));
					glm::mat4 matrixUI = translateMatrixUI * scaleMatrixUI;
					glUniformMatrix4fv(m_texShader_modelLoc, 1, GL_FALSE, glm::value_ptr(matrixUI));

					glDrawArrays(GL_TRIANGLES, 0, cubeMesh.vertexCount);
					isDrawn = true;
					break;
				}
			}
		}
		if (not isDrawn) {
			for (int i = 0; i < alpacaCount; ++i) {
				if (chics[i]->isNear) {
					GLuint targetTex = chics[i]->isBaby ? feedButtonTexture : sellTexture;
					glBindTexture(GL_TEXTURE_2D, targetTex);

					glm::mat4 scaleMatrixUI = glm::scale(glm::mat4(1.f), glm::vec3(1000.f, 500.f, 0.001f));
					glm::mat4 matrixUI = translateMatrixUI * scaleMatrixUI;
					glUniformMatrix4fv(m_texShader_modelLoc, 1, GL_FALSE, glm::value_ptr(matrixUI));

					glDrawArrays(GL_TRIANGLES, 0, cubeMesh.vertexCount);
					isDrawn = true;
					break;
				}
			}
		}
		if (not isDrawn) {
			for (int i = 0; i < alpacaCount; ++i) {
				if (foxes[i]->isNear) {
					GLuint targetTex = foxes[i]->isBaby ? feedButtonTexture : sellTexture;
					glBindTexture(GL_TEXTURE_2D, targetTex);

					glm::mat4 scaleMatrixUI = glm::scale(glm::mat4(1.f), glm::vec3(1000.f, 500.f, 0.001f));
					glm::mat4 matrixUI = translateMatrixUI * scaleMatrixUI;
					glUniformMatrix4fv(m_texShader_modelLoc, 1, GL_FALSE, glm::value_ptr(matrixUI));

					glDrawArrays(GL_TRIANGLES, 0, cubeMesh.vertexCount);
					isDrawn = true;
					break;
				}
			}
		}

		glEnable(GL_DEPTH_TEST);
		glDisable(GL_BLEND);
	}

	// 플레이어가 상점 근처에 오면 UI뜨게..
	GLuint storeUITexture = m_resourceManager->getTexture("storeScene"); // UI 텍스처
	if (player->isStoreShow) {
		{
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			glUseProgram(texShader);
			glBindVertexArray(cubeMesh.VAO);
			glDisable(GL_DEPTH_TEST); // UI는 보통 깊이 테스트를 끔..

			glUniform1i(m_texShader_useLightLoc, GL_FALSE); // UI는 조명 영향 안 받음..

			glm::mat4 translateMatrix = glm::translate(glm::mat4(1.f), glm::vec3(width / 2, height / 2, 0.f));
			glm::mat4 scaleMatrix = glm::scale(glm::mat4(1.f), glm::vec3(1000.f, 1000.f, 0.001f));
			glm::mat4 matrix = translateMatrix * scaleMatrix;
			glUniformMatrix4fv(m_texShader_modelLoc, 1, GL_FALSE, glm::value_ptr(matrix));

			glBindTexture(GL_TEXTURE_2D, storeUITexture);
			glDrawArrays(GL_TRIANGLES, 0, cubeMesh.vertexCount);

			glEnable(GL_DEPTH_TEST); // 깊이 테스트 복원
			glDisable(GL_BLEND);
		}
	}

	glUniform1i(m_texShader_useLightLoc, GL_TRUE); // UI는 조명 영향 안 받음..



	if (isSnow)
	{
		MeshData sphereMesh = m_resourceManager->getMesh("sphere");
		glUseProgram(objShader);
		glBindVertexArray(sphereMesh.VAO);

		glUniform1i(m_objShader_useGlobalColorLoc, GL_TRUE); // 단색 사용 ON
		glUniform3f(m_objShader_globalColorLoc, 1.f, 1.f, 1.f);

		for (auto& snow : snows) {
			if (snow.isDraw) {
				glm::mat4 transTransform = glm::translate(glm::mat4(1.f), glm::vec3(snow.x, snow.y, snow.z));
				glm::mat4 scaleMat = glm::scale(glm::mat4(1.f), glm::vec3(snow.size));
				glm::mat4 finalMat = transTransform * scaleMat;
				glUniformMatrix4fv(m_objShader_modelLoc, 1, GL_FALSE, glm::value_ptr(finalMat));

				glDrawArrays(GL_TRIANGLES, 0, sphereMesh.vertexCount);
			}
		}
	}


	// 낮/밤 조명..
	glm::vec3 lightColorValue;
	if (isDay) {
		lightColorValue = glm::vec3(255.f / 255.f, 242.f / 255.f, 230.f / 255.f); // 밝은 색
	}
	else { // not isDay
		lightColorValue = glm::vec3(0.f / 255.f, 40.f / 255.f, 145.f / 255.f);   // 어두운 색 (파란색 계열)
	}

	glUseProgram(texShader);
	glUniform3fv(m_texShader_lightColorLoc, 1, glm::value_ptr(lightColorValue));

	glUseProgram(objShader);
	glUniform3fv(m_objShader_lightColorLoc, 1, glm::value_ptr(lightColorValue));

	glUseProgram(animalShader);
	glUniform3fv(m_animalShader_lightColorLoc, 1, glm::value_ptr(lightColorValue));
}

void gameScene::checkInteraction(glm::vec3 playerPosition) {
	m_currentInteractType = EInteractType::NONE;

	// 똥 근접 체크
	for (auto& pair : poopMap) {
		GameObject* poop = pair.second;

		// 똥의 위치 가져오기
		glm::vec3 poopPos = poop->getPosition();

		bool isHit = (playerPosition[0] > poopPos.x - 1.2f and
			playerPosition[0] < poopPos.x + 1.2f and
			playerPosition[2] > poopPos.z - 1.2f and
			playerPosition[2] < poopPos.z + 1.2f);

		if (isHit and m_currentInteractType == EInteractType::NONE) {
			m_currentInteractType = EInteractType::DDONG;
			poop->isNear = true;
		}
		else {
			poop->isNear = false;
		}
	}

	// 똥  근접 아니면 동물 근접 체크
	if (m_currentInteractType == EInteractType::NONE) {
		for (int i = 0; i < pigCount; ++i) {
			const glm::vec3 pigPosition = pigs[i]->getPosition();

			bool isHit = (playerPosition[0] > pigPosition[0] - 1.5f and
				playerPosition[0] < pigPosition[0] + 1.5f and
				playerPosition[2] > pigPosition[2] - 1.5f and
				playerPosition[2] < pigPosition[2] + 1.5f);

			if (isHit and m_currentInteractType == EInteractType::NONE) {
				m_currentInteractType = EInteractType::ANIMAL;
				pigs[i]->isNear = true;
			}
			else {
				pigs[i]->isNear = false;
			}
		}

		for (int i = 0; i < alpacaCount; ++i) {
			const glm::vec3 animalPosition = alpacas[i]->getPosition();

			bool isHit = (playerPosition[0] > animalPosition[0] - 1.5f and
				playerPosition[0] < animalPosition[0] + 1.5f and
				playerPosition[2] > animalPosition[2] - 1.5f and
				playerPosition[2] < animalPosition[2] + 1.5f);

			if (isHit && m_currentInteractType == EInteractType::NONE) {
				m_currentInteractType = EInteractType::ANIMAL;
				alpacas[i]->isNear = true;
			}
			else {
				alpacas[i]->isNear = false;
			}
		}

		for (int i = 0; i < penguinCount; ++i) {
			const glm::vec3 animalPosition = penguins[i]->getPosition();
			bool isHit = (playerPosition[0] > animalPosition[0] - 1.5f and
				playerPosition[0] < animalPosition[0] + 1.5f and
				playerPosition[2] > animalPosition[2] - 1.5f and
				playerPosition[2] < animalPosition[2] + 1.5f);

			if (isHit and m_currentInteractType == EInteractType::NONE) {
				m_currentInteractType = EInteractType::ANIMAL;
				penguins[i]->isNear = true;
			}
			else {
				penguins[i]->isNear = false;
			}
		}

		for (int i = 0; i < chickenCount; ++i) {
			const glm::vec3 animalPosition = chics[i]->getPosition();

			bool isHit = (playerPosition[0] > animalPosition[0] - 1.5f and
				playerPosition[0] < animalPosition[0] + 1.5f and
				playerPosition[2] > animalPosition[2] - 1.5f and
				playerPosition[2] < animalPosition[2] + 1.5f);

			if (isHit and m_currentInteractType == EInteractType::NONE) {
				m_currentInteractType = EInteractType::ANIMAL;
				chics[i]->isNear = true;
			}
			else {
				chics[i]->isNear = false;
			}
		}

		for (int i = 0; i < foxCount; ++i) {
			const glm::vec3 animalPosition = foxes[i]->getPosition();
			bool isHit = (playerPosition[0] > animalPosition[0] - 1.5f and
				playerPosition[0] < animalPosition[0] + 1.5f and
				playerPosition[2] > animalPosition[2] - 1.5f and
				playerPosition[2] < animalPosition[2] + 1.5f);

			if (isHit and m_currentInteractType == EInteractType::NONE) {
				m_currentInteractType = EInteractType::ANIMAL;
				foxes[i]->isNear = true;
			}
			else {
				foxes[i]->isNear = false;
			}
		}
	}
}

void gameScene::updateAnimalPos(int type, int id, float x, float y)
{
	switch (type) {
	case AnimalType::PIG:
		if (id >= 0 && id < pigCount) // 범위 체크
			static_cast<Pig*>(pigs[id])->setTargetPosition(x, y);
		break;
	case AnimalType::PENGUIN:
		if (id >= 0 && id < penguinCount) // 범위 체크
			static_cast<Penguin*>(penguins[id])->setTargetPosition(x, y);
		break;
	case AnimalType::FOX:
		if (id >= 0 && id < foxCount) // 범위 체크
			static_cast<Fox*>(foxes[id])->setTargetPosition(x, y);
		break;
	case AnimalType::ALPACA:
		if (id >= 0 && id < alpacaCount) // 범위 체크
			static_cast<Alpaca*>(alpacas[id])->setTargetPosition(x, y);
		break;
	case AnimalType::CHICKEN:
		if (id >= 0 && id < chickenCount) // 범위 체크
			static_cast<Chic*>(chics[id])->setTargetPosition(x, y);
		break;
	}
}

void gameScene::keyboard(unsigned char key, bool isPressed)
{
	// 이동 처리 (WASD) 
	player->keyboard(key, isPressed);

	if (isPressed)
	{
		switch (key)
		{
		case '0': // 돼지 구매
		case '1': // 병아리 구매
		case '2': // 알파카 구매
		case '3': // 펭귄 구매
		case '4': // 여우 구매
			if (player->isStoreShow)
			{
				cs_request_buy_animal packet;
				packet.animalType = key - '0';
				sendPacket(g_sock, PacketType::CS_REQUEST_BUY_ANIMAL, (char*)&packet, sizeof(cs_request_buy_animal));
			}
			break;
		case 32: // 스페이스바
			printf("test용 문구..스페이스바눌림\n");
			// 똥 줍기..
			if (m_currentInteractType == EInteractType::DDONG) {

				int targetPoopID = -1;
				for (auto& pair : poopMap) {
					if (pair.second->isNear) {
						targetPoopID = pair.first;
						break;
					}
				}

				if (targetPoopID != -1) {
					cs_request_clean_poop packet;
					packet.poopID = targetPoopID;
					sendPacket(g_sock, PacketType::CS_REQUEST_CLEAN_POOP, (char*)&packet, sizeof(cs_request_clean_poop));

					printf("[클라] 똥(%d) 청소 요청 보냄\n", targetPoopID);
				}
			}

			// 동물들가까이 갔을 때.. 상호작용들
			// 돼지
			if (m_currentInteractType == EInteractType::ANIMAL) {

				for (int i = 0; i < pigCount; ++i) {
					if (pigs[i]->isNear) {
						if (pigs[i]->isBaby) { // 아기 돼지?? -> 밥 주기
							if (player->getFeed() > 0) {
								// feed 요청 패킷 보내기
								cs_request_feed_animal packet;
								packet.AnimalID = i;
								packet.AnimalType = AnimalType::PIG;
								sendPacket(g_sock, PacketType::CS_REQUEST_FEED, (char*)(&packet), sizeof(cs_request_feed_animal));
								break;
							}
						}
						else {
							std::cout << " 돼지 팔음" << std::endl;
							if (player->getCoin() < player->getMaxCoin() - 1) {
								// 패킷 보내기
								cs_request_sell_animal packet;
								packet.AnimalID = i;
								packet.AnimalType = AnimalType::PIG;
								sendPacket(g_sock, PacketType::CS_REQUEST_SELL, (char*)(&packet), sizeof(cs_request_feed_animal));
							}
							/*delete pigs[i];
							if (i != pigCount - 1) {
								pigs[i] = pigs[pigCount - 1];
							}
							pigs[pigCount - 1] = nullptr;
							pigCount--;
							i--;*/
						}
					}
				}

				for (int i = 0; i < alpacaCount; ++i) {
					if (alpacas[i]->isNear) {
						if (alpacas[i]->isBaby) { // 아기 돼지?? -> 밥 주기
							if (player->getFeed() > 0) {
								// feed 요청 패킷 보내기
								cs_request_feed_animal packet;
								packet.AnimalID = i;
								packet.AnimalType = AnimalType::ALPACA;
								sendPacket(g_sock, PacketType::CS_REQUEST_FEED, (char*)(&packet), sizeof(cs_request_feed_animal));
								break;
							}
						}
						else {
							std::cout << " 알파카 팔음" << std::endl;
							if (player->getCoin() < player->getMaxCoin() - 1) {
								// 패킷 보내기
								cs_request_sell_animal packet;
								packet.AnimalID = i;
								packet.AnimalType = AnimalType::ALPACA;
								sendPacket(g_sock, PacketType::CS_REQUEST_SELL, (char*)(&packet), sizeof(cs_request_feed_animal));
							}
							/*delete alpacas[i];
							if (i != alpacaCount - 1) {
								alpacas[i] = alpacas[alpacaCount - 1];
							}
							alpacas[alpacaCount - 1] = nullptr;
							alpacaCount--;
							i--;*/
						}
					}
				}
				// 펭귄
				for (int i = 0; i < penguinCount; ++i) {
					if (penguins[i]->isNear) {
						if (penguins[i]->isBaby) {
							if (player->getFeed() > 0) {
								// feed 요청 패킷 보내기
								cs_request_feed_animal packet;
								packet.AnimalID = i;
								packet.AnimalType = AnimalType::PENGUIN;
								sendPacket(g_sock, PacketType::CS_REQUEST_FEED, (char*)(&packet), sizeof(cs_request_feed_animal));
								break;
							}
						}
						else {
							std::cout << " 펭귄 팔음" << std::endl;
							if (player->getCoin() < player->getMaxCoin() - 1) {
								// 패킷 보내기
								cs_request_sell_animal packet;
								packet.AnimalID = i;
								packet.AnimalType = AnimalType::PENGUIN;
								sendPacket(g_sock, PacketType::CS_REQUEST_SELL, (char*)(&packet), sizeof(cs_request_feed_animal));

							}
							/*delete penguins[i];
							if (i != penguinCount - 1) {
								penguins[i] = alpacas[penguinCount - 1];
							}
							penguins[penguinCount - 1] = nullptr;
							penguinCount--;
							i--;*/
						}

					}
				}
				// 양념 치킨
				for (int i = 0; i < chickenCount; ++i) {
					if (chics[i]->isNear) {
						if (chics[i]->isBaby) {
							if (player->getFeed() > 0) {
								// feed 요청 패킷 보내기
								cs_request_feed_animal packet;
								packet.AnimalID = i;
								packet.AnimalType = AnimalType::CHICKEN;
								sendPacket(g_sock, PacketType::CS_REQUEST_FEED, (char*)(&packet), sizeof(cs_request_feed_animal));
								break;
							}
						}
						else {
							std::cout << " 취킨 팔음" << std::endl;
							if (player->getCoin() < player->getMaxCoin() - 1) {
								// 패킷보내야지
								cs_request_sell_animal packet;
								packet.AnimalID = i;
								packet.AnimalType = AnimalType::CHICKEN;
								sendPacket(g_sock, PacketType::CS_REQUEST_SELL, (char*)(&packet), sizeof(cs_request_feed_animal));
							}
							/*delete chics[i];
							if (i != chickenCount - 1) {
								chics[i] = alpacas[chickenCount - 1];
							}
							chics[chickenCount - 1] = nullptr;
							chickenCount--;
							i--;*/
						}

					}
				}
				// 여우
				for (int i = 0; i < foxCount; ++i) {
					if (foxes[i]->isNear) {
						if (foxes[i]->isBaby) {
							if (player->getFeed() > 0) {
								// feed 요청 패킷 보내기
								cs_request_feed_animal packet;
								packet.AnimalID = i;
								packet.AnimalType = AnimalType::FOX;
								sendPacket(g_sock, PacketType::CS_REQUEST_FEED, (char*)(&packet), sizeof(cs_request_feed_animal));
								break;
							}
						}
						else {
							std::cout << " 여우 팔음" << std::endl;
							if (player->getCoin() < player->getMaxCoin() - 1) {
								// 패킷 보내기
								cs_request_sell_animal packet;
								packet.AnimalID = i;
								packet.AnimalType = AnimalType::ALPACA;
								sendPacket(g_sock, PacketType::CS_REQUEST_SELL, (char*)(&packet), sizeof(cs_request_feed_animal));
							}
							/*delete foxes[i];
							if (i != foxCount - 1) {
								foxes[i] = alpacas[foxCount - 1];
							}
							foxes[foxCount - 1] = nullptr;
							foxCount--;
							i--;*/
						}

					}
				}
			}

			break; // case 32 끝
		default:
			// 다른 키는 아무것도 안 함
			break;
		}
	}


}

// 이게 큐에 넣는거
void gameScene::EnqueueAnimalMove(int type, int id, float x, float y)
{
	std::lock_guard<std::mutex> lock(m_queueLock);

	AnimalMoveInfo mv;
	mv.type = type;
	mv.id = id;
	mv.x = x;
	mv.y = y;

	m_animalMoveQueue.push(mv);
	printf("큐에넣음\n");
}

void gameScene::ProcessAnimalMoveQueue()	// 이제 여기서 처리.. 큐에있으면 업뎃
{
	std::lock_guard<std::mutex> lock(m_queueLock);

	while (!m_animalMoveQueue.empty())	// 빈게 아니면
	{
		AnimalMoveInfo mv = m_animalMoveQueue.front();
		m_animalMoveQueue.pop();

		updateAnimalPos(mv.type, mv.id, mv.x, mv.y);
		printf("업뎃함\n");
	}
}

void gameScene::specialKeyboard(int key, bool isPressed)
{
}

void gameScene::mouse(int button, int state, int x, int y)
{
	player->mouse(button, state, x, y);
	constexpr int WHEEL_UP = 3;
	constexpr int WHEEL_DOWN = 4;
	switch (state) {
	case GLUT_DOWN:
		switch (button) {
		case GLUT_LEFT_BUTTON: {
			float xPos = (static_cast<float>(x) / width * 2.f - 1.f);
			float yPos = -(static_cast<float>(y) / height * 2.f - 1.f);
			/*	std::cout << "좌클릭 : " << x << ", " << y << std::endl;
				std::cout << "OpenGL x 좌표는 " << xPos << std::endl;
				std::cout << "OpenGL y 좌표는 " << yPos << std::endl;*/
			break;
		}
		case GLUT_MIDDLE_BUTTON:
			//std::cout << "휠클릭 : " << x << ", " << y << std::endl;
			break;
		case GLUT_RIGHT_BUTTON:
			//std::cout << "우클릭 : " << x << ", " << y << std::endl;
			break;
		case WHEEL_UP:
			//std::cout << "휠  업 : " << x << ", " << y << std::endl;
			break;
		case WHEEL_DOWN:
			//std::cout << "휠다운 : " << x << ", " << y << std::endl;
			break;
		}
		break;
	case GLUT_UP:
		switch (button) {
		case GLUT_LEFT_BUTTON:
			break;
		case GLUT_RIGHT_BUTTON:
			break;
		case GLUT_MIDDLE_BUTTON:
			break;
		}
		break;
	}
}

void gameScene::mouseMove(int x, int y)
{
	player->mouseMove(x, y);
}

void gameScene::setWindowSize(int winWidth, int winHeight)
{
	width = winWidth;
	height = winHeight;
}


