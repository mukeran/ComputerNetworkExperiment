#include "mail.h"

string mail::to_string() const
{
	return
		"From: " + from + "\r\n" +
		"To: " + to + "\r\n" +
		"Subject: " + subject + "\r\n" +
		"\r\n" + content +
		"\r\n.\r\n";
}
