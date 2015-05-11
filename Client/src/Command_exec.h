#pragma once

#include "Command.h"

class Command_exec : public Command
{
public:
	Command_exec();
	~Command_exec();

	bool OnExecute(const std::vector<std::wstring> &args);
};