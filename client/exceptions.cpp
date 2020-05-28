//----------------------------------------------------
//
// SA:MP Multiplayer Modification For GTA:SA
// Copyright 2004-2005 SA:MP team
//
// Version: $Id: exceptions.cpp,v 1.11 2006/05/08 17:35:55 kyeman Exp $
//
//----------------------------------------------------

#include "main.h"
#include "resource.h"
#include <Tlhelp32.h>

extern HINSTANCE hInstance;
extern DWORD dwScmOpcodeDebug;
extern bool bScmLocalDebug;
extern GAME_SCRIPT_THREAD* gst;

static PEXCEPTION_POINTERS pExceptionPtrs = NULL;
static char szCrashInfoFile[50] = { 0 };

static void DumpLoadedModules(FILE* f)
{
	HANDLE hModuleSnap;
	MODULEENTRY32 me32;

	hModuleSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, GetCurrentProcessId());

	fputs("\nLoaded Modules:\n", f);
	fflush(f);

	if (hModuleSnap == INVALID_HANDLE_VALUE) {
		fputs("-FailedCreate-\n", f);
		fflush(f);
		return;
	}

	me32.dwSize = sizeof(MODULEENTRY32);

	if (Module32First(hModuleSnap, &me32) == FALSE) {
		fputs("-FailedFirst-\n", f);
		fflush(f);
		CloseHandle(hModuleSnap);
		return;
	}
	
	do {
		fprintf_s(f, "%s\tB: 0x%p\tS: 0x%08X\t(%s)\n",
			me32.szModule, me32.modBaseAddr, me32.modBaseSize, me32.szExePath);
		fflush(f);
	} while (Module32Next(hModuleSnap, &me32));

	CloseHandle(hModuleSnap);
}

static void DumpMemory(FILE* f, BYTE* pData, DWORD dwCount, BOOL bAsDWords = FALSE)
{
	if (bAsDWords) {
		for (DWORD i = 0; i < dwCount; i += 16) {
			fprintf_s(f, "+%04X: 0x%08X 0x%08X 0x%08X 0x%08X\n", i,
				*(DWORD*)(pData + i + 0), *(DWORD*)(pData + i + 4),
				*(DWORD*)(pData + i + 8), *(DWORD*)(pData + i + 12));
			fflush(f);
		}
	} else {
		for (DWORD i = 0; i < dwCount; i += 16) {
			fprintf_s(f, "+%04X: %02X %02X %02X %02X  %02X %02X %02X %02X  "
				"%02X %02X %02X %02X  %02X %02X %02X %02X\n", i,
				pData[i + 0], pData[i + 1], pData[i + 2], pData[i + 3],
				pData[i + 4], pData[i + 5], pData[i + 6], pData[i + 7],
				pData[i + 8], pData[i + 9], pData[i + 10], pData[i + 11],
				pData[i + 12], pData[i + 13], pData[i + 14], pData[i + 15]);
			fflush(f);
		}
	}
}

static void DumpMain()
{
	SYSTEMTIME t;
	FILE* f;
	errno_t err;

	GetLocalTime(&t);
	t.wYear %= 100;
	sprintf_s(szCrashInfoFile, "samp-crashinfo-%02d%02d%02d%02d%02d%02d.txt",
		t.wDay, t.wMonth, t.wYear, t.wHour, t.wMinute, t.wSecond);

	err = fopen_s(&f, szCrashInfoFile, "w");
	if (err == 0) {
		DWORD* pdwStack;
		
		fprintf_s(f, "SA-MP " SAMP_VERSION " (" __DATE__ " " __TIME__ ")\n"
			"Exception At Address: 0x%p\n",
			pExceptionPtrs->ExceptionRecord->ExceptionAddress);
		fflush(f);

		switch (pExceptionPtrs->ExceptionRecord->ExceptionCode) {
		case EXCEPTION_ACCESS_VIOLATION:
			fprintf_s(f, "Exception Code: ACCESS_VIOLATION T: %d A: 0x%08x\n\n",
				pExceptionPtrs->ExceptionRecord->ExceptionInformation[0],
				pExceptionPtrs->ExceptionRecord->ExceptionInformation[1]);
			break;
		case EXCEPTION_IN_PAGE_ERROR:
			fprintf_s(f, "Exception Code: IN_PAGE_ERROR T: %d A: 0x%08x NTSTATUS: 0x%08x\n\n",
				pExceptionPtrs->ExceptionRecord->ExceptionInformation[0],
				pExceptionPtrs->ExceptionRecord->ExceptionInformation[1],
				pExceptionPtrs->ExceptionRecord->ExceptionInformation[2]);
			break;
		case EXCEPTION_ARRAY_BOUNDS_EXCEEDED:
			fputs("Exception Code: ARRAY_BOUNDS_EXCEEDED\n\n", f);
			break;
		case EXCEPTION_BREAKPOINT:
			fputs("Exception Code: BREAKPOINT\n\n", f);
			break;
		case EXCEPTION_DATATYPE_MISALIGNMENT:
			fputs("Exception Code: DATATYPE_MISALIGNMENT\n\n", f);
			break;
		case EXCEPTION_FLT_DENORMAL_OPERAND:
			fputs("Exception Code: FLT_DENORMAL_OPERAND\n\n", f);
			break;
		case EXCEPTION_FLT_DIVIDE_BY_ZERO:
			fputs("Exception Code: FLT_DIVIDE_BY_ZERO\n\n", f);
			break;
		case EXCEPTION_FLT_INEXACT_RESULT:
			fputs("Exception Code: FLT_INEXACT_RESULT\n\n", f);
			break;
		case EXCEPTION_FLT_INVALID_OPERATION:
			fputs("Exception Code: FLT_INVALID_OPERATION\n\n", f);
			break;
		case EXCEPTION_FLT_OVERFLOW:
			fputs("Exception Code: FLT_OVERFLOW\n\n", f);
			break;
		case EXCEPTION_FLT_STACK_CHECK:
			fputs("Exception Code: FLT_STACK_CHECK\n\n", f);
			break;
		case EXCEPTION_FLT_UNDERFLOW:
			fputs("Exception Code: FLT_UNDERFLOW\n\n", f);
			break;
		case EXCEPTION_ILLEGAL_INSTRUCTION:
			fputs("Exception Code: ILLEGAL_INSTRUCTION\n\n", f);
			break;
		case EXCEPTION_INT_DIVIDE_BY_ZERO:
			fputs("Exception Code: INT_DIVIDE_BY_ZERO\n\n", f);
			break;
		case EXCEPTION_INT_OVERFLOW:
			fputs("Exception Code: INT_OVERFLOW\n\n", f);
			break;
		case EXCEPTION_INVALID_DISPOSITION:
			fputs("Exception Code: INVALID_DISPOSITION\n\n", f);
			break;
		case EXCEPTION_NONCONTINUABLE_EXCEPTION:
			fputs("Exception Code: NONCONTINUABLE_EXCEPTION\n\n", f);
			break;
		case EXCEPTION_PRIV_INSTRUCTION:
			fputs("Exception Code: PRIV_INSTRUCTION\n\n", f);
			break;
		case EXCEPTION_SINGLE_STEP:
			fputs("Exception Code: SINGLE_STEP\n\n", f);
			break;
		case EXCEPTION_STACK_OVERFLOW:
			fputs("Exception Code: STACK_OVERFLOW\n\n", f);
			break;
		case DBG_CONTROL_C:
			fputs("Exception Code: DBG_CONTROL_C\n\n", f);
			break;
		}
		fflush(f);

		fprintf_s(f, "Registers:\n"
			"EAX: 0x%08X\tEBX: 0x%08X\tECX: 0x%08X\tEDX: 0x%08X\n"
			"ESI: 0x%08X\tEDI: 0x%08X\tEBP: 0x%08X\tESP: 0x%08X\n"
			"EIP: 0x%08X\tEFS: 0x%p\tEFLAGS: 0x%08X\n\nStack:\n",
			pExceptionPtrs->ContextRecord->Eax, pExceptionPtrs->ContextRecord->Ebx,
			pExceptionPtrs->ContextRecord->Ecx, pExceptionPtrs->ContextRecord->Edx,
			pExceptionPtrs->ContextRecord->Esi, pExceptionPtrs->ContextRecord->Edi,
			pExceptionPtrs->ContextRecord->Ebp, pExceptionPtrs->ContextRecord->Esp,
			pExceptionPtrs->ContextRecord->Eip, hInstance,
			pExceptionPtrs->ContextRecord->EFlags);
		fflush(f);

		pdwStack = (DWORD*)pExceptionPtrs->ContextRecord->Esp;
		DumpMemory(f, reinterpret_cast<BYTE*>(pdwStack), 320, TRUE);

		fprintf_s(f, "\nSCM Op: 0x%X, L: %d, Dump:\n", dwScmOpcodeDebug, bScmLocalDebug);
		if (gst)
			DumpMemory(f, reinterpret_cast<BYTE*>(gst), sizeof(GAME_SCRIPT_THREAD));
		else
			fputs("-Not initialized-\n", f);
		fflush(f);

		if(iGtaVersion == GTASA_VERSION_USA10)
			fputs("\nGame Version: US 1.0\n", f);
		else if (iGtaVersion == GTASA_VERSION_USA10)
			fputs("\nGame Version: EU 1.0\n", f);
		else
			fputs("\nGame Version: UNKNOWN\n", f);
		fflush(f);

		DumpLoadedModules(f);

		fclose(f);
	}
}

static void SetDlgItemTextFromFile(HWND hDlg)
{
	HANDLE hFile = CreateFileA(szCrashInfoFile, GENERIC_READ,
		FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
	if (hFile != INVALID_HANDLE_VALUE) {
		DWORD dwSize = GetFileSize(hFile, NULL);
		if (dwSize != INVALID_FILE_SIZE) {
			char* szOutput = (char*)malloc(dwSize + 1);
			if (szOutput != NULL) {
				if (ReadFile(hFile, szOutput, dwSize, NULL, NULL) == TRUE) {
					szOutput[dwSize] = '\0';
					SetDlgItemText(hDlg, IDC_REPORT_OUTPUT, szOutput);
				}
				free(szOutput);
			}
		}
		CloseHandle(hFile);
	}
}

static INT_PTR CALLBACK GuiDlgProcMain(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg) {
	case WM_MOUSEMOVE:
		ShowCursor(TRUE);
		break;
	case WM_INITDIALOG:
		SetCursor(LoadCursorA(NULL, IDC_ARROW));
		ShowCursor(TRUE);

		DumpMain();

		SetDlgItemTextFromFile(hDlg);
		SetForegroundWindow(GetDlgItem(hDlg, IDD_EXCEPTION));
		SetFocus(GetDlgItem(hDlg, IDCLOSE));
		break;
	case WM_DESTROY:
		EndDialog(hDlg, TRUE);
		break;
	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case IDCLOSE:
			EndDialog(hDlg, TRUE);
			break;
		}
		break;
	}
	return FALSE;
}

LONG WINAPI exc_handler(_EXCEPTION_POINTERS* exc_inf)
{
	pExceptionPtrs = exc_inf;

	if (pGame) {
		HWND hwnd = pGame->GetMainWindowHwnd();
		ShowWindow(hwnd, SW_MINIMIZE);
		DialogBoxA(hInstance, MAKEINTRESOURCE(IDD_EXCEPTION), hwnd, GuiDlgProcMain);
	} else {
		DumpMain();
	}
	return EXCEPTION_CONTINUE_SEARCH;
}
