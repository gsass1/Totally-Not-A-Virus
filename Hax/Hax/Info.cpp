#include "stdafx.h"
#include "Info.h"
#include <Psapi.h>

Info info;

static void GetProcessInfoStr(DWORD processID, TCHAR *dst, size_t size)
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

	_tcscat_s(dst, size, _T("n:"));
	_tcscat_s(dst, size, processName);

	_tcscat_s(dst, size, _T(";"));

	_tcscat_s(dst, size, _T("i:"));

	TCHAR idBuf[32];
	_stprintf_s(idBuf, 32, _T("%d"), processID);

	_tcscat_s(dst, size, idBuf);
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
			TCHAR infoStr[2048] = { 0 };
			GetProcessInfoStr(processes[i], infoStr, 2048);

			char dest[2048] = { 0 };
			std::wcstombs(dest, infoStr, 2048);
			ret += dest;
		}
	}

	return ret;
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

	ZeroMemory(&osVersionInfo, sizeof(OSVERSIONINFO));
	osVersionInfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);

	// TODO: this function is deprecated since Windows 8?
	GetVersionEx(&osVersionInfo);

	info += "osVer:";
	info += std::to_string(osVersionInfo.dwMajorVersion);
	info += ".";
	info += std::to_string(osVersionInfo.dwMinorVersion);
	info += ".";
	info += std::to_string(osVersionInfo.dwBuildNumber);

	info += "\nprocs:";

	info += EnumerateProcesses();

	return info;
}