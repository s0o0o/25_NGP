#pragma once
//========================================================
// 패킷 프로토콜 정의 - 2025.11.04
//======================================================== 

// 로그인 요청
struct cs_login_request {
	char ID[15];
};

// 플레이어 이동
struct cs_move
{
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