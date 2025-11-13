#include "main.h"

// 각종 콜백 함수 미리 선언
void DisplayFunc(void);
void ReshapeFunc(int width, int height);
void KeyboardFunc(unsigned char, int, int);
void KeyboardUpFunc(unsigned char, int, int);
void SpecialFunc(int, int, int);
void SpecialUpFunc(int, int, int);
void MouseFunc(int, int, int, int);
void MotionFunc(int, int);
void TimerFunc(int);
//void IdleFunc();
void PassiveMotionFunc(int, int);

sceneManager* g_Manager = nullptr;

int main(int argc, char** argv)
{
	//--- 윈도우 생성하기 (freeglut)
	glutInit(&argc, argv);							// glut 초기화
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	// 디스플레이 모드 설정(더블 버퍼링 모드 설정.. -> GLUT_DOUBLE
	// 우리가 그림을 그리는 '후면 버퍼'랑, 사용자가 보는 '전면 버퍼'를 분리..
	// 다 그리면 glutSwapBuffers를 사용해 교체해준다!!
	// GLUT_RGBA -> RGB 모드 사용
	// GLUT_DEPTH -> Z버퍼 사용한다는것.. 3D 물체가 겹칠 때 앞뒤 구분하기 위해..
	glutInitWindowPosition(100, 100);					// 윈도우의 위치 지정
	glutInitWindowSize(winWidth, winHeight);			// 윈도우의 크기 지정
	glutCreateWindow(windowTitle.c_str());				// 윈도우 생성 (윈도우 이름)

	//--- GLEW 초기화하기
	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK) {					// glew 초기화
		std::cerr << "Unable to initialize GLEW\n";
		exit(EXIT_FAILURE);
	}
	else {
		std::cout << "GLEW Initialized\n";
	}

	glClearColor(196 / 255.f, 222 / 255.f, 255 / 255.f, 1.f);		// 클리어 색상 (아무것도 안했을 때)

	srand(static_cast<unsigned int>(time(NULL)));

	g_Manager = new sceneManager(winWidth, winHeight);
	g_Manager->initialize();	// 이 안에서 씬들을 생성하고 "start" 씬으로 설정


	glEnable(GL_CULL_FACE);			// 컬링(뒷면 제거) 활성화
	//glCullFace(GL_FRONT);			// 앞면 지울건지 뒷면 지울건지
	//glFrontFace(GL_CCW);			// 어떤게 앞면인지?? GL_CW -> 시계, GL_CCW -> 반시계(기본값)

	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	// 깊이 검사 활성화 --> z버퍼에다가 기록을 하기 시작하고, 그리지 말아야 할 것은 그리지 않는다....
	glEnable(GL_DEPTH_TEST);

	// 콜백 함수들
	// 콜백이란? 만약 이벤트가 발생하면.. 이 함수를 실행해달라고 등록하는 과정..
	glutKeyboardFunc(KeyboardFunc);		// 일반 키보드 (abcde..)
	glutKeyboardUpFunc(KeyboardUpFunc);	// 일반 키보드가 떼졌을 때
	glutSpecialFunc(SpecialFunc);		// 특수 키보드 (F1 ~ F12, HOME, CTRL, ALT 등등..)
	glutSpecialUpFunc(SpecialUpFunc);	// 특수 키보드가 떼졌을 때

	glutMouseFunc(MouseFunc);			// 마우스 입력 (좌클릭, 우클릭, 휠클릭... 
	//glutMotionFunc(MotionFunc);			// 화면 누르고 드래그
	glutPassiveMotionFunc(PassiveMotionFunc);	// 마우스 화면 실시간 회전

	glutDisplayFunc(DisplayFunc);		// 출력 함수의 지정

	glutReshapeFunc(ReshapeFunc);		// 화면 크기가 변경되었을 때....
	glutTimerFunc(16, TimerFunc, 0);		// 16ms --> 초당 60번
	//glutIdleFunc(IdleFunc);		
	// 이게 게임 루프 핵심.. CPU일 안할때, IdleFunc() 실행해줘.. 
	// 그러나 지금은 timer쓸거임

	glutMainLoop(); // 이벤트 처리 시작
	// freeglut가 제어하는 무한루프에 진입

	std::cout << "Exiting Main Loop...\n";
	delete g_Manager; // 생성했던 씬 매니저 삭제
	g_Manager = nullptr;

	return 0;
}


// 해당 게임의 핵심 루프는.. IdleFunc과, DisplayFunc의 연계로 이루어진다..
void DisplayFunc(void)
{
	// 씬을 그린다
	if (g_Manager)
		g_Manager->draw(); // 화면 그리기...
}

void ReshapeFunc(int width, int height)
{
	if (g_Manager)
	{
		g_Manager->reshape(width, height);
	}
}

void KeyboardFunc(unsigned char key, int x, int y)
{

	if (27 == key)		// ESC Key
		glutLeaveMainLoop();

	if (g_Manager)
		g_Manager->keyboard(key, true);
}

void KeyboardUpFunc(unsigned char key, int, int)
{
	if (g_Manager)
		g_Manager->keyboard(key, false);
}

void SpecialFunc(int key, int x, int y)
{
	if (g_Manager)
		g_Manager->specialKeyboard(key, false);
}

void SpecialUpFunc(int key, int, int)
{
	if (g_Manager)
		g_Manager->specialKeyboard(key, false);
}

void MouseFunc(int button, int state, int x, int y)
{
	if (g_Manager)
		g_Manager->mouse(button, state, x, y);
}

void MotionFunc(int x, int y)
{
	if (g_Manager)
		g_Manager->mouseMove(x, y);
}

static std::chrono::high_resolution_clock::time_point last_time =
std::chrono::high_resolution_clock::now();

// TimerFunc이 심장박동 역할.. 게임의 루프
void TimerFunc(int id)
{
	switch (id) {
	case 0:			// 업데이트

		auto now_time = std::chrono::high_resolution_clock::now();
		auto dur_time = now_time - last_time;
		last_time = now_time;

		float deltaTime = dur_time.count() / static_cast<float>(1E09);

		if (g_Manager)
			g_Manager->update(deltaTime);
		glutPostRedisplay();	// 씬을 업데이트 했으니, 화면에 그려줘야 한다..

		glutTimerFunc(16, TimerFunc, 0);
		break;
	}
}

void PassiveMotionFunc(int x, int y)
{
	if (g_Manager) {
		g_Manager->mouseMove(x, y); // SceneManager에 mouseMove 전달
	}
}

