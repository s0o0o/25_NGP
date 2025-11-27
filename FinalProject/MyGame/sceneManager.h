#pragma once

#include "library.h"
#include "ResourceManager.h"
class MyScene;

#include <map>
#include <string>
#include <chrono>

class sceneManager {
public:
    sceneManager(int width, int height);
	~sceneManager();

    ResourceManager* m_resoureManager;    // 리소스 매니저

	void initialize();	// 여기서 sceneManager 클래스 자체 초기화..

	// 콜백들이 호출할 공용 함수들..
    void update(float deltaTime);
    void draw();
    void reshape(int w, int h);

    // 입력 관련
    void keyboard(unsigned char key, bool pressed);
    void specialKeyboard(int key, bool pressed);
    void mouse(int button, int state, int x, int y);
    void mouseMove(int x, int y);

    // 씬 바꿔주는애
    void changeScene(const std::string& sceneName);
    MyScene* getCurrentScene();

private:
    std::map<std::string, MyScene*> m_scenes; // 모든 씬을 소유..
    MyScene* m_currentScene;                  // 현재 활성화된 씬

    int m_width;
    int m_height;

    int fps;
    // 지난 시각
    std::chrono::high_resolution_clock::time_point last_time;   // 지난 시간
    std::chrono::high_resolution_clock::duration accm_time;	// 누적 시간

    std::string m_windowTitle;

};