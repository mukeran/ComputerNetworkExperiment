#include "logger.h"

logger::logger(const string& log_file)
{
	ofs_.open(log_file, std::ofstream::app);
	if (!ofs_.is_open())
	{
		cout << get_time() << "[ERROR] Failed to open log file!" << endl;
		return;
	}
	cout << get_time() << "[INFO] The log will be written to " << log_file << endl;
}

logger::~logger()
{
	ofs_.close();
}

void logger::log(const string& x)
{
	std::unique_lock<std::mutex> lock(mt_);
	ofs_.seekp(std::ios::end);
	const auto time = get_time();
	ofs_ << time << x << endl;
	if (print)
		cout << time << x << endl;
}

std::string logger::get_time()
{
	auto t = time(nullptr);
	tm tm;
	char tmp[64];
	localtime_s(&tm, &t);
	strftime(tmp, sizeof(tmp), "[%Y-%m-%d %X]", &tm);
	return std::string(tmp);
}

std::string logger::get_log_name()
{
	auto t = time(nullptr);
	tm tm;
	char tmp[64];
	localtime_s(&tm, &t);
	strftime(tmp, sizeof(tmp), "log-%Y%m%d%H%M.txt", &tm);
	return std::string(tmp);
}