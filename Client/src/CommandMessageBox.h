#pragma once

#include "Command.h"

class CommandMessageBox : public Command
{
public:
	CommandMessageBox();
	~CommandMessageBox();

	bool OnExecute(const std::vector<std::tstring> &args);
};