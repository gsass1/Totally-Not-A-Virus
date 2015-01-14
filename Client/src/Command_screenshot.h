#pragma once

#include "Command.h"

#pragma comment(lib, "gdiplus.lib")

class Command_screenshot : public Command
{
public:
	Command_screenshot();
	~Command_screenshot();

	bool OnExecute(const std::vector<std::tstring> &args);

private:
	void TakeScreenshot(const std::tstring& filepath);
};