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
	unserialize(fields.at("to"), to);
	subject = fields.at("subject");
	content = fields.at("content");
	status = static_cast<mail_status>(std::stoi(fields.at("status")));
	created_time = std::stoll(fields.at("created_time"));
	sent_time = std::stoll(fields.at("sent_time"));
	unserialize(fields.at("log"), log);
}

mail::mail(const string& from, const vector<string>& to, const string& subject, const string& content)
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
		"To: " + utils::join(to, ',') + "\r\n" +
		"Subject: " + subject + "\r\n" +
		"\r\n" + content +
		"\r\n.\r\n";
}

kv mail::get_kv() const
{
	kv fields;
	fields["from"] = from;
	fields["to"] = serialize(to);
	fields["subject"] = subject;
	fields["content"] = content;
	fields["status"] = std::to_string(static_cast<int>(status));
	fields["created_time"] = std::to_string(created_time);
	fields["sent_time"] = std::to_string(sent_time);
	fields["log"] = serialize(log);
	return fields;
}

void mail::append_log(const string& line)
{
	if (line.empty())
		log.emplace_back("<empty>");
	else
		log.push_back(line);
}

string mail::serialize(const vector<string>& vec)
{
	string encoded;
	for (auto it = vec.cbegin(); it != vec.end(); ++it)
	{
		if (it != vec.cbegin())
			encoded += " ";
		encoded += utils::base64_encode(*it);
	}
	return encoded;
}

void mail::unserialize(const string& str, vector<string>& vec)
{
	std::stringstream ss(str);
	while (!ss.eof())
	{
		string buf;
		ss >> buf;
		vec.emplace_back(utils::base64_decode(buf));
	}
}
