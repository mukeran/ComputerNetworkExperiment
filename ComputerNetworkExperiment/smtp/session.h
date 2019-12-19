#pragma once
#include <WS2tcpip.h>
#include "../mail.h"
#include "auth.h"
#include "../utils.h"

namespace smtp
{
	constexpr auto buffer_size = 1024;
	
	class session
	{
		SOCKET sock_;
		sockaddr_in sa_;
		mail mail_;
		auth auth_;
		char* buf_;
		string send(const string& data) const;
	public:
		session(const string& address, const u_short port, mail mail, auth auth);
		~session();
		void ehlo();
		void auth();
		void mail_from();
		void rcpt_to();
		void data();
		void quit();
	};
}

