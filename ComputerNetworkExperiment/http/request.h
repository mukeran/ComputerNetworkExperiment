#pragma once
#include "methods.h"
#include <string>
#include <map>
using std::map;
using std::string;

typedef map<string, string> kv;

namespace http
{
	class request
	{
	private:
		string method_;
		string path_;
		string version_;
		kv headers_;
		string body_;
		string query_string_;
		kv queries_;
		kv form_fields_;
	public:
		request(const string& method, const string& path, const string& version, kv headers, const string& body);
		string get_method() const;
		string get_path() const;
		string get_version() const;
		string get_header(const string& key) const;
		string get_body() const;
		string get_query(const string& key) const;
		kv get_queries() const;
		string get_form_field(const string& key) const;
		kv get_form_fields() const;
	};
}

