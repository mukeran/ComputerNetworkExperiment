// #pragma once

// #include <iostream>
// #include <set>
// #include <fstream>
// #include <random>
// #include <time.h>
// #include <io.h>
// #include <direct.h>
// #include "Mail.h"

// using std::set;
// using std::string;

// class file_system
// {
// private:
// 	set<string>* mail_uuid;

// public:
// 	file_system()
// 	{
// 		string DirectoryPath = "..\\MailDataStorage";
// 		if (_access(DirectoryPath.c_str(), 0) == -1)
// 		{
// 			bool flag = _mkdir(DirectoryPath.c_str());
// 		}

// 		mail_uuid = new set<string>();
// 	}

// 	~file_system()
// 	{
// 		delete mail_uuid;
// 	}

// 	void save_mail(mail*);
	
// 	set<string> get_uuid_list();

// 	map<string, string> load_mail(const std::string&);
// };

