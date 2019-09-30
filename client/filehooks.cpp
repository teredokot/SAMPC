//----------------------------------------------------------
//
//   SA:MP Multiplayer Modification For GTA:SA
//   Copyright 2004-2006 SA:MP team
//
// Version: $Id: filehooks.cpp,v 1.1 2008-01-07 03:43:03 kyecvs Exp $
//
//----------------------------------------------------------

#include <windows.h>
#include <stdio.h>
#include "detours.h"
#include "filehooks.h"
#include "filesystem.h"
#include "filechecks.h"
#include "outputdebugstring.h"
#include "runutil.h"

//----------------------------------------------------------

// Original procedures
def_GetFileSize Real_GetFileSize = NULL;
def_SetFilePointer Real_SetFilePointer = NULL;
def_CreateFileA Real_CreateFileA = NULL;
def_CreateFileW Real_CreateFileW = NULL;
def_ReadFile Real_ReadFile = NULL;
def_CloseHandle Real_CloseHandle = NULL;
def_GetFileType Real_GetFileType = NULL;
def_GetAsyncKeyState Real_GetAsyncKeyState = NULL;
def_GetModuleHandleA Real_GetModuleHandleA = NULL;

extern CFileSystem *pFileSystem;

ARCH_FILE_RECORD	OpenArchRecords[MAX_OPEN_ARCH_FILES];
BOOL				bArchRecordSlotState[MAX_OPEN_ARCH_FILES];

BOOL bFileHooksInstalled = FALSE;
int iCustomHandle=CUSTOM_HANDLE_BASE;

char * FileNameOnly(char *sz);
char * ExtensionOnly(char *sz);
char* strtolower(char* sz);

//----------------------------------------------------------

BOOL IsCustomFileHandle(DWORD handle)
{
	if(handle >= CUSTOM_HANDLE_BASE && handle < CUSTOM_HANDLE_LIMIT) {
		return TRUE;
	}
	return FALSE;
}

//----------------------------------------------------------

int FindUnusedArchRecordIndex()
{
	int x=0;
	while(x!=MAX_OPEN_ARCH_FILES) {
		if(bArchRecordSlotState[x] == FALSE) {
			return x;
		}
		x++;
	}
	return (-1);
}

//----------------------------------------------------------

int FindArchRecordIndexFromHandle(HANDLE hFile)
{
	int x=0;
	while(x!=MAX_OPEN_ARCH_FILES) {
		if(bArchRecordSlotState[x] == TRUE) {
			if(OpenArchRecords[x].hHandle == hFile) {
				return x;
			}				
		}
		x++;
	}
	return (-1);
}

//----------------------------------------------------------

HANDLE CreateArchRecord(DWORD dwFileIndex)
{
	int iArchRecordIndex = FindUnusedArchRecordIndex();

	if(iArchRecordIndex == (-1)) return 0;
	
	bArchRecordSlotState[iArchRecordIndex] = TRUE;

	OpenArchRecords[iArchRecordIndex].hHandle = (HANDLE)iCustomHandle;
	iCustomHandle+=2;

	OpenArchRecords[iArchRecordIndex].dwReadPosition = 0;
	OpenArchRecords[iArchRecordIndex].dwFileSize = pFileSystem->GetFileSize(dwFileIndex);
	OpenArchRecords[iArchRecordIndex].pbyteDataStart = pFileSystem->GetFileData(dwFileIndex);
	OpenArchRecords[iArchRecordIndex].pbyteDataCurrent = OpenArchRecords[iArchRecordIndex].pbyteDataStart;

	return OpenArchRecords[iArchRecordIndex].hHandle;
}

//----------------------------------------------------------

HANDLE WINAPI Arch_CreateFileA( LPCTSTR lpFileName,DWORD dwDesiredAccess,
							   DWORD dwShareMode,LPSECURITY_ATTRIBUTES lpSecurityAttributes,
							   DWORD dwCreationDisposition,DWORD dwFlagsAndAttributes,
							   HANDLE hTemplateFile )
{

#ifdef _DEBUG
/*
	char szBuffer[FILENAME_MAX];
	sprintf(szBuffer, "CreateFileA: %s\n", lpFileName);
	OutputDebugString(szBuffer);
*/
#endif

	DWORD dwFileIndex = pFileSystem->GetFileIndex(FileNameOnly((PCHAR)lpFileName));

	HANDLE ret=0;

	if(dwFileIndex != FS_INVALID_FILE)
	{
		// The request file is in the archive, so we should return the handle.
		ret = CreateArchRecord(dwFileIndex);
		
		/*
		sprintf(s,"Opening Archive File %s(0x%X:%u)",FileNameOnly((PCHAR)lpFileName),ret,dwFileIndex);
		OutputDebugString(s);
		*/

	}
	else
	{
		// Don't check if it's in the archive
		ret = Real_CreateFileA(lpFileName,dwDesiredAccess,dwShareMode,
			lpSecurityAttributes,dwCreationDisposition,dwFlagsAndAttributes,hTemplateFile);

		if (IsCheckableFile(ExtensionOnly((PCHAR)lpFileName)))
		{
			CheckFileHash(GetFileNameHash(strtolower(FileNameOnly((PCHAR)lpFileName))), ret);
		}
	}
	
	return ret;
}

//----------------------------------------------------------

HANDLE WINAPI Arch_CreateFileW( WORD * lpFileName,DWORD dwDesiredAccess,
							   DWORD dwShareMode,LPSECURITY_ATTRIBUTES lpSecurityAttributes,
							   DWORD dwCreationDisposition,DWORD dwFlagsAndAttributes,
							   HANDLE hTemplateFile )
{

#ifdef _DEBUG
/*
	wchar_t wszBuffer[FILENAME_MAX];
	swprintf(wszBuffer, L"CreateFileW: %s\n", lpFileName);
	OutputDebugStringW(wszBuffer);
*/
#endif

	HANDLE ret=Real_CreateFileW(lpFileName,dwDesiredAccess,dwShareMode,
		lpSecurityAttributes,dwCreationDisposition,dwFlagsAndAttributes,hTemplateFile);
	if (IsCheckableFile(ExtensionOnly((PCHAR)lpFileName)))
	{
		CheckFileHash(GetFileNameHash(strtolower(FileNameOnly((PCHAR)lpFileName))), ret);
	}
	return ret;
}

//----------------------------------------------------------

BOOL WINAPI Arch_ReadFile( HANDLE hFile,LPVOID lpBuffer,
						   DWORD nNumberOfBytesToRead,
						   LPDWORD lpNumberOfBytesRead,
						   LPOVERLAPPED lpOverlapped )
{
	int iArch=0;
	HANDLE hEvent=0;

	if( IsCustomFileHandle((DWORD)hFile) && 
		((iArch = FindArchRecordIndexFromHandle(hFile)) != (-1)))
	{
		//char s[256];
		//sprintf(s,"ReadFile(0x%X)",hFile);
		//OutputDebugString(s);

		DWORD dwFileSize = OpenArchRecords[iArch].dwFileSize;
		DWORD dwNumBytesRead;

		if(lpOverlapped) {
			OpenArchRecords[iArch].dwReadPosition = lpOverlapped->Offset;
			OpenArchRecords[iArch].pbyteDataCurrent =
				OpenArchRecords[iArch].pbyteDataStart + lpOverlapped->Offset;
			hEvent = lpOverlapped->hEvent;
		}

		// First condition: EOF
		if(OpenArchRecords[iArch].dwReadPosition >= dwFileSize)
		{
			if(lpNumberOfBytesRead)	*lpNumberOfBytesRead = 0;
			if(hEvent) SetEvent(hEvent);

			SetLastError(ERROR_HANDLE_EOF);
			return TRUE;
		}

		DWORD dwEndPoint = (nNumberOfBytesToRead + OpenArchRecords[iArch].dwReadPosition);
				
		// Second condition: Read will exceed or match EOF
		if( dwEndPoint >= dwFileSize ) {

			DWORD dwOverHang = dwEndPoint - dwFileSize;
			dwNumBytesRead = nNumberOfBytesToRead - dwOverHang;
	
			memcpy(lpBuffer,OpenArchRecords[iArch].pbyteDataCurrent,dwNumBytesRead);
			OpenArchRecords[iArch].pbyteDataCurrent += dwNumBytesRead;
			OpenArchRecords[iArch].dwReadPosition += dwNumBytesRead;
			
			if(lpNumberOfBytesRead) *lpNumberOfBytesRead = dwNumBytesRead;
			if(hEvent) SetEvent(hEvent);

			SetLastError(ERROR_HANDLE_EOF);

			return TRUE;
		}
		
		// Read will not exceed EOF
		memcpy(lpBuffer,OpenArchRecords[iArch].pbyteDataCurrent,nNumberOfBytesToRead);
		
		dwNumBytesRead = nNumberOfBytesToRead;
		OpenArchRecords[iArch].pbyteDataCurrent += dwNumBytesRead;
		OpenArchRecords[iArch].dwReadPosition += dwNumBytesRead;
		
		if(lpNumberOfBytesRead)	*lpNumberOfBytesRead = dwNumBytesRead;
		if(hEvent) SetEvent(hEvent);

		return TRUE;

	}

	return Real_ReadFile(hFile,lpBuffer,nNumberOfBytesToRead,
		lpNumberOfBytesRead,lpOverlapped);
}

//----------------------------------------------------------

DWORD WINAPI Arch_GetFileSize( HANDLE hFile, PDWORD pdwSize )
{
	int iArch;

	if( IsCustomFileHandle((DWORD)hFile) && 
		((iArch = FindArchRecordIndexFromHandle(hFile)) != (-1)))
	{
		//char s[256];
		//sprintf(s,"GetFileSize(0x%X)",hFile);
		//OutputDebugString(s);

		if (pdwSize)
			*pdwSize = 0;
		return OpenArchRecords[iArch].dwFileSize;
	}
	return Real_GetFileSize(hFile,pdwSize);
}

//----------------------------------------------------------

DWORD WINAPI Arch_SetFilePointer( HANDLE hFile,LONG lDistanceToMove,
								  PLONG lpDistanceToMoveHigh,DWORD dwMoveMethod )
{
	int iArch;

	if( IsCustomFileHandle((DWORD)hFile) && 
		((iArch = FindArchRecordIndexFromHandle(hFile)) != (-1)))
	{
		//char s[256];
		//sprintf(s,"SetFilePointer(0x%X)",hFile);
		//OutputDebugString(s);

		if(dwMoveMethod ==  FILE_BEGIN) {
			OpenArchRecords[iArch].dwReadPosition = lDistanceToMove;
			OpenArchRecords[iArch].pbyteDataCurrent = OpenArchRecords[iArch].pbyteDataStart + lDistanceToMove;
		}
		else if(dwMoveMethod == FILE_CURRENT) {
			OpenArchRecords[iArch].dwReadPosition += lDistanceToMove;
			OpenArchRecords[iArch].pbyteDataCurrent += lDistanceToMove;
		}
		else if(dwMoveMethod == FILE_END) {
			OpenArchRecords[iArch].dwReadPosition = OpenArchRecords[iArch].dwFileSize;
			OpenArchRecords[iArch].pbyteDataCurrent = 
				OpenArchRecords[iArch].pbyteDataStart + OpenArchRecords[iArch].dwFileSize;
		}
		return OpenArchRecords[iArch].dwReadPosition;
	}
	return Real_SetFilePointer(hFile,lDistanceToMove,lpDistanceToMoveHigh,dwMoveMethod);
}

//----------------------------------------------------------

BOOL WINAPI Arch_CloseHandle( HANDLE hObject )
{
	int iArch;

	if( IsCustomFileHandle((DWORD)hObject) && 
		((iArch = FindArchRecordIndexFromHandle(hObject)) != (-1)))
	{
		bArchRecordSlotState[iArch] = FALSE;
		return TRUE;
	}

	return Real_CloseHandle(hObject);
}

//----------------------------------------------------------

DWORD WINAPI Arch_GetFileType( HANDLE hFile )
{
	int iArch;

	if( IsCustomFileHandle((DWORD)hFile) && 
		((iArch = FindArchRecordIndexFromHandle(hFile)) != (-1)))
	{
		return FILE_TYPE_DISK;
	}

	return Real_GetFileType(hFile);
}

//----------------------------------------------------------

DWORD dwCaller=0;
char dbgmsg[256];

SHORT WINAPI Arch_GetAsyncKeyState( int hKey )
{
	_asm mov eax, [esp]
	_asm mov dwCaller, eax

	sprintf(dbgmsg,"GetAsyncKeyState(0x%X)",dwCaller);
	OutputDebugStringA(dbgmsg);

	return Real_GetAsyncKeyState(hKey);
}

//----------------------------------------------------------

HMODULE WINAPI Arch_GetModuleHandleA(LPCTSTR lpszModule)
{
	//sprintf(dbgmsg,"GetModuleHandleA(%s)",lpszModule);
	//OutputDebugString(dbgmsg);

	if( lpszModule && strlen(lpszModule) == 8 && lpszModule[0] == 's' )
	{
		return 0;
	}

    return Real_GetModuleHandleA(lpszModule);
}

//----------------------------------------------------------

void InstallFileSystemHooks()
{
	BYTE szKernel32Enc[13] = {0x6D,0xAC,0x4E,0xCD,0xAC,0x8D,0x66,0x46,0xC5,0x8C,0x8D,0x8D,0}; // kernel32.dll
    char *szKernel32Dec = NULL;

	BYTE szCreateFileAEnc[12] = {0x68,0x4E,0xAC,0x2C,0x8E,0xAC,0xC8,0x2D,0x8D,0xAC,0x28,0}; // CreateFileA
	BYTE szReadFileEnc[9] = {0x4A,0xAC,0x2C,0x8C,0xC8,0x2D,0x8D,0xAC,0}; // ReadFile
	BYTE szGetFileSizeEnc[12] = {0xE8,0xAC,0x8E,0xC8,0x2D,0x8D,0xAC,0x6A,0x2D,0x4F,0xAC,0}; // GetFileSize
	BYTE szSetFilePointerEnc[15] = {0x6A,0xAC,0x8E,0xC8,0x2D,0x8D,0xAC,0xA,0xED,0x2D,0xCD,0x8E,0xAC,0x4E,0}; // SetFilePointer
	BYTE szCloseHandleEnc[12] = {0x68,0x8D,0xED,0x6E,0xAC,0x9,0x2C,0xCD,0x8C,0x8D,0xAC,0}; // CloseHandle
	BYTE szGetFileTypeEnc[12] = {0xE8,0xAC,0x8E,0xC8,0x2D,0x8D,0xAC,0x8A,0x2F,0xE,0xAC,0}; // GetFileType
	BYTE szGetModuleHandleAEnc[17] = {0xE8,0xAC,0x8E,0xA9,0xED,0x8C,0xAE,0x8D,0xAC,0x9,0x2C,0xCD,0x8C,0x8D,0xAC,0x28,0}; // GetModuleHandleA

	if(!bFileHooksInstalled) {
		OutputDebugString("Installing File System Hooks");

		// reset our structures memory
		memset(OpenArchRecords,0,sizeof(ARCH_FILE_RECORD) * MAX_OPEN_ARCH_FILES);
		memset(bArchRecordSlotState,FALSE,sizeof(BOOL) * MAX_OPEN_ARCH_FILES);

		szKernel32Dec = K_DecodeString(szKernel32Enc);

		Real_CreateFileA = (def_CreateFileA)DetourFunction(
			(PBYTE)DetourFindFunction(szKernel32Dec, K_DecodeString(szCreateFileAEnc)),
			(PBYTE)Arch_CreateFileA);

		/*
		Real_CreateFileW = (def_CreateFileW)DetourFunction(
			(PBYTE)DetourFindFunction(szKernel32Dec, "CreateFileW"),
			(PBYTE)Arch_CreateFileW);*/

		Real_ReadFile = (def_ReadFile)DetourFunction(
			(PBYTE)DetourFindFunction(szKernel32Dec, K_DecodeString(szReadFileEnc)),
			(PBYTE)Arch_ReadFile);

		Real_GetFileSize = (def_GetFileSize)DetourFunction(
			(PBYTE)DetourFindFunction(szKernel32Dec, K_DecodeString(szGetFileSizeEnc)),
			(PBYTE)Arch_GetFileSize);

		Real_SetFilePointer = (def_SetFilePointer)DetourFunction(
			(PBYTE)DetourFindFunction(szKernel32Dec, K_DecodeString(szSetFilePointerEnc)),
			(PBYTE)Arch_SetFilePointer);

		Real_CloseHandle = (def_CloseHandle)DetourFunction(
			(PBYTE)DetourFindFunction(szKernel32Dec, K_DecodeString(szCloseHandleEnc)),
			(PBYTE)Arch_CloseHandle);

		Real_GetFileType = (def_GetFileType)DetourFunction(
			(PBYTE)DetourFindFunction(szKernel32Dec, K_DecodeString(szGetFileTypeEnc)),
			(PBYTE)Arch_GetFileType);

		Real_GetModuleHandleA = (def_GetModuleHandleA)DetourFunction(
			(PBYTE)DetourFindFunction(szKernel32Dec, K_DecodeString(szGetModuleHandleAEnc)),
			(PBYTE)Arch_GetModuleHandleA);

		/*
		Real_GetAsyncKeyState = (def_GetAsyncKeyState)DetourFunction(
			(PBYTE)DetourFindFunction("user32.dll", "GetAsyncKeyState"),
			(PBYTE)Arch_GetAsyncKeyState);*/

		bFileHooksInstalled = TRUE;
	}
}

//----------------------------------------------------------

void UninstallFileSystemHooks()
{
	if(bFileHooksInstalled) {
		DetourRemove((PBYTE)Real_CreateFileA,(PBYTE)Arch_CreateFileA);
		DetourRemove((PBYTE)Real_ReadFile,(PBYTE)Arch_ReadFile);
		DetourRemove((PBYTE)Real_GetFileSize,(PBYTE)Arch_GetFileSize);
		DetourRemove((PBYTE)Real_SetFilePointer,(PBYTE)Arch_SetFilePointer);
		DetourRemove((PBYTE)Real_CloseHandle,(PBYTE)Arch_CloseHandle);
		DetourRemove((PBYTE)Real_GetFileType,(PBYTE)Arch_GetFileType);
		DetourRemove((PBYTE)Real_GetModuleHandleA,(PBYTE)Arch_GetModuleHandleA);
		//DetourRemove((PBYTE)Real_GetAsyncKeyState,(PBYTE)Arch_GetAsyncKeyState);
		bFileHooksInstalled = FALSE;
	}
}

//----------------------------------------------------------

char * FileNameOnly(char *sz)
{
	// remove the trailing space, if it's there.
	if(sz[strlen(sz) - 1] == ' ') {
		sz[strlen(sz) - 1] = '\0';
	}

	char * org = sz;
	char * search = sz + strlen(sz);

	while(search != org) {
		if(*search == '/' || *search == '\\') {
			return (search+1);
		}
		search--;
	}
	return org;
}

//----------------------------------------------------------

char * ExtensionOnly(char *sz)
{
	// remove the trailing space, if it's there.
	if(sz[strlen(sz) - 1] == ' ') {
		sz[strlen(sz) - 1] = '\0';
	}

	char * org = sz;
	char * search = sz + strlen(sz);

	while(search != org) {
		if(*search == '.') {
			return (search+1);
		}
		search--;
	}
	return org;
}

//----------------------------------------------------------

char* strtolower(char* sz)
{
	char* ret = sz;
	while (*sz)
	{
		*sz = tolower(*sz);
		sz++;
	}
	return ret;
}

//----------------------------------------------------------
