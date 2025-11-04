//Sever.cpp

#include "Common.h"
#include "Packet.h"

// Client.h 에 정의된 포트와 버퍼 크기 사용
#define SERVERPORT 9000
#define BUFSIZE    512


// 클라이언트 처리를 위한 스레드 함수 
unsigned __stdcall ClientThread(void* arg)
{
	SOCKET client_sock = (SOCKET)arg;
	int retval;

	// 클라이언트 주소 정보 (필요시 사용)
	struct sockaddr_in clientaddr;
	int addrlen = sizeof(clientaddr);
	getpeername(client_sock, (struct sockaddr*)&clientaddr, &addrlen);

	printf("[TCP 서버] 클라이언트 접속: IP 주소 : %s, 포트번호 : %d (소켓: %lld)\n",
		inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port), client_sock);

	// --- 로그인 처리 ---

	// 1. 로그인 패킷 수신 (ID/PW)
	//    [수정] CS_Login_Request 타입의 변수와 크기를 사용
	CS_Login_Request loginPkt;
	retval = recv(client_sock, (char*)&loginPkt, sizeof(CS_Login_Request), MSG_WAITALL);
	if (retval == SOCKET_ERROR || retval == 0) {
		printf("[TCP 서버] 클라이언트(%lld) 연결 끊김 (로그인 전)\n", client_sock);
		closesocket(client_sock);
		return 1;
	}

	printf("[TCP 서버] 클라이언트(%lld) 로그인 시도: ID=%s, PW=%s\n",
		client_sock, loginPkt.id, loginPkt.password);

	// 2. (임시) 로그인 로직: ID "test", PW "1234" 만 허용
//SC_Login_Result 타입의 변수(resultPkt)를 선언
	SC_Login_Result resultPkt;
	if (strcmp(loginPkt.id, "test") == 0 && strcmp(loginPkt.password, "1234") == 0) {
		resultPkt.success = true;
		strcpy_s(resultPkt.message, "[서버] 로그인 성공! 게임 서버에 접속합니다.");
	}
	else {
		resultPkt.success = false;
		strcpy_s(resultPkt.message, "[서버] ID 또는 비밀번호가 틀립니다.");
	}

	// 3. 로그인 결과 전송
	retval = send(client_sock, (char*)&resultPkt, sizeof(SC_Login_Result), 0);
	if (retval == SOCKET_ERROR) {
		printf("[TCP 서버] 클라이언트(%lld) 연결 오류 (결과 전송 실패)\n", client_sock);
		closesocket(client_sock);
		return 1;
	}

	// 4. 로그인 성공 시 -> 게임 로직 루프 (여기서부터 시작)
	if (resultPkt.success) {
		printf("[TCP 서버] 클라이언트(%lld) 로그인 성공.\n", client_sock);

		// TODO: 이 지점에서부터 클라이언트와 게임 데이터 (이동, 채팅 등)를
		//       주고받는 while(true) 루프를 구현.
		char buf[BUFSIZE];
		while (true) {
			retval = recv(client_sock, buf, BUFSIZE, 0);
			if (retval == SOCKET_ERROR || retval == 0) {
				printf("[TCP 서버] 클라이언트(%lld) 연결 끊김 (게임 중)\n", client_sock);
				break; // 루프 탈출
			}

			// (ex) 받은 데이터가 'PlayerMovePacket' 이라면 처리...
			// (ex) send(client_sock, ...) 로 응답
		}
	}
	else {
		printf("[TCP 서버] 클라이언트(%lld) 로그인 실패. 접속을 종료합니다.\n", client_sock);
	}

	// 스레드 종료 및 소켓 닫기
	closesocket(client_sock);
	printf("[TCP 서버] 클라이언트(%lld) 접속 종료.\n", client_sock);
	return 0;
}

int main(int argc, char* argv[])
{
	int retval;

	// 1. 윈속 초기화
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return 1;

	// 2. 리슨 소켓 생성
	SOCKET listen_sock = socket(AF_INET, SOCK_STREAM, 0);
	if (listen_sock == INVALID_SOCKET) err_quit("socket()");

	// 3. bind()
	struct sockaddr_in serveraddr;
	memset(&serveraddr, 0, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY); // 모든 IP에서 접속 허용 (127.0.0.1 포함)
	serveraddr.sin_port = htons(SERVERPORT); // 9000번 포트
	retval = bind(listen_sock, (struct sockaddr*)&serveraddr, sizeof(serveraddr));
	if (retval == SOCKET_ERROR) err_quit("bind()");

	// 4. listen()
	retval = listen(listen_sock, SOMAXCONN); // 동시 접속 대기 수
	if (retval == SOCKET_ERROR) err_quit("listen()");

	printf("[TCP 서버] 게임 서버가 %d번 포트에서 시작되었습니다.\n", SERVERPORT);

	SOCKET client_sock;
	struct sockaddr_in clientaddr;
	int addrlen = sizeof(clientaddr);

	// 5. accept() - 메인 스레드는 클라이언트 접속만 처리
	while (true) {
		client_sock = accept(listen_sock, (struct sockaddr*)&clientaddr, &addrlen);
		if (client_sock == INVALID_SOCKET) {
			printf("[오류] accept() 실패: %d\n", WSAGetLastError());
			continue;
		}

		// 6. 클라이언트 접속 시 새 스레드 생성
		// (ClientThread 함수가 이 클라이언트 소켓을 전담 처리)
		HANDLE hThread = (HANDLE)_beginthreadex(NULL, 0, ClientThread, (void*)client_sock, 0, NULL);
		if (hThread == 0) {
			printf("[오류] 스레드를 생성할 수 없습니다.\n");
			closesocket(client_sock);
		}
		else {
			CloseHandle(hThread); // 스레드 핸들 정리
		}
	}

	// 7. 서버 종료 (정리)
	closesocket(listen_sock);
	WSACleanup();

	return 0;
}