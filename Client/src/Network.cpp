#include "stdafx.h"
#include "Network.h"
#include "Settings.h"
#include "Util.h"

Network network;

Network::Network()
{
	WSAStartup(MAKEWORD(2, 0), &WSAData);
}
Network::~Network()
{
	WSACleanup();
}

bool Network::Send(const char *req_method, const char *req_url, const char *req_type,
				   size_t req_num_parts, size_t *req_data_len, const char **req_data,
				   size_t *resp_len, char **resp_data)
{
	static const char* headers_proto =
		"%s " V_NET_BASE "%s HTTP/1.1\r\n"
		"Host: " V_NET_DOMAIN "\r\n"
		"Content-Type: %s\r\n"
		"Content-Length: %d\r\n\r\n";

	size_t req_len = 0;
	for (size_t i = 0; i < req_num_parts; i++)
		req_len += req_data_len[i];

	char buf[1024];
	sprintf_s(buf, headers_proto, req_method, req_url, req_type, req_len);

	SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);

	struct hostent *host = gethostbyname(V_NET_DOMAIN);
	if (host == nullptr)
	{
		VError("gethostbyname failed");
		return false;
	}

	SOCKADDR_IN sin;
	sin.sin_addr.s_addr = *((unsigned long*)host->h_addr);
	sin.sin_family = AF_INET;
	sin.sin_port = htons(80);

	if (connect(sock, (struct sockaddr *) &sin, sizeof(sin)) < 0)
	{
		VError("connect() failed");
		return false;
	}
	
	send(sock, buf, strlen(buf), 0);

	for (size_t i = 0; i < req_num_parts; i++)
		send(sock, req_data[i], req_data_len[i], 0);

	if (resp_len == nullptr || resp_data == nullptr)
	{
		recv(sock, buf, sizeof(buf), 0);
		closesocket(sock);
		return true;
	}

	size_t len_first = recv(sock, buf, sizeof(buf), 0);
	if (len_first <= 0)
	{
		VError("recv <= 0");
		return false;
	}

	static const char *header_clen_str = "Content-Length: ";
	static const size_t header_clen_str_len = strlen(header_clen_str);
	const char *clen_str = Util::memfind(buf, header_clen_str, len_first, header_clen_str_len);
	if (!clen_str)
	{
		VError("Can't find Content-Length");
		return false;
	}
	clen_str += header_clen_str_len;

	static const char* header_br = "\r\n\r\n";
	static const size_t header_br_len = strlen(header_br);
	const char *data_begin = Util::memfind(buf, header_br, len_first, header_br_len);
	if (!data_begin)
	{
		VError("Can't find linebreaks before data");
		return false;
	}
	data_begin += header_br_len;

	*resp_len = strtoul(clen_str, nullptr, 10);
	if (*resp_len == ULONG_MAX)
	{
		VError("Can't parse Content-Length");
		return false;
	}
	
	size_t len_header = data_begin - buf;
	size_t len_data_first = len_first - len_header;
	
	*resp_data = (char*)malloc(*resp_len);
	if (!*resp_data)
	{
		VError("malloc() failed");
		return false;
	}
	memcpy(*resp_data, data_begin, len_data_first);

	if (len_first < *resp_len)
	{
		size_t read = len_data_first;
		do
		{
			size_t ret = recv(sock, *resp_data + read, *resp_len - read, 0);
			if (ret < 0)
			{
				VError("recv #2 <= 0");
				return false;
			}
			read += ret;
		} while (read < *resp_len);
	}

	//TODO: clean up on all returns or jump here
	closesocket(sock);
	return true;
}

bool Network::SendFile(const char* req_url, size_t file_size, const char *file)
{
	static const char* req_disp =
		"--gc0p4Jq0M2Yt08jU534c0p\r\n"
		"Content-Disposition: form-data; name=\"s\"; filename=\"1\"\r\n"
		"Content-Type: image/png\r\n\r\n";
	static const size_t req_disp_len = strlen(req_disp);

	static const char* req_dispend =
		"\r\n--gc0p4Jq0M2Yt08jU534c0p--\r\n";
	static const size_t req_dispend_len = strlen(req_dispend);

	size_t req_data_len[3] = {
		req_disp_len,
		file_size,
		req_dispend_len
	};
	const char* req_data[3] = {
		req_disp,
		file,
		req_dispend
	};

	return this->Send(
		"POST", req_url, "multipart/form-data; boundary=gc0p4Jq0M2Yt08jU534c0p",
		3, req_data_len, req_data,
		nullptr, nullptr);
}
bool Network::SendAndGetText(const char* req_url, const char *text, size_t *resp_len, char **resp_data)
{
	size_t req_data_len[1] = {
		strlen(text)
	};
	const char* req_data[1] = {
		text
	};

	return this->Send(
		"POST", req_url, "application/x-www-form-urlencoded",
		1, req_data_len, req_data,
		resp_len, resp_data);
}
bool Network::SendText(const char* req_url, const char *text)
{
	return this->SendAndGetText(req_url, text, nullptr, nullptr);
}
bool Network::GetFile(const char* req_url, size_t *resp_len, char **resp_data)
{
	return this->Send(
		"GET", req_url, "application/x-www-form-urlencoded",
		0, nullptr, nullptr,
		resp_len, resp_data);
}