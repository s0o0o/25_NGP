#pragma once


// �ش� ������ ���ٰ� �߸鼭 �������� �ȵȴٸ�
// ������Ʈ - (�̸�)�Ӽ� - ��Ŀ - �Ϲ� - �߰� ���̺귯�� ���͸�
// .\Dependencies �Է� (. �ʼ�)
#ifdef NDEBUG	// Release	�϶�
#	pragma comment (lib, "libglew32.lib")
#else			// Debug	�϶�
#   pragma comment (lib, "libglew32d.lib")
#endif


// �Ʒ� ����� �������� ��ٸ�
// ������Ʈ - (�̸�)�Ӽ� - C/C++ - �Ϲ� - �߰� ���� ���͸�
// .\Dependencies (. �ʼ�)
#define GLEW_STATIC
#include "glew.h"

#define FREEGLUT_STATIC
#include "freeglut.h"

#include "glm/glm.hpp"
#include "glm/ext.hpp"
