#pragma once
#include "stdafx.h"

#pragma comment(lib, "Ws2_32.lib")

class Network
{
public:
	Network();
	~Network();
	
	bool SendPost(char* buf, size_t buf_len, size_t *response_len, const char** start, const char* msg, size_t len, bool isText);

private:
	WSADATA WSAData;

	char dummyBuf[500];
};

extern Network network;