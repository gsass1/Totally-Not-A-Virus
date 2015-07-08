#include "stdafx.h"
#include "ImageUtils.h"

#include "Util.h"

using namespace Gdiplus;

/* Due to a big clusterfuck with Gdiplus */
bool ImageUtils::SaveGdiImageToMemory(unsigned char **memDst, DWORD *size, Bitmap *bmp)
{
	IStream *pStream;
	LARGE_INTEGER liZero = {};
	ULARGE_INTEGER pos = {};
	STATSTG stg = {};
	ULONG bytesRead = 0;
	HRESULT status = S_OK;
	DWORD bufferSize;

	CLSID pngClsid;
	ImageUtils::GetEncoderClsid(L"image/png", &pngClsid);

	status = CreateStreamOnHGlobal(NULL, TRUE, &pStream);
	status = bmp->Save(pStream, &pngClsid, NULL) == 0 ? S_OK : E_FAIL;
	status = pStream->Seek(liZero, STREAM_SEEK_SET, &pos);
	status = pStream->Stat(&stg, STATFLAG_NONAME);

	*size = bufferSize = stg.cbSize.LowPart;

	*memDst = new unsigned char[bufferSize];
	status = memDst == NULL ? E_OUTOFMEMORY : S_OK;

	status = pStream->Read(*memDst, bufferSize, &bytesRead);

	if(pStream) {
		pStream->Release();
	}

	return true;
}

ImageUtils imageUtils;

ImageUtils::ImageUtils()
{
	GdiplusStartup(&gdiPlusToken, &gdiplusStartupInput, NULL);
}

ImageUtils::~ImageUtils()
{
	GdiplusShutdown(gdiPlusToken);
}

bool ImageUtils::TakeScreenshot(unsigned char **buffer, DWORD *size)
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

	if(!SaveGdiImageToMemory(buffer, size, bmp)) {
		return false;
	}

	delete bmp;
	DeleteObject(hBmp);
	return true;
}

int ImageUtils::GetEncoderClsid(const wchar_t* format, CLSID* pClsid)
{
	UINT num = 0;	// number of image encoders
	UINT size = 0;	// size of the image encoder array in bytes

	ImageCodecInfo* pImageCodecInfo = NULL;

	GetImageEncodersSize(&num, &size);
	if (size == 0)
		return -1;

	pImageCodecInfo = (ImageCodecInfo*)(malloc(size));
	if (pImageCodecInfo == NULL)
		return -1;

	GetImageEncoders(num, size, pImageCodecInfo);

	for (UINT j = 0; j < num; ++j)
	{
		if (wcscmp(pImageCodecInfo[j].MimeType, format) == 0)
		{
			*pClsid = pImageCodecInfo[j].Clsid;
			free(pImageCodecInfo);
			return j;
		}
	}

	free(pImageCodecInfo);
	return -1;
}