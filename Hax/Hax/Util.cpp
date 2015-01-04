#include "stdafx.h"
#include "Util.h"

const char*
Util::memfind(const char *data, const char *find, size_t data_len, size_t find_len)
{
	if (find_len == 0)
		nullptr;

	if (data == nullptr || find == nullptr || find_len > data_len)
		return nullptr;

	for (size_t i = 0; i <= data_len - find_len; i++)
	{
		if (memcmp(data+i, find, find_len) == 0)
		{
			return (data+i);
		}
	}

	return nullptr;
}