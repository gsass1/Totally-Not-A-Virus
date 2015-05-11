#include "Command_batch.h"
#include "stdafx.h"

Command_batch::Command_batch()
{
}

Command_batch::~Command_batch()
{
}

bool Command_batch::OnExecute(const std::vector<std::wstring> &args)
{
	if(!(args.size() > 1))
		return false;

	std::wstring fileArgs = Util::join_at_index(args, L" ", 1);
	if(fileArgs.length() > _MAX_PATH) {
		VError(L"batch: Filename too long");
		return false;
	}

	PROCESS_INFORMATION pi;
	STARTUPINFO si;
	BOOL ret;

	ZeroMemory(&pi, sizeof(pi));
	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);

	// Create a raw copy of fileArgs
	wchar_t fileArgsCopy[_MAX_PATH];
	wcscpy_s(fileArgsCopy, fileArgs.c_str());

	ret = CreateProcess(NULL, fileArgsCopy, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi);
	if(!ret) {
		VError(L"batch: Failed to create process");
		return false;
	}

	return true;
}