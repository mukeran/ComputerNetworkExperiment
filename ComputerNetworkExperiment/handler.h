#pragma once
#include "http/request.h"
#include "http/response.h"
using http::request;
using http::response;

class handler
{
public:
	static void handle(const request& req, response* resp);
	static string get_time_string(const time_t& t);
	static string html_escape_string(const string& data);
};
