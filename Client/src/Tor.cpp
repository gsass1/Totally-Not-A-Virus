#include "stdafx.h"
#include "Tor.h"
#include "resource.h"

#define MINIZ_HEADER_FILE_ONLY
#include "External/miniz.c"

#include <strsafe.h>
#include <TlHelp32.h>

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