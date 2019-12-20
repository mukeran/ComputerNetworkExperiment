#pragma comment(lib, "ws2_32.lib")
#include "smtp/client.h"
using smtp::client;

void init_socket()
{
	WSADATA wsa_data;
	WSAStartup(MAKEWORD(2, 2), &wsa_data);
}

int main()
{
	init_socket();
	client c("157.255.174.111", 25);
	auto m = mail{ "123", "mukeran<mukeran1000@qq.com>", "mukeran<mukeran@mukeran.com>", "test", "test", vector<string>{} };
	c.send(&m, smtp::auth{ "mukeran1000@qq.com", "couhunarkjvgbaie" });
}
