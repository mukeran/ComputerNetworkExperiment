#include "mail.h"

mail::mail()
{
	uuid = utils::random_uuid();
	status = mail_status::pending;
	created_time = time(nullptr);
	sent_time = 0;
}

mail::mail(const string& uuid, const kv& fields)
{
	this->uuid = uuid;
	from = fields.at("from");
	to = fields.at("to");
	subject = fields.at("subject");
	content = fields.at("content");
	status = static_cast<mail_status>(std::stoi(fields.at("status")));
	created_time = std::stoll(fields.at("created_time"));
	sent_time = std::stoll(fields.at("sent_time"));
	std::stringstream ss(fields.at("log"));
	while (!ss.eof())
	{
		string buf;
		ss >> buf;
		log.emplace_back(utils::base64_decode(buf));
	}
}

mail::mail(const string& from, const string& to, const string& subject, const string& content)
{
	uuid = utils::random_uuid();
	this->from = from;
	this->to = to;
	this->subject = subject;
	this->content = content;
	this->status = mail_status::pending;
	created_time = time(nullptr);
	sent_time = 0;
}

string mail::to_string() const
{
	return
		"From: " + from + "\r\n" +
		"To: " + to + "\r\n" +
		"Subject: " + subject + "\r\n" +
		"\r\n" + content +
		"\r\n.\r\n";
}

kv mail::get_kv() const
{
	kv fields;
	fields["from"] = from;
	fields["to"] = to;
	fields["subject"] = subject;
	fields["content"] = content;
	fields["status"] = std::to_string(static_cast<int>(status));
	fields["created_time"] = std::to_string(created_time);
	fields["sent_time"] = std::to_string(sent_time);
	string encoded;
	for (auto it = log.begin(); it != log.end(); ++it)
	{
		if (it != log.begin())
			encoded += " ";
		encoded += utils::base64_encode(*it);
	}
	fields["log"] = encoded;
	return fields;
}

void mail::append_log(const string& line)
{
	if (line.empty())
		log.emplace_back("<empty>");
	else
		log.push_back(line);
}
