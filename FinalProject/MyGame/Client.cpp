#define _CRT_SECURE_NO_WARNINGS
#include "Client.h"
#include "PacketProcessor.h"

volatile GameState g_gameState = GameState::STATE_LOGIN_SCENE;
SOCKET g_sock;


void err_quit(const char* msg)
{
	LPVOID lpMsgBuf;
	FormatMessageA(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, WSAGetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(char*)&lpMsgBuf, 0, NULL);
	MessageBoxA(NULL, (const char*)lpMsgBuf, msg, MB_ICONERROR);
	LocalFree(lpMsgBuf);
	exit(1);
}

DWORD WINAPI ReceiveThread(LPVOID arg)
{
	SOCKET sock = (SOCKET)arg;
	PacketHeader header;
	int retval;
	char dataBuffer[BUFSIZE];

	while (true)
	{
		retval = recv(sock, (char*)&header, sizeof(PacketHeader), MSG_WAITALL);
		if (retval == SOCKET_ERROR || retval == 0) {
			printf("[클라] 헤더 수신 실패\n");
			break; 
		}
		if (header.size > 0)	
		{
			if (header.size > BUFSIZE) {
				printf("[클라] 비정상적인 크기의 패킷 (%d)\n", header.size);
				break;
			}
			retval = recv(sock, dataBuffer, header.size, MSG_WAITALL);	// 데이터
			if (retval == SOCKET_ERROR || retval == 0) {
				printf("[클라] 서버와 연결 끊김 (데이터 수신 실패)\n");
				break; 
			}
		}

		// 서버가 보낸 패킷 처리하기
		ProcessPacket(header.type, dataBuffer);
	}

	printf("[클라] 수신 스레드 종료..\n");
	return 0;
}

int sendPacket(SOCKET sock, PacketType type, const char* data, uint16_t dataSize)
{
	PacketHeader header;
	header.type = type;
	header.size = dataSize;

	// 1. 헤더 전송 (4바이트)
	int retval = send(sock, (const char*)&header, sizeof(PacketHeader), 0);
	if (retval == SOCKET_ERROR) {
		printf("send() 헤더 전송 오류: %d\n", WSAGetLastError());
		return SOCKET_ERROR;
	}

	// 2. 데이터 전송 (size 만큼)
	if (dataSize > 0) {
		retval = send(sock, data, dataSize, 0);
		if (retval == SOCKET_ERROR) {
			printf("send() 데이터 전송 오류: %d\n", WSAGetLastError());
			return SOCKET_ERROR;
		}
	}

	return (sizeof(PacketHeader) + dataSize); // 총 보낸 바이트 수 반환
}


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
	g_sock = socket(AF_INET, SOCK_STREAM, 0);
	if (g_sock == INVALID_SOCKET) err_quit("socket()");

	// connect()
	struct sockaddr_in serveraddr;
	memset(&serveraddr, 0, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	inet_pton(AF_INET, SERVERIP.c_str(), &serveraddr.sin_addr);
	serveraddr.sin_port = htons(SERVERPORT);
	retval = connect(g_sock, (struct sockaddr*)&serveraddr, sizeof(serveraddr));
	if (retval == SOCKET_ERROR) err_quit("connect()");

	CS_Login_Request login_pk;
	strcpy_s(login_pk.id, MAX_ID_LEN, ID.c_str());

	retval = sendPacket(g_sock, PacketType::CS_LOGIN_REQUEST, reinterpret_cast<char*>(&login_pk), sizeof(CS_Login_Request));

	if (retval == SOCKET_ERROR) {
		err_quit("send() 오류");
	}
	printf(" 로그인 요청 보냄 ");

	// 로그인 보냇으면 이제 받기 시작..수신스레드시작!!
	HANDLE hThread = (HANDLE)CreateThread(NULL, 0, ReceiveThread, (LPVOID)g_sock, 0, NULL);
	if (hThread == 0) {
		printf("수신 스레드 생성 실패\n");
		g_gameState = GameState::STATE_LOGIN_FAILED;
	}
	else {
		// 스레드 시작 성공
		CloseHandle(hThread); // 스레드 핸들은 바로 닫아도 스레드는 계속 돔
	}
	//

	//retval = send(sock, reinterpret_cast<char*>(&login_pk), sizeof(CS_Login_Request), 0);

	//// 로그인 결과 수신
	//SC_Login_Result result_pk;
	//retval = recv(sock, reinterpret_cast<char*>(&result_pk), sizeof(SC_Login_Result), MSG_WAITALL);
	//if (retval == SOCKET_ERROR || retval == 0) {
	//	err_quit("recv() 오류 또는 서버 연결 끊김");
	//}

	//// 로그인 결과
	//if (result_pk.success) {
	//	// 로그인 성공!
	//	// (ex: 게임 씬으로 이동)
	//	printf("서버: %s\n", result_pk.message);
	//	// GoToGameScene();
	//}
	//else {
	//	// 로그인 실패
	//	printf("서버: %s\n", result_pk.message);
	//	err_quit("로그인 실패");
	//}
}


void CloseClient() // 클라이언트 종료
{
	// 소켓 닫기
	closesocket(g_sock);

	// 윈속 종료
	WSACleanup();
}

