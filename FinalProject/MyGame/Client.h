#define _CRT_SECURE_NO_WARNINGS

//===================================================
// 클라이언트 추가 - 2025.10.31
//===================================================
#pragma once
#include "Common.h"
#include "Packet.h"

extern std::string SERVERIP;
extern std::string ID;
#define SERVERPORT 9000
#define BUFSIZE    512

SOCKET sock;

// Client.h 또는 Client.cpp

// ( ... 상단 ... )

void InitClient() // 클라이언트 초기화
{
	int retval;

	// 윈속 초기화
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
	{
		err_quit("윈도우 생성 실패");
	}

	// 소켓 생성
	sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock == INVALID_SOCKET) err_quit("socket()");

	// connect()
	struct sockaddr_in serveraddr;
	memset(&serveraddr, 0, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	inet_pton(AF_INET, SERVERIP.c_str(), &serveraddr.sin_addr);
	serveraddr.sin_port = htons(SERVERPORT);
	retval = connect(sock, (struct sockaddr*)&serveraddr, sizeof(serveraddr));
	if (retval == SOCKET_ERROR) err_quit("connect()");

	CS_Login_Request login_pk;
	strcpy_s(login_pk.id, MAX_ID_LEN, ID.c_str());

	retval = send(sock, reinterpret_cast<char*>(&login_pk), sizeof(CS_Login_Request), 0);
	if (retval == SOCKET_ERROR) {
		err_quit("send() 오류");
	}

	// 로그인 결과 수신
	SC_Login_Result result_pk;
	retval = recv(sock, reinterpret_cast<char*>(&result_pk), sizeof(SC_Login_Result), MSG_WAITALL);
	if (retval == SOCKET_ERROR || retval == 0) {
		err_quit("recv() 오류 또는 서버 연결 끊김");
	}

	// 로그인 결과
	if (result_pk.success) {
		// 로그인 성공!
		// (ex: 게임 씬으로 이동)
		printf("서버: %s\n", result_pk.message);
		// GoToGameScene();
	}
	else {
		// 로그인 실패
		printf("서버: %s\n", result_pk.message);
		err_quit("로그인 실패");
	}
}


void CloseClient() // 클라이언트 종료
{
	// 소켓 닫기
	closesocket(sock);

	// 윈속 종료
	WSACleanup();
}