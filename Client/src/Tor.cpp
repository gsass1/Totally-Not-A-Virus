#include "stdafx.h"
#include "Tor.h"
#include "resource.h"

#define MINIZ_HEADER_FILE_ONLY
#include "External/miniz.c"

#include <strsafe.h>
#include <TlHelp32.h>

bool SOCKS5SendData(SOCKET fd, void *data, int len)
{
	char *ptr = (char *)data;
	while(len > 0)
	{
		int sent = send(fd, ptr, len, 0);
		if(sent <= 0)
		{
			TCHAR ErrorMessage[256];
			_sntprintf(ErrorMessage, 255, L"send() error: %d", WSAGetLastError());
			VError(ErrorMessage);
			return -1;
			return false;
		}
		ptr += sent;
		len -= sent;
	}

	return true;
}

bool SOCKS5ReceiveData(SOCKET fd, void *data, int len, bool disconnectOk)
{
	char *ptr = (char *)data;
	int total = 0;

	while(len > 0)
	{
		int recvd = recv(fd, ptr, len, 0);
		if(recvd < 0)
		{
			TCHAR ErrorMessage[256];
			_sntprintf(ErrorMessage, 255, L"recv() error: %d", WSAGetLastError());
			VError(ErrorMessage);
			return -1;
		}
		if(recvd == 0)
		{
			if(disconnectOk)
				break;
			VError(L"disconnected");
			return -1;
		}
		ptr += recvd;
		len -= recvd;
		total -= recvd;
	}

	return total;
}

bool SOCKS5Login(SOCKET fd)
{
	SOCKS5IdentificationRequest req;
	SOCKS5IdentificationResponse resp;

	req.Version = 5;
	req.NumberOfMethods = 1;
	req.Methods[0] = 0;

	if(!SOCKS5SendData(fd, &req, 2 + req.NumberOfMethods)) {
		return false;
	}

	if(SOCKS5ReceiveData(fd, &resp, sizeof(resp)) == -1) {
		return false;
	}

	if(resp.Version != 5 || resp.Method == 0xFF) {
		VError(L"SOCKS5 identification failed\n");
		return false;
	}

	return true;
}

bool SOCKS5RequestConnection(SOCKET fd, const SOCKS5Request &req, SOCKS5Response &resp)
{
	memset(&resp, 0, sizeof(resp));

	if(!SOCKS5SendData(fd, (void *)&req, 4)) {
		return false;
	}

	switch(req.AddrType)
	{
		case 1:
		{
			if(!SOCKS5SendData(fd, (void *)&(req.DestAddr.IPv4), sizeof(in_addr)))
				return false;

			break;
		}
		case 3:
		{
			if(!SOCKS5SendData(fd, (void *)&(req.DestAddr.DomainLen), 1))
				return false;

			if(!SOCKS5SendData(fd, (void *)req.DestAddr.Domain, req.DestAddr.DomainLen))
				return false;

			break;
		}

		default:
		{
			/* Uhh */
			VError(L"SOCKS 5 requesting unknown address type\n");
			return false;
		}
	}

	unsigned short port = htons(req.DestPort);
	if(!SOCKS5SendData(fd, &port, 2))
		return false;

	if(SOCKS5ReceiveData(fd, &resp, 4) == -1)
		return false;

	switch(resp.AddrType)
	{
		case 1:
		{
			if(SOCKS5ReceiveData(fd, (void *)&(resp.BindAddr.IPv4), sizeof(in_addr)) == -1)
				return false;

			break;
		}
		case 3:
		{
			if(SOCKS5ReceiveData(fd, (void *)&(resp.BindAddr.DomainLen), 1) == -1)
				return false;

			if(SOCKS5ReceiveData(fd, (void *)resp.BindAddr.Domain, resp.BindAddr.DomainLen) == -1)
				return false;

			break;
		}

		default:
		{
			printf("SOCKS 5 bound to unknown address type");
			return false;
		}
	}

	if(SOCKS5ReceiveData(fd, &port, 2, 0) == -1)
		return false;

	resp.BindPort = ntohs(port);

	return true;
}

bool SOCKS5Connect(SOCKET fd, const char *dst, unsigned short port)
{
	SOCKS5Request req;
	SOCKS5Response resp;

	req.Version = 5;
	req.Cmd = 1;
	req.Reserved = 0;
	req.AddrType = 3;
	strcpy(req.DestAddr.Domain, dst);
	req.DestAddr.DomainLen = strlen(req.DestAddr.Domain);
	req.DestPort = port;

	if(!SOCKS5RequestConnection(fd, req, resp)) {
		return false;
	}

	if(resp.Reply != 0) {
		char ErrorMessage[256];
		_snprintf(ErrorMessage, 256, "SOCKS5 connect failed, error: 0x%02X", resp.Reply);
		WCHAR ErrorMessageW[256];
		mbstowcs(ErrorMessageW, ErrorMessage, 256);
		//VError(ErrorMessageW);
		return false;
	}

	return true;
}

TCHAR *TorDirectory = L"";
static bool TorProcessCreated = false;

static bool IsTorProcessAlreadyRunning()
{
	HANDLE hSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPALL, NULL);
	PROCESSENTRY32 pEntry;
	pEntry.dwSize = sizeof(pEntry);
	BOOL hRes = Process32First(hSnapShot, &pEntry);
	while(hRes)
	{
		if(wcscmp(pEntry.szExeFile, L"tor.exe") == 0)
		{
			return true;
		}
		hRes = Process32Next(hSnapShot, &pEntry);
	}
	CloseHandle(hSnapShot);
	return false;
}

/*	This initializes the TOR subsystem
	First we extract the TOR.zip file from the EXE into a temp directory
	Then we unzip that and start tor.exe
*/
void TOR_Init()
{
	/* Already running? That's good. */
	if(IsTorProcessAlreadyRunning()) {
		return;
	}

	/* Extract TOR zip data */
	HRSRC hResource = FindResource(NULL, MAKEINTRESOURCE(IDR_ZIPDATA1), RT_RCDATA);

	if(!hResource) {
		VError(L"FindResource failed for TOR data!\n");
		return;
	}

	HGLOBAL hResHandle = LoadResource(NULL, hResource);

	if(!hResHandle) {
		VError(L"LoadResource failed for TOR data!\n");
		return;
	}

	DWORD size = SizeofResource(NULL, hResource);

	const char *data = (char *)LockResource(hResHandle);
	if(!data) {
		VError(L"LockResource return NULL for TOR data!\n");
		return;
	}

	TCHAR tempPath[MAX_PATH];
	if(GetTempPath(MAX_PATH, tempPath) == 0) {
		VError(L"GetTempPath failed!");
		goto out_free_resource;
	}

	/* Generate a random path */
	TCHAR *destPath = _wtempnam(NULL, NULL);

	FILE *fp = _wfopen(destPath, L"wb");
	if(!fp) {
		VError(L"_wfopen failed for TOR data!\n");
		goto out_free_resource;
	}

	fwrite(data, size, 1, fp);

	fclose(fp);

	/* Extract */
	mz_zip_archive zipArchive;
	mz_bool status;

	memset(&zipArchive, 0, sizeof(zipArchive));

	char aDestPath[MAX_PATH];

	wcstombs(aDestPath, destPath, MAX_PATH);

	status = mz_zip_reader_init_file(&zipArchive, aDestPath, 0);
	if(!status) {
		VError(L"Zip file is invalid for TOR data!\n");
		goto out_free_resource;
	}

	/* New random path for extracted files*/
	destPath = _wtempnam(NULL, NULL);
	CreateDirectory(destPath, NULL);

	TorDirectory = new TCHAR[MAX_PATH];
	StringCchCopy(TorDirectory, MAX_PATH, destPath);

	char torExePath[MAX_PATH];

	for(mz_uint i = 0; i < mz_zip_reader_get_num_files(&zipArchive); i++) {
		mz_zip_archive_file_stat fileStat;
		if(!mz_zip_reader_file_stat(&zipArchive, i, &fileStat)) {
			VError(L"mz_zip_reader_file_stat() failed!\n");
			goto out_zip_reader_end;
		}

		char extractFilePath[MAX_PATH];
		wcstombs(extractFilePath, destPath, MAX_PATH);

		StringCchCatA(extractFilePath, MAX_PATH, "\\");
		StringCchCatA(extractFilePath, MAX_PATH, fileStat.m_filename);

		if(strcmp(fileStat.m_filename, "tor.exe") == 0) {
			StringCchCopyA(torExePath, MAX_PATH, extractFilePath);
		}

		if(!mz_zip_reader_extract_file_to_file(&zipArchive, fileStat.m_filename, extractFilePath, 0)) {
			VError(L"Failed to extract file!\n");
			goto out_zip_reader_end;
		}
	}

	/* Create the tor process*/
	char extractFilePath[MAX_PATH];
	wcstombs(extractFilePath, destPath, MAX_PATH);

	STARTUPINFOA si;
	PROCESS_INFORMATION pi;

	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);
	ZeroMemory(&pi, sizeof(pi));

	CreateProcessA(torExePath, "", NULL, NULL, FALSE, 0, NULL, extractFilePath, &si, &pi);

	/* Sleep for 10 seconds to let Tor initialize itself */
	Sleep(10000);

	TorProcessCreated = true;

	/* Cleanup */
	DeleteFileA(aDestPath);

out_zip_reader_end:
	mz_zip_reader_end(&zipArchive);
out_free_resource:
	FreeResource(hResource);
}

void TOR_Cleanup()
{
	/* Kill tor.exe, but ONLY if we created it */
	if(!TorProcessCreated) {
		return;
	}

	HANDLE hSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPALL, NULL);
	PROCESSENTRY32 pEntry;
	pEntry.dwSize = sizeof(pEntry);
	BOOL hRes = Process32First(hSnapShot, &pEntry);
	while(hRes)
	{
		if(wcscmp(pEntry.szExeFile, L"tor.exe") == 0)
		{
			HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, 0,
				(DWORD)pEntry.th32ProcessID);
			if(hProcess != NULL)
			{
				TerminateProcess(hProcess, 9);
				CloseHandle(hProcess);
			}
		}
		hRes = Process32Next(hSnapShot, &pEntry);
	}

	CloseHandle(hSnapShot);

	/* Delete tor directory */
	SHFILEOPSTRUCT file_op = {
		NULL,
		FO_DELETE,
		TorDirectory,
		L"",
		FOF_NOCONFIRMATION |
		FOF_NOERRORUI |
		FOF_SILENT,
		false,
		0,
		L"" };
	SHFileOperation(&file_op);
}