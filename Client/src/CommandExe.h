#pragma once
#include "stdafx.h"

class CommandExe
{
public:
	CommandExe();
	~CommandExe();
	
	void JoinThreads();
	void Run(const std::tstring& cmds);

private:
	std::vector<std::thread> cmdThreads;
};