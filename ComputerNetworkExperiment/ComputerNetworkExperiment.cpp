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

logger* logger::instance = new logger(get_log_name());
bool logger::print = true;
file_system* file_system::instance;
message_queue* message_queue::instance;
smtp::client* smtp::client::instance;

int main(const int argc, const char* argv[])
{
	if (argc != 3)
	{
		logger::error("Invalid parameters. Please type the command in following format:");
		logger::error("ComputerNetworkExperiment.exe <ip> <port>");
		logger::error("Notice: <ip> means the SMTP server of the sender email account; <port> means its SMTP port.");
		return 1;
	}
	init_socket();
	try
	{
		logger::info("Start to initialize components...");
		smtp::client::instance = new smtp::client(argv[1], std::stoi(argv[2]));
		message_queue::instance = new message_queue(max_message_queue_thread);
		message_queue::instance->init();
		file_system::instance = new file_system();
		logger::info("Components initialized");
		logger::info("The SMTP server has been set to " + string(argv[1]) + ":" + string(argv[2]));
		http::server server("0.0.0.0", 8080);
		server.start();
	}
	catch (std::exception& e)
	{
		logger::error(e.what());
		return 2;
	}
}
