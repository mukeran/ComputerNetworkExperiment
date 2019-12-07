#pragma once
#include "request.h"
#include "response.h"

namespace http
{
	class handler
	{
	private:
	public:
		static void handle(const request& req, response* resp);
	};
}

