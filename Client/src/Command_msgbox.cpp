#include "stdafx.h"
#include "Command_msgbox.h"

#include "Logger.h"
#include "Util.h"

Command_msgbox::Command_msgbox()
{
}

Command_msgbox::~Command_msgbox()
{
}

bool Command_msgbox::OnExecute(const std::vector<std::wstring> &args)
{
	if(!(args.size() > 1))
	{
		VLog(LERROR, L"msgbox: missing argument");
		return false;
	}

	std::wstring text = Util::join_at_index(args, L" ", 1);

	MessageBox(NULL, text.c_str(), NULL, MB_OK);
	return true;
}