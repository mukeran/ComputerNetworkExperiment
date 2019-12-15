#pragma once
#include <string>
#include <regex>
#include <map>
#include <set>
#include <fstream>
using std::map;
using std::set;
using std::string;
using std::ifstream;
using std::ios;
using std::regex;

namespace http
{
	class template_loader
	{
	public:
		static string render(const string& name, const map<string, string>& args);
	};
}
