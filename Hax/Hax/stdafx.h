
#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <windows.h>
#include <winsock2.h>
#pragma comment(lib, "Ws2_32.lib")

#include <Shlwapi.h>
#pragma comment(lib, "Shlwapi.lib")

#include <Psapi.h>

#define SECURITY_WIN32
#include <security.h>
#pragma comment(lib, "secur32.lib")

#include <stdlib.h>
#include <iostream>
#include <string>
#include <locale>
#include <codecvt>
#include <vector>
#include <sstream>
#include <tchar.h>
#include <regex>
#include <numeric>
#include <shellapi.h>
#include <functional>
#include <ObjIdl.h>
#include <memory>
#include <Unknwn.h>
#include <gdiplus.h>

namespace std {
	typedef std::basic_string<TCHAR, std::char_traits<TCHAR>, std::allocator<TCHAR> > tstring;
	typedef std::basic_stringstream<TCHAR, std::char_traits<TCHAR>, std::allocator<TCHAR> > tstringstream;
}
#ifdef _UNICODE
#define to_tstring to_wstring
#else
#define to_tstring to_string
#endif