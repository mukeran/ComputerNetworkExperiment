#pragma once
#include <string>
#include <map>
using std::map;
using std::string;

typedef map<string, string> kv;

namespace http
{
	enum class status_code
	{
		ok = 200,
		created = 201,
		move_permanently = 301,
		found = 302,
		temporary_redirect = 307,
		permanent_redirect = 308,
		bad_request = 400,
		unauthorized = 401,
		forbidden = 403,
		not_found = 404,
		method_not_allowed = 405,
		internal_server_error = 500,
		not_implemented = 501,
		bad_gateway = 502,
		service_unavailable = 503,
		gateway_timeout = 504,
		http_version_not_supported = 505,
		network_authentication_required = 511
	};

	const map<uint8_t, string> status_string = {
		{200, "OK"},
		{201, "Created"},
		{301, "Moved Permanently"},
		{302, "Found"},
		{307, "Temporary Redirect"},
		{308, "Permanent Redirect"},
		{400, "Bad Request"},
		{403, "Forbidden"},
		{404, "Not Found"},
		{405, "Method Not Allowed"},
		{500, "Internal Server Error"},
		{501, "Not Implemented"},
		{502, "Bad Gateway"},
		{503, "Service Unavailable"},
		{504, "Gateway Timeout"},
		{505, "HTTP Version Not Supported"},
		{511, "Network Authentication Required"}
	};
	
	class response
	{
	private:
		kv headers_;
		string body_;
		status_code status_;
	public:
		response();
		response(kv headers, string body, status_code status);
		void set_header(const string& key, const string& value);
		void set_body(const string& body);
		void set_status(status_code status);
		string get_header(const string& key) const;
		string get_body() const;
		status_code get_status() const;
		string raw() const;
	};
}
