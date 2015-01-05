#pragma once

#pragma comment(lib, "secur32.lib")

#include <string>

class Info
{
public:
	Info();
	~Info();

	std::string GetInformation();
};

extern Info info;