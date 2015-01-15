#pragma once

#include "Command.h"

class Command_info : public Command
{
public:
	Command_info();
	~Command_info();

	bool OnExecute(const std::vector<std::tstring> &args);

private:
	void GetInformation(std::tstring& str);
	bool GetOSVersion(std::tstring& str);
	bool GetProcessInfoStr(DWORD processID, std::tstring& str);
	bool EnumerateProcesses(std::tstring& str);
	bool GetHostname(std::tstring& str);
	bool GetTime(std::tstring &str);
	bool GetMemoryStatus(std::tstring& str);
	bool GetUsernameReal(std::tstring& str);
	bool GetUsernameLogin(std::tstring& str);
	bool GetProgramList(std::tstring& str);
	bool GetCPUInfo(std::tstring &str);
	bool GetRAMInfo(std::tstring &str);
	bool GetDisplayDeviceInfo(std::tstring &str);
	bool GetAudioDeviceInfo(std::tstring &str);

	static bool GetCPULoad(std::tstring& str);

	static DWORD dwLastProcessTime, dwLastSystemTime;
	static double dCPULoad;
};