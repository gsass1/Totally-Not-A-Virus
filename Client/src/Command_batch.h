#pragma once

#include "Command.h"

class Command_batch : public Command
{
public:
	Command_batch();
	~Command_batch();

	bool OnExecute(const std::vector<std::tstring> &args);
};