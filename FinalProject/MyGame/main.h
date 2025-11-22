#pragma once
#include <iostream>
#include <chrono>

#include "sceneManager.h"
#include "library.h"

// 전역 변수
constexpr int winWidth = 1600/2, winHeight = 900/2;

const std::string windowTitle = "25_NGP_AnimalFarm";

extern sceneManager* g_Manager;