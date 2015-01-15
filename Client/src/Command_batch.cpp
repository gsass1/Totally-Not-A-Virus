#include "Command_batch.h"
#include "stdafx.h"

Command_batch::Command_batch()
{
}

Command_batch::~Command_batch()
{
}

bool Command_batch::OnExecute(const std::vector<std::tstring> &args)
{
	if(!(args.size() > 1))
		return false;

	std::tstring fileArgs = Util::join_at_index(args, _T(" "), 1);
	if(fileArgs.length() > _MAX_PATH) {
		Error(_T("batch: Filename too long"));
		return false;
	}

	PROCESS_INFORMATION pi;
	STARTUPINFO si;
	BOOL ret;

	ZeroMemory(&pi, sizeof(pi));
	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);

	// Create a raw copy of fileArgs
	TCHAR fileArgsCopy[_MAX_PATH];
	_tcscpy_s(fileArgsCopy, fileArgs.c_str());

	ret = CreateProcess(NULL, fileArgsCopy, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi);
	if(!ret) {
		Error(_T("batch: Failed to create process"));
		return false;
	}

	return true;
}