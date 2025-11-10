#pragma once
#include <cstdint>
//========================================================
// 패킷 프로토콜 정의 - 2025.11.04
//======================================================== 

#define MAX_ID_LEN 10
#define MAX_PASS_LEN 10


enum class PacketType : uint16_t {
	// C->S
	CS_LOGIN_REQUEST = 1000, // 로그인 요청
	CS_MOVE = 1001,
	
	// S->C
	SC_LOGIN_ACCEPT = 2000, // 로그인 ㅇㅋ
	SC_LOGIN_FAIL = 2001,	// 로그인 실패
	SC_MOVE_UPDATE = 1002,
};

struct PacketHeader {	// 2+2 바이트 고정, 모든 패킷 제일 앞에 붙여보내면됨
	PacketType type;
	uint16_t size;
};

// 로그인 요청. C -> S
struct CS_Login_Request {
	char id[MAX_ID_LEN];
//	char password[MAX_PASS_LEN];
};

// 로그인 ㅇㅋ S -> C 
struct SC_Login_Accept {
	int id;
	int coin;
	int feed;
	char message[200];
};

// 로그인 실패 S -> C 
struct SC_Login_Fail {
	char message[100]; // 실패사유보내기?
};

// 플레이어 이동
struct cs_move
{
	float x;
	float y;
};

struct sc_move_update
{
	int playerID; // 누가 움직였는지 ID
	float x;
	float y;
};

// 똥 청소 요청
struct cs_request_clean_poop
{
	int poopID;
};

// 동물 구매 요청
struct cs_request_buy_animal
{
	int AnimalType;
};

// 동물 사료 급여 요청
struct cs_request_feed_animal
{
	int AnimalID;
};

// 동물 판매 요청
struct cs_request_sell_animal
{
	int AnimalID;
	int AnimalType;
};