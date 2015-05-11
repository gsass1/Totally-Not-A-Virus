#include "stdafx.h"
#include "Command_remove.h"

#include "Installer.h"

Command_remove::Command_remove()
{
}

Command_remove::~Command_remove()
{
}

bool Command_remove::OnExecute(const std::vector<std::wstring> &args)
{
	installer.SetAutorun(false);
	return true;
}