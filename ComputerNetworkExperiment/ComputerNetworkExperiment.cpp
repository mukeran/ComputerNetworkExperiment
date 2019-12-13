//#include "http/server.h"
//
//int main()
//{
//	http::server server("0.0.0.0", 10010);
//	server.start();
//}


#include "SMTPClient.h"
#include "MessageQueue.h"


void simulate_hard_computation() {
	std::this_thread::sleep_for(std::chrono::milliseconds(2000));
}

void multiply(const int a, const int b) {
	simulate_hard_computation();
	const int res = a * b;
	std::cout << a << " * " << b << " = " << res << std::endl;
}

int main()
{
	MessageQueue queue(2);
	queue.init();
	queue.submitFunc(multiply, 3, 5);
	return 0;
}
