#include "request.h"

namespace http
{
	request::request(const string& method, const string& path, const string& version, kv headers, const string& body)
	{
		this->method_ = method;
		this->version_ = version;
		this->headers_ = headers;
		this->body_ = body;
		const auto q_pos = path.find('?');
		if (q_pos == string::npos)
			this->path_ = url_decode(path);
		else
		{
			this->path_ = url_decode(path.substr(0, q_pos));
			this->query_string_ = path.substr(q_pos + 1);
			this->queries_ = parse_query(this->query_string_);
		}
		if (headers.count("content-type") && headers.at("content-type") == "application/x-www-form-urlencoded")
			this->form_fields_ = parse_query(this->body_);
	}

	string request::get_method() const
	{
		return this->method_;
	}

	string request::get_path() const
	{
		return this->path_;
	}

	string request::get_version() const
	{
		return this->version_;
	}

	string request::get_header(const string& key) const
	{
		if (!this->headers_.count(key))
			return "";
		return this->headers_.at(key);
	}

	string request::get_body() const
	{
		return this->body_;
	}
	
	string request::get_query(const string& key) const
	{
		if (!this->queries_.count(key))
			return "";
		return this->queries_.at(key);
	}
	
	kv request::get_queries() const
	{
		return this->queries_;
	}
	
	string request::get_form_field(const string& key) const
	{
		if (!this->form_fields_.count(key))
			return "";
		return this->form_fields_.at(key);
	}
	
	kv request::get_form_fields() const
	{
		return this->form_fields_;
	}
}
