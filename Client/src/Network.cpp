#include "stdafx.h"
#include "Network.h"
#include "Settings.h"
#include "Util.h"
#include "Tor.h"

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
		"Host: " V_NET_DOMAIN_ONION "\r\n"
		"Content-Type: %s\r\n"
		"Content-Length: %u\r\n\r\n";

	size_t req_len = 0;
	for (size_t i = 0; i < req_num_parts; i++)
		req_len += req_data_len[i];

	char buf[1024];
	sprintf_s(buf, headers_proto, req_method, req_url, req_type, req_len);

	SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);

	SOCKADDR_IN sin;
	memset(&sin, 0, sizeof(sin));
	sin.sin_family = AF_INET;
	sin.sin_addr.s_addr = inet_addr("127.0.0.1");
	sin.sin_port = htons(9050); // 9050 should be the port for tor

	if (connect(sock, (struct sockaddr *) &sin, sizeof(sin)) < 0)
	{
		VError(L"connect() failed");
		return false;
	}

	if(!SOCKS5Login(sock)) {
		return false;
	}

	if(!SOCKS5Connect(sock, V_NET_DOMAIN_ONION, 80))
		return 1;
	
	send(sock, buf, strlen(buf), 0);

	for (size_t i = 0; i < req_num_parts; i++)
		send(sock, req_data[i], req_data_len[i], 0);

	size_t len_first = recv(sock, buf, sizeof(buf), 0);
	if (len_first == 0 || len_first == SOCKET_ERROR)
	{
		VError(L"recv <= 0");
		closesocket(sock);
		return false;
	}

	if (resp_len == nullptr || resp_data == nullptr)
	{
		closesocket(sock);
		return true;
	}

	static const char *header_clen_str = "Content-Length: ";
	static const size_t header_clen_str_len = strlen(header_clen_str);
	const char *clen_str = Util::memfind(buf, header_clen_str, len_first, header_clen_str_len);
	if (!clen_str)
	{
		VError(L"Can't find Content-Length");
		closesocket(sock);
		return false;
	}
	clen_str += header_clen_str_len;

	static const char* header_br = "\r\n\r\n";
	static const size_t header_br_len = strlen(header_br);
	const char *data_begin = Util::memfind(buf, header_br, len_first, header_br_len);
	if (!data_begin)
	{
		VError(L"Can't find linebreaks before data");
		closesocket(sock);
		return false;
	}
	data_begin += header_br_len;

	*resp_len = strtoul(clen_str, nullptr, 10);
	if (*resp_len == ULONG_MAX)
	{
		VError(L"Can't parse Content-Length");
		closesocket(sock);
		return false;
	}
	
	size_t len_header = data_begin - buf;
	size_t len_data_first = len_first - len_header;
	
	*resp_data = (char*)malloc(*resp_len);
	if (!*resp_data)
	{
		VError(L"malloc() failed");
		closesocket(sock);
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
				VError(L"recv #2 <= 0");
				closesocket(sock);
				free(resp_data);
				return false;
			}
			read += ret;
		} while (read < *resp_len);
	}

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
bool Network::SendAndGetTextA(const char* req_url, const char *text, size_t *resp_len, char **resp_data)
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
bool Network::SendAndGetTextW(const char* req_url, const wchar_t *text, size_t *resp_len, wchar_t **resp_data)
{
	char *text_mb = Util::w2mb(text, wcslen(text));
	size_t resp_len_mb;
	char *resp_data_mb;

	size_t *a;
	char **b;

	if (resp_len == nullptr || resp_data == nullptr) {
		a = nullptr;
		b = nullptr;
	}
	else {
		a = &resp_len_mb;
		b = &resp_data_mb;
	}

	bool ret = this->SendAndGetTextA(req_url, text_mb, a, b);

	free(text_mb);

	if (ret && a && b)
	{
		*resp_data = Util::mb2w(resp_data_mb, resp_len_mb);
		*resp_len = wcslen(*resp_data);
		free(resp_data_mb);
	}
	return ret;
}
bool Network::SendTextA(const char* req_url, const char *text)
{
	return this->SendAndGetTextA(req_url, text, nullptr, nullptr);
}
bool Network::SendTextW(const char* req_url, const wchar_t *text)
{
	char *text_mb = Util::w2mb(text, wcslen(text));
	bool ret = this->SendTextA(req_url, text_mb);
	free(text_mb);
	return ret;
}
bool Network::GetFile(const char* req_url, size_t *resp_len, char **resp_data)
{
	return this->Send(
		"GET", req_url, "application/x-www-form-urlencoded",
		0, nullptr, nullptr,
		resp_len, resp_data);
}