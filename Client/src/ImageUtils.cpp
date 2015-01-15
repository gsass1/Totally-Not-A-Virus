#include "stdafx.h"
#include "ImageUtils.h"
using namespace Gdiplus;

ImageUtils imageUtils;

ImageUtils::ImageUtils()
{
	GdiplusStartup(&gdiPlusToken, &gdiplusStartupInput, NULL);
}

ImageUtils::~ImageUtils()
{
	GdiplusShutdown(gdiPlusToken);
}