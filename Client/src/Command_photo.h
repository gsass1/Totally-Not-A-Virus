#pragma once

#include "Command.h"

class Command_photo : public Command
{
public:
	Command_photo();
	~Command_photo();

	bool OnExecute(const std::vector<std::tstring> &args);
};