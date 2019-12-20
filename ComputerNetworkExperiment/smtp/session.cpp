#include "session.h"
#include <iostream>

namespace smtp
{
	string session::send(const string& data) const
	{
		::send(sock_, data.data(), data.length(), 0);
		memset(buf_, 0, sizeof(char) * buffer_size);
		recv(sock_, buf_, buffer_size, 0);
		return buf_;
	}
	
	session::session(const string& address, const u_short port, mail* mail, const smtp::auth& auth)
	{
		this->sock_ = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
		memset(&this->sa_, 0, sizeof(this->sa_));
		this->sa_.sin_family = AF_INET;
		this->sa_.sin_port = htons(port);
		inet_pton(AF_INET, address.data(), &(this->sa_.sin_addr));
		connect(this->sock_, reinterpret_cast<SOCKADDR*>(&this->sa_), sizeof SOCKADDR);
		if (this->sock_ == INVALID_SOCKET) {
			std::cout << WSAGetLastError() << std::endl;
			throw std::exception("Connection failed");
		}
		this->buf_ = new char[buffer_size + 1];
		this->mail_ = mail;
		this->auth_ = auth;
		memset(buf_, 0, sizeof(char) * buffer_size);
		recv(sock_, buf_, buffer_size, 0);
		mail_->append_log(buf_);
	}

	session::~session()
	{
		closesocket(this->sock_);
	}
	
	void session::ehlo() const
	{
		mail_->append_log(send("EHLO ncat.xyz\r\n"));
	}
	
	void session::auth() const
	{
		const auto username = utils::base64_encode(auth_.username);
		const auto password = utils::base64_encode(auth_.password);
		mail_->append_log(send("AUTH LOGIN\r\n"));
		mail_->append_log(send(username + "\r\n"));
		mail_->append_log(send(password + "\r\n"));
	}
	
	void session::mail_from() const
	{
		mail_->append_log(send("MAIL FROM:" + mail_->from + "\r\n"));
	}
	
	void session::rcpt_to() const
	{
		mail_->append_log(send("RCPT TO:" + mail_->to + "\r\n"));
	}
	
	void session::data() const
	{
		mail_->append_log(send("DATA\r\n"));
		mail_->append_log(send(mail_->to_string()));
	}
	
	void session::quit() const
	{
		mail_->append_log(send("QUIT\r\n"));
	}
};
