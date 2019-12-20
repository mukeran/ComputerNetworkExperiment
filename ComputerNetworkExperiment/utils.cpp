#include "utils.h"
#include "utils.h"
#include <ctime>

namespace utils
{
	bool is_base64(const unsigned char c)
	{
		return (isalnum(c) || (c == '+') || (c == '/'));
	}
    
    string base64_encode(const string& string_to_encode)
	{
		
		string ret;
		auto i = 0, j = 0, k = 0;
		unsigned char char_array_3[3];
		unsigned char char_array_4[4];
		auto len = string_to_encode.length();

		while (len--)
		{
			char_array_3[i++] = string_to_encode[k++];
			if (i == 3) {
				char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
				char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
				char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
				char_array_4[3] = char_array_3[2] & 0x3f;

				for (i = 0; (i < 4); i++)
					ret += base64_chars[char_array_4[i]];
				i = 0;
			}
		}

		if (i)
		{
			for (j = i; j < 3; j++)
				char_array_3[j] = '\0';

			char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
			char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
			char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);

			for (j = 0; (j < i + 1); j++)
				ret += base64_chars[char_array_4[j]];

			while ((i++ < 3))
				ret += '=';

		}

		return ret;

	}

    string base64_decode(const string& encoded_string)
	{
		auto in_len = encoded_string.size();
		auto i = 0, j = 0, k = 0;
		unsigned char char_array_4[4], char_array_3[3];
		std::string ret;

		while (in_len-- && (encoded_string[k] != '=') && is_base64(encoded_string[k]))
		{
			char_array_4[i++] = encoded_string[k]; k++;
			if (i == 4)
			{
				for (i = 0; i < 4; i++)
					char_array_4[i] = base64_chars.find(char_array_4[i]) & 0xff;

				char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
				char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
				char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

				for (i = 0; (i < 3); i++)
					ret += char_array_3[i];
				i = 0;
			}
		}

		if (i)
		{
			for (j = 0; j < i; j++)
				char_array_4[j] = base64_chars.find(char_array_4[j]) & 0xff;

			char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
			char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);

			for (j = 0; (j < i - 1); j++) ret += char_array_3[j];
		}

		return ret;
	}
	bool is_uuid(const string& str)
	{
		if (str.length() != 35)
			return false;
		if (str.at(8) != '-' || str.at(13) != '-' || str.at(18) != '-')
			return false;
		for (auto i = 0u; i < str.length(); ++i)
		{
			if (i == 8 || i == 13 || i == 18) continue;
			if (!((str.at(i) >= '0' && str.at(i) <= '9') || (str.at(i) >= 'a' && str.at(i) <= 'f')))
				return false;
		}
		return true;
	}

	string random_uuid()
	{
		const string uuid_chars = "0123456789abcdef";
		srand(time(nullptr));
		string uuid;
		for (auto i = 1; i <= 8; ++i)
			uuid += uuid_chars[rand() % 16];
		uuid += '-';
		for (auto i = 1; i <= 4; ++i)
			uuid += uuid_chars[rand() % 16];
		uuid += '-';
		for (auto i = 1; i <= 4; ++i)
			uuid += uuid_chars[rand() % 16];
		uuid += '-';
		for (auto i = 1; i <= 16; ++i)
			uuid += uuid_chars[rand() % 16];
		return uuid;
	}
}
