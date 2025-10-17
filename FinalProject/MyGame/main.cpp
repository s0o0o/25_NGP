#include <iostream>
#include <chrono>

#include "Scene.h"
#include "startScene.h"
#include "PlayerObject.h"


// ���� �ݹ� �Լ� �̸� ����
void DisplayFunc(void);
void ReshapeFunc(int width, int height);
void KeyboardFunc(unsigned char, int, int);
void KeyboardUpFunc(unsigned char, int, int);
void SpecialFunc(int, int, int);
void SpecialUpFunc(int, int, int);
void MouseFunc(int, int, int, int);
void MotionFunc(int, int);
//void TimerFunc(int);
void IdleFunc();

// ���� ����
constexpr int winWidth = 1600, winHeight = 900;

const std::string windowTitle = "MyGame";

// Scene Ŭ����... ���߿��� ���ӿ� ī�޶�...�÷��̾�... -> GameFramework
Scene g_scene{ winWidth, winHeight };		// initialize_list --> ���� �� �� ������...
startScene s_scene{ winWidth, winHeight };



int main(int argc, char** argv)
{
	//--- ������ �����ϱ� (freeglut)
	glutInit(&argc, argv);							// glut �ʱ�ȭ
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);	// ���÷��� ��� ����
	glutInitWindowPosition(100, 100);				// �������� ��ġ ����
	glutInitWindowSize(winWidth, winHeight);			// �������� ũ�� ����
	glutCreateWindow(windowTitle.c_str());				// ������ ���� (������ �̸�)

	//--- GLEW �ʱ�ȭ�ϱ�
	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK) {					// glew �ʱ�ȭ
		std::cerr << "Unable to initialize GLEW\n";
		exit(EXIT_FAILURE);
	}
	else {
		std::cout << "GLEW Initialized\n";
	}

	glClearColor(196/255.f, 222/255.f, 255/255.f , 1.f);		// Ŭ���� ���� (�ƹ��͵� ������ ��)

	// �ʱ� ������
	s_scene.initialize();
	g_scene.initialize();
	//store_scene.initialize();
	

	
	glEnable(GL_CULL_FACE);			// �ø�(�޸� ����) Ȱ��ȭ
	//glCullFace(GL_FRONT);			// �ո� ������� �޸� �������
	//glFrontFace(GL_CCW);			// ��� �ո�����?? GL_CW -> �ð�, GL_CCW -> �ݽð�(�⺻��)

	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	// ���� �˻� Ȱ��ȭ --> z���ۿ��ٰ� ����� �ϱ� �����ϰ�, �׸��� ���ƾ� �� ���� �׸��� �ʴ´�....
	glEnable(GL_DEPTH_TEST);

	// �ݹ� �Լ���
	glutKeyboardFunc(KeyboardFunc);		// �Ϲ� Ű���� (abcde..)
	glutKeyboardUpFunc(KeyboardUpFunc);	// �Ϲ� Ű���尡 ������ ��
	glutSpecialFunc(SpecialFunc);		// Ư�� Ű���� (F1 ~ F12, HOME, CTRL, ALT ���..)
	glutSpecialUpFunc(SpecialUpFunc);	// Ư�� Ű���尡 ������ ��

	glutMouseFunc(MouseFunc);			// ���콺 �Է� (��Ŭ��, ��Ŭ��, ��Ŭ��... 
	glutMotionFunc(MotionFunc);			// ȭ�� ������ �巡��

	glutDisplayFunc(DisplayFunc);		// ��� �Լ��� ����
	glutReshapeFunc(ReshapeFunc);		// ȭ�� ũ�Ⱑ ����Ǿ��� ��....
	//glutTimerFunc(16, TimerFunc, 0);		// 16ms --> �ʴ� 60��
	glutIdleFunc(IdleFunc);

	glutMainLoop(); // �̺�Ʈ ó�� ����
}

void DisplayFunc(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	// �ĸ���� �����

	// ���� �׸���
	
	if(s_scene.isShow)
		g_scene.draw();
	
	else {
		s_scene.draw(); // �ʱ�ȭ��... �׸�����..!
	}

	// �ĸ���۸� ���� ���ۿ� �ٲ��ش�!
	glutSwapBuffers();
}

void ReshapeFunc(int width, int height)
{
	s_scene.setWindowSize(width, height);
	g_scene.setWindowSize(width, height);
	
	glutPostRedisplay();

	glViewport(0, 0, width, height);
}

void KeyboardFunc(unsigned char key, int x, int y)
{
	if (27 == key)		// ESC Key
		glutLeaveMainLoop();
	s_scene.keyboard(key, true);
	g_scene.keyboard(key, true);
	
}

void KeyboardUpFunc(unsigned char key, int, int)
{
	s_scene.keyboard(key, false);
	g_scene.keyboard(key, false);
}

void SpecialFunc(int key, int x, int y)
{
	s_scene.specialKeyboard(key, true);
	g_scene.specialKeyboard(key, true);
}

void SpecialUpFunc(int key, int, int)
{
	s_scene.specialKeyboard(key, false);
	g_scene.specialKeyboard(key, false);
}

void MouseFunc(int button, int state, int x, int y)
{
	s_scene.mouse(button, state, x, y);
	g_scene.mouse(button, state, x, y);
	
}

void MotionFunc(int x, int y)
{
	s_scene.mouseMove(x, y);
	g_scene.mouseMove(x, y);
	
}

//void TimerFunc(int id)
//{
//	switch (id) {
//	case 0:			// ������Ʈ
//		g_scene.update();
//		glutPostRedisplay();	// ���� ������Ʈ ������, ȭ�鿡 �׷���� �Ѵ�..
//
//		glutTimerFunc(16, TimerFunc, 0);
//		break;
//	}
//}

void IdleFunc()	// ���� ����ȭ�� �ǰ��ִ�.. --> ��ǻ�͸��� �ٸ��� ������ �� ����
{
	// static�� ���� ����ó�� Ȱ��
	static int fps = 0;
	// ���� �ð�
	static std::chrono::high_resolution_clock::time_point last_time = std::chrono::high_resolution_clock::now();
	static std::chrono::high_resolution_clock::duration accm_time;	// ���� �ð�

	// ms->10-3, us->10-6, ns->10-9

	++fps;
	auto now_time = std::chrono::high_resolution_clock::now();
	auto dur_time = now_time - last_time;
	accm_time += dur_time;
	last_time = now_time;

	if (accm_time >= std::chrono::seconds(1)) {
		std::string str = windowTitle + " | (FPS : " + std::to_string(fps) + ")";
		glutSetWindowTitle(str.c_str());		// â�� FPS ǥ��

		fps = 0;
		accm_time = std::chrono::seconds(0);
	}

	if(s_scene.isShow)
		g_scene.update(dur_time.count() / static_cast<float>(1E09));
	else
		s_scene.update(dur_time.count() / static_cast<float>(1E09));

	glutPostRedisplay();	// ���� ������Ʈ ������, ȭ�鿡 �׷���� �Ѵ�..
}
