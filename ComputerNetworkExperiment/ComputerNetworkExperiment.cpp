//#include "http/server.h"
//
//int main()
//{
//	http::server server("0.0.0.0", 10010);
//	server.start();
//}


// #include "SMTPClient.h"
// #include "MessageQueue.h"


// void simulate_hard_computation() {
// 	std::this_thread::sleep_for(std::chrono::milliseconds(2000));
// }

// void multiply(const int a, const int b) {
// 	simulate_hard_computation();
// 	const int res = a * b;
// 	std::cout << a << " * " << b << " = " << res << std::endl;
// }
#include "http/template_loader.h"
#include <iostream>
#include <map>
using std::map;
using std::cout;

int main()
{
	cout << http::template_loader::render("index", map<string, string>{{"test", "Hello world"}});
}
