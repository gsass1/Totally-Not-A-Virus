#pragma once

#include <string>

class Info
{
public:
	Info();
	~Info();

	std::string GetInformation();
};

extern Info info;