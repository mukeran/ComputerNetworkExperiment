#pragma once
#include <WS2tcpip.h>
#include "../mail.h"
#include "auth.h"
#include "../utils.h"
#include "../logger.h"
#include <iostream>

namespace smtp
{
	constexpr auto buffer_size = 1024;
	
	class session
	{
		SOCKET sock_;
		sockaddr_in sa_;
		mail* mail_;
		auth auth_;
		char* buf_;
		string send(const string& data) const;
	public:
		session(const string& address, const u_short port, mail* mail, const auth& auth);
		~session();
		void ehlo() const;
		void auth() const;
		void mail_from() const;
		void rcpt_to() const;
		void data() const;
		void quit() const;
	};
}

