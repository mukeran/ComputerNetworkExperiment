
#include "SMTPClient.h"
#define SMTP_BUFSIZE 1024

using namespace std;

bool SMTPClient::SendMail(Mail mail)
{
	char buf[SMTP_BUFSIZE] = { 0 };
	char account[128] = { 0 };
	char password[128] = { 0 };
	char* pcname = "911AIR";

	strcpy_s(account, mail.mail_from);
	strcpy_s(password, mail.password);
	EncodeBase64(mail.mail_from, account);
	EncodeBase64(mail.password, password);

	struct sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(25);

	char smtphost[128] = { 0 };
	int len = strlen(mail.mail_from), i = 0, j;
	for (i = 0; i < len; i++)
	{
		if (mail.mail_from[i] != '@')
			continue;
		else
		{
			smtphost[0] = 's';
			smtphost[1] = 'm';
			smtphost[2] = 't';
			smtphost[3] = 'p';
			smtphost[4] = '.';
			for (i = i + 1, j = 5; i < len; i++, j++)
			{
				smtphost[j] = mail.mail_from[i];
			}
			break;
		}
	}
	printf("%s", smtphost);

	SOCKET sockfd = socket(PF_INET, SOCK_STREAM, 0);
	hostent* phost = gethostbyname(smtphost);
	memcpy(&addr.sin_addr.S_un.S_addr, phost->h_addr_list[0], phost->h_length);

	if ((sockfd = socket(PF_INET, SOCK_STREAM, 0)) < 0)
	{
		printf("SMTP SOCKET ERROR!");
		return false;
	}
	else if (connect(sockfd, (struct sockaddr*)&addr, sizeof(struct sockaddr)) < 0)
	{
		printf("SMTP CONNECT ERROR!");
		return false;
	}
	Ehlo(sockfd, buf, pcname); printf("1\n");

	Auth(sockfd, buf, account, password); printf("2\n");
	MailFrom(sockfd, buf, mail.mail_from); printf("3\n");
	RcptTo(sockfd, buf, mail.mail_to); printf("4\n");
	Data(sockfd, buf, mail.title, mail.content); printf("5\n");
	Quit(sockfd, buf); printf("6\n");
	closesocket(sockfd); printf("7\n");
	return true;
};
void SMTPClient::Ehlo(SOCKET sockfd, char*buf, char* pcname)
{
	DWORD size = 128;

	sprintf_s(buf, SMTP_BUFSIZE, "EHLO  911AIR\r\n");
	send(sockfd, buf, strlen(buf), 0);
	recv(sockfd, buf, SMTP_BUFSIZE, 0);
}
void SMTPClient::Auth(SOCKET sockfd, char*buf, char* account, char* password)
{
	sprintf_s(buf, SMTP_BUFSIZE, "AUTH LOGIN\r\n");//auth login
	send(sockfd, buf, strlen(buf), 0);
	recv(sockfd, buf, SMTP_BUFSIZE, 0);
	sprintf_s(buf, SMTP_BUFSIZE, "%s\r\n", account);
	printf("%s", account);
	send(sockfd, buf, strlen(buf), 0);
	recv(sockfd, buf, SMTP_BUFSIZE, 0);
	sprintf_s(buf, SMTP_BUFSIZE, "%s\r\n", password);
	send(sockfd, buf, strlen(buf), 0);
	recv(sockfd, buf, SMTP_BUFSIZE, 0);
	printf("%s\n", password);
}
void SMTPClient::MailFrom(SOCKET sockfd, char* buf, char* mail_from)
{
	sprintf_s(buf, SMTP_BUFSIZE, "MAIL FROM:<%s>\r\n", mail_from);
	send(sockfd, buf, strlen(buf), 0);
	recv(sockfd, buf, SMTP_BUFSIZE, 0);
}
void SMTPClient::RcptTo(SOCKET sockfd, char* buf, char* mail_to)
{
	sprintf_s(buf, SMTP_BUFSIZE, "RCPT TO:<%s>\r\n", mail_to);
	send(sockfd, buf, strlen(buf), 0);
	recv(sockfd, buf, SMTP_BUFSIZE, 0);
}
void SMTPClient::Data(SOCKET sockfd, char* buf, char* title, char* content)//发送文件标题和内容
{
	sprintf_s(buf, SMTP_BUFSIZE,
		"Subject:%s\r\n%s\r\n.\r\n", title, content);
	send(sockfd, buf, strlen(buf), 0);
	recv(sockfd, buf, SMTP_BUFSIZE, 0);
}
void SMTPClient::Quit(SOCKET sockfd, char* buf)
{
	sprintf_s(buf, SMTP_BUFSIZE, "QUIT\r\n");
	send(sockfd, buf, strlen(buf), 0);
	recv(sockfd, buf, SMTP_BUFSIZE, 0);
	if (strlen(buf) >= 3)
	{
		if (buf[0] == '2' && buf[1] == '5' && buf[2] == '0')
		{
			printf("sucess\n");
		}
	}
}

int main(void)
{
	Mail mail;
	mail.mail_from = "969929268@qq.com";
	mail.password = "uewhidelybmvbcgj";
	mail.mail_to = "969929268@qq.com";
	mail.title = "Hello";
	mail.content = "This is a test mail";
	WSADATA WSAData;
	WSAStartup(MAKEWORD(2, 2), &WSAData);
	SMTPClient test;
	test.SendMail(mail);
	WSACleanup();
	return 0;
}


