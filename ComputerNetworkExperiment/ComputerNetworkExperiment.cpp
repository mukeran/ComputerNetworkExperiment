#pragma comment(lib, "ws2_32.lib")
#include "smtp/client.h"
#include "message_queue.h"
using smtp::client;

void init_socket()
{
	WSADATA wsa_data;
	WSAStartup(MAKEWORD(2, 2), &wsa_data);
}

mail m;

int main()
{
	message_queue mq(1);
	mq.init();
	init_socket();
	client c("157.255.174.111", 25);
	m = mail{ "123", "mukeran<mukeran1000@qq.com>", "mukeran<mukeran@mukeran.com>", "test", "test", vector<string>{} };
	auto b = mq.send_mail(c, &m, smtp::auth{ "mukeran1000@qq.com", "couhunarkjvgbaie" });
	b.get();
}
