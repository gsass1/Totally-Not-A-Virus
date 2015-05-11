#pragma once

#include "Command.h"

class Command_screenshot : public Command
{
public:
	Command_screenshot();
	~Command_screenshot();

	bool OnExecute(const std::vector<std::wstring> &args);
};