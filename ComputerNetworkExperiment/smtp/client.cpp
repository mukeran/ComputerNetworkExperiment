#include "client.h"

namespace smtp
{
	bool client::send(mail* mail, const auth& auth) const
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
