#include "stdafx.h"
#include "CommandExe.h"
#include "Hax.h"

CommandExe::CommandExe()
{

}
CommandExe::~CommandExe()
{

}
void CommandExe::Run(const char* cmd)
{
    MessageBoxA(NULL, cmd, NULL, MB_OK);
}