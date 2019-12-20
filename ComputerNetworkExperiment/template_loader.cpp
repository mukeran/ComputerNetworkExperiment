#include "template_loader.h"

string template_loader::render(const string& name)
{
	ifstream fin("templates/" + name + ".html", ios::in);
	const auto content = string(std::istreambuf_iterator<char>(fin), std::istreambuf_iterator<char>());
	fin.close();
	return content;
}

string template_loader::render(const string& name, const map<string, string>& args)
{
	ifstream fin("templates/" + name + ".html", ios::in);
	const auto content = string(std::istreambuf_iterator<char>(fin), std::istreambuf_iterator<char>());
	auto result = content;
	fin.close();
	auto r = regex(R"(\{\{[\w_ ]+\}\})");
	auto it = std::sregex_iterator(content.begin(), content.end(), r);
	auto end = std::sregex_iterator();
	set<string> vis;
	int offset = 0;
	while (it != end)
	{
		auto key = it->str();
		key = key.substr(2, key.length() - 4);
		while (*key.begin() == ' ') key.erase(key.begin());
		while (*key.rbegin() == ' ') key.erase(--key.end());
		string value;
		if (args.count(key))
			value = args.at(key);
		result.replace(it->position() + offset, it->length(), value);
		offset += value.length() - it->length();
		++it;
	}
	return result;
}
