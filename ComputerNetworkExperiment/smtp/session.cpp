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
		this->buf_ = new char[buffer_size + 1];
		connect(this->sock_, reinterpret_cast<SOCKADDR*>(&this->sa_), sizeof SOCKADDR);
		if (this->sock_ == INVALID_SOCKET) {
			std::cout << WSAGetLastError() << std::endl;
			throw std::exception("Connection failed");
		}
		this->mail_ = mail;
		this->auth_ = auth;
		memset(buf_, 0, sizeof(char) * buffer_size);
		recv(sock_, buf_, buffer_size, 0);
		mail_->log.emplace_back(buf_);
	}

	session::~session()
	{
		closesocket(this->sock_);
	}

	
	void session::ehlo() const
	{
		const auto res = send("EHLO ncat.xyz\r\n");
		mail_->log.push_back(res);
	}
	
	void session::auth() const
	{
		const auto username = utils::base64_encode(auth_.username);
		const auto password = utils::base64_encode(auth_.password);
		auto res = send("AUTH LOGIN\r\n");
		mail_->log.push_back(res);
		res = send(username + "\r\n");
		mail_->log.push_back(res);
		res = send(password + "\r\n");
		mail_->log.push_back(res);
	}
	
	void session::mail_from() const
	{
		const auto res = send("MAIL FROM:" + mail_->from + "\r\n");
		mail_->log.push_back(res);
	}
	
	void session::rcpt_to() const
	{
		const auto res = send("RCPT TO:" + mail_->to + "\r\n");
		mail_->log.push_back(res);
	}
	
	void session::data() const
	{
		auto res = send("DATA\r\n");
		mail_->log.push_back(res);
		res = send(mail_->to_string());
		mail_->log.push_back(res);
	}
	
	void session::quit() const
	{
		const auto res = send("QUIT\r\n");
		mail_->log.push_back(res);
	}
};
