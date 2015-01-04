#pragma once
#include "stdafx.h"

#pragma comment(lib, "Ws2_32.lib")

class Network
{
public:
	Network();
	~Network();
    
    std::string SendPost(const char* msg, size_t len, bool isText);

private:
    WSADATA WSAData;

    char buf[8192];
    size_t bufLen;
};

extern Network network;