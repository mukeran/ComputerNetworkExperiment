#include "http/server.h"
#include "file_system/FileSystem.h"

int main()
{
	http::server server("0.0.0.0", 10010);
	server.start();

	file_system* email_files = new file_system();
}
