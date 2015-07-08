#pragma once

#include <winsock2.h>
#include <tchar.h>

struct SOCKS5IdentificationRequest
{
	unsigned char Version;
	unsigned char NumberOfMethods;
	unsigned char Methods[256];
};

struct SOCKS5IdentificationResponse
{
	unsigned char Version;
	unsigned char Method;
};

struct SOCKS5Request
{
	unsigned char Version;
	unsigned char Cmd;
	unsigned char Reserved;
	unsigned char AddrType;
	union {
		in_addr IPv4;
		//in6_addr IPv6;
		struct {
			unsigned char DomainLen;
			char Domain[256];
		};
	} DestAddr;
	unsigned short DestPort;
};

struct SOCKS5Response
{
	unsigned char Version;
	unsigned char Reply;
	unsigned char Reserved;
	unsigned char AddrType;
	union {
		in_addr IPv4;
		//in6_addr IPv6;
		struct {
			unsigned char DomainLen;
			char Domain[256];
		};
	} BindAddr;
	unsigned short BindPort;
};

bool SOCKS5SendData(SOCKET fd, void *data, int len);
bool SOCKS5ReceiveData(SOCKET fd, void *data, int len, bool disconnectOk = false);
bool SOCKS5Login(SOCKET fd);
bool SOCKS5RequestConnection(SOCKET fd, const SOCKS5Request &req, SOCKS5Response &resp);
bool SOCKS5Connect(SOCKET fd, const char *dst, unsigned short port);

extern TCHAR TorDirectory[MAX_PATH];

void TOR_Init();

void TOR_Cleanup();