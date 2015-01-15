#include "stdafx.h"
#include "Command_remove.h"
#include "Installer.h"

Command_remove::Command_remove()
{
}

Command_remove::~Command_remove()
{
}

bool Command_remove::OnExecute(const std::vector<std::tstring> &args)
{
	installer.SetAutorun(false);
	return true;
}