#pragma once
#include "stdafx.h"

namespace Util {

	inline std::vector<std::string> split(const std::string &s, char delim) {
		std::stringstream ss(s);
		std::string item;
		std::vector<std::string> elems;
		while(std::getline(ss, item, delim)) {
			elems.push_back(item);
		}
		return elems;
	}

	inline std::string join_at_index(const std::vector<std::string> &v, std::string delim, int index = 0) {
		std::string out;
		std::for_each(v.begin() + index, v.end(), [&](const std::string &s) { out += s + delim; });
		return out;
	}

const char* memfind(const char *data, const char *find, size_t data_len, size_t find_len);

}