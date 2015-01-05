
#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <windows.h>

#include <winsock2.h>
#pragma comment(lib, "Ws2_32.lib")

#include <Shlwapi.h>
#pragma comment(lib, "Shlwapi.lib")

#include <stdlib.h>
#include <iostream>
#include <string>
#include <vector>
#include <tchar.h>
#include <sstream>
#include <regex>
#include <numeric>
#include <shellapi.h>
#include <functional>
#include <ObjIdl.h>
#include <memory>
#include <Unknwn.h>
#include <gdiplus.h>