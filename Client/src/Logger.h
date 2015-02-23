#pragma once
#include "stdafx.h"

enum LogLevel {
	LMESSAGE = 0,
	LNOTICE,
	LWARNING,
	LERROR
};

class Logger
{
public:
	Logger();
	~Logger();

	void Log(LogLevel lvl, const std::tstring& txt);
	void Log(LogLevel lvl, const TCHAR* txt);
private:
	
};

extern Logger logger;

#define VLog(lvl, msg) do { logger.Log(lvl, _T(msg)); } while(0)