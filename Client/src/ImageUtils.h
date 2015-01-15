#pragma once

#include <gdiplus.h>

class ImageUtils
{
public:
	ImageUtils();
	~ImageUtils();

private:
	Gdiplus::GdiplusStartupInput gdiplusStartupInput;
	unsigned long gdiPlusToken;
};

extern ImageUtils imageUtils;