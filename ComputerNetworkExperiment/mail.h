#pragma once
#include <string>
#include <vector>
using std::string;
using std::vector;

class mail
{
public:
	string uuid;
	string from;
	string to;
	string subject;
	string content;
	vector<string> log;
};
