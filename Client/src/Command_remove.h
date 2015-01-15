#pragma once

#include "Command.h"

class Command_remove : public Command
{
public:
	Command_remove();
	~Command_remove();

	bool OnExecute(const std::vector<std::tstring> &args);
};