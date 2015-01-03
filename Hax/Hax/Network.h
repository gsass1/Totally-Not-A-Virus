#pragma once
#include "stdafx.h"


class Network
{
public:
	Network();
	~Network();
    
    std::string SendPost(const char* msg, size_t len);

private:
    WSADATA WSAData;

    char buf[8192];
    size_t bufLen;
};

extern Network network;