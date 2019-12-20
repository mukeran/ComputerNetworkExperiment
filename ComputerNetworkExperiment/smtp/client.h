#pragma once
#include "auth.h"
#include "../file_system.h"
#include "../logger.h"
#include "../mail.h"
#include <iostream>
#include <string>
#include <WS2tcpip.h>
#include "session.h"
#include <future>
#pragma comment(lib, "ws2_32.lib")
using std::string;

namespace smtp
{
	class client
	{
		string address_;
		u_short port_;
	public:
		static client* instance;
		explicit client(const string& address, const u_short port) : address_(address), port_(port) {}
		bool send(mail* mail, const auth& auth) const;
	};
}
