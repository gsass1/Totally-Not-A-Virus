#pragma once
#include "stdafx.h"

namespace Util {
	
	const char* memfind(const char *data, const char *find, size_t data_len, size_t find_len);
	std::vector<std::wstring> split(const std::wstring &s, wchar_t delim);
	std::wstring join_at_index(const std::vector<std::wstring> &v, std::wstring delim, int index = 0);
	char* w2mb(const wchar_t *str, size_t maxlen);
	wchar_t* mb2w(const char *str, size_t maxlen);
}