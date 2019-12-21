#include "session.h"

#define CHECK_STATUS(code, want) \
	if ((code) != (want))\
	{\
		mail_->append_log("Error: " + (code) + " " + status_string.at(code));\
		throw std::exception(("Server do not return " + (code)).data());\
	}


namespace smtp
{
	string session::send(const string& data) const
	{
		::send(sock_, data.data(), data.length(), 0);
		memset(buf_, 0, sizeof(char) * buffer_size);
		const auto size = recv(sock_, buf_, buffer_size, 0);
		if (size <= 0)
		{
			const auto code = WSAGetLastError();
			if (code == WSAETIMEDOUT) {
				mail_->append_log("<timeout>");
				throw std::exception("SMTP Waiting timeout!");
			}
			mail_->append_log("<error:" + std::to_string(code) + ">");
			throw std::exception(("SMTP receive error. Error code: " + std::to_string(code)).data());
		}
		return buf_;
	}
	
	session::session(const string& address, const u_short port, mail* mail, const smtp::auth& auth)
	{
		logger::info("Establishing a connection with server...");
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
		setsockopt(this->sock_, SOL_SOCKET, SO_RCVTIMEO, reinterpret_cast<const char*>(&recv_timeout), sizeof(int));
		logger::info("Connection established");
		this->buf_ = new char[buffer_size + 1];
		this->mail_ = mail;
		this->auth_ = auth;
		memset(buf_, 0, sizeof(char) * buffer_size);
		recv(sock_, buf_, buffer_size, 0);
		mail_->append_log(buf_);
		logger::info("Received \"" + string(buf_) + "\" from server");
	}

	session::~session()
	{
		closesocket(this->sock_);
	}
	
	void session::ehlo() const
	{
		logger::info("Sending EHLO command...");
		mail_->append_log(send("EHLO ncat.xyz\r\n"));
		logger::info("Received \"" + string(buf_) + "\" from server");
		CHECK_STATUS(mail_->log.back().substr(0, 3), "250");
	}
	
	void session::auth() const
	{
		logger::info("Sending AUTH command...");
		const auto username = utils::base64_encode(auth_.username);
		const auto password = utils::base64_encode(auth_.password);
		mail_->append_log(send("AUTH LOGIN\r\n"));
		logger::info("Received \"" + string(buf_) + "\" from server");
		CHECK_STATUS(mail_->log.back().substr(0, 3), "334");
		mail_->append_log(send(username + "\r\n"));
		logger::info("Received \"" + string(buf_) + "\" from server");
		CHECK_STATUS(mail_->log.back().substr(0, 3), "334");
		mail_->append_log(send(password + "\r\n"));
		logger::info("Received \"" + string(buf_) + "\" from server");
		CHECK_STATUS(mail_->log.back().substr(0, 3), "235");
	}
	
	void session::mail_from() const
	{
		logger::info("Sending MAIL FROM command...");
		mail_->append_log(send("MAIL FROM:" + mail_->from + "\r\n"));
		logger::info("Received \"" + string(buf_) + "\" from server");
		CHECK_STATUS(mail_->log.back().substr(0, 3), "250");
	}
	
	void session::rcpt_to() const
	{
		logger::info("Sending RCPT TO command...");
		for (auto it = mail_->to.cbegin(); it != mail_->to.cend(); ++it) {
			mail_->append_log(send("RCPT TO:" + *it + "\r\n"));
			logger::info("Received \"" + string(buf_) + "\" from server");
			CHECK_STATUS(mail_->log.back().substr(0, 3), "250");
		}
	}
	
	void session::data() const
	{
		logger::info("Sending DATA command and mail content...");
		mail_->append_log(send("DATA\r\n"));
		logger::info("Received \"" + string(buf_) + "\" from server");
		CHECK_STATUS(mail_->log.back().substr(0, 3), "354");
		mail_->append_log(send(mail_->to_string()));
		logger::info("Received \"" + string(buf_) + "\" from server");
		CHECK_STATUS(mail_->log.back().substr(0, 3), "250");
	}
	
	void session::quit() const
	{
		logger::info("Sending QUIT command...");
		mail_->append_log(send("QUIT\r\n"));
		logger::info("Received \"" + string(buf_) + "\" from server");
		CHECK_STATUS(mail_->log.back().substr(0, 3), "221");
	}
};
