#include "Command_dl.h"
#include "stdafx.h"

#include "Logger.h"

Command_dl::Command_dl()
{
}

Command_dl::~Command_dl()
{
}

bool Command_dl::OnExecute(const std::vector<std::wstring> &args)
{
	if (!(args.size() > 1))
		return false;

	std::wstring fileArgs = Util::join_at_index(args, L" ", 1);
	if (fileArgs.length() > _MAX_PATH) {
		VLog(LERROR, L"dl: filename too long");
		return false;
	}

	return true;
}