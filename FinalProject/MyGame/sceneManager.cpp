#pragma once

#include "sceneManager.h"
#include "startScene.h"
#include "gameScene.h"
#include "ResourceManager.h"

#include <iostream>
#include <chrono>

sceneManager::sceneManager(int width, int height)
    : m_width(width),
    m_height(height),
    m_currentScene(nullptr), // 처음엔 현재 씬이 없음
    fps(0),
    accm_time(std::chrono::seconds(0)),
    last_time(std::chrono::high_resolution_clock::now()),
    m_windowTitle("25_NGP_REPACK") // (main.cpp에서 가져온 기본 제목)
{
    std::cout << "=============== SceneManager Created." << std::endl;
    m_resoureManager = new ResourceManager();   // 리소스 매니저 생성..
}

sceneManager::~sceneManager()
{
    delete m_resoureManager;
 
    // 씬이 갖고있던 메모리 해제
    for (auto& pair : m_scenes) {
        delete pair.second;
    }
    m_scenes.clear();
}

void sceneManager::initialize()
{
    // 리소스 먼저 등록..
    m_resoureManager->loadAllResource();

    // 씬 객체 생성
    MyScene* start = new startScene(m_width, m_height);
    MyScene* mainGame = new gameScene(m_width, m_height);

    start->setSceneManager(this);
    start->setResourceManager(m_resoureManager);
    mainGame->setSceneManager(this);
    mainGame->setResourceManager(m_resoureManager);

    m_scenes["start"] = start;
    m_scenes["mainGame"] = mainGame;

    changeScene("start");   // 처음 화면은 시작화면
}



void sceneManager::draw()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    if (m_currentScene)
        m_currentScene->draw();
    // 후면버퍼를 전면 버퍼와 바꿔준다!
    glutSwapBuffers();
}

void sceneManager::reshape(int w, int h)
{
    m_width = w;
    m_height = h;

    glViewport(0, 0, m_width, m_height);

    if (m_currentScene)
        m_currentScene->setWindowSize(m_width, m_height);

    glutPostRedisplay();
}

void sceneManager::update(float deltaTime)
{
    if (m_currentScene)
        m_currentScene->update(deltaTime);

    // fps계싼
    fps++;
    accm_time += std::chrono::nanoseconds(static_cast<long long>(deltaTime * 1E09));
    if (accm_time >= std::chrono::seconds(1)) {
        std::string str = m_windowTitle + " | (FPS : " + std::to_string(fps) + ")";
        glutSetWindowTitle(str.c_str());

        fps = 0;
        accm_time = std::chrono::seconds(0);
    }
}

void sceneManager::keyboard(unsigned char key, bool isPressed)
{
    if (m_currentScene) {
        m_currentScene->keyboard(key, isPressed);
    }
}

void sceneManager::specialKeyboard(int key, bool isPressed)
{
    if (m_currentScene) {
        m_currentScene->specialKeyboard(key, isPressed);
    }
}

void sceneManager::mouse(int button, int state, int x, int y)
{
    if (m_currentScene) {
        m_currentScene->mouse(button, state, x, y);
    }
}

void sceneManager::mouseMove(int x, int y)
{
    if (m_currentScene) {
        m_currentScene->mouseMove(x, y);
    }
}

void sceneManager::changeScene(const std::string& sceneName)
{
    if (m_currentScene != nullptr)
    {
        m_currentScene->sceneOnExit();
    }

    auto it = m_scenes.find(sceneName); // scene있는지 찾고,
    if (it != m_scenes.end())
    {
        m_currentScene = it->second;
        m_currentScene->sceneOnEnter(); // 해당 씬으로 입장~
    }
}