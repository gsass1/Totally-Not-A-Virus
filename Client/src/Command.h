#pragma once

#include "Util.h"

/* Represents an executable command */
class Command
{
public:
	virtual ~Command() {};

	/* Function to be called when the command is to be executed
	   The return value indicates if the command has executed successfully or not */
	virtual bool OnExecute(const std::vector<std::tstring> &args) = 0;
};
