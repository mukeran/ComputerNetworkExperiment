#include "handler.h"

namespace http
{
	void handler::handle(const request& req, response* resp)
	{
		resp->set_header("Content-Type", "text/html;charset=utf-8");
		const auto method = req.get_method();
		const auto path = req.get_path();
		const auto queries = req.get_queries();
		const auto form_fields = req.get_form_fields();
		string msg;
		msg += R"(<html><head><meta charset="utf-8"></head><body>)";
		msg += "method: " + method + "; path: " + path + "<br>";
		msg += "Queries:<br>";
		msg += R"(<table><thead><th>key</th><th>value</th></thead><tbody>)";
		auto it = queries.begin();
		while (it != queries.end())
		{
			msg += R"(<tr><td>)" + it->first + R"(</td><td>)" + it->second + R"(</td></tr>)";
			++it;
		}
		msg += R"(</tbody></table><br>)";
		msg += "Form fields:<br>";
		msg += R"(<table><thead><th>key</th><th>value</th></thead><tbody>)";
		it = form_fields.begin();
		while (it != form_fields.end())
		{
			msg += R"(<tr><td>)" + it->first + R"(</td><td>)" + it->second + R"(</td></tr>)";
			++it;
		}
		msg += R"(</tbody></table><br>)";
		resp->set_body(msg);
		if (req.get_header("connection") == "keep-alive")
			resp->set_header("Connection", "Keep-Alive");
		else
			resp->set_header("Connection", "Close");
		resp->set_status(status_code::ok);
	}
}
