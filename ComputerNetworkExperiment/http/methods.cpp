#include "methods.h"

namespace http
{
	constexpr char hex[] = "0123456789ABCDEF";
	
	string url_encode(const string& str)
	{
		string res;
		for (auto c : str)
		{
			if (isalnum(c) || c == '-' || c == '_' || c == '.' || c == '~')
				res += c;
			else if (c == ' ')
				res += '+';
			else
			{
				res += '%';
				res += hex[static_cast<uint8_t>(c) >> 4];
				res += hex[static_cast<uint8_t>(c) & 0b1111];
			}
		}
		return res;
	}

	string url_decode(const string& str)
	{
		string res;
		const auto length = str.length();
		for (auto i = 0u; i < length; ++i)
		{
			const auto c = str.at(i);
			if (c == '+')
				res += ' ';
			else if (c == '%')
			{
				assert(i + 2 < length, "% should follow 2 alphanum character");
				auto from_hex = [](const char c) -> uint8_t
				{
					if (c >= 'A' && c <= 'Z')
						return c - 'A' + 10;
					if (c >= 'a' && c <= 'z')
						return c - 'a' + 10;
					assert(c >= '0' && c <= '9', "% should follow 2 alphanum character");
					return c - '0';
				};
				const auto high = from_hex(str.at(++i));
				const auto low = from_hex(str.at(++i));
				res += static_cast<char>((high << 4) + low);
			}
			else
				res += c;
		}
		return res;
	}

	kv parse_query(string query)
	{
		kv queries;
		while (query.length() != 0)
		{
			const auto a_pos = query.find('&');
			string entry;
			if (a_pos == string::npos)
				entry = query, query = "";
			else
				entry = query.substr(0, a_pos), query = query.substr(a_pos + 1);
			const auto e_pos = entry.find('=');
			string key, value;
			if (e_pos == string::npos)
				key = entry, value = "";
			else
				key = entry.substr(0, e_pos), value = entry.substr(e_pos + 1);
			key = url_decode(key);
			value = url_decode(value);
			queries[key] = value;
		}
		return queries;
	}

	string make_query(const kv& queries)
	{
		string res;
		auto it = queries.begin();
		while (it != queries.end())
		{
			if (it != queries.begin())
				res += '&';
			res += it->first + '=' + it->second;
			++it;
		}
		return res;
	}
}
