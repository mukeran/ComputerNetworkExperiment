#include "client.h"

namespace smtp
{
	bool client::send(mail* mail, const auth& auth) const
	{
		logger::info("Sending mail " + mail->uuid);
		mail->status = mail_status::sending;
		file_system::instance->save_mail(mail);
		try
		{
			session session(this->address_, this->port_, mail, auth);
			session.ehlo();
			session.auth();
			session.mail_from();
			session.rcpt_to();
			session.data();
			session.quit();
		}
		catch (std::exception& e)
		{
			mail->status = mail_status::failed;
			file_system::instance->save_mail(mail);
			logger::error("Failed to sent mail. Error message: " + string(e.what()));
			return false;
		}
		mail->sent_time = time(nullptr);
		mail->status = mail_status::success;
		file_system::instance->save_mail(mail);
		logger::info("Successfully sent mail " + mail->uuid);
		return true;
	}
}
