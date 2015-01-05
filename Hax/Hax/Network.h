#pragma once
#include "stdafx.h"


class Network
{
public:
	Network();
	~Network();
	
	bool Send(const char *req_method, const char *req_file, const char *req_type,
		size_t req_num_parts, size_t *req_data_len, const char **req_data,
		size_t *resp_len, char **resp_data);
	
	bool SendFile(const char* req_url, size_t file_size, const char *file);
	bool SendAndGetText(const char* req_url, const char *text, size_t *resp_len, char **resp_data);
	bool SendText(const char* req_url, const char *text);
	bool GetFile(const char* req_url, size_t *resp_len, char **resp_data);

private:
	WSADATA WSAData;
};

extern Network network;