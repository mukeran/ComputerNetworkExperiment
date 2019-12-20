#pragma comment(lib, "ws2_32.lib")
#include "smtp/client.h"
#include "file_system.h"
#include <iostream>
using std::cout;
using std::endl;

void init_socket()
{
	WSADATA wsa_data;
	WSAStartup(MAKEWORD(2, 2), &wsa_data);
}

int main()
{
	const file_system fs;
	const auto list = fs.get_mail_list();
	for (auto it = list.cbegin(); it != list.cend(); ++it)
	{
		cout << it->uuid << " " << it->from << " " << it->to << " " << it->subject << " " << it->content << endl;
		for (auto it2 = it->log.cbegin(); it2 != it->log.cend(); ++it2)
			cout << *it2 << endl;
		cout << it->created_time << " " << it->sent_time << endl;
	}
	/*auto m = mail( "mukeran<mukeran1000@qq.com>", "mukeran<mukeran@mukeran.com>", "test", "test");
	m.append_log("");
	m.append_log("dv");
	m.append_log("123");
	m.append_log("asq");
	m.append_log("e2e");
	m.append_log("testtehtufjafhaeifbehafbiaefieafuieahufiheaffheufhuaef");
	m.append_log("dfd");
	fs.save_mail(&m);*/
}
