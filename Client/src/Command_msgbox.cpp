#include "stdafx.h"
#include "Command_msgbox.h"
#include "Util.h"

Command_msgbox::Command_msgbox()
{
}

Command_msgbox::~Command_msgbox()
{
}

bool Command_msgbox::OnExecute(const std::vector<std::tstring> &args)
{
	if(!(args.size() > 1))
		return false;

	std::tstring text = Util::join_at_index(args, _T(" "), 1);

	MessageBox(NULL, text.c_str(), NULL, MB_OK);
	return true;
}