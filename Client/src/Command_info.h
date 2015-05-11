#pragma once

#include "Command.h"

class Command_info : public Command
{
public:
	Command_info();
	~Command_info();

	bool OnExecute(const std::vector<std::wstring> &args);

private:
	void GetInformation(std::wstring& str);
	bool GetOSVersion(std::wstring& str);
	bool GetProcessInfoStr(DWORD processID, std::wstring& str);
	bool EnumerateProcesses(std::wstring& str);
	bool GetHostname(std::wstring& str);
	bool GetTime(std::wstring &str);
	bool GetMemoryStatus(std::wstring& str);
	bool GetUsernameReal(std::wstring& str);
	bool GetUsernameLogin(std::wstring& str);
	bool GetProgramList(std::wstring& str);
	bool GetCPUInfo(std::wstring &str);
	bool GetRAMInfo(std::wstring &str);
	bool GetDisplayDeviceInfo(std::wstring &str);
	bool GetAudioDeviceInfo(std::wstring &str);

	static bool GetCPULoad(std::wstring& str);

	static DWORD dwLastProcessTime, dwLastSystemTime;
	static double dCPULoad;
};