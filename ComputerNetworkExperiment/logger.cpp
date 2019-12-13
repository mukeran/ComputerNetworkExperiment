#include "logger.h"


logger* logger::self = new logger(std::string(".\\log.txt"));
bool logger::print = true;

logger::logger(std::string path)
{
	// 判断log文件存在性,打开文件
	// 
	//
	ofs.open(path, std::ofstream::app);
	if (!ofs.is_open())
	{
		std::cout << "[ERROR] open log file failed !" << std::endl;
		return;
	}
}

logger::~logger()
{
	ofs.close();
}

void logger::log(std::string s)
{
	std::unique_lock<std::mutex> lock(mt);
	ofs.seekp(std::ios::end);
	std::string time = get_time();
	ofs << time /* << "[" << __FILE__ << "]"*/ << s << std::endl;
	if (print)
	{
		std::cout << time << s << std::endl;
	}
}

std::string logger::get_time()
{
	time_t t = time(0);
	struct tm tm_;
	char tmp[64];
	localtime_s(&tm_, &t);
	strftime(tmp, sizeof(tmp), "[%Y-%m-%d %X]", &tm_);
	return std::string(tmp);
}