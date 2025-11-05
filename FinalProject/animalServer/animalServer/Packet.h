#pragma once

#define MAX_ID_LEN 10

// 로그인 요청. C -> S
struct CS_Login_Request {
	char id[MAX_ID_LEN];
};


// 2. S -> C (서버가 클라이언트로 )
struct SC_Login_Result {
	bool success;
	char message[100]; // 로그인 메세지 
};