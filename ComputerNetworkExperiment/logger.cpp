#include "logger.h"


logger* logger::self = new logger(std::string(".\\log.txt"));

logger::logger(std::string path)
{
	// �ж�log�ļ�������,���ļ�
	// 
	//
	ofs.open(path, std::ofstream::app);
	if (!ofs.is_open())return;
}

logger::~logger()
{ 
	ofs.close();
}

void logger::log(std::string x)
{
	
	ofs.seekp(std::ios::end);
	ofs << get_time() << _T("[") << __FUNCTION__ << _T("]") << x << std::endl;
}

std::string logger::get_time()
{
	time_t t = time(0);
	char tmp[64];
	strftime(tmp, sizeof(tmp), "[%Y-%m-%d %X]", localtime(&t));
	return std::string(tmp);
}