#pragma once
#include <Windows.h>
#include <iostream>
#include <stdio.h>
#include <WinSock.h>
#pragma comment(lib,"ws2_32.lib")
struct Mail
{
	char* mail_from;
	char* password;
	char* mail_to;
	char* title;
	char* content;

};

class SMTPClient
{
private:
	SOCKET sockfd;

	struct sockaddr_in addr;
public:
	bool SendMail(Mail mail);
	void Ehlo(SOCKET sockfd, char* buf, char* pcname);
	void Auth(SOCKET sockfd, char* buf, char* account, char* password);
	void MailFrom(SOCKET sockfd, char* buf, char* mail_from);
	void RcptTo(SOCKET sockfd, char* buf, char* mail_to);
	void Data(SOCKET sockfd, char* buf, char*title, char* content);
	void Quit(SOCKET sockfd, char* buf);
};

void EncodeBase64(char* src, char* encode)
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

