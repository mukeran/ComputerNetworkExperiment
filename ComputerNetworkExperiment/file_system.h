#pragma once
#include <iostream>
#include <fstream>
#include <random>
#include <io.h>
#include <direct.h>
#include <set>
#include <map>
#include "mail.h"
#include "utils.h"

using std::map;
using std::set;
using std::string;

class file_system
{
public:
	const string storage_path = "..\\mails";
	file_system();
	void save_mail(mail* mail) const;
	vector<mail> get_mail_list() const;
	mail get_mail(const string& uuid) const;
};
