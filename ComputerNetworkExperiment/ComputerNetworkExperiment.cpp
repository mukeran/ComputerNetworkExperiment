#include "http/server.h"

int main()
{
	http::server server("0.0.0.0", 10010);
	server.start();
}
