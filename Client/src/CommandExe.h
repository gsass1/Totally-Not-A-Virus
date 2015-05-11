#pragma once
#include "stdafx.h"

class CommandExe
{
public:
	CommandExe();
	~CommandExe();
	
	void Run(const std::wstring& cmds);
};