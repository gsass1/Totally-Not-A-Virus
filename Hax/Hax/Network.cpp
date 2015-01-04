#include "stdafx.h"
#include "Network.h"
#include "Hax.h"
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

bool
Network::SendPost(char* buf, size_t buf_len, size_t *response_len, const char** response_start,
	const char* file, const char* msg, size_t msg_len, bool isText)
{
	static const char* request_header1 =
		"POST ";
	static const char* request_header2 =
		" HTTP/1.1\r\n"
		"Host: " V_NET_DOMAIN "\r\n"
		"Content-Type: ";
	static const char* request_type_application =
		"application/x-www-form-urlencoded\r\n"
		"Content-Length: ";
	static const char* request_type_form =
		"multipart/form-data; boundary=gc0p4Jq0M2Yt08jU534c0p\r\n"
		"Content-Length: ";
	static const char* request_br = "\r\n\r\n";
	static const char* request_disp =
		"--gc0p4Jq0M2Yt08jU534c0p\r\n"
		"Content-Disposition: form-data; name=\"s\"; filename=\"1\"\r\n"
		"Content-Type: image/png\r\n\r\n";
	static const char* request_dispend =
		"\r\n--gc0p4Jq0M2Yt08jU534c0p--\r\n";
	
	static const size_t request_header1_len             = strlen(request_header1);
	static const size_t request_header2_len             = strlen(request_header2);
	static const size_t request_type_application_len    = strlen(request_type_application);
	static const size_t request_type_form_len           = strlen(request_type_form);
	static const size_t request_br_len                  = strlen(request_br);
	static const size_t request_disp_len                = strlen(request_disp);
	static const size_t request_dispend_len             = strlen(request_dispend);

	int total_len;
	if (isText)
		total_len = msg_len;
	else
		total_len = msg_len + request_disp_len + request_dispend_len;

	std::string reqLen = std::to_string(total_len);


	SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);

	struct hostent *host;
	host = gethostbyname(V_NET_DOMAIN);

	SOCKADDR_IN sin;
	sin.sin_addr.s_addr = *((unsigned long*)host->h_addr);
	sin.sin_family = AF_INET;
	sin.sin_port = htons(80);

	if (connect(sock, (struct sockaddr *) &sin, sizeof(sin)) < 0)
	{
		Error(_T("connect() failed"));
		return false;
	}
	
	send(sock, request_header1, request_header1_len, 0);
	send(sock, file, strlen(file), 0);
	send(sock, request_header2, request_header2_len, 0);
	if (isText)
		send(sock, request_type_application, request_type_application_len, 0);
	else
		send(sock, request_type_form, request_type_form_len, 0);
	send(sock, reqLen.c_str(), reqLen.size(), 0);
	send(sock, request_br, request_br_len, 0);
	if (!isText)
		send(sock, request_disp, request_disp_len, 0);
	send(sock, msg, msg_len, 0);
	if (!isText)
		send(sock, request_dispend, request_dispend_len, 0);

	if (buf_len == 0 || buf == nullptr || response_start == nullptr)
	{
		while (recv(sock, dummyBuf, sizeof(dummyBuf), 0) > 0) {}
		closesocket(sock);
		return true;
	}

	size_t len = recv(sock, buf, buf_len, 0);
	closesocket(sock);

	if (len <= 0)
	{
		Error(_T("recv <= 0"));
		return false;
	}

	*response_start = Util::memfind(buf, request_br, len, request_br_len);
	*response_start += request_br_len;
	*response_len = len - (*response_start - buf);

	return true;
}