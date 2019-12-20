#include "http/server.h"
#include "smtp/client.h"
#include "file_system.h"
#include "message_queue.h"
#include "logger.h"
#include <iostream>
#pragma comment(lib, "ws2_32.lib")
using std::cout;
using std::endl;

constexpr auto max_message_queue_thread = 2;

void init_socket()
{
	WSADATA wsa_data;
	WSAStartup(MAKEWORD(2, 2), &wsa_data);
}

file_system* file_system::instance;
message_queue* message_queue::instance;
smtp::client* smtp::client::instance;

int main(const int argc, const char* argv[])
{
	if (argc != 3)
	{
		cout << "Invalid parameters. Please type the command in following format:" << endl;
		cout << "ComputerNetworkExperiment.exe <ip> <port>" << endl;
		cout << "Notice: <ip> means the SMTP server of the sender email account; <port> means its SMTP port." << endl;
		return 1;
	}
	init_socket();
	try
	{
		smtp::client::instance = new smtp::client(argv[1], std::stoi(argv[2]));
	}
	catch (std::exception& e)
	{
		logger::error(e.what());
		return 2;
	}
	message_queue::instance = new message_queue(max_message_queue_thread);
	file_system::instance = new file_system();
	http::server server("0.0.0.0", 8080);
	server.start();
}
