#pragma once
#include <cstdint>
//========================================================
// 패킷 프로토콜 정의 - 2025.11.04
//======================================================== 

#define MAX_ID_LEN 10
#define MAX_PASS_LEN 10

enum direction {
	W = 1,
	A,
	S,
	D
};

enum AnimalType {
	PIG = 0,
	CHICKEN,
	ALPACA,
	PENGUIN,
	FOX
};

enum class PacketType : uint16_t {
	// C->S
	CS_LOGIN_REQUEST = 1000, // 로그인 요청
	CS_MOVE = 1001,
	CS_REQUEST_BUY_ANIMAL = 1060,
	CS_REQUEST_FEED = 1070,
	CS_REQUEST_CLEAN_POOP = 1050,
	CS_REQUEST_SELL = 1080,


	// S->C
	SC_LOGIN_ACCEPT = 2000, // 로그인 ㅇㅋ
	SC_LOGIN_FAIL = 2001,	// 로그인 실패
	SC_MOVE_UPDATE = 2002,
	SC_SPAWN_ANIMAL = 2003,
	SC_SPAWN_POOP = 2004,
	SC_STAT_CHANGE = 2005,
	SC_LOGIN_NOTIFY = 2006,
	SC_LOGOUT = 2007,
	SC_UPDATE_ANIMAL_STATE = 2008,
	SC_ANIMAL_COUNT = 2009,
	SC_REMOVE_ANIMAL = 2010,
	SC_REMOVE_POOP = 2051,
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
	char name[MAX_ID_LEN];
	float x, y, z; // 초기위치설정
	int coin;
	int feed;
	int maxCoin;
	int maxFeed;
	char message[200];
};

// 로그인 실패 S -> C 
struct SC_Login_Fail {
	char message[100]; // 실패사유보내기?
};

struct sc_login_notify {
	int playerID;
	float x;
	float y;
	float z;
};

struct sc_logout {
	int playerID;
};



// 플레이어 이동
struct cs_move
{
	int inputDir;
	float currentYaw; // 플레이어가 지금 바라보고있는방향
};


struct sc_move_update
{
	int playerID; // 누가 움직였는지 ID
	float x;
	float z;
	float yaw;
};


// 스탯 변경
struct sc_stat_change {
	int coin;
	int feed;
};

//==============================================================
//(2003)
struct sc_spawn_animal {
	int animalID;
	int animalType;
	int growStep;
	float x;
	float y;
};

struct sc_spawn_poop {
	int poopID;
	float x;
	float y;
};

struct cs_request_buy_animal {
	int animalType;
};

//=======================================================


// 똥 청소 요청
struct cs_request_clean_poop
{
	int poopID;
};

struct sc_remove_poop {
	int poopID;
};


// 동물 사료 급여 요청
struct cs_request_feed_animal
{
	int AnimalID;
	int AnimalType;
};

// 동물 판매 요청
struct cs_request_sell_animal
{
	int AnimalID;
	int AnimalType;
};

// 동물 성장 단계 업데이트
struct sc_update_animal_state {
	int AnimalID;
	int AnimalType;
	int GrowStep;
};

struct sc_animal_count {
	int pigCount;
	int chickenCount;
	int alpacaCount;
	int penguinCount;
	int foxCount;
};

struct sc_remove_animal {
	int animalID;
	int animalType;
};