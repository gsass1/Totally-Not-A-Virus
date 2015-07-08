#pragma once
#include "stdafx.h"

class ImageUtils
{
public:
	ImageUtils();
	~ImageUtils();

	bool TakeScreenshot(unsigned char **buffer, DWORD *size);

private:
	int GetEncoderClsid(const wchar_t* format, CLSID* pClsid);
	bool SaveGdiImageToMemory(unsigned char **memDst, DWORD *size, Gdiplus::Bitmap *bmp);

	Gdiplus::GdiplusStartupInput gdiplusStartupInput;
	unsigned long gdiPlusToken;
};

extern ImageUtils imageUtils;