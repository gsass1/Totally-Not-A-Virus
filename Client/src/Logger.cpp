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
	std::string *msg = (std::string *) lpParam;
	network.SendText(V_NET_FILE_DATA, msg->c_str());
	delete msg;
	return 0;
}

void Logger::Send(std::tstring& txt)
{
#ifdef _DEBUG
	MessageBox(NULL, txt.c_str(), NULL, MB_OK);
#endif

	std::string *msg = new std::string("l=");
	msg->append(Util::t2s(txt));

	CreateThread(0, 0xFFFF, ProcLog, msg, 0, 0);
}
void Logger::Send(const TCHAR* txt)
{
	std::tstring txt_str(txt);
	this->Send(txt_str);
}