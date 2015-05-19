#pragma once

#include "Command.h"

class Command_ls : public Command
{
public:
	Command_ls();
	~Command_ls();

	bool OnExecute(const std::vector<std::wstring> &args);
	bool ListFiles(const std::wstring& dir, std::wstring& str, int lvl, int maxlvl);
};