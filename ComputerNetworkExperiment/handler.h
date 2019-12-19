#pragma once
#include "http/request.h"
#include "http/response.h"
using http::request;
using http::response;

class handler
{
public:
	static void handle(const request& req, response* resp);
};
