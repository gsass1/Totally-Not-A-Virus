#pragma once
#include "stdafx.h"


class Info
{
public:
	Info();
	~Info();

	void GetInformation(std::tstring& str);
};

extern Info info;