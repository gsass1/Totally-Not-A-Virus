#include "stdafx.h"
#include "CommandMessageBox.h"
#include "Util.h"

CommandMessageBox::CommandMessageBox()
{
}

CommandMessageBox::~CommandMessageBox()
{
}

bool CommandMessageBox::OnExecute(const std::vector<std::tstring> &args)
{
	if(!args.size())
		return false;

	std::tstring text = Util::join_at_index(args, _T(" "), 1);

	MessageBox(NULL, text.c_str(), NULL, MB_OK);
	return true;
}