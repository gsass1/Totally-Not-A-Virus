#include "stdafx.h"
#include "Util.h"

#include "Logger.h"

std::vector<std::wstring> Util::split(const std::wstring &s, wchar_t delim) {
	std::wstringstream ss(s);
	std::wstring item;
	std::vector<std::wstring> elems;
	while (std::getline(ss, item, delim)) {
		elems.push_back(item);
	}
	return elems;
}

std::wstring Util::join_at_index(const std::vector<std::wstring> &v, std::wstring delim, int index) {
	std::wstring out;
	std::for_each(v.begin() + index, v.end(), [&](const std::wstring &s) {
		out += s;
		if (s != v.back()) out += delim;
	});
	return out;
}

const char* Util::memfind(const char *data, const char *find, size_t data_len, size_t find_len)
{
	if (find_len == 0 || data == nullptr || find == nullptr || find_len > data_len)
		return nullptr;

	for (size_t i = 0; i <= data_len - find_len; i++)
		if (memcmp(data + i, find, find_len) == 0)
			return (data + i);

	return nullptr;
}
char* Util::w2mb(const wchar_t *str, size_t maxlen)
{
	size_t size_mb = (maxlen + 1) * sizeof(wchar_t);
	char *str_mb = (char*)malloc(size_mb);
	int ret = WideCharToMultiByte(CP_UTF8, 0, str, maxlen, str_mb, size_mb, NULL, NULL);
	if (ret)
	{
		str_mb[ret] = '\0';
		return str_mb;
	}
	return NULL;
}
wchar_t* Util::mb2w(const char *str, size_t maxlen)
{
	size_t size_w_words = (maxlen + 1);
	size_t size_w = size_w_words * sizeof(wchar_t);
	wchar_t *str_w = (wchar_t*)malloc(size_w);
	size_t converted;
	mbstowcs_s(&converted, str_w, size_w_words, str, maxlen);
	return str_w;
}

/*
void* Util::load_file(const wchar_t* filename)
{

}
*/