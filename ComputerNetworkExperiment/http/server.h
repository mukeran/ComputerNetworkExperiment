#pragma once
#include "request.h"
#include "response.h"
#include "../handler.h"
#include "../logger.h"
#include <Ws2tcpip.h>
#include <iostream>
#include <string>
#include <map>
#include <thread>
#pragma comment (lib, "ws2_32.lib")
using std::cout;
using std::endl;
using std::string;
using std::thread;
using std::exception;
using std::map;
using std::stoi;

typedef map<string, string> kv;

namespace http
{
	constexpr auto max_buffer_size = 10000;
	constexpr auto max_header_size = 5000;
	constexpr auto recv_timeout = 10000;
	constexpr auto max_content_length = 1048576;

	struct meta
	{
		string method;
		string path;
		string version;
	};
	
	class server
	{
		string address_;
		u_short port_;
		SOCKET sock_;
		SOCKADDR_IN sa_;
	public:
		server(const string& address, u_short port);
		void start(const int backlog = 20);
		static meta parse_meta(const string& line);
		static kv parse_header(const string& data);
		static string receive(SOCKET client, u_int size);
		static int send_response(SOCKET client, const response& resp);
		static void client_accept(server* server, SOCKET client, SOCKADDR_IN client_sa);
	};
}

