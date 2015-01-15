#pragma once

#include "stdafx.h"

class Logger
{
public:
	Logger();
	~Logger();

	void Send(std::tstring& txt);
	void Send(const TCHAR* txt);
private:
	
};

extern Logger logger;