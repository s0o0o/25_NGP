#include "Scene.h"

#include <iostream>
#include <fstream>
#include <sstream>

#include <vector>

#include "PlayerObject.h"
#include "Pig.h"
#include "Alpaca.h"
#include "Penguin.h"
#include "Chic.h"
#include "Fox.h"

#include <random>

#define STB_IMAGE_IMPLEMENTATION		// 단 하나의 .cpp 에만 define 해줘야 한다.. 중복 include 주의!
#include <stb_image.h>

struct Ddong {
	float x, y, z;

	bool isDraw;
	bool isNear;
};
std::vector<Ddong> ddongs;

struct Snow {
	float x, y, z;
	float size;
	float speed;

	bool isDraw;
};

std::vector<Snow> snows;

Scene::Scene(int winWidth, int winHeight)
	: width{ winWidth }, height{ winHeight }
{

}

Scene::~Scene()
{
	release();
}

void Scene::initialize()
{

	srand(static_cast<unsigned int>(time(NULL)));

	shader = makeShader("./Shader/vertex.glsl", "./Shader/fragment.glsl");

	objShader = makeShader("./Shader/obj_vertex.glsl", "./Shader/obj_fragment.glsl");
	bgShader = makeShader("./Shader/bg_vertex.glsl", "./Shader/bg_fragment.glsl");

	plainShader = makeShader("./Shader/plainVert.glsl", "./Shader/plainFrag.glsl");
	texShader = makeShader("./Shader/tex_vertex.glsl", "./Shader/tex_fragement.glsl");

	animalShader = makeShader("./Shader/Animalvertex.glsl", "./Shader/Animalfragment.glsl");

	initBuffer(&sphereVAO, &sphereVertexCount, "./OBJ/sphere.obj");

	initCubeBuffer(&cubeVAO, &cubeVertexCount);

	// 울타리 설치
	initBuffer(&fenceVAO, &fenceVertexCount, "./OBJ/fence.obj");

	// 나무 설치
	initBuffer(&treeVAO, &treeVertexCount, "./OBJ/tree_up.obj");
	initBuffer(&treeVAO2, &treeVertexCount2, "./OBJ/tree_bottom.obj");

	// 동물 그릴용 큐브
	initBuffer(&animalVAO, &animalVertexCount, "./OBJ/cube.obj");

	// 집 설치
	initBuffer(&house_top_VAO, &houseTopVertexCount, "./OBJ/house_top.obj");
	initBufferWithUV(&house_bottom_VAO, &houseBottomVertexCount, "./OBJ/house_bottom.obj");

	// farm sign설치
	initBufferWithUV(&sign_top_VAO, &signTopVertexCount, "./OBJ/sign_up.obj");
	initBufferWithUV(&sign_bottom_VAO, &signBottomVertexCount, "./OBJ/sign_bottom.obj");

	// 똥 설치
	initBuffer(&ddongVAO, &ddongVertexCount, "./OBJ/ddong.obj");

	// 코인
	initBufferWithUV(&coinVAO, &coinVertexCount, "./OBJ/coin.obj");

	initBufferWithUV(&foodVAO, &foodVertexCount, "./OBJ/feed.obj");


	std::string filenames5 = { "./Img/farmsign_rest.png" };
	initTexture(&signTexture[0], 1, &filenames5);
	std::string filenames6 = { "./Img/farmsign.png" };
	initTexture(&signTexture[1], 1, &filenames6);

	initBackGroundBuffer();
	std::string filenames3 = { "./Img/grass.jpg" };
	initTexture(&GroundTexture, 1, &filenames3);

	std::string filenames15 = { "./Img/snowGrass.png" };
	initTexture(&GroundTexture2, 1, &filenames15);

	std::string filenames = { "./Img/sky1.png" };
	initTexture(&BGTexture, 1, &filenames);

	std::string filenames2 = { "./Img/sky2.png" };
	initTexture(&BGTexture2, 1, &filenames2);

	std::string filenames4 = { "./Img/houseWood.png" };
	initTexture(&houseTexture[0], 1, &filenames4);

	std::string filenames7 = { "./Img/store.png" };
	initTexture(&houseTexture[1], 1, &filenames7);

	std::string filenames8 = { "./Img/storeScene.png" };
	initTexture(&storeTexture, 1, &filenames8);

	std::string filenames9 = { "./Img/ddongButton.png" };
	initTexture(&ddongLogTexture, 1, &filenames9);

	std::string filenames10 = { "./Img/feedButton.png" };
	initTexture(&feedLogTexture, 1, &filenames10);

	std::string filenames11 = { "./Img/coin.png" };
	initTexture(&coinTexture, 1, &filenames11);

	std::string filenames12 = { "./Img/buy.png" };
	initTexture(&buySuccessTexture, 1, &filenames12);

	std::string filenames13 = { "./Img/feedpack.png" };
	initTexture(&feedPackTexture, 1, &filenames13);

	std::string filenames14 = { "./Img/growNsell.png" };
	initTexture(&growDoneTexture, 1, &filenames14);

	light.x = 0.f;
	light.y = 2.f;
	light.z = -2.f;
	isLightMove = false;

	player = new PlayerObject;

	player->rotateY(180.f);

	player->setPosition(0.f, 0.f, 15.f);

	maxCoin = 20;
	maxFeed = 20;
	objectCount = 2;
	townObjectCount = 15;

	for (int i = 0; i < 10; ++i) {
		rotateTreeY[i] = static_cast<float>(std::rand()) / RAND_MAX * 360.0f;
	}

	pigs[0] = new Pig; // pig는 게임객체... 업캐스팅........
	pigs[0]->setShader(animalShader);
	pigs[0]->setVAO(animalVAO, animalVertexCount);

	pigs[1] = new Pig;
	pigs[1]->setShader(animalShader);
	pigs[1]->setVAO(animalVAO, animalVertexCount);

	pigCount = 2;

	alpacas[0] = new Alpaca; // alpaca는 게임객체... 업캐스팅........
	alpacas[0]->setShader(animalShader);
	alpacas[0]->setVAO(animalVAO, animalVertexCount);

	alpacaCount = 1;

	chics[0] = new Chic; // penguin는 게임객체... 업캐스팅........
	chics[0]->setShader(animalShader);
	chics[0]->setVAO(animalVAO, animalVertexCount);

	chickenCount = 1;

	foxes[0] = new Fox; // penguin는 게임객체... 업캐스팅........
	foxes[0]->setShader(animalShader);
	foxes[0]->setVAO(animalVAO, animalVertexCount);

	foxCount = 1;

	penguins[0] = new Penguin; // penguin는 게임객체... 업캐스팅........
	penguins[0]->setShader(animalShader);
	penguins[0]->setVAO(animalVAO, animalVertexCount);

	penguinCount = 1;


	cameraY = 25.5f;
	isTitleAniEnd = false;
	isTitleAni = false;

	for (int i = 0; i < maxCoin; ++i) {
		coin_x[i] = -0.9f + i * 0.1f;
		isCoin[i] = false;
	}
	isCoin[0] = true;
	isCoin[1] = true;
	isCoin[2] = true;
	isCoin[3] = true;
	isCoin[4] = true;
	nowCoin = 5;

	for (int i = 0; i < maxFeed; ++i) {
		feed_x[i] = -0.9f + i * 0.1f;
		isFood[i] = false;
	}

	isFood[0] = true;
	isFood[1] = true;
	isFood[2] = true;
	isFood[3] = true;
	isFood[4] = true;
	nowFeed = 5;

	isSnow = true;
	isDay = true;
}

void Scene::release()
{
	for (int i = 0; i < objectCount; ++i)
		delete objects[i];

	//for (int i = 0; i < townObjectCount; ++i)
	//	delete townObjects[i];

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
}


void Scene::update(float elapsedTime)
{
	//std::cout << "hello 아임 씬2" << std::endl;
	// 플레이어 업뎃 
	player->update(elapsedTime);

	const glm::vec3 playerPosition = player->getPosition();

	for (int i = 0; i < pigCount; ++i) {
		const glm::vec3 pigPosition = pigs[i]->getPosition();

		if (playerPosition[0] > pigPosition[0] - 1.5f and
			playerPosition[0] < pigPosition[0] + 1.5f and
			playerPosition[2] > pigPosition[2] - 1.5f and
			playerPosition[2] < pigPosition[2] + 1.5f) {

			//std::cout << playerPosition[0] << std::endl;
			//std::cout << playerPosition[2] << std::endl;
			//std::cout << i << "번째 pig posX :" << pigPosition[0] << std::endl;
			//std::cout << i << "번째 pig posZ :" << pigPosition[2] << std::endl;

			pigs[i]->isNear = true;
			//if (pigs[i]->isNear) std::cout << i << " 번째 pig 와 만남!!" << std::endl;

		}
		else {
			pigs[i]->isNear = false;
		}
	}

	for (int i = 0; i < alpacaCount; ++i) {
		const glm::vec3 animalPosition = alpacas[i]->getPosition();

		if (playerPosition[0] > animalPosition[0] - 1.5f and
			playerPosition[0] < animalPosition[0] + 1.5f and
			playerPosition[2] > animalPosition[2] - 1.5f and
			playerPosition[2] < animalPosition[2] + 1.5f) {

			alpacas[i]->isNear = true;
			//if (alpacas[i]->isNear) std::cout << i << " 번째 alpaca 와 만남!!" << std::endl;

		}
		else {
			alpacas[i]->isNear = false;
		}
	}


	for (int i = 0; i < penguinCount; ++i) {
		const glm::vec3 animalPosition = penguins[i]->getPosition();

		if (playerPosition[0] > animalPosition[0] - 1.5f and
			playerPosition[0] < animalPosition[0] + 1.5f and
			playerPosition[2] > animalPosition[2] - 1.5f and
			playerPosition[2] < animalPosition[2] + 1.5f) {

			penguins[i]->isNear = true;
			//if (penguins[i]->isNear) std::cout << i << " 번째 penguin 와 만남!!" << std::endl;

		}
		else {
			penguins[i]->isNear = false;
		}
	}

	for (int i = 0; i < chickenCount; ++i) {
		const glm::vec3 animalPosition = chics[i]->getPosition();

		if (playerPosition[0] > animalPosition[0] - 1.5f and
			playerPosition[0] < animalPosition[0] + 1.5f and
			playerPosition[2] > animalPosition[2] - 1.5f and
			playerPosition[2] < animalPosition[2] + 1.5f) {

			chics[i]->isNear = true;
			//if (penguins[i]->isNear) std::cout << i << " 번째 penguin 와 만남!!" << std::endl;

		}
		else {
			chics[i]->isNear = false;
		}
	}

	for (int i = 0; i < foxCount; ++i) {
		const glm::vec3 animalPosition = foxes[i]->getPosition();

		if (playerPosition[0] > animalPosition[0] - 1.5f and
			playerPosition[0] < animalPosition[0] + 1.5f and
			playerPosition[2] > animalPosition[2] - 1.5f and
			playerPosition[2] < animalPosition[2] + 1.5f) {

			foxes[i]->isNear = true;
			//if (penguins[i]->isNear) std::cout << i << " 번째 penguin 와 만남!!" << std::endl;

		}
		else {
			foxes[i]->isNear = false;
		}
	}

	if (isLightMove) {
		light.x += 0.001f;
		if (light.x >= 5.f) {
			isLightMove = false;
		}
	}
	else {
		light.x -= 0.001f;
		if (light.x <= -5.f) {
			isLightMove = true;
		}
	}

	ddongTimeCount++;
	if (ddongTimeCount > 2500) {
		float x = -10.f + static_cast<float>(std::rand()) / (static_cast<float>(RAND_MAX / 8.f));
		float z = -5.f + static_cast<float>(std::rand()) / (static_cast<float>(RAND_MAX / 15.f));

		ddongs.emplace_back(Ddong{ x, 0.f, z , true ,false });
		ddongTimeCount = 0;
	}

	for (auto& ddong : ddongs) {
		if (ddong.isDraw)
		{
			if (playerPosition[0] > ddong.x - 1.2f and
				playerPosition[0] < ddong.x + 1.2f and
				playerPosition[2] > ddong.z - 1.2f and
				playerPosition[2] < ddong.z + 1.2f) {
				//	std::cout << "똥과 만남!!" << std::endl;
				ddong.isNear = true;
			}
			else {
				ddong.isNear = false;
			}
		}
	}

	// 객체들 업데이트..
	/*for (int i = 0; i < objectCount; ++i)
		objects[i]->update(elapsedTime);*/	// 업캐스팅 시에도 RotateObject의 update가 호출된다! -> virtual

	if (not isAnimalSleep) {
		for (int i = 0; i < pigCount; ++i) {
			pigs[i]->update(elapsedTime);
			if (pigs[i]->feedNum >= 5) {
				if (pigs[i]->isBaby) {
					pigs[i]->isBaby = false;
				}
				if (not pigs[i]->isBaby) {
					//std::cout << " 이제 돼지 어른" << std::endl;
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
					std::cout << " 이제 알파카 어른" << std::endl;
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
					//std::cout << " 이제 돼지 어른" << std::endl;
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
					//std::cout << " 이제 돼지 어른" << std::endl;
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

	if (isTitleAni) {
		cameraY -= 0.03f;
		if (cameraY <= 2.f) {
			isTitleAni = false;
			isTitleAniEnd = true;
		}
	}

	snowTime++;
	if (snowTime > 15000) {
		isSnow = not isSnow;
		snowTime = 0;
		snows.clear();
	}

	if (isSnow) {
		snowCount++;
		if (snowCount > 40) {
			randx = -20.f + static_cast<float>(std::rand()) / (static_cast<float>(RAND_MAX / 40.f));
			randz = -10.f + static_cast<float>(std::rand()) / (static_cast<float>(RAND_MAX / 20.f));
			size = 0.08f + (rand() / (float)RAND_MAX) * (0.03f - 0.01f);
			speed = 0.005f + (rand() / (float)RAND_MAX) * (0.000075f - 0.00005f);

			snows.emplace_back(Snow{ randx, 20.5f, randz, size,speed });
			snowCount = 0;
		}


		for (auto& snow : snows) {
			snow.isDraw = true;
			snow.y -= snow.speed;
			if (snow.y < 0.f) {
				snow.isDraw = false;
			}
		}
	}

	dayTime++;
	if (dayTime > 12000) {
		dayTime = 0;
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

void Scene::draw() const
{
	glm::mat4 viewMatrix = glm::lookAt(glm::vec3(0.f, cameraY, 15.f), glm::vec3(0.f, 0.5f, 0.f), glm::vec3(0.f, 1.f, 0.f));
	glm::vec3 cameraPos = glm::vec3(1.f);
	if (isTitleAniEnd) {
		cameraPos = player->getPosition();		// 플레이어 위치에서
		cameraPos.y = 1.5f;
		glm::vec3 targetPos = cameraPos + player->getLook(); // 플레이어 앞을 바라본다

		viewMatrix = glm::lookAt(cameraPos, targetPos, glm::vec3(0.f, 1.f, 0.f));
	}
	else if (not isTitleAniEnd) {

		viewMatrix = glm::lookAt(glm::vec3(0.f, cameraY, 25.f), glm::vec3(0.f, 2.5f, 10.f), glm::vec3(0.f, 1.f, 0.f));
	}

	glm::mat4 projMatrix = glm::perspective(glm::radians(45.f), float(width) / float(height), 0.1f, 100.f);


	// 배경 먼저...
	glUseProgram(texShader);
	{
		glDisable(GL_DEPTH_TEST);
		glBindVertexArray(BGVAO);


		{
			glm::mat4 translateMatrix = glm::translate(glm::mat4(1.f), glm::vec3(-40.0f, 15.f, -3.f));
			glm::mat4 rotMatrixY = glm::rotate(glm::mat4(1.f), glm::radians(90.f), glm::vec3(0.f, 1.f, 0.f));

			glm::mat4 sclaeMatrix = glm::scale(glm::mat4(1.f), glm::vec3(23.f));
			glm::mat4 matrix = translateMatrix * rotMatrixY * sclaeMatrix;


			GLint worldLoc = glGetUniformLocation(texShader, "modelTransform");
			if (worldLoc < 0)
				std::cout << "bgShader worldLoc 찾지 못함\n";
			else
				glUniformMatrix4fv(worldLoc, 1, GL_FALSE, glm::value_ptr(matrix));

			glBindTexture(GL_TEXTURE_2D, BGTexture2);


			glDrawArrays(GL_TRIANGLES, 0, 6);
		}

		{
			glm::mat4 translateMatrix = glm::translate(glm::mat4(1.f), glm::vec3(40.0f, 15.f, -3.f));
			glm::mat4 rotMatrixY = glm::rotate(glm::mat4(1.f), glm::radians(-90.f), glm::vec3(0.f, 1.f, 0.f));

			glm::mat4 sclaeMatrix = glm::scale(glm::mat4(1.f), glm::vec3(23.f));
			glm::mat4 matrix = translateMatrix * rotMatrixY * sclaeMatrix;


			GLint worldLoc = glGetUniformLocation(texShader, "modelTransform");
			if (worldLoc < 0)
				std::cout << "bgShader worldLoc 찾지 못함\n";
			else
				glUniformMatrix4fv(worldLoc, 1, GL_FALSE, glm::value_ptr(matrix));

			glBindTexture(GL_TEXTURE_2D, BGTexture2);


			glDrawArrays(GL_TRIANGLES, 0, 6);

		}

		{ // z뒤
			glm::mat4 translateMatrix = glm::translate(glm::mat4(1.f), glm::vec3(0.0f, 18.f, -45.f));
			glm::mat4 sclaeMatrix = glm::scale(glm::mat4(1.f), glm::vec3(20.f));
			glm::mat4 matrix = translateMatrix * sclaeMatrix;


			GLint worldLoc = glGetUniformLocation(texShader, "modelTransform");
			if (worldLoc < 0)
				std::cout << "worldLoc 찾지 못함\n";
			else
				glUniformMatrix4fv(worldLoc, 1, GL_FALSE, glm::value_ptr(matrix));

			glBindTexture(GL_TEXTURE_2D, BGTexture2);


			glDrawArrays(GL_TRIANGLES, 0, 6);
		}

		{ // z앞
			glDisable(GL_CULL_FACE);
			glm::mat4 translateMatrix = glm::translate(glm::mat4(1.f), glm::vec3(0.0f, 18.f, 50.f));
			glm::mat4 rotMatrixY = glm::rotate(glm::mat4(1.f), glm::radians(180.f), glm::vec3(0.f, 1.f, 0.f));
			glm::mat4 sclaeMatrix = glm::scale(glm::mat4(1.f), glm::vec3(30.f));
			glm::mat4 matrix = translateMatrix * rotMatrixY * sclaeMatrix;


			GLint worldLoc = glGetUniformLocation(bgShader, "modelTransform");
			if (worldLoc < 0)
				std::cout << "worldLoc 찾지 못함\n";
			else
				glUniformMatrix4fv(worldLoc, 1, GL_FALSE, glm::value_ptr(matrix));

			glBindTexture(GL_TEXTURE_2D, BGTexture2);


			glDrawArrays(GL_TRIANGLES, 0, 6);
			glEnable(GL_CULL_FACE);

		}

		glEnable(GL_DEPTH_TEST);
	}

	// 바닥 먼저....
	{
		//glDisable(GL_CULL_FACE);

		glUseProgram(texShader);
		glBindVertexArray(BGVAO);

		glm::mat4 translateMatrix = glm::translate(glm::mat4(1.f), glm::vec3(0.0f, 0.f, 0.f));
		glm::mat4 rotMatrixX = glm::rotate(glm::mat4(1.f), glm::radians(-90.f), glm::vec3(1.f, 0.f, 0.f));

		glm::mat4 sclaeMatrix = glm::scale(glm::mat4(1.f), glm::vec3(25.f));
		glm::mat4 matrix = translateMatrix * rotMatrixX * sclaeMatrix;

		GLint worldLoc = glGetUniformLocation(texShader, "modelTransform");
		if (worldLoc < 0)
			std::cout << "worldLoc 찾지 못함\n";
		else
			glUniformMatrix4fv(worldLoc, 1, GL_FALSE, glm::value_ptr(matrix));

		GLint viewLoc = glGetUniformLocation(texShader, "viewTransform");
		if (viewLoc < 0)
			std::cout << "viewLoc 찾지 못함\n";
		GLint projLoc = glGetUniformLocation(texShader, "projTransform");
		if (projLoc < 0)
			std::cout << "projLoc 찾지 못함\n";

		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(viewMatrix));
		glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projMatrix));

		if (isSnow) glBindTexture(GL_TEXTURE_2D, GroundTexture2);
		else glBindTexture(GL_TEXTURE_2D, GroundTexture);
		glDrawArrays(GL_TRIANGLES, 0, 6);

	}


	glUseProgram(animalShader);

	// 카메라, 투영은 씬 전체에 적용..
	GLint viewLoc = glGetUniformLocation(animalShader, "viewTransform");
	if (viewLoc < 0)
		std::cout << "viewLoc 찾지 못함\n";
	GLint projLoc = glGetUniformLocation(animalShader, "projTransform");
	if (projLoc < 0)
		std::cout << "projLoc 찾지 못함\n";
	GLint lightPos = glGetUniformLocation(animalShader, "lightPos");
	if (projLoc < 0)
		std::cout << "lightPos 찾지 못함\n";

	glUniform3f(lightPos, light.x, light.y, light.z);
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(viewMatrix));
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projMatrix));
	//	glUniform3f(cameraPosLoc, cameraPos.x, cameraPos.y, cameraPos.z);

	glUseProgram(shader);

	// 카메라, 투영은 씬 전체에 적용..
	viewLoc = glGetUniformLocation(shader, "viewTransform");
	if (viewLoc < 0)
		std::cout << "viewLoc 찾지 못함\n";
	projLoc = glGetUniformLocation(shader, "projTransform");
	if (projLoc < 0)
		std::cout << "projLoc 찾지 못함\n";
	GLuint cameraPosLoc = glGetUniformLocation(shader, "cameraPos");
	if (cameraPosLoc < 0)
		std::cout << "cameraPosLoc 찾지 못함\n";

	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(viewMatrix));
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projMatrix));
	glUniform3f(cameraPosLoc, cameraPos.x, cameraPos.y, cameraPos.z);


	// 오브젝트 그리기
	player->draw();		// 지금 플레이어 안그리긴 해도... 나중에 그릴 수 있으니 호출해준다

	/*for (int i = 0; i < objectCount; ++i)
		objects[i]->draw();	*/	// 부모 클래스에서 draw를 virtual.. 
		// 순수가상함수로 만들어줬기에 그의 자식클래스의 draw를 가져와그리는것이다..
		// virtual keyword 없으면.. 업캐스팅 하면 안된다ㅏ!!!



// --------------------------------- 펜스 
	glUseProgram(objShader);

	viewLoc = glGetUniformLocation(objShader, "viewTransform");
	if (viewLoc < 0)
		std::cout << "viewLoc 찾지 못함\n";
	projLoc = glGetUniformLocation(objShader, "projTransform");
	if (projLoc < 0)
		std::cout << "projLoc 찾지 못함\n";

	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(viewMatrix));
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projMatrix));

	GLint useGlobalColor = glGetUniformLocation(objShader, "useGlobalColor");

	glUseProgram(texShader);
	GLint useLightColor = glGetUniformLocation(texShader, "lightColor");
	if (useLightColor < 0) {
		std::cout << " lightColor 찾을 수 없음.." << std::endl;
	}
	glUniform3f(useLightColor, 1.f, 1.f, 1.f);

	glUseProgram(objShader);
	// 펜스 그리기
	for (int i = 0; i < 6; ++i) {

		glm::mat4 translateMatrix = glm::translate(glm::mat4(1.f), glm::vec3(-15.f + i * 3.f, 0.f, -8.f));
		glm::mat4 rotMatrixY = glm::rotate(glm::mat4(1.f), glm::radians(90.f), glm::vec3(0.f, 1.f, 0.f));

		glm::mat4 sclaeMatrix = glm::scale(glm::mat4(1.f), glm::vec3(1.f));
		glm::mat4 matrix = translateMatrix * rotMatrixY * sclaeMatrix;

		GLuint modelLoc = glGetUniformLocation(objShader, "modelTransform");
		if (modelLoc < 0)
			std::cout << " modelLoc 찾을수 없음!";
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(matrix));

		GLint color = glGetUniformLocation(objShader, "globalColor");
		glUniform1i(useGlobalColor, GL_TRUE);
		if (color < 0)
			std::cout << "globalColor 찾지 못함\n";
		else
			glUniform3f(color, 138 / 255.f, 73 / 255.f, 36 / 255.f);


		glBindVertexArray(fenceVAO);
		glDrawArrays(GL_TRIANGLES, 0, fenceVertexCount);

		////////////////////////////

		translateMatrix = glm::translate(glm::mat4(1.f), glm::vec3(-17.f, 0.f, -6.f + i * 3.f));
		rotMatrixY = glm::rotate(glm::mat4(1.f), glm::radians(0.f), glm::vec3(0.f, 1.f, 0.f));

		matrix = translateMatrix * rotMatrixY * sclaeMatrix;

		modelLoc = glGetUniformLocation(objShader, "modelTransform");
		if (modelLoc < 0)
			std::cout << " modelLoc 찾을수 없음!";
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(matrix));

		glBindVertexArray(fenceVAO);
		glDrawArrays(GL_TRIANGLES, 0, fenceVertexCount);

		////////////////////////////

		translateMatrix = glm::translate(glm::mat4(1.f), glm::vec3(1.f, 0.f, -6.f + i * 3.f));
		rotMatrixY = glm::rotate(glm::mat4(1.f), glm::radians(0.f), glm::vec3(0.f, 1.f, 0.f));

		matrix = translateMatrix * rotMatrixY * sclaeMatrix;

		modelLoc = glGetUniformLocation(objShader, "modelTransform");
		if (modelLoc < 0)
			std::cout << " modelLoc 찾을수 없음!";
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(matrix));

		glBindVertexArray(fenceVAO);
		glDrawArrays(GL_TRIANGLES, 0, fenceVertexCount);

	}

	for (int i = 0; i < 6; ++i) {

		glm::mat4 translateMatrix = glm::translate(glm::mat4(1.f), glm::vec3(-15.f + i * 3.f, 0.f, 10.f));
		glm::mat4 rotMatrixY = glm::rotate(glm::mat4(1.f), glm::radians(90.f), glm::vec3(0.f, 1.f, 0.f));

		glm::mat4 sclaeMatrix = glm::scale(glm::mat4(1.f), glm::vec3(1.f));
		glm::mat4 matrix = translateMatrix * rotMatrixY * sclaeMatrix;

		GLuint modelLoc = glGetUniformLocation(objShader, "modelTransform");
		if (modelLoc < 0)
			std::cout << " modelLoc 찾을수 없음!";
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(matrix));

		GLint color = glGetUniformLocation(objShader, "globalColor");
		glUniform1i(useGlobalColor, GL_TRUE);
		if (color < 0)
			std::cout << "globalColor 찾지 못함\n";
		else
			glUniform3f(color, 138 / 255.f, 73 / 255.f, 36 / 255.f);


		glBindVertexArray(fenceVAO);
		glDrawArrays(GL_TRIANGLES, 0, fenceVertexCount);

	}




	// --------------------------------- 

	// 농장 sign


	glUseProgram(texShader);

	// 조명 일시적으로 끌거임


	//glUniform1i(usingLight, GL_TRUE);

	glm::mat4 translateMatrix = glm::translate(glm::mat4(1.f), glm::vec3(0.f, 0.f, 11.5f));
	glm::mat4 rotMatrixY = glm::rotate(glm::mat4(1.f), glm::radians(20.f), glm::vec3(0.f, 1.f, 0.f));
	glm::mat4 sclaeMatrix = glm::scale(glm::mat4(1.f), glm::vec3(0.2f));

	glm::mat4 matrix = translateMatrix * rotMatrixY * sclaeMatrix;

	GLuint modelLoc = glGetUniformLocation(texShader, "modelTransform");
	if (modelLoc < 0)
		std::cout << " modelLoc 찾을수 없음!";
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(matrix));


	if (not isDay) {
		GLint useLightColor = glGetUniformLocation(texShader, "lightColor");
		if (useLightColor < 0) {
			std::cout << " lightColor 찾을 수 없음.." << std::endl;
		}
		glUniform3f(useLightColor, 0.f / 255.f, 40.f / 255.f, 145.f / 255.f);
	}
	else if (isDay) {
		GLint useLightColor = glGetUniformLocation(texShader, "lightColor");
		if (useLightColor < 0) {
			std::cout << " lightColor 찾을 수 없음.." << std::endl;
		}
		glUniform3f(useLightColor, 255.f / 255.f, 242.f / 255.f, 230.f / 255.f);
	}

	glBindVertexArray(sign_bottom_VAO);
	glBindTexture(GL_TEXTURE_2D, signTexture[0]);
	glDrawArrays(GL_TRIANGLES, 0, signBottomVertexCount);

	glBindVertexArray(sign_top_VAO);
	glBindTexture(GL_TEXTURE_2D, signTexture[0]);
	glDrawArrays(GL_TRIANGLES, 0, signTopVertexCount);


	translateMatrix = glm::translate(glm::mat4(1.f), glm::vec3(0.f, 1.65f, 11.6f));
	rotMatrixY = glm::rotate(glm::mat4(1.f), glm::radians(20.f), glm::vec3(0.f, 1.f, 0.f));
	sclaeMatrix = glm::scale(glm::mat4(1.f), glm::vec3(1.5f / 1.5f, 1.f / 1.5f, 0.001f));

	matrix = translateMatrix * rotMatrixY * sclaeMatrix;

	modelLoc = glGetUniformLocation(texShader, "modelTransform");
	if (modelLoc < 0)
		std::cout << " modelLoc 찾을수 없음!";
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(matrix));

	glBindVertexArray(cubeVAO);
	glBindTexture(GL_TEXTURE_2D, signTexture[1]);
	glDrawArrays(GL_TRIANGLES, 0, cubeVertexCount);

	GLuint usingLight = glGetUniformLocation(texShader, "useLight");
	glUniform1i(usingLight, GL_TRUE);


	// -------- 나무 윗부분
	// 그리기....
	glUseProgram(objShader);
	usingLight = glGetUniformLocation(objShader, "useLight");
	glUniform1i(usingLight, GL_TRUE);
	for (int i = 0; i < 10; ++i)
	{
		// Z뒷부분
		viewLoc = glGetUniformLocation(objShader, "viewTransform");
		if (viewLoc < 0)
			std::cout << "viewLoc 찾지 못함\n";
		projLoc = glGetUniformLocation(objShader, "projTransform");
		if (projLoc < 0)
			std::cout << "projLoc 찾지 못함\n";

		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(viewMatrix));
		glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projMatrix));

		glm::mat4 translateMatrix = glm::translate(glm::mat4(1.f), glm::vec3(-20.f + i * 4.f, 0.f, -10.f));
		glm::mat4 rotMatrixY = glm::rotate(glm::mat4(1.f), glm::radians(rotateTreeY[i]), glm::vec3(0.f, 1.f, 0.f));

		glm::mat4 sclaeMatrix = glm::scale(glm::mat4(1.f), glm::vec3(1.f));
		glm::mat4 matrix = translateMatrix * rotMatrixY * sclaeMatrix;

		GLuint modelLoc = glGetUniformLocation(objShader, "modelTransform");
		if (modelLoc < 0)
			std::cout << " modelLoc 찾을수 없음!";
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(matrix));

		GLint color = glGetUniformLocation(objShader, "globalColor");
		glUniform1i(useGlobalColor, GL_TRUE);
		if (color < 0)
			std::cout << "globalColor 찾지 못함\n";
		else
			glUniform3f(color, 98 / 255.f, 193 / 255.f, 91 / 255.f);

		glBindVertexArray(treeVAO);
		glDrawArrays(GL_TRIANGLES, 0, treeVertexCount);

		color = glGetUniformLocation(objShader, "globalColor");
		glUniform1i(useGlobalColor, GL_TRUE);
		if (color < 0)
			std::cout << "globalColor 찾지 못함\n";
		else
			glUniform3f(color, 138 / 255.f, 73 / 255.f, 36 / 255.f);

		glBindVertexArray(treeVAO2);
		glDrawArrays(GL_TRIANGLES, 0, treeVertexCount2);

		translateMatrix = glm::translate(glm::mat4(1.f), glm::vec3(-18.f + i * 4.f, 0.f, -15.f));
		rotMatrixY = glm::rotate(glm::mat4(1.f), glm::radians(rotateTreeY[i]), glm::vec3(0.f, 1.f, 0.f));

		sclaeMatrix = glm::scale(glm::mat4(1.f), glm::vec3(1.f));
		matrix = translateMatrix * rotMatrixY * sclaeMatrix;

		modelLoc = glGetUniformLocation(objShader, "modelTransform");
		if (modelLoc < 0)
			std::cout << " modelLoc 찾을수 없음!";
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(matrix));

		color = glGetUniformLocation(objShader, "globalColor");
		glUniform1i(useGlobalColor, GL_TRUE);
		if (color < 0)
			std::cout << "globalColor 찾지 못함\n";
		else
			glUniform3f(color, 98 / 255.f, 193 / 255.f, 91 / 255.f);

		glBindVertexArray(treeVAO);
		glDrawArrays(GL_TRIANGLES, 0, treeVertexCount);

		color = glGetUniformLocation(objShader, "globalColor");
		glUniform1i(useGlobalColor, GL_TRUE);
		if (color < 0)
			std::cout << "globalColor 찾지 못함\n";
		else
			glUniform3f(color, 138 / 255.f, 73 / 255.f, 36 / 255.f);

		glBindVertexArray(treeVAO2);
		glDrawArrays(GL_TRIANGLES, 0, treeVertexCount2);

		/////// 양옆

		translateMatrix = glm::translate(glm::mat4(1.f), glm::vec3(20.f, 0.f, -18.f + i * 4.f));
		rotMatrixY = glm::rotate(glm::mat4(1.f), glm::radians(rotateTreeY[i]), glm::vec3(0.f, 1.f, 0.f));

		sclaeMatrix = glm::scale(glm::mat4(1.f), glm::vec3(1.f));
		matrix = translateMatrix * rotMatrixY * sclaeMatrix;

		modelLoc = glGetUniformLocation(objShader, "modelTransform");
		if (modelLoc < 0)
			std::cout << " modelLoc 찾을수 없음!";
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(matrix));

		color = glGetUniformLocation(objShader, "globalColor");
		glUniform1i(useGlobalColor, GL_TRUE);
		if (color < 0)
			std::cout << "globalColor 찾지 못함\n";
		else
			glUniform3f(color, 98 / 255.f, 193 / 255.f, 91 / 255.f);

		glBindVertexArray(treeVAO);
		glDrawArrays(GL_TRIANGLES, 0, treeVertexCount);

		color = glGetUniformLocation(objShader, "globalColor");
		glUniform1i(useGlobalColor, GL_TRUE);
		if (color < 0)
			std::cout << "globalColor 찾지 못함\n";
		else
			glUniform3f(color, 138 / 255.f, 73 / 255.f, 36 / 255.f);

		glBindVertexArray(treeVAO2);
		glDrawArrays(GL_TRIANGLES, 0, treeVertexCount2);

		///////

		translateMatrix = glm::translate(glm::mat4(1.f), glm::vec3(-20.f, 0.f, -18.f + i * 4.f));
		rotMatrixY = glm::rotate(glm::mat4(1.f), glm::radians(rotateTreeY[i]), glm::vec3(0.f, 1.f, 0.f));

		sclaeMatrix = glm::scale(glm::mat4(1.f), glm::vec3(1.f));
		matrix = translateMatrix * rotMatrixY * sclaeMatrix;

		modelLoc = glGetUniformLocation(objShader, "modelTransform");
		if (modelLoc < 0)
			std::cout << " modelLoc 찾을수 없음!";
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(matrix));

		color = glGetUniformLocation(objShader, "globalColor");
		glUniform1i(useGlobalColor, GL_TRUE);
		if (color < 0)
			std::cout << "globalColor 찾지 못함\n";
		else
			glUniform3f(color, 98 / 255.f, 193 / 255.f, 91 / 255.f);

		glBindVertexArray(treeVAO);
		glDrawArrays(GL_TRIANGLES, 0, treeVertexCount);

		color = glGetUniformLocation(objShader, "globalColor");
		glUniform1i(useGlobalColor, GL_TRUE);
		if (color < 0)
			std::cout << "globalColor 찾지 못함\n";
		else
			glUniform3f(color, 138 / 255.f, 73 / 255.f, 36 / 255.f);

		glBindVertexArray(treeVAO2);
		glDrawArrays(GL_TRIANGLES, 0, treeVertexCount2);
	}

	// --------------------------------- 
	// 상점 그리기

	{
		glUseProgram(texShader);
		glBindVertexArray(house_bottom_VAO);
		viewLoc = glGetUniformLocation(texShader, "viewTransform");
		if (viewLoc < 0)
			std::cout << "viewLoc 찾지 못함\n";
		projLoc = glGetUniformLocation(texShader, "projTransform");
		if (projLoc < 0)
			std::cout << "projLoc 찾지 못함\n";

		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(viewMatrix));
		glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projMatrix));

		glm::mat4 translateMatrix = glm::translate(glm::mat4(1.f), glm::vec3(10.f, 0.f, 0.f));
		glm::mat4 rotMatrixY = glm::rotate(glm::mat4(1.f), glm::radians(90.f), glm::vec3(0.f, 1.f, 0.f));

		glm::mat4 sclaeMatrix = glm::scale(glm::mat4(1.f), glm::vec3(0.3f));
		glm::mat4 matrix = translateMatrix * rotMatrixY * sclaeMatrix;

		GLuint modelLoc = glGetUniformLocation(texShader, "modelTransform");
		if (modelLoc < 0)
			std::cout << " modelLoc 찾을수 없음!";
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(matrix));

		if (not isDay) {
			GLint useLightColor = glGetUniformLocation(texShader, "lightColor");
			if (useLightColor < 0) {
				std::cout << " lightColor 찾을 수 없음.." << std::endl;
			}
			glUniform3f(useLightColor, 0.f / 255.f, 40.f / 255.f, 145.f / 255.f);
		}
		else if (isDay) {
			GLint useLightColor = glGetUniformLocation(texShader, "lightColor");
			if (useLightColor < 0) {
				std::cout << " lightColor 찾을 수 없음.." << std::endl;
			}
			glUniform3f(useLightColor, 255.f / 255.f, 242.f / 255.f, 230.f / 255.f);
		}


		glBindTexture(GL_TEXTURE_2D, houseTexture[0]);
		glDrawArrays(GL_TRIANGLES, 0, houseBottomVertexCount);
	}

	{
		glUseProgram(objShader);
		glBindVertexArray(house_top_VAO);
		viewLoc = glGetUniformLocation(objShader, "viewTransform");
		if (viewLoc < 0)
			std::cout << "viewLoc 찾지 못함\n";
		projLoc = glGetUniformLocation(objShader, "projTransform");
		if (projLoc < 0)
			std::cout << "projLoc 찾지 못함\n";

		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(viewMatrix));
		glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projMatrix));

		glm::mat4 translateMatrix = glm::translate(glm::mat4(1.f), glm::vec3(10.f, 0.f, 0.f));
		glm::mat4 rotMatrixY = glm::rotate(glm::mat4(1.f), glm::radians(90.f), glm::vec3(0.f, 1.f, 0.f));

		glm::mat4 sclaeMatrix = glm::scale(glm::mat4(1.f), glm::vec3(0.3f));
		glm::mat4 matrix = translateMatrix * rotMatrixY * sclaeMatrix;

		GLuint modelLoc = glGetUniformLocation(objShader, "modelTransform");
		if (modelLoc < 0)
			std::cout << " modelLoc 찾을수 없음!";
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(matrix));

		GLint color = glGetUniformLocation(objShader, "globalColor");
		glUniform1i(useGlobalColor, GL_TRUE);
		if (color < 0)
			std::cout << "globalColor 찾지 못함\n";
		else
			glUniform3f(color, 255 / 255.f, 178 / 255.f, 217 / 255.f);


		glDrawArrays(GL_TRIANGLES, 0, houseTopVertexCount);
	}

	{
		glUseProgram(texShader);

		GLuint usingLight = glGetUniformLocation(texShader, "useLight");
		glUniform1i(usingLight, GL_FALSE);

		translateMatrix = glm::translate(glm::mat4(1.f), glm::vec3(10.f, 3.f, 1.8f));
		rotMatrixY = glm::rotate(glm::mat4(1.f), glm::radians(0.f), glm::vec3(0.f, 1.f, 0.f));
		sclaeMatrix = glm::scale(glm::mat4(1.f), glm::vec3(2.f, 1.f, 0.001f));

		matrix = translateMatrix * rotMatrixY * sclaeMatrix;

		modelLoc = glGetUniformLocation(texShader, "modelTransform");
		if (modelLoc < 0)
			std::cout << " modelLoc 찾을수 없음!";
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(matrix));

		glBindVertexArray(cubeVAO);
		glBindTexture(GL_TEXTURE_2D, houseTexture[1]);
		glDrawArrays(GL_TRIANGLES, 0, cubeVertexCount);

		usingLight = glGetUniformLocation(texShader, "useLight");
		glUniform1i(usingLight, GL_TRUE);
	}



	if (player->isStoreShow) {
		{

			glUseProgram(texShader);

			GLuint usingLight = glGetUniformLocation(texShader, "useLight");
			glUniform1i(usingLight, GL_FALSE);

			translateMatrix = glm::translate(glm::mat4(1.f), glm::vec3(10.f, 1.5f, 1.8f));
			rotMatrixY = glm::rotate(glm::mat4(1.f), glm::radians(0.f), glm::vec3(0.f, 1.f, 0.f));
			sclaeMatrix = glm::scale(glm::mat4(1.f), glm::vec3(3.f, 3.f / 2.f, 0.001f));

			matrix = translateMatrix * rotMatrixY * sclaeMatrix;

			modelLoc = glGetUniformLocation(texShader, "modelTransform");
			if (modelLoc < 0)
				std::cout << " modelLoc 찾을수 없음!";
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(matrix));

			glBindVertexArray(cubeVAO);
			glBindTexture(GL_TEXTURE_2D, storeTexture);
			glDrawArrays(GL_TRIANGLES, 0, cubeVertexCount);

			usingLight = glGetUniformLocation(texShader, "useLight");
			glUniform1i(usingLight, GL_TRUE);
		}
	}







	// 똥그릴거임..
	for (auto& ddong : ddongs) {
		if (ddong.isDraw)
		{
			glUseProgram(objShader);
			glBindVertexArray(ddongVAO);
			viewLoc = glGetUniformLocation(objShader, "viewTransform");
			if (viewLoc < 0)
				std::cout << "viewLoc 찾지 못함\n";
			projLoc = glGetUniformLocation(objShader, "projTransform");
			if (projLoc < 0)
				std::cout << "projLoc 찾지 못함\n";

			glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(viewMatrix));
			glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projMatrix));

			glm::mat4 translateMatrix = glm::translate(glm::mat4(1.f), glm::vec3(ddong.x, 0.f, ddong.z));
			glm::mat4 rotMatrixY = glm::rotate(glm::mat4(1.f), glm::radians(90.f), glm::vec3(0.f, 1.f, 0.f));

			glm::mat4 sclaeMatrix = glm::scale(glm::mat4(1.f), glm::vec3(0.3f));
			glm::mat4 matrix = translateMatrix * rotMatrixY * sclaeMatrix;

			GLuint modelLoc = glGetUniformLocation(objShader, "modelTransform");
			if (modelLoc < 0)
				std::cout << " modelLoc 찾을수 없음!";
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(matrix));

			GLint color = glGetUniformLocation(objShader, "globalColor");
			glUniform1i(useGlobalColor, GL_TRUE);
			if (color < 0)
				std::cout << "globalColor 찾지 못함\n";
			else
				glUniform3f(color, 93 / 255.f, 44 / 255.f, 11 / 255.f);


			glDrawArrays(GL_TRIANGLES, 0, ddongVertexCount);
		}
		if (ddong.isDraw and ddong.isNear) {
			{
				glDisable(GL_DEPTH_TEST);
				glUseProgram(bgShader);

				translateMatrix = glm::translate(glm::mat4(1.f), glm::vec3(0.f, 0.f, 0.f));
				rotMatrixY = glm::rotate(glm::mat4(1.f), glm::radians(0.f), glm::vec3(0.f, 1.f, 0.f));
				sclaeMatrix = glm::scale(glm::mat4(1.f), glm::vec3(1.5f / 2, 1.5f / 4, 0.001f));

				matrix = sclaeMatrix;

				modelLoc = glGetUniformLocation(bgShader, "modelTransform");
				if (modelLoc < 0)
					std::cout << " modelLoc 찾을수 없음!";
				glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(matrix));

				glBindVertexArray(cubeVAO);
				glBindTexture(GL_TEXTURE_2D, ddongLogTexture);
				glDrawArrays(GL_TRIANGLES, 0, cubeVertexCount);
				glEnable(GL_DEPTH_TEST);
			}
		}
	}


	// 코인..
	for (int i = 0; i < nowCoin; ++i)
	{
		if (isCoin[i]) {
			glUseProgram(bgShader);

			GLuint usingLight = glGetUniformLocation(bgShader, "useLight");
			glUniform1i(usingLight, GL_FALSE);

			translateMatrix = glm::translate(glm::mat4(1.f), glm::vec3(coin_x[i], 0.75f, 0.f));
			rotMatrixY = glm::rotate(glm::mat4(1.f), glm::radians(0.f), glm::vec3(0.f, 1.f, 0.f));
			sclaeMatrix = glm::scale(glm::mat4(1.f), glm::vec3(0.08f / 2, 0.1f / 2, 0.1f));

			matrix = translateMatrix * rotMatrixY * sclaeMatrix;

			modelLoc = glGetUniformLocation(bgShader, "modelTransform");
			if (modelLoc < 0)
				std::cout << " modelLoc 찾을수 없음!";
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(matrix));

			glBindVertexArray(coinVAO);
			glBindTexture(GL_TEXTURE_2D, coinTexture);
			glDrawArrays(GL_TRIANGLES, 0, coinVertexCount);

			usingLight = glGetUniformLocation(bgShader, "useLight");
			glUniform1i(usingLight, GL_TRUE);
		}
	}

	for (int i = 0; i < nowFeed; ++i)
	{
		if (isFood[i]) {
			glUseProgram(bgShader);

			GLuint usingLight = glGetUniformLocation(bgShader, "useLight");
			glUniform1i(usingLight, GL_FALSE);

			translateMatrix = glm::translate(glm::mat4(1.f), glm::vec3(feed_x[i], -0.85f, 0.f));
			rotMatrixY = glm::rotate(glm::mat4(1.f), glm::radians(0.f), glm::vec3(0.f, 1.f, 0.f));
			sclaeMatrix = glm::scale(glm::mat4(1.f), glm::vec3(0.08f / 2, 0.12f / 2, 0.1f));

			matrix = translateMatrix * rotMatrixY * sclaeMatrix;

			modelLoc = glGetUniformLocation(bgShader, "modelTransform");
			if (modelLoc < 0)
				std::cout << " modelLoc 찾을수 없음!";
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(matrix));

			glBindVertexArray(foodVAO);
			glBindTexture(GL_TEXTURE_2D, feedPackTexture);
			glDrawArrays(GL_TRIANGLES, 0, foodVertexCount);

			usingLight = glGetUniformLocation(bgShader, "useLight");
			glUniform1i(usingLight, GL_TRUE);
		}
	}


	for (int i = 0; i < pigCount; ++i) {
		pigs[i]->draw();
		if (pigs[i]->isNear) {
			if (not pigs[i]->isBaby) {

				// 다 컸을 때 파는 문구
				glDisable(GL_DEPTH_TEST);
				glUseProgram(bgShader);

				translateMatrix = glm::translate(glm::mat4(1.f), glm::vec3(0.f, 0.f, 0.f));
				rotMatrixY = glm::rotate(glm::mat4(1.f), glm::radians(0.f), glm::vec3(0.f, 1.f, 0.f));
				sclaeMatrix = glm::scale(glm::mat4(1.f), glm::vec3(1.5f, 1.5f / 2, 0.001f));

				matrix = sclaeMatrix;

				modelLoc = glGetUniformLocation(bgShader, "modelTransform");
				if (modelLoc < 0)
					std::cout << " modelLoc 찾을수 없음!";
				glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(matrix));

				glBindVertexArray(cubeVAO);
				glBindTexture(GL_TEXTURE_2D, growDoneTexture);
				glDrawArrays(GL_TRIANGLES, 0, cubeVertexCount);
				glEnable(GL_DEPTH_TEST);
			}
			else if (pigs[i]->isBaby) {

				// 아직 다 안커서 밥주기
				glDisable(GL_DEPTH_TEST);
				glUseProgram(bgShader);

				translateMatrix = glm::translate(glm::mat4(1.f), glm::vec3(0.f, 0.f, 0.f));
				rotMatrixY = glm::rotate(glm::mat4(1.f), glm::radians(0.f), glm::vec3(0.f, 1.f, 0.f));
				sclaeMatrix = glm::scale(glm::mat4(1.f), glm::vec3(1.5f / 2, 1.5f / 4, 0.001f));

				matrix = sclaeMatrix;

				modelLoc = glGetUniformLocation(bgShader, "modelTransform");
				if (modelLoc < 0)
					std::cout << " modelLoc 찾을수 없음!";
				glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(matrix));

				glBindVertexArray(cubeVAO);
				glBindTexture(GL_TEXTURE_2D, feedLogTexture);
				glDrawArrays(GL_TRIANGLES, 0, cubeVertexCount);
				glEnable(GL_DEPTH_TEST);
			}
		}
	}
	for (int i = 0; i < alpacaCount; ++i) {
		alpacas[i]->draw();
		if (alpacas[i]->isNear) {
			if (not alpacas[i]->isBaby) {

				// 다 컸을 때 파는 문구
				glDisable(GL_DEPTH_TEST);
				glUseProgram(bgShader);

				translateMatrix = glm::translate(glm::mat4(1.f), glm::vec3(0.f, 0.f, 0.f));
				rotMatrixY = glm::rotate(glm::mat4(1.f), glm::radians(0.f), glm::vec3(0.f, 1.f, 0.f));
				sclaeMatrix = glm::scale(glm::mat4(1.f), glm::vec3(1.5f, 1.5f / 2, 0.001f));

				matrix = sclaeMatrix;

				modelLoc = glGetUniformLocation(bgShader, "modelTransform");
				if (modelLoc < 0)
					std::cout << " modelLoc 찾을수 없음!";
				glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(matrix));

				glBindVertexArray(cubeVAO);
				glBindTexture(GL_TEXTURE_2D, growDoneTexture);
				glDrawArrays(GL_TRIANGLES, 0, cubeVertexCount);
				glEnable(GL_DEPTH_TEST);
			}
			else if (alpacas[i]->isBaby) {

				// 아직 다 안커서 밥주기
				glDisable(GL_DEPTH_TEST);
				glUseProgram(bgShader);

				translateMatrix = glm::translate(glm::mat4(1.f), glm::vec3(0.f, 0.f, 0.f));
				rotMatrixY = glm::rotate(glm::mat4(1.f), glm::radians(0.f), glm::vec3(0.f, 1.f, 0.f));
				sclaeMatrix = glm::scale(glm::mat4(1.f), glm::vec3(1.5f / 2, 1.5f / 4, 0.001f));

				matrix = sclaeMatrix;

				modelLoc = glGetUniformLocation(bgShader, "modelTransform");
				if (modelLoc < 0)
					std::cout << " modelLoc 찾을수 없음!";
				glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(matrix));

				glBindVertexArray(cubeVAO);
				glBindTexture(GL_TEXTURE_2D, feedLogTexture);
				glDrawArrays(GL_TRIANGLES, 0, cubeVertexCount);
				glEnable(GL_DEPTH_TEST);
			}
		}
	}


	for (int i = 0; i < penguinCount; ++i) {
		penguins[i]->draw();
		if (penguins[i]->isNear) {
			if (not penguins[i]->isBaby) {

				// 다 컸을 때 파는 문구
				glDisable(GL_DEPTH_TEST);
				glUseProgram(bgShader);

				translateMatrix = glm::translate(glm::mat4(1.f), glm::vec3(0.f, 0.f, 0.f));
				rotMatrixY = glm::rotate(glm::mat4(1.f), glm::radians(0.f), glm::vec3(0.f, 1.f, 0.f));
				sclaeMatrix = glm::scale(glm::mat4(1.f), glm::vec3(1.5f, 1.5f / 2, 0.001f));

				matrix = sclaeMatrix;

				modelLoc = glGetUniformLocation(bgShader, "modelTransform");
				if (modelLoc < 0)
					std::cout << " modelLoc 찾을수 없음!";
				glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(matrix));

				glBindVertexArray(cubeVAO);
				glBindTexture(GL_TEXTURE_2D, growDoneTexture);
				glDrawArrays(GL_TRIANGLES, 0, cubeVertexCount);
				glEnable(GL_DEPTH_TEST);
			}
			else if (penguins[i]->isBaby) {

				// 아직 다 안커서 밥주기
				glDisable(GL_DEPTH_TEST);
				glUseProgram(bgShader);

				translateMatrix = glm::translate(glm::mat4(1.f), glm::vec3(0.f, 0.f, 0.f));
				rotMatrixY = glm::rotate(glm::mat4(1.f), glm::radians(0.f), glm::vec3(0.f, 1.f, 0.f));
				sclaeMatrix = glm::scale(glm::mat4(1.f), glm::vec3(1.5f / 2, 1.5f / 4, 0.001f));

				matrix = sclaeMatrix;

				modelLoc = glGetUniformLocation(bgShader, "modelTransform");
				if (modelLoc < 0)
					std::cout << " modelLoc 찾을수 없음!";
				glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(matrix));

				glBindVertexArray(cubeVAO);
				glBindTexture(GL_TEXTURE_2D, feedLogTexture);
				glDrawArrays(GL_TRIANGLES, 0, cubeVertexCount);
				glEnable(GL_DEPTH_TEST);
			}
		}
	}

	for (int i = 0; i < chickenCount; ++i) {
		chics[i]->draw();
		if (chics[i]->isNear) {
			if (not chics[i]->isBaby) {

				// 다 컸을 때 파는 문구
				glDisable(GL_DEPTH_TEST);
				glUseProgram(bgShader);

				translateMatrix = glm::translate(glm::mat4(1.f), glm::vec3(0.f, 0.f, 0.f));
				rotMatrixY = glm::rotate(glm::mat4(1.f), glm::radians(0.f), glm::vec3(0.f, 1.f, 0.f));
				sclaeMatrix = glm::scale(glm::mat4(1.f), glm::vec3(1.5f, 1.5f / 2, 0.001f));

				matrix = sclaeMatrix;

				modelLoc = glGetUniformLocation(bgShader, "modelTransform");
				if (modelLoc < 0)
					std::cout << " modelLoc 찾을수 없음!";
				glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(matrix));

				glBindVertexArray(cubeVAO);
				glBindTexture(GL_TEXTURE_2D, growDoneTexture);
				glDrawArrays(GL_TRIANGLES, 0, cubeVertexCount);
				glEnable(GL_DEPTH_TEST);
			}
			else if (chics[i]->isBaby) {

				// 아직 다 안커서 밥주기
				glDisable(GL_DEPTH_TEST);
				glUseProgram(bgShader);

				translateMatrix = glm::translate(glm::mat4(1.f), glm::vec3(0.f, 0.f, 0.f));
				rotMatrixY = glm::rotate(glm::mat4(1.f), glm::radians(0.f), glm::vec3(0.f, 1.f, 0.f));
				sclaeMatrix = glm::scale(glm::mat4(1.f), glm::vec3(1.5f / 2, 1.5f / 4, 0.001f));

				matrix = sclaeMatrix;

				modelLoc = glGetUniformLocation(bgShader, "modelTransform");
				if (modelLoc < 0)
					std::cout << " modelLoc 찾을수 없음!";
				glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(matrix));

				glBindVertexArray(cubeVAO);
				glBindTexture(GL_TEXTURE_2D, feedLogTexture);
				glDrawArrays(GL_TRIANGLES, 0, cubeVertexCount);
				glEnable(GL_DEPTH_TEST);
			}
		}
	}

	for (int i = 0; i < foxCount; ++i) {
		foxes[i]->draw();
		if (foxes[i]->isNear) {
			if (not foxes[i]->isBaby) {

				// 다 컸을 때 파는 문구
				glDisable(GL_DEPTH_TEST);
				glUseProgram(bgShader);

				translateMatrix = glm::translate(glm::mat4(1.f), glm::vec3(0.f, 0.f, 0.f));
				rotMatrixY = glm::rotate(glm::mat4(1.f), glm::radians(0.f), glm::vec3(0.f, 1.f, 0.f));
				sclaeMatrix = glm::scale(glm::mat4(1.f), glm::vec3(1.5f, 1.5f / 2, 0.001f));

				matrix = sclaeMatrix;

				modelLoc = glGetUniformLocation(bgShader, "modelTransform");
				if (modelLoc < 0)
					std::cout << " modelLoc 찾을수 없음!";
				glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(matrix));

				glBindVertexArray(cubeVAO);
				glBindTexture(GL_TEXTURE_2D, growDoneTexture);
				glDrawArrays(GL_TRIANGLES, 0, cubeVertexCount);
				glEnable(GL_DEPTH_TEST);
			}
			else if (foxes[i]->isBaby) {

				// 아직 다 안커서 밥주기
				glDisable(GL_DEPTH_TEST);
				glUseProgram(bgShader);

				translateMatrix = glm::translate(glm::mat4(1.f), glm::vec3(0.f, 0.f, 0.f));
				rotMatrixY = glm::rotate(glm::mat4(1.f), glm::radians(0.f), glm::vec3(0.f, 1.f, 0.f));
				sclaeMatrix = glm::scale(glm::mat4(1.f), glm::vec3(1.5f / 2, 1.5f / 4, 0.001f));

				matrix = sclaeMatrix;

				modelLoc = glGetUniformLocation(bgShader, "modelTransform");
				if (modelLoc < 0)
					std::cout << " modelLoc 찾을수 없음!";
				glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(matrix));

				glBindVertexArray(cubeVAO);
				glBindTexture(GL_TEXTURE_2D, feedLogTexture);
				glDrawArrays(GL_TRIANGLES, 0, cubeVertexCount);
				glEnable(GL_DEPTH_TEST);
			}
		}
	}

	glUseProgram(objShader);
	glBindVertexArray(sphereVAO);
	if (isSnow)
	{
		for (auto& snow : snows) {
			if (snow.isDraw) {
				//std::cout << "눈 그려지니..?" << std::endl;
			// 모델변환행렬
				GLint modelLoc = glGetUniformLocation(objShader, "modelTransform");
				if (modelLoc < 0)
					std::cout << "modelLoc 찾지 못함\n";
				glm::mat4 transTransform2 = glm::translate(glm::mat4(1.f), glm::vec3(snow.x, snow.y, snow.z));
				glm::mat4 scaleMat = glm::scale(glm::mat4(1.f), glm::vec3(snow.size));

				glm::mat4 finalMat;
				finalMat = transTransform2 * scaleMat;

				glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(finalMat));


				GLint color = glGetUniformLocation(objShader, "globalColor");
				glUniform1i(useGlobalColor, GL_TRUE);
				if (color < 0)
					std::cout << "globalColor 찾지 못함\n";
				else
					glUniform3f(color, 1.f, 1.f, 1.f);


				glDrawArrays(GL_TRIANGLES, 0, sphereVertexCount);
			}
		}
	}

	if (isDay) {
		glUseProgram(texShader);
		GLint useLightColor = glGetUniformLocation(texShader, "lightColor");
		if (useLightColor < 0) {
			std::cout << " lightColor 찾을 수 없음.." << std::endl;
		}
		glUniform3f(useLightColor, 255.f / 255.f, 242.f / 255.f, 230.f / 255.f);

		glUseProgram(objShader);
		useLightColor = glGetUniformLocation(objShader, "lightColor");
		if (useLightColor < 0) {
			std::cout << " lightColor 찾을 수 없음.." << std::endl;
		}
		glUniform3f(useLightColor, 255.f / 255.f, 242.f / 255.f, 230.f / 255.f);

		glUseProgram(animalShader);
		useLightColor = glGetUniformLocation(animalShader, "lightColor");
		if (useLightColor < 0) {
			std::cout << " lightColor 찾을 수 없음.." << std::endl;
		}
		glUniform3f(useLightColor, 255.f / 255.f, 242.f / 255.f, 230.f / 255.f);
	}
	else if (not isDay) {
		glUseProgram(texShader);
		GLint useLightColor = glGetUniformLocation(texShader, "lightColor");
		if (useLightColor < 0) {
			std::cout << " lightColor 찾을 수 없음.." << std::endl;
		}
		glUniform3f(useLightColor, 0.f / 255.f, 40.f / 255.f, 145.f / 255.f);

		glUseProgram(objShader);
		useLightColor = glGetUniformLocation(objShader, "lightColor");
		if (useLightColor < 0) {
			std::cout << " lightColor 찾을 수 없음.." << std::endl;
		}
		glUniform3f(useLightColor, 0.f / 255.f, 40.f / 255.f, 145.f / 255.f);

		glUseProgram(animalShader);
		useLightColor = glGetUniformLocation(animalShader, "lightColor");
		if (useLightColor < 0) {
			std::cout << " lightColor 찾을 수 없음.." << std::endl;
		}
		glUniform3f(useLightColor, 0.f / 255.f, 40.f / 255.f, 145.f / 255.f);
	}

}

void Scene::keyboard(unsigned char key, bool isPressed)
{
	player->keyboard(key, isPressed);

	if (isPressed) {			// 눌러졌을 때
		switch (key) {

		case '0':
			if (player->isStoreShow) {
				if (nowCoin > 1) {
					pigs[pigCount] = new Pig;
					pigs[pigCount]->setShader(animalShader);
					pigs[pigCount]->setVAO(animalVAO, animalVertexCount);

					++pigCount;
					std::cout << "돼지 구매 " << std::endl;
					--nowCoin;
					isCoin[nowCoin] = false;
					--nowCoin;
					isCoin[nowCoin] = false;
				}
			}

			break;
		case '1':
			if (player->isStoreShow) {
				if (nowCoin > 1) {
					chics[chickenCount] = new Chic;
					chics[chickenCount]->setShader(animalShader);
					chics[chickenCount]->setVAO(animalVAO, animalVertexCount);

					++chickenCount;
					std::cout << "병아리 구매 " << std::endl;
					--nowCoin;
					isCoin[nowCoin] = false;
					--nowCoin;
					isCoin[nowCoin] = false;
				}
			}
			break;
		case '2':
			if (player->isStoreShow) {
				if (nowCoin > 2) {
					alpacas[alpacaCount] = new Alpaca;
					alpacas[alpacaCount]->setShader(animalShader);
					alpacas[alpacaCount]->setVAO(animalVAO, animalVertexCount);

					++alpacaCount;
					std::cout << "알파카 구매 " << std::endl;
					--nowCoin;
					isCoin[nowCoin] = false;
					--nowCoin;
					isCoin[nowCoin] = false;
					--nowCoin;
					isCoin[nowCoin] = false;
				}

			}
			break;
		case '3':
			if (player->isStoreShow) {
				if (nowCoin > 1) {
					penguins[penguinCount] = new Penguin;
					penguins[penguinCount]->setShader(animalShader);
					penguins[penguinCount]->setVAO(animalVAO, animalVertexCount);

					++penguinCount;
					std::cout << "펭귄 구매 " << std::endl;

					--nowCoin;
					isCoin[nowCoin] = false;
					--nowCoin;
					isCoin[nowCoin] = false;
				}
			}
			break;
		case '4':
			if (player->isStoreShow) {
				if (nowCoin > 1) {
					foxes[foxCount] = new Fox;
					foxes[foxCount]->setShader(animalShader);
					foxes[foxCount]->setVAO(animalVAO, animalVertexCount);

					++foxCount;
					std::cout << "여우 구매 " << std::endl;

					--nowCoin;
					isCoin[nowCoin] = false;
					--nowCoin;
					isCoin[nowCoin] = false;
				}
			}
			break;

		case 32:
			if (not isTitleAniEnd) {
				isTitleAni = true;
			}

			for (auto& ddong : ddongs) {
				if (ddong.isNear and ddong.isDraw) {
					ddong.isDraw = false;
					if (nowFeed < 20) {
						isFood[nowFeed] = true;
						++nowFeed;
					}
				}
			}
			for (int i = 0; i < pigCount; ++i) {
				if (pigs[i]->isNear) {
					if (pigs[i]->isBaby) {
						if (nowFeed > 0) {
							--nowFeed;
							isFood[nowFeed] = false;

							++pigs[i]->feedNum;
							std::cout << "돼지 밥준 횟수 : " << pigs[i]->feedNum << std::endl;
						}

					}
					else if (not pigs[i]->isBaby) {
						std::cout << " 돼지 팔음" << std::endl;
						isCoin[nowCoin] = true;
						++nowCoin;
						delete pigs[i];
						pigCount--;
					}

				}
			}
			for (int i = 0; i < alpacaCount; ++i) {
				if (alpacas[i]->isNear) {
					if (alpacas[i]->isBaby) {
						if (nowFeed > 0) {
							--nowFeed;
							isFood[nowFeed] = false;

							++alpacas[i]->feedNum;
							std::cout << "알파카 밥준 횟수 : " << alpacas[i]->feedNum << std::endl;
						}
					}
					else if (not alpacas[i]->isBaby) {
						std::cout << " 알파카 팔음" << std::endl;
						isCoin[nowCoin] = true;
						++nowCoin;
						isCoin[nowCoin] = true;
						++nowCoin;
						delete alpacas[i];
						alpacaCount--;
					}
				}
			}
			for (int i = 0; i < penguinCount; ++i) {
				if (penguins[i]->isNear) {
					if (penguins[i]->isBaby) {
						if (nowFeed > 0) {
							--nowFeed;
							isFood[nowFeed] = false;

							++penguins[i]->feedNum;
							std::cout << "펭귄  밥준 횟수 : " << penguins[i]->feedNum << std::endl;
						}
					}
					else if (not penguins[i]->isBaby) {
						std::cout << " 펭귄 팔음" << std::endl;
						isCoin[nowCoin] = true;
						++nowCoin;
						delete penguins[i];
						penguinCount--;
					}

				}
			}
			for (int i = 0; i < chickenCount; ++i) {
				if (chics[i]->isNear) {
					if (chics[i]->isBaby) {
						if (nowFeed > 0) {
							--nowFeed;
							isFood[nowFeed] = false;

							++chics[i]->feedNum;
							std::cout << "치킨  밥준 횟수 : " << chics[i]->feedNum << std::endl;
						}
					}
					else if (not chics[i]->isBaby) {
						std::cout << " 닭 팔음" << std::endl;
						isCoin[nowCoin] = true;
						++nowCoin;
						delete chics[i];
						chickenCount--;
					}
				}
			}
			for (int i = 0; i < foxCount; ++i) {
				if (foxes[i]->isNear) {
					if (foxes[i]->isBaby) {
						if (nowFeed > 0) {
							--nowFeed;
							isFood[nowFeed] = false;
							++foxes[i]->feedNum;
							std::cout << "여우 밥준 횟수 : " << foxes[i]->feedNum << std::endl;
						}
					}
					else if (not foxes[i]->isBaby) {
						std::cout << " 여우 팔음" << std::endl;
						isCoin[nowCoin] = true;
						++nowCoin;
						delete foxes[i];
						foxCount--;
					}
				}
			}
			break;

		default:
			break;
		}
	}
	else {						// 떼졌을 때
		switch (key) {
		default:
			break;
		}
	}
}

void Scene::specialKeyboard(int key, bool isPressed)
{

}

void Scene::mouse(int button, int state, int x, int y)
{
	player->mouse(button, state, x, y);

	// 화면 업데이트가 된다....

	constexpr int WHEEL_UP = 3;
	constexpr int WHEEL_DOWN = 4;

	switch (state) {
	case GLUT_DOWN:
		switch (button) {
		case GLUT_LEFT_BUTTON: {
			float xPos = (static_cast<float>(x) / width * 2.f - 1.f);
			float yPos = -(static_cast<float>(y) / height * 2.f - 1.f);

			std::cout << "좌클릭 : " << x << ", " << y << std::endl;
			std::cout << "OpenGL x 좌표는 " << xPos << std::endl;
			std::cout << "OpenGL y 좌표는 " << yPos << std::endl;


			break;
		}
		case GLUT_MIDDLE_BUTTON:
			std::cout << "휠클릭 : " << x << ", " << y << std::endl;
			break;
		case GLUT_RIGHT_BUTTON:
			std::cout << "우클릭 : " << x << ", " << y << std::endl;
			break;
		case WHEEL_UP:
			std::cout << "휠  업 : " << x << ", " << y << std::endl;
			break;
		case WHEEL_DOWN:
			std::cout << "휠다운 : " << x << ", " << y << std::endl;
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

void Scene::mouseMove(int x, int y)
{
	player->mouseMove(x, y);
}

void Scene::setWindowSize(int winWidth, int winHeight)
{
	width = winWidth;
	height = winHeight;
}

void Scene::initBuffer(GLuint* VAO, GLsizei* vertexCount, std::string objFilename)
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

void Scene::initCubeBuffer(GLuint* VAO, GLsizei* vertexCount)
{
	glGenVertexArrays(1, VAO);		// 동적 할당....
	glBindVertexArray(*VAO);

	GLuint VBO;
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);


	*vertexCount = 36;

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
}


void Scene::initBackGroundBuffer() {
	glGenVertexArrays(1, &BGVAO);		// 동적 할당....
	glBindVertexArray(BGVAO);

	GLuint VBO3;
	glGenBuffers(1, &VBO3);
	glBindBuffer(GL_ARRAY_BUFFER, VBO3);


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
}


void Scene::initTexture(GLuint* texture, int size, std::string* texFilename)
{
	glGenTextures(size, texture);

	// 실제 이미지 데이터(binary) --> 우리가 아는 실제 이미지를 바이트 단위로 읽어서 저장해야 한다..
	stbi_set_flip_vertically_on_load(true);

	for (int i = 0; i < size; ++i) {
		glBindTexture(GL_TEXTURE_2D, texture[i]);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		int width, height, level;
		unsigned char* data = stbi_load(texFilename[i].c_str(), &width, &height, &level, 0);		// 동적 할당...

		if (3 == level)
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);	// GPU로 데이터 복사
		else if (4 == level)
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		else {
			std::cerr << texFilename[i] << " load Error! 이미지 파일 오류..\n";
			exit(1);
		}

		stbi_image_free(data);
	}
}




GLuint Scene::makeShader(std::string vertexFilename, std::string fragmentFilename)
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

std::string Scene::readFile(std::string filename)
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


std::vector<glm::vec3> Scene::readOBJ(std::string filename)
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



std::vector<glm::vec3> Scene::readOBJWithUV(std::string filename)
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


void Scene::initBufferWithUV(GLuint* VAO, GLsizei* vertexCount, std::string objFilename)
{
	glGenVertexArrays(1, VAO);		// 동적 할당....
	glBindVertexArray(*VAO);

	GLuint VBO;
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	std::vector<glm::vec3> verticies = readOBJWithUV(objFilename);

	*vertexCount = verticies.size() / 3;

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

