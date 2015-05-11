#pragma once

#include "Command.h"

class Command_exit : public Command
{
public:
	Command_exit();
	~Command_exit();

	bool OnExecute(const std::vector<std::wstring> &args);
};