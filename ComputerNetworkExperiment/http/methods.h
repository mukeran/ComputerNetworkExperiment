#pragma once
#include <string>
#include <map>
#include <cassert>
using std::map;
using std::string;

typedef map<string, string> kv;

namespace http
{
	string url_encode(const string& str);
	string url_decode(const string& str);
	kv parse_query(string query);
	string make_query(const kv& queries);
}
