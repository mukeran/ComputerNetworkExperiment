#include "client.h"

#pragma warning(disable: 4996)
#define SMTP_BUFSIZE 1024

using namespace std;

namespace smtp
{
	bool client::send(const mail& mail, const auth& auth) const
	{
		session session(this->address_, this->port_, mail, auth);
		session.ehlo();
		session.auth();
		session.mail_from();
		session.rcpt_to();
		session.data();
		session.quit();
		return true;
	}
}
