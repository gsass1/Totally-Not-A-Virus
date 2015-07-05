
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

#include <gdiplus.h>
#pragma comment(lib, "gdiplus.lib")

#include <stdlib.h>
#include <iostream>
#include <string>
#include <locale>
#include <vector>
#include <sstream>
#include <regex>
#include <numeric>
#include <shellapi.h>
#include <functional>
#include <ObjIdl.h>
#include <memory>
#include <Unknwn.h>
#include <mmdeviceapi.h>
#include <functiondiscoverykeys_devpkey.h>

#ifdef _DEBUG
#define VError(msg) MessageBox(NULL, msg, NULL, MB_OK)
#else
#define VError(msg)
#endif