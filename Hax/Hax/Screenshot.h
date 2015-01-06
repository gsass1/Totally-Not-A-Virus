#pragma once
#include "stdafx.h"

#pragma comment(lib, "gdiplus.lib")

using namespace Gdiplus;

class Screenshot
{
public:
	Screenshot();
	~Screenshot();
	
	void TakeScreenshot(const std::tstring& filepath);

private:
	GdiplusStartupInput gdiplusStartupInput;
	ULONG_PTR           gdiplusToken;
};

extern Screenshot screenshot;