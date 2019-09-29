/*
		SA:MP Remote Process Anti-Cheat Module
		Copyright (c) 2005-2006 SA:MP Team
*/

#include <windows.h>
#include "detours.h"
#include <list>
#include <algorithm>

#pragma warning( disable : 4305 )

//----------------------------------------------------------

#ifdef _DEBUG
#ifndef NODPF
	inline void __cdecl dpf(const char* fmt, ...)
	{
		char szDebugMsg[1024];

		va_list args;
		va_start(args, fmt);
		vsprintf(szDebugMsg, fmt, args);
		OutputDebugString(szDebugMsg);
		OutputDebugString("\n");
		va_end(args);
	}
#else
#define dpf 
#endif
#else
	//inline void __cdecl dpf(const char* fmt, ...) { }
#define dpf 
#endif

#define DECODE_STRING(str) { for (UINT i=0; i<sizeof(str)-1; i++) { str[i] ^= 0x7351 * i; } }

//----------------------------------------------------------
#pragma warning(disable:4305) // Const Int to Char warning. 
#pragma warning(disable:4309)
#pragma comment(linker, "/SECTION:.shared,RWS")
#pragma data_seg(".shared")
HHOOK hHook				= 0;
DWORD dwProtProcessId	= 0;
#pragma data_seg()
HMODULE hUser32			= 0;
HMODULE hKernel32		= 0;
BOOL bInitProcess		= FALSE;

//----------------------------------------------------------

// A list of hProcesses opened on our process.
typedef std::list<HANDLE> HandleList_t;
HandleList_t *pProcessHandles = NULL;


#ifdef VC60

inline HandleList_t::iterator find(HandleList_t::iterator begin, HandleList_t::iterator end, HANDLE value)
{
	HandleList_t::iterator i;
	for(i=begin; i!=end;i++)
		if (*i == value)
			break;
	return i;
}

#endif

//----------------------------------------------------------

// Slightly encrypted strings for the APIs...
char szGetWindowThreadProcessId[25]	= { 0x47, 0x34, 0xD6, 0xA4, 0x2D, 0xFB, 0x82, 0x58, 0xFF, 0x8D,
										0x42, 0x09, 0xA9, 0x7C, 0x0A, 0xEF, 0x62, 0x0E, 0xD1, 0x66,
										0x27, 0xD6, 0xBF, 0x23, 0x00 };
char szFindWindowA[12]				= { 0x46, 0x38, 0xCC, 0x97, 0x13, 0xFC, 0x88, 0x53, 0xE7, 0xAE,
										0x6B, 0x00 };
char szSetWindowsHookExA[18]		= { 0x53, 0x34, 0xD6, 0xA4, 0x2D, 0xFB, 0x82, 0x58, 0xFF, 0xAA,
										0x62, 0x14, 0xA3, 0x76, 0x2B, 0xC7, 0x51, 0x00 };
char szUnhookWindowsHookEx[20]		= { 0x55, 0x3F, 0xCA, 0x9C, 0x2B, 0xFE, 0xB1, 0x5E, 0xE6, 0xBD,
										0x45, 0x0C, 0xBF, 0x55, 0x01, 0xD0, 0x7B, 0x24, 0xCA, 0x00 };
char szCallNextHookEx[15]			= { 0x43, 0x30, 0xCE, 0x9F, 0x0A, 0xF0, 0x9E, 0x43, 0xC0, 0xB6,
										0x45, 0x10, 0x89, 0x65, 0x00 };
char szOpenProcess[12]				= { 0x4F, 0x21, 0xC7, 0x9D, 0x14, 0xE7, 0x89, 0x54, 0xED, 0xAA,
										0x59, 0x00 };
char szCloseHandle[12]				= { 0x43, 0x3D, 0xCD, 0x80, 0x21, 0xDD, 0x87, 0x59, 0xEC, 0xB5,
										0x4F, 0x00 };
char szWriteProcessMemory[19]		= { 0x57, 0x23, 0xCB, 0x87, 0x21, 0xC5, 0x94, 0x58, 0xEB, 0xBC,
										0x59, 0x08, 0x81, 0x78, 0x03, 0xD0, 0x62, 0x18, 0x00 };
char szCreateRemoteThread[19]		= { 0x43, 0x23, 0xC7, 0x92, 0x30, 0xF0, 0xB4, 0x52, 0xE5, 0xB6,
										0x5E, 0x1E, 0x98, 0x75, 0x1C, 0xDA, 0x71, 0x05, 0x00 };
char szGrandTheftAutoSA[29]			= { 0x47, 0x23, 0xC3, 0x9D, 0x20, 0xB5, 0x92, 0x5F, 0xED, 0xBF,
										0x5E, 0x5B, 0xAD, 0x68, 0x1A, 0xD0, 0x30, 0x32, 0xD3, 0x6D,
										0x74, 0xE4, 0x98, 0x23, 0xEA, 0x8C, 0x5B, 0xF8, 0x00 };
char szGTASanAndreas[17]			= { 0x47, 0x05, 0xE3, 0xC9, 0x64, 0xC6, 0x87, 0x59, 0xA8, 0x98,
										0x44, 0x1F, 0xBE, 0x78, 0x0F, 0xCC, 0x00 };


//----------------------------------------------------------

// API typedefs.
typedef DWORD (WINAPI *def_GetWindowThreadProcessId)(HWND hWnd, LPDWORD lpdwProcessId);
typedef HWND (WINAPI *def_FindWindowA)(LPCSTR lpClassName, LPCSTR lpWindowName);
typedef HHOOK (WINAPI *def_SetWindowsHookExA)(int idHook, HOOKPROC lpfn, HINSTANCE hmod, DWORD dwThreadId);
typedef LRESULT (WINAPI *def_CallNextHookEx)(HHOOK hhk, int nCode, WPARAM wParam, LPARAM lParam);
typedef BOOL (WINAPI *def_UnhookWindowsHookEx)(HHOOK hhk);
typedef HANDLE (WINAPI *def_OpenProcess)(DWORD dwDesiredAccess, BOOL bInheritHandle, DWORD dwProcessId);
typedef BOOL (WINAPI *def_CloseHandle)(HANDLE hObject);
typedef BOOL (WINAPI *def_WriteProcessMemory)(HANDLE hProcess, LPVOID lpBaseAddress, LPCVOID lpBuffer, SIZE_T nSize, SIZE_T * lpNumberOfBytesWritten);
typedef HANDLE (WINAPI *def_CreateRemoteThread)(HANDLE hProcess, LPSECURITY_ATTRIBUTES lpThreadAttributes, SIZE_T dwStackSize, LPTHREAD_START_ROUTINE lpStartAddress, LPVOID lpParameter, DWORD dwCreationFlags, LPDWORD lpThreadId);

//----------------------------------------------------------

// Original pointers to hooked functions.
def_GetWindowThreadProcessId	pfnGetWindowThreadProcessId	= NULL;
def_FindWindowA					pfnFindWindowA				= NULL;
def_SetWindowsHookExA			pfnSetWindowsHookExA		= NULL;
def_CallNextHookEx				pfnCallNextHookEx			= NULL;
def_UnhookWindowsHookEx			pfnUnhookWindowsHookEx		= NULL;
def_OpenProcess					pfnOpenProcess				= NULL;
def_CloseHandle					pfnCloseHandle				= NULL;
def_WriteProcessMemory			pfnWriteProcessMemory		= NULL;
def_CreateRemoteThread			pfnCreateRemoteThread		= NULL;

//----------------------------------------------------------

HANDLE WINAPI hook_OpenProcess(DWORD dwDesiredAccess, BOOL bInheritHandle, DWORD dwProcessId)
{
	//dpf("hook_OpenProcess(0x%x, 0x%x, 0x%x)", dwDesiredAccess, bInheritHandle, dwProcessId);
	HANDLE hReturn = pfnOpenProcess(dwDesiredAccess, bInheritHandle, dwProcessId);
	if (dwProcessId == dwProtProcessId)
	{
		dpf("hook_OpenProcess() - ProcessID Match");
		// This is our protected process... Add the handle to the list.
		if (pProcessHandles)
			pProcessHandles->push_back(hReturn);
		else
			dpf("hook_OpenProcess() - pProcessHandles is NULL");
	}
	return hReturn;
}

//----------------------------------------------------------

BOOL WINAPI hook_CloseHandle(HANDLE hObject)
{
	//dpf("hook_CloseHandle(0x%x)", hObject);
	if (pProcessHandles) {
		HandleList_t::iterator itor;
		itor = find(pProcessHandles->begin(), pProcessHandles->end(), hObject); 
		if (itor != pProcessHandles->end())
		{
			// They're closing one of our handles... Remove the handle from the list.
			dpf("hook_CloseHandle() - Handle to our process!");
			pProcessHandles->remove(hObject);
		}
	} else {
		dpf("hook_CloseHandle() - pProcessHandles is NULL!");
	}
	return pfnCloseHandle(hObject);
}

//----------------------------------------------------------

BOOL WINAPI hook_WriteProcessMemory(HANDLE hProcess, LPVOID lpBaseAddress, LPCVOID lpBuffer, SIZE_T nSize, SIZE_T * lpNumberOfBytesWritten)
{
	dpf("hook_WriteProcessMemory(0x%x, 0x%x, 0x%x, 0x%x, 0x%x)", hProcess, lpBaseAddress, lpBuffer, nSize, lpNumberOfBytesWritten);
	if (pProcessHandles) {
		HandleList_t::iterator itor;
		itor = find(pProcessHandles->begin(), pProcessHandles->end(), hProcess); 
		if (itor != pProcessHandles->end())
		{
			// The bastards are trying to WPM our process! Fake it!
			dpf("hook_WriteProcessMemory() - Writing to our process!");
			if (lpNumberOfBytesWritten != NULL)
				*lpNumberOfBytesWritten = nSize;
			return 1; // Let's pretend it succeeded :)
		}
	} else {
		dpf("hook_WriteProcessMemory() - pProcessHandles is NULL!");
	}
	return pfnWriteProcessMemory(hProcess, lpBaseAddress, lpBuffer, nSize, lpNumberOfBytesWritten);
}

//----------------------------------------------------------

HANDLE WINAPI hook_CreateRemoteThread(HANDLE hProcess, LPSECURITY_ATTRIBUTES lpThreadAttributes, SIZE_T dwStackSize, LPTHREAD_START_ROUTINE lpStartAddress, LPVOID lpParameter, DWORD dwCreationFlags, LPDWORD lpThreadId)
{
	dpf("hook_CreateRemoteThread(0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x)", hProcess, lpThreadAttributes, dwStackSize, lpStartAddress, lpParameter, dwCreationFlags, lpThreadId);
	if (pProcessHandles) {
		HandleList_t::iterator itor;
		itor = find(pProcessHandles->begin(), pProcessHandles->end(), hProcess); 
		if (itor != pProcessHandles->end())
		{
			dpf("hook_CreateRemoteThread() - Threading our process!");
			// Creating a remote thread in our process, eh? Fuck their day up!
			return NULL;
		}
	} else {
		dpf("hook_CreateRemoteThread() - pProcessHandles is NULL!");
	}
	return pfnCreateRemoteThread(hProcess, lpThreadAttributes, dwStackSize, lpStartAddress, lpParameter, dwCreationFlags, lpThreadId);
}

//----------------------------------------------------------

HHOOK WINAPI hook_SetWindowsHookExA(int idHook, HOOKPROC lpfn, HINSTANCE hmod, DWORD dwThreadId)
{
	dpf("hook_SetWindowsHookExA(0x%x, 0x%x, 0x%x, 0x%x)", idHook, lpfn, hmod, dwThreadId);
	DWORD dwGTAThreadId = pfnGetWindowThreadProcessId(pfnFindWindowA(szGrandTheftAutoSA, szGTASanAndreas), NULL);

	if (dwGTAThreadId == NULL)
	{
		dpf("hook_SetWindowsHookExA() - WARNING: The GTASA thread id is NULL.");
	}

	if ((dwThreadId == 0) || (dwThreadId == dwGTAThreadId))
	{
		dpf("hook_SetWindowsHookExA() - Hooking our process/system.");
		// They're setting a hook on GTA_SA, or the whole system! Fuck it up!
		return NULL;
	}
	return pfnSetWindowsHookExA(idHook, lpfn, hmod, dwThreadId);
}

//----------------------------------------------------------

LRESULT CALLBACK HookProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	// This hook is just for getting our DLL injected... Pass it on!
	return pfnCallNextHookEx(hHook, nCode, wParam, lParam);
}

//----------------------------------------------------------

BOOL WINAPI DllMain(HINSTANCE hInstance, DWORD fdwReason, LPVOID)
{
	switch (fdwReason)
	{
		case DLL_PROCESS_ATTACH:
		{
			dpf("DLL_PROCESS_ATTACH ProcessID=0x%x", GetCurrentProcessId());

			// Create our list
			pProcessHandles = new HandleList_t();

			// It should be loaded... but meh.
			hUser32					= LoadLibrary("user32.dll");
			hKernel32				= LoadLibrary("kernel32.dll");

			// Decode the strings
			DECODE_STRING(szGetWindowThreadProcessId);
			DECODE_STRING(szFindWindowA);
			DECODE_STRING(szSetWindowsHookExA);
			DECODE_STRING(szUnhookWindowsHookEx);
			DECODE_STRING(szCallNextHookEx);
			DECODE_STRING(szOpenProcess);
			DECODE_STRING(szCloseHandle);
			DECODE_STRING(szWriteProcessMemory);
			DECODE_STRING(szCreateRemoteThread);
			DECODE_STRING(szGrandTheftAutoSA);
			DECODE_STRING(szGTASanAndreas);

			pfnGetWindowThreadProcessId	= (def_GetWindowThreadProcessId)GetProcAddress(hUser32, szGetWindowThreadProcessId);
			pfnFindWindowA				= (def_FindWindowA)GetProcAddress(hUser32, szFindWindowA);
			pfnSetWindowsHookExA		= (def_SetWindowsHookExA)GetProcAddress(hUser32, szSetWindowsHookExA);
			pfnCallNextHookEx			= (def_CallNextHookEx)GetProcAddress(hUser32, szCallNextHookEx);
			pfnUnhookWindowsHookEx		= (def_UnhookWindowsHookEx)GetProcAddress(hUser32, szUnhookWindowsHookEx);
			pfnOpenProcess				= (def_OpenProcess)DetourFunction(
				(PBYTE)DetourFindFunction("kernel32.dll", szOpenProcess),
				(PBYTE)hook_OpenProcess);
			pfnCloseHandle				= (def_CloseHandle)DetourFunction(
				(PBYTE)DetourFindFunction("kernel32.dll", szCloseHandle),
				(PBYTE)hook_CloseHandle);
			pfnWriteProcessMemory		= (def_WriteProcessMemory)DetourFunction(
				(PBYTE)DetourFindFunction("kernel32.dll", szWriteProcessMemory),
				(PBYTE)hook_WriteProcessMemory);
			pfnCreateRemoteThread		= (def_CreateRemoteThread)DetourFunction(
				(PBYTE)DetourFindFunction("kernel32.dll", szCreateRemoteThread),
				(PBYTE)hook_CreateRemoteThread);

			if (!dwProtProcessId)	// This is the first load, protect this process!
			{
				dpf("bInitProcess = TRUE");
				bInitProcess = TRUE; // We started this shit, we'll end it!
				dwProtProcessId = GetCurrentProcessId();
				hHook = pfnSetWindowsHookExA(WH_GETMESSAGE, HookProc, hInstance, 0);
			}

			pfnSetWindowsHookExA	= (def_SetWindowsHookExA)DetourFunction(
				(PBYTE)DetourFindFunction("user32.dll", szSetWindowsHookExA),
				(PBYTE)hook_SetWindowsHookExA);

			break;
		}
		case DLL_PROCESS_DETACH:
		{
			dpf("DLL_PROCESS_DETACH ProcessID=0x%x", GetCurrentProcessId());

			DetourRemove((PBYTE)pfnSetWindowsHookExA,	(PBYTE)hook_SetWindowsHookExA);
			DetourRemove((PBYTE)pfnCreateRemoteThread,	(PBYTE)hook_CreateRemoteThread);
			DetourRemove((PBYTE)pfnWriteProcessMemory,	(PBYTE)hook_WriteProcessMemory);
			DetourRemove((PBYTE)pfnCloseHandle,			(PBYTE)hook_CloseHandle);
			DetourRemove((PBYTE)pfnOpenProcess,			(PBYTE)hook_OpenProcess);

			if (bInitProcess) {
				// Did we install the windows hook?
				pfnUnhookWindowsHookEx(hHook);
				dpf("bInitProcess = FALSE");
				bInitProcess = FALSE;
			}
			
			FreeLibrary(hKernel32);
			FreeLibrary(hUser32);

			// Delete the list
			delete pProcessHandles;
			pProcessHandles = NULL;

			break;
		}
	}
	return TRUE;
}

//----------------------------------------------------------
// EOF