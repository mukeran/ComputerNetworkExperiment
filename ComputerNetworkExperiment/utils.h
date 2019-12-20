#pragma once
#include <string>
using std::string;

namespace utils
{
	const string base64_chars =
		"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
		"abcdefghijklmnopqrstuvwxyz"
		"0123456789+/";
	bool is_base64(const unsigned char c);
	string base64_encode(const string& string_to_encode);
	string base64_decode(const string& encoded_string);
	bool is_uuid(const string& str);
	string random_uuid();
}
