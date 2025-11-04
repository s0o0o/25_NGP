#pragma once

// 로그인 요청
struct sc_login_request {
	char id[15];
};

// 플레이어 이동
struct sc_move
{
	double x;
	double y;
};
