#pragma once
#include "stdafx.h"


class Info
{
public:
	Info();
	~Info();

	void GetInformation(std::tstring& str);

private:
	bool GetOSVersion(std::tstring& str);
	bool GetProcessInfoStr(DWORD processID, std::tstring& str);
	bool EnumerateProcesses(std::tstring& str);
	bool GetHostname(std::tstring& str);
	bool GetTime(std::tstring &str);
	bool GetMemoryStatus(std::tstring& str);
	bool GetCPULoad(std::tstring& str);
	bool GetUsernameReal(std::tstring& str);
	bool GetUsernameLogin(std::tstring& str);
	bool GetProgramList(std::tstring& str);
	bool GetCPUInfo(std::tstring &str);
	bool GetRAMInfo(std::tstring &str);
	bool GetDisplayDeviceInfo(std::tstring &str);

	// See: GetCPULoad
	DWORD dwLastProcessTime;
	DWORD dwLastSystemTime;
	double dCPULoad;
};

extern Info info;