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
	constexpr auto recv_timeout = 5000;
	
	const map<string, string> status_string = {
		{"101", "The server is unable to connect"},
		{"111", "Connection refused or inability to open an SMTP stream"},
		{"200", "System status message or help reply"},
		{"214", "A response to the HELP command"},
		{"220", "The server is ready"},
		{"221", "The server is closing its transmission channel"},
		{"235", "Authentication succeed"},
		{"250", "Requested mail action okay completed"},
		{"251", "User not local will forward"},
		{"252", "Cannot verify the user, but it will try to deliver the message anyway"},
		{"334", "Waiting for user input"},
		{"354", "Start mail input"},
		{"420", "Timeout connection problem"},
		{"421", "Service is unavailable due to a connection problem"},
		{"422", "The recipient¡¯s mailbox has exceeded its storage limit"},
		{"431", "Not enough space on the disk"},
		{"432", "Recipient¡¯s incoming mail queue has been stopped"},
		{"441", "The recipient¡¯s server is not responding"},
		{"442", "The connection was dropped during the transmission"},
		{"446", "The maximum hop count was exceeded for the message"},
		{"447", "Message timed out because of issues concerning the incoming server"},
		{"449", "Routing error"},
		{"450", "User¡¯s mailbox is unavailable"},
		{"451", "Aborted ¨C Local error in processing"},
		{"452", "Too many emails sent or too many recipients"},
		{"471", "An error of your mail server"},
		{"500", "Syntax error"},
		{"501", "Syntax error in parameters or arguments"},
		{"502", "Command parameter is not implemented"},
		{"503", "Bad sequence of commands, or requires authentication"},
		{"504", "Command parameter is not implemented"},
		{"510", "Bad email address"},
		{"511", "Bad email address"},
		{"512", "Host server for the recipient¡¯s domain name cannot be found in DNS"},
		{"513", "Address type is incorrect"},
		{"523", "Size of your mail exceeds the server limits"},
		{"530", "Authentication problem"},
		{"535", "Authentication failed"},
		{"541", "The recipient address rejected your message"},
		{"550", "Non-existent email address"},
		{"551", "User not local or invalid address ¨C relay denied"},
		{"552", "Exceeded storage allocation"},
		{"553", "Mailbox name invalid"},
		{"554", "Transaction has failed"}
	};
	
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

