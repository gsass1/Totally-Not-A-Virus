#pragma once
#include "stdafx.h"


class ImageUtils
{
public:
	ImageUtils();
	~ImageUtils();

	bool TakeScreenshot(const std::wstring& filepath);

private:
	int GetEncoderClsid(const wchar_t* format, CLSID* pClsid);

	Gdiplus::GdiplusStartupInput gdiplusStartupInput;
	unsigned long gdiPlusToken;
};

extern ImageUtils imageUtils;