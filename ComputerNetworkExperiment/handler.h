#pragma once
#include "http/request.h"
#include "http/response.h"
#include "template_loader.h"
#include "file_system.h"
#include "message_queue.h"
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <cstdlib>
#include <ctime>
using http::request;
using http::response;

class handler
{
public:
	static void handle(const request& req, response* resp);
	static string get_time_string(const time_t& t);
	static string html_escape_string(const string& data);
};
