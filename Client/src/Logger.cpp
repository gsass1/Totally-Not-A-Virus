#include "stdafx.h"
#include "Logger.h"
#include "Settings.h"
#include "Util.h"
#include "Network.h"

Logger logger;

Logger::Logger()
{

}
Logger::~Logger()
{
	
}

DWORD WINAPI ProcLog(LPVOID lpParam)
{
	std::wstring *msg = (std::wstring*) lpParam;
	network.SendTextW(V_NET_FILE_DATA, msg->c_str());
	delete msg;
	return 0;
}

void Logger::Log(LogLevel lvl, const std::wstring& txt)
{
#ifdef _DEBUG
	if (lvl >= LWARNING) {
		VError(txt.c_str());
	}
#endif

	std::wstring *msg = new std::wstring(L"v=");
	msg->append(std::to_wstring(lvl));
	msg->append(L"&l=");
	msg->append(txt);

	CreateThread(0, 0xFFFF, ProcLog, msg, 0, 0);
}
void Logger::Log(LogLevel lvl, const wchar_t* txt)
{
	std::wstring txt_str(txt);
	this->Log(lvl, txt_str);
}