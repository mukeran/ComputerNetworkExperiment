#pragma once
#include <iostream>
#include <string>
#include <fstream>
#include <ctime>
#include <mutex>

class logger
{
public:


	static void info(std::string x) { self->log(std::string("[INFO]") + x); }
	static void warning(std::string x) { self->log(std::string("[WARNING]") + x); }
	static void critical(std::string x) { self->log(std::string("[CRITICAL]") + x); }
	static void debug(std::string x) { self->log(std::string("[DEBUG]") + x); }
	static void error(std::string x) { self->log(std::string("[ERROR]") + x); }


private:
	logger(const std::string LogFile);
	~logger();

	void log(std::string x);
	std::string path;
	std::ofstream ofs;
	static logger* self;
	std::mutex mt;
	std::string get_time();
	static bool print;
};


