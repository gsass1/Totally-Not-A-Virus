#pragma once
#include "stdafx.h"

namespace Util {
	
	inline const char* memfind(const char *data, const char *find, size_t data_len, size_t find_len)
	{
		if (find_len == 0 || data == nullptr || find == nullptr || find_len > data_len)
			return nullptr;

		for (size_t i = 0; i <= data_len - find_len; i++)
			if (memcmp(data+i, find, find_len) == 0)
				return (data+i);

		return nullptr;
	}

	inline std::vector<std::tstring> split(const std::tstring &s, TCHAR delim) {
		std::tstringstream ss(s);
		std::tstring item;
		std::vector<std::tstring> elems;
		while(std::getline(ss, item, delim)) {
			elems.push_back(item);
		}
		return elems;
	}

	inline std::tstring join_at_index(const std::vector<std::tstring> &v, std::tstring delim, int index = 0) {
		std::tstring out;
		std::for_each(v.begin() + index, v.end(), [&](const std::tstring &s) { out += s + delim; });
		return out;
	}

	inline std::wstring s2ws(const std::string& str)
	{
		typedef std::codecvt_utf8<wchar_t> convert_typeX;
		std::wstring_convert<convert_typeX, wchar_t> converterX;

		return converterX.from_bytes(str);
	}

	inline std::string ws2s(const std::wstring& wstr)
	{
		typedef std::codecvt_utf8<wchar_t> convert_typeX;
		std::wstring_convert<convert_typeX, wchar_t> converterX;

		return converterX.to_bytes(wstr);
	}
	
	inline std::string t2s(const std::tstring& tstr)
	{
#ifdef _UNICODE
		return ws2s(tstr);
#else
		return tstr;
#endif
	}

	inline std::tstring s2t(const std::string& str)
	{
#ifdef _UNICODE
		return s2ws(str);
#else
		return str;
#endif
	}
	
	inline std::wstring t2ws(const std::tstring& tstr)
	{
#ifdef _UNICODE
		return tstr;
#else
		return s2ws(tstr);
#endif
	}

	inline std::tstring ws2t(const std::wstring& wstr)
	{
#ifdef _UNICODE
		return wstr;
#else
		return ws2s(wstr);
#endif
	}
}