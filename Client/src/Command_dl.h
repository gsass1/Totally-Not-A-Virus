#pragma once

#include "Command.h"

class Command_dl : public Command
{
public:
	Command_dl();
	~Command_dl();

	bool OnExecute(const std::vector<std::wstring> &args);
};