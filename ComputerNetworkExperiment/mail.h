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

class mail
{
public:
	mail();
	mail(const string& uuid, const kv& fields);
	mail(const string& from, const string& to, const string& subject, const string& content);
	string uuid;
	string from;
	string to;
	string subject;
	string content;
	time_t created_time;
	time_t sent_time;
	vector<string> log;
	string to_string() const;
	kv get_kv() const;
	void append_log(const string& line);
};
