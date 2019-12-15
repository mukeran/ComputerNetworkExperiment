// #include "SMTPClient.h"

// #pragma warning(disable: 4996)
// #define SMTP_BUFSIZE 1024

// using namespace std;

// bool SMTPClient::SendMail(Mail mail)
// {
// 	char buf[SMTP_BUFSIZE] = { 0 };
// 	char account[128] = { 0 };

// 	char password[128] = { 0 };
// 	char pcname[] = "911AIR";

// 	strcpy_s(account, mail.mail_from.c_str());
// 	strcpy_s(password, mail.password.c_str());
	

// 	struct sockaddr_in addr;
// 	addr.sin_family = AF_INET;
// 	addr.sin_port = htons(25);

// 	char smtphost[128] = { 0 };


// 	int len = strlen(account), i = 0, j;
// 	for (i = 0; i < len; i++)
// 	{
// 		if (account[i] != '@')
// 			continue;
// 		else
// 		{
// 			smtphost[0] = 's';
// 			smtphost[1] = 'm';
// 			smtphost[2] = 't';
// 			smtphost[3] = 'p';
// 			smtphost[4] = '.';
// 			for (i = i + 1, j = 5; i < len; i++, j++)
// 			{
// 				smtphost[j] = account[i];
				
// 			}
// 			break;
// 		}
// 	}
	
	
// 	printf("\n%s\n", smtphost);

// 	SOCKET sockfd = socket(PF_INET, SOCK_STREAM, 0);
// 	hostent* phost = gethostbyname(smtphost);
// 	memcpy(&addr.sin_addr.S_un.S_addr, phost->h_addr_list[0], phost->h_length);

// 	if ((sockfd = socket(PF_INET, SOCK_STREAM, 0)) < 0)
// 	{
// 		printf("SMTP SOCKET ERROR!");
// 		return false;
// 	}
// 	else if (connect(sockfd, (struct sockaddr*)&addr, sizeof(struct sockaddr)) < 0)
// 	{
// 		printf("SMTP CONNECT ERROR!");
// 		return false;
// 	}
// 	EHLO(sockfd, buf, pcname, mail);
// 	AUTH(sockfd, buf, account, password,mail);
// 	MAILFROM(sockfd, buf, account, mail);
// 	RCPTTO(sockfd, buf, mail.mail_to.c_str(), mail);
// 	DATA(sockfd, buf, mail.title.c_str(), mail.content.c_str(), mail);
// 	QUIT(sockfd, buf, mail);
// 	closesocket(sockfd);
// 	return true;
// };


// void SMTPClient::EHLO(SOCKET sockfd, char*buf, char* pcname,Mail mail)
// {
// 	DWORD size = 128;

// 	sprintf_s(buf, SMTP_BUFSIZE, "EHLO  911AIR\r\n");
// 	send(sockfd, buf, strlen(buf), 0);
// 	recv(sockfd, buf, SMTP_BUFSIZE, 0);

// 	string tmp(buf, 3);
// 	cout << tmp;

// 	mail.statuscode += tmp;
// 	mail.statuscode += '\r\n';

// }
// void SMTPClient::AUTH(SOCKET sockfd, char* buf, char* account, char* password, Mail mail)
// {
// 	char account_buf[128] = { 0 };
// 	char password_buf[128]{ 0 };
// 	EncodeBase64(account, account_buf);
// 	EncodeBase64(password, password_buf);
// 	sprintf_s(buf, SMTP_BUFSIZE, "AUTH LOGIN\r\n");//auth login
// 	send(sockfd, buf, strlen(buf), 0);
// 	recv(sockfd, buf, SMTP_BUFSIZE, 0);
// 	sprintf_s(buf, SMTP_BUFSIZE, "%s\r\n", account_buf);
// 	//printf("%s\n", account);
// 	send(sockfd, buf, strlen(buf), 0);
// 	recv(sockfd, buf, SMTP_BUFSIZE, 0);

// 	sprintf_s(buf, SMTP_BUFSIZE, "%s\r\n", password_buf);
// 	//printf("%s\n", buf);
// 	send(sockfd, buf, strlen(buf), 0);
// 	recv(sockfd, buf, SMTP_BUFSIZE, 0);
// 	//printf("%s\n", buf);
// 	string tmp(buf, 3);
// 	cout << tmp;

// 	mail.statuscode += tmp;
// 	mail.statuscode += '\r\n';
// }
// void SMTPClient::MAILFROM(SOCKET sockfd, char* buf, char* mail_from, Mail mail)
// {
// 	sprintf_s(buf, SMTP_BUFSIZE, "MAIL FROM:<%s>\r\n", mail_from);
// 	send(sockfd, buf, strlen(buf), 0);
// 	recv(sockfd, buf, SMTP_BUFSIZE, 0);
// 	//printf("%s", buf);

// 	string tmp(buf, 3);
// 	cout << tmp;

// 	mail.statuscode += tmp;
// 	mail.statuscode += '\r\n';
// }
// void SMTPClient::RCPTTO(SOCKET sockfd, char* buf, const char* mail_to, Mail mail)
// {
// 	sprintf_s(buf, SMTP_BUFSIZE, "RCPT TO:<%s>\r\n", mail_to);
// 	send(sockfd, buf, strlen(buf), 0);
// 	recv(sockfd, buf, SMTP_BUFSIZE, 0);
// 	//printf("%s", buf);

// 	string tmp(buf, 3);
// 	cout << tmp;

// 	mail.statuscode += tmp;
// 	mail.statuscode += '\r\n';
// }
// void SMTPClient::DATA(SOCKET sockfd, char* buf, const char* title, const char* content, Mail mail)//�����ļ����������
// {

// 	sprintf_s(buf, SMTP_BUFSIZE, "data\r\n");
// 	//printf("%s", buf);
// 	send(sockfd, buf, strlen(buf), 0);
// 	recv(sockfd, buf, SMTP_BUFSIZE, 0);
// 	//printf("%s", buf);
// 	sprintf_s(buf, SMTP_BUFSIZE, "Subject:%s\r\n", title);
// 	send(sockfd, buf, strlen(buf), 0);
// 	//printf("%s\n", buf);
// 	sprintf_s(buf, SMTP_BUFSIZE, "\r\n%s\r\n", content);
// 	send(sockfd, buf, strlen(buf), 0);
// 	//printf("%s\n", buf);
// 	sprintf_s(buf, SMTP_BUFSIZE, "\r\n.\r\n");
// 	send(sockfd, buf, strlen(buf), 0);
// 	recv(sockfd, buf, SMTP_BUFSIZE, 0);

// 	string tmp(buf, 3);
// 	cout << tmp;

// 	mail.statuscode += tmp;
// 	mail.statuscode += '\r\n';

// }
// void SMTPClient::QUIT(SOCKET sockfd, char* buf, Mail mail)
// {
// 	sprintf_s(buf, SMTP_BUFSIZE, "QUIT\r\n");
// 	send(sockfd, buf, strlen(buf), 0);
// 	recv(sockfd, buf, SMTP_BUFSIZE, 0);
// 	printf("%s", buf);

// 	string tmp(buf, 3);
// 	cout << tmp;

// 	mail.statuscode += tmp;
// 	mail.statuscode += '\n';
// 	if (strlen(buf) >= 3)
// 	{
// 		if (buf[0] == '2' && buf[1] == '5' && buf[2] == '0')
// 		{
// 			printf("success\n");
// 		}
// 	}
// }

// int main(void)
// {
// 	Mail mail;
// 	mail.mail_from = std::string("969929268@qq.com");
// 	mail.password = std::string("uewhidelybmvbcgj");
// 	mail.mail_to = std::string("969929268@qq.com");
// 	mail.title = std::string("Hello");
// 	mail.content = std::string("This is a test mail");
	
// 	WSADATA WSAData;
// 	WSAStartup(MAKEWORD(2, 2), &WSAData);
// 	SMTPClient test;
// 	test.SendMail(mail);
// 	WSACleanup();
// 	cout << mail.statuscode << endl;
// 	return 0;
// }


