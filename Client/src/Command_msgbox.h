#pragma once

#include "Command.h"

class Command_msgbox : public Command
{
public:
	Command_msgbox();
	~Command_msgbox();

	bool OnExecute(const std::vector<std::tstring> &args);
};