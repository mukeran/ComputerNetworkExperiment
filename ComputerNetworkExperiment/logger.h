#pragma once
#include <iostream>
#include <string>
#include <fstream>
#include <ctime>
#include <mutex>
using std::string;
using std::cout;
using std::endl;

class logger
{
	explicit logger(const string& log_file);
	~logger();
	void log(const string& x);
	string path_;
	std::ofstream ofs_;
	std::mutex mt_;
public:
	static void info(const string& x) { instance->log(string("[INFO] ") + x); }
	static void warning(const string& x) { instance->log(string("[WARNING] ") + x); }
	static void critical(const string& x) { instance->log(string("[CRITICAL] ") + x); }
	static void debug(const string& x) { instance->log(string("[DEBUG] ") + x); }
	static void error(const string& x) { instance->log(string("[ERROR] ") + x); }
	static string get_time();
	static string get_log_name();
	static logger* instance;
	static bool print;
};


