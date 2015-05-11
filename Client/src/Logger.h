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

	void Log(LogLevel lvl, const std::wstring& txt);
	void Log(LogLevel lvl, const wchar_t* txt);
private:
	
};

extern Logger logger;

#define VLog(lvl, msg) do { logger.Log(lvl, msg); } while(0)