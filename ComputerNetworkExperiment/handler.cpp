#include "handler.h"
#include "template_loader.h"
#include "file_system.h"
#include "message_queue.h"
#include<string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <stdlib.h>
#include <time.h>
#include <set>
using std::set;
void handler::handle(const request& req, response* resp)
{
	resp->set_header("Content-Type", "text/html;charset=utf-8");
	const auto method = req.get_method();
	const auto path = req.get_path();
	const auto queries = req.get_queries();
	const auto form_fields = req.get_form_fields();


	std::ifstream ofs;
	char* it;
	if (path == "/send" and method == "GET")
	{
		std::ifstream ofs("templates/send.html");
		const auto msg = string(std::istreambuf_iterator<char>(ofs), std::istreambuf_iterator<char>());
		resp->set_body(msg);
		resp->set_status(http::status_code::ok);
	}
	else if (path == "/send" and method == "POST")
	{
		string from = form_fields.at("from_name") + string("<") + form_fields.at("from_mail") + string(">");
		string to = form_fields.at("to_name") + string("<") + form_fields.at("to_mail") + string(">");
		mail m(from, to, form_fields.at("subject"),form_fields.at("content"));
		file_system::instance->save_mail(&m);
		string username = form_fields.at("username");
		string password = form_fields.at("password");
		auto auth = smtp::auth{ username,password };
		message_queue::instance->send_mail(*smtp::client::instance, &m, auth);

		resp->set_status(http::status_code::found);
		resp->set_header("Location", "/detail/" + m.uuid);
	}
	else if (path == "/list")
	{
		auto V = file_system::instance->get_mail_list();
		if (!V.empty())
		{
			string replace = "mail_list";
			string list;
			for (auto it = V.begin(); it != V.end(); it++)
			{
				mail m = *it;
				string created_time = asctime(gmtime(&m.created_time));
				string sent_time = asctime(gmtime(&m.sent_time));
				//gmtime
				list += R"(<tr><td>)";
				list += m.to;
				list += R"(</td><td><a href="/detail/)";
				list += m.uuid;
				list += R"(">)";
				list += m.subject;
				list += R"(</a></td><td>)";
				list += created_time;
				list += R"(</td><td>)";
				list += sent_time;
				list += R"(</td></tr>)";
			}
			map<string, string> M = { {replace,list} };
			string msg = template_loader::render("list", M);
			resp->set_body(msg);
			resp->set_status(http::status_code::ok);
		}
	}
	else if (path.compare(0, 6, "/detail", 0, 6))
	{
		string id = path.substr(8);
		mail m;
		try {
			m = file_system::instance->get_mail(id);
		}
		catch (std::exception & e)
		{
			std::ifstream ofs("templates/detail_not_found.html");
			const auto msg = string(std::istreambuf_iterator<char>(ofs), std::istreambuf_iterator<char>());
			resp->set_body(msg);
			resp->set_status(http::status_code::not_found);
			return;
		}
		vector<string> log = m.log;
		string msg;
		for (auto it = log.begin(); it != log.end(); ++it)
		{
			msg += *it;
			msg += R"(</br>)";
		}
		auto get_time = [](time_t t) -> const char* {
			if (t == 0)
				return "-";
			auto tmp = asctime(gmtime(&t));
			if (tmp == nullptr)
				return "Error time";
			return tmp;
		};
		string created_time = get_time(m.created_time);
		string sent_time = get_time(m.sent_time);
		map<string, string> M = { {"from",m.from},{"to",m.to},{"created_time",created_time},{"sent_time",sent_time},{"subject",m.subject},{"content",m.content},{"log",msg} };
		string msg = template_loader::render("detail", M);
		resp->set_body(msg);
		resp->set_status(http::status_code::ok);
	}
	if (req.get_header("connection") == "keep-alive")
		resp->set_header("Connection", "Keep-Alive");
	else
		resp->set_header("Connection", "Close");

}
