#pragma once
#include <string>
#include <vector>
#include <map>
#include <sstream>
#include <ctime>
#include "utils.h"
using std::map;
using std::string;
using std::vector;

typedef map<string, string> kv;

enum class mail_status
{
	pending = 0,
	sending = 1,
	failed = 2,
	success = 3
};

class mail
{
public:
	mail();
	mail(const string& uuid, const kv& fields);
	mail(const string& from, const vector<string>& to, const string& subject, const string& content);
	string uuid;
	string from;
	vector<string> to;
	string subject;
	string content;
	mail_status status;
	time_t created_time;
	time_t sent_time;
	vector<string> log;
	string to_string() const;
	kv get_kv() const;
	void append_log(const string& line);
	static string serialize(const vector<string>& vec);
	static void unserialize(const string& str, vector<string>& vec);
};
