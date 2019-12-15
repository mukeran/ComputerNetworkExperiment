#include "template_loader.h"

namespace http
{
	string template_loader::render(const string& name, const map<string, string>& args)
	{
		ifstream fin("templates/" + name + ".html", ios::in);
		const auto content = string(std::istreambuf_iterator<char>(fin), std::istreambuf_iterator<char>());
		auto result = content;
		fin.close();
		auto r = regex(R"(\{\{[\S\s]+\}\})");
		auto it = std::sregex_iterator(content.begin(), content.end(), r);
		auto end = std::sregex_iterator();
		set<string> vis;
		while (it != end)
		{
			auto key = it->str();
			key = key.substr(2, key.length() - 4);
			while (*key.begin() == ' ') key.erase(key.begin());
			while (*key.rbegin() == ' ') key.erase(--key.end());
			string value;
			if (args.count(key))
				value = args.at(key);
			result.replace(it->position(), it->length(), value);
			++it;
		}
		return result;
	}
}