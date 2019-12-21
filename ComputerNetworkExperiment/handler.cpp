#include "handler.h"

void handler::handle(const request& req, response* resp)
{
	resp->set_header("Content-Type", "text/html;charset=utf-8");
	const auto method = req.get_method();
	const auto path = req.get_path();
	const auto queries = req.get_queries();
	const auto form_fields = req.get_form_fields();
	auto status_to_string = [](const mail_status status) -> string
	{
		if (status == mail_status::pending)
			return "Pending";
		if (status == mail_status::sending)
			return "Sending";
		if (status == mail_status::failed)
			return "Failed";
		return "Success";
	};
	
	if (path.empty() || path == "/")
	{
		resp->set_status(http::status_code::found);
		resp->set_header("Location", "/list");
	}
	else if (path == "/send" && method == "get")
	{
		const auto msg = template_loader::render("send");
		resp->set_body(msg);
		resp->set_status(http::status_code::ok);
	}
	else if (path == "/send" && method == "post")
	{
		const auto from = form_fields.at("from_name") + string("<") + form_fields.at("from_email") + string(">");
		const auto to = utils::split(form_fields.at("to"), ';');
		mail m(from, to, form_fields.at("subject"),form_fields.at("content"));
		file_system::instance->save_mail(&m);
		string username = form_fields.at("username");
		string password = form_fields.at("password");
		message_queue::instance->send_mail(m, smtp::auth{ username,password });

		resp->set_status(http::status_code::found);
		resp->set_header("Location", "/detail/" + m.uuid);
	}
	else if (path == "/list")
	{
		auto mail_list = file_system::instance->get_mail_list();
		string replace = "mail_list";
		string list;
		if (!mail_list.empty())
		{
			auto compare = [](auto a, auto b) -> bool
			{
				return a.created_time > b.created_time;
			};
			std::sort(mail_list.begin(), mail_list.end(), compare);
			for (auto it = mail_list.begin(); it != mail_list.end(); ++it)
			{
				auto m = *it;
				auto created_time = get_time_string(m.created_time);
				auto sent_time = get_time_string(m.sent_time);
				list += R"(<tr><td>)";
				list += html_escape_string(utils::join(m.to, ','));
				list += R"(</td><td><a href="/detail/)";
				list += m.uuid;
				list += R"(">)";
				list += html_escape_string(m.subject);
				list += R"(</a></td><td>)";
				list += created_time;
				list += R"(</td><td>)";
				list += sent_time;
				list += R"(</td><td>)";
				list += status_to_string(m.status);
				list += R"(</td></tr>)";
			}
		}
		map<string, string> M = { {replace,list} };
		const auto msg = template_loader::render("list", M);
		resp->set_body(msg);
		resp->set_status(http::status_code::ok);
	}
	else if (path.compare(0, 7, "/detail") == 0)
	{
		string id = path.substr(8);
		mail m;
		try {
			m = file_system::instance->get_mail(id);
		}
		catch (std::exception&)
		{
			const auto msg = template_loader::render("detail_not_found");
			resp->set_body(msg);
			resp->set_status(http::status_code::not_found);
			return;
		}
		string msg;
		for (auto it = m.log.cbegin(); it != m.log.cend(); ++it)
		{
			msg += html_escape_string(*it);
			msg += R"(</br>)";
		}
		const auto created_time = get_time_string(m.created_time);
		const auto sent_time = get_time_string(m.sent_time);
		map<string, string> mp =
		{
			{"from", html_escape_string(m.from)},
			{"to", html_escape_string(utils::join(m.to, ','))},
			{"created_time", created_time},
			{"sent_time", sent_time},
			{"subject", html_escape_string(m.subject)},
			{"content", html_escape_string(m.content)},
			{"log", msg},
			{"status", status_to_string(m.status)}
		};
		msg = template_loader::render("detail", mp);
		resp->set_body(msg);
		resp->set_status(http::status_code::ok);
	}
	else
	{
		resp->set_body("404 Not Found!");
		resp->set_status(http::status_code::not_found);
	}
	if (req.get_header("connection") == "keep-alive")
		resp->set_header("Connection", "Keep-Alive");
	else
		resp->set_header("Connection", "Close");

}

string handler::get_time_string(const time_t& t)
{
	if (t == 0)
		return "-";
	tm tm;
	char tmp[64];
	localtime_s(&tm, &t);
	strftime(tmp, sizeof(tmp), "%Y-%m-%d %X", &tm);
	return tmp;
}

string handler::html_escape_string(const string& data)
{
	std::string buffer;
	buffer.reserve(data.size());
	for (auto &c : data) {
		switch (c) {
		case '&':  buffer.append("&amp;");       break;
		case '\"': buffer.append("&quot;");      break;
		case '\'': buffer.append("&apos;");      break;
		case '<':  buffer.append("&lt;");        break;
		case '>':  buffer.append("&gt;");        break;
		default:   buffer.append(&c, 1); break;
		}
	}
	return buffer;
}
