#include "stdafx.h"
#include "CommandExe.h"
#include "Keylogger.h"
#include "Installer.h"
#include "Network.h"
#include "Screenshot.h"
#include "Settings.h"
#include "Info.h"
#include "Util.h"
#include "Command.h"
#include "CommandMessageBox.h"

struct CommandThreadParams
{
	std::string name;
	std::vector<std::tstring> args;
};

static Command *CreateCommandFromName(const std::string &name)
{
	if(name == "msgbox")
		return new CommandMessageBox();
	else
		return NULL;
}

static DWORD WINAPI ExecuteCommand(LPVOID param)
{
	CommandThreadParams *cmdParams = (CommandThreadParams *)param;

	Command *command = CreateCommandFromName(cmdParams->name);
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
			params->name = args[0];
			params->args = Util::split(Util::join_at_index(args, _T(" "), 1), ' ');

			CreateThread(NULL, 0, ExecuteCommand, params, 0, NULL);
		}
	}
}