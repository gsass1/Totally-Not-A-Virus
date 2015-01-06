#pragma once
#include "stdafx.h"

class Installer
{
public:
	Installer();
	~Installer();

	bool SetAutorun(bool autorun);

private:
	bool CopyTo(TCHAR* path);
	bool SetAutorunAppdata(bool autorun);
	bool SetAutorunRegistry(bool autorun);

	TCHAR exeName[MAX_PATH];
	TCHAR appDataPath[1024];
	TCHAR registryPath[1024];
};

extern Installer installer;