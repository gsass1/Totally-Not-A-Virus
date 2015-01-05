#include "stdafx.h"
#include "Info.h"
#include <Psapi.h>

Info info;

static void GetProcessInfoStr(DWORD processID, TCHAR *dst)
{
	std::string ret;
	TCHAR processName[MAX_PATH] = _T("unknown");
	HANDLE hProcess = OpenProcess(	PROCESS_QUERY_INFORMATION | PROCESS_VM_READ,
									FALSE, processID);

	if(hProcess != NULL) {
		HMODULE hMod;
		DWORD bytesNeeded;

		if(EnumProcessModules(hProcess, &hMod, sizeof(hMod), &bytesNeeded)) {
			GetModuleBaseName(	hProcess, hMod, processName,
								sizeof(processName) / sizeof(TCHAR));
		}
	}

	_tcscat(dst, _T("n:"));
	_tcscat(dst, processName);
	_tcscat(dst, _T("i:"));

	TCHAR idBuf[32];
	_stprintf(idBuf, _T("%d"), processID);

	_tcscat(dst, idBuf);
}

static std::string EnumerateProcesses()
{
	DWORD processes[1024];
	DWORD bytesNeeded;
	DWORD processCount;
	size_t i;
	std::string ret;

	if(!EnumProcesses(processes, sizeof(processes), &bytesNeeded)) {
		return false;
	}

	processCount = bytesNeeded / sizeof(DWORD);

	for(i = 0; i < processCount; i++) {
		if(processes[i] != 0) {
			TCHAR infoStr[512];
			GetProcessInfoStr(processes[i], infoStr);

			char dest[512];
			wcstombs(dest, infoStr, 512);

			ret += dest;
		}
	}
}

Info::Info()
{
}

Info::~Info()
{
}

std::string Info::GetInformation()
{
	std::string info;
	OSVERSIONINFO osVersionInfo;
	TCHAR procInfo[16384];

	GetVersionEx(&osVersionInfo);

	info += "osVer:";
	info += osVersionInfo.dwMajorVersion;
	info += ".";
	info += osVersionInfo.dwMinorVersion;
	info += ".";
	info += osVersionInfo.dwBuildNumber;

	info += EnumerateProcesses();

	return info;
}