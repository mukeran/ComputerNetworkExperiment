#include "response.h"

namespace http
{
	response::response()
	{
		this->status_ = status_code::ok;
	}
	
	response::response(kv headers, string body, const status_code status): headers_(move(headers)), body_(move(body))
	{
		this->status_ = status;
	}
	
	void response::set_header(const string& key, const string& value)
	{
		this->headers_[key] = value;
	}
	
	void response::set_body(const string& body)
	{
		this->body_ = body;
	}
	
	void response::set_status(const status_code status)
	{
		this->status_ = status;
	}

	string response::get_header(const string& key) const
	{
		if (!this->headers_.count(key))
			return "";
		return this->headers_.at(key);
	}

	string response::get_body() const
	{
		return this->body_;
	}

	status_code response::get_status() const
	{
		return this->status_;
	}
	
	string response::raw() const
	{
		auto headers = this->headers_;
		headers["Content-Length"] = std::to_string(this->body_.length());
		auto raw = "HTTP/1.1 " + std::to_string(static_cast<uint16_t>(this->status_)) + " " + status_string.at(static_cast<uint16_t>(this->status_)) + "\r\n";
		auto it = headers.begin();
		while (it != headers.end())
		{
			raw += it->first + ": " + it->second + "\r\n";
			++it;
		}
		raw += "\r\n";
		raw += this->body_;
		return raw;
	}
}
