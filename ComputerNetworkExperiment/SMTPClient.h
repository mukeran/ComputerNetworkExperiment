#pragma once

using std::string;


struct Mail
{
	string mail_from;
	string password;
	string mail_to;
	string title;
	string content;
	string statuscode;
};

class SMTPClient
{
private:
	SOCKET sockfd;

	struct sockaddr_in addr;
public:
	bool SendMail(Mail mail);
	void EHLO(SOCKET sockfd, char* buf, char* pcname);
	void AUTH(SOCKET sockfd, char* buf, char* account, char* password);
	void MAILFROM(SOCKET sockfd, char* buf, char* mail_from);
	void RCPTTO(SOCKET sockfd, char* buf, const char* mail_to);
	void DATA(SOCKET sockfd, char* buf, const char*title, const char* content);
	void QUIT(SOCKET sockfd, char* buf);
};

void EncodeBase64(const char* src, char* encode)
{
	char base64_table[] = {
		'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P',
		'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f',
		'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v',
		'w', 'x', 'y', 'z', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '+', '/', '=' };
	int len = strlen(src);
	int i = 0;
	for (i = 0; i < len / 3; i++)
	{
		int temp = byte(src[3 * i + 2]) + (byte(src[3 * i + 1]) << 8) + (byte(src[3 * i]) << 16);
		encode[4 * i] = base64_table[(temp & 0xfc0000) >> 18];
		encode[4 * i + 1] = base64_table[(temp & 0x3f000) >> 12];
		encode[4 * i + 2] = base64_table[(temp & 0xfc0) >> 6];
		encode[4 * i + 3] = base64_table[temp & 0x3f];
	}
	encode[4 * i] = 0;
	if (1 == len % 3)
	{
		int temp = byte(src[3 * i]) << 16;
		encode[4 * i] = base64_table[(temp & 0xfc0000) >> 18];
		encode[4 * i + 1] = base64_table[(temp & 0x3f000) >> 12];
		encode[4 * i + 2] = base64_table[64];
		encode[4 * i + 3] = base64_table[64];
		encode[4 * i + 4] = 0;
	}
	else if (2 == len % 3)
	{
		int temp = (byte(src[3 * i + 1]) << 8) + (byte(src[3 * i]) << 16);
		encode[4 * i] = base64_table[(temp & 0xfc0000) >> 18];
		encode[4 * i + 1] = base64_table[(temp & 0x3f000) >> 12];
		encode[4 * i + 2] = base64_table[(temp & 0xfc0) >> 6];
		encode[4 * i + 3] = base64_table[64];
		encode[4 * i + 4] = 0;
	}
}