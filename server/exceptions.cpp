//----------------------------------------------------
//
// SA:MP Multiplayer Modification For GTA:SA
// Copyright 2004-2005 SA:MP team
//
// Version: $Id: exceptions.cpp,v 1.11 2006/05/08 17:35:55 kyeman Exp $
//
//----------------------------------------------------

#ifdef WIN32

#include "main.h"
#include <Tlhelp32.h>

static const char* GetExpCodeAsStr(int iCode)
{
	switch (iCode)
	{
	case EXCEPTION_ACCESS_VIOLATION: return "EXCEPTION_ACCESS_VIOLATION";
	case EXCEPTION_ARRAY_BOUNDS_EXCEEDED: return "EXCEPTION_ARRAY_BOUNDS_EXCEEDED";
	case EXCEPTION_BREAKPOINT: return "EXCEPTION_BREAKPOINT";
	case EXCEPTION_DATATYPE_MISALIGNMENT: return "EXCEPTION_DATATYPE_MISALIGNMENT";
	case EXCEPTION_FLT_DENORMAL_OPERAND: return "EXCEPTION_FLT_DENORMAL_OPERAND";
	case EXCEPTION_FLT_DIVIDE_BY_ZERO: return "EXCEPTION_FLT_DIVIDE_BY_ZERO";
	case EXCEPTION_FLT_INEXACT_RESULT: return "EXCEPTION_FLT_INEXACT_RESULT";
	case EXCEPTION_FLT_INVALID_OPERATION: return "EXCEPTION_FLT_INVALID_OPERATION";
	case EXCEPTION_FLT_OVERFLOW: return "EXCEPTION_FLT_OVERFLOW";
	case EXCEPTION_FLT_STACK_CHECK: return "EXCEPTION_FLT_STACK_CHECK";
	case EXCEPTION_FLT_UNDERFLOW: return "EXCEPTION_FLT_UNDERFLOW";
	case EXCEPTION_ILLEGAL_INSTRUCTION: return "EXCEPTION_ILLEGAL_INSTRUCTION";
	case EXCEPTION_IN_PAGE_ERROR: return "EXCEPTION_IN_PAGE_ERROR";
	case EXCEPTION_INT_DIVIDE_BY_ZERO: return "EXCEPTION_INT_DIVIDE_BY_ZERO";
	case EXCEPTION_INT_OVERFLOW: return "EXCEPTION_INT_OVERFLOW";
	case EXCEPTION_INVALID_DISPOSITION: return "EXCEPTION_INVALID_DISPOSITION";
	case EXCEPTION_NONCONTINUABLE_EXCEPTION: return "EXCEPTION_NONCONTINUABLE_EXCEPTION";
	case EXCEPTION_PRIV_INSTRUCTION: return "EXCEPTION_PRIV_INSTRUCTION";
	case EXCEPTION_SINGLE_STEP: return "EXCEPTION_SINGLE_STEP";
	case EXCEPTION_STACK_OVERFLOW: return "EXCEPTION_STACK_OVERFLOW";
	}
	return "Unknown";
}

static void DumpCrashInfo(_EXCEPTION_POINTERS* exc_inf)
{
	PEXCEPTION_RECORD pExpRec = exc_inf->ExceptionRecord;
	PCONTEXT pCtx = exc_inf->ContextRecord;

	FILE* f = fopen("crashinfo.txt","a");
	if (f) {
		SYSTEMTIME tm;
		GetSystemTime(&tm);

		fprintf_s(f, "\n-----------------------------------------------\n"
			"Crash report generation date: %02d/%02d/%04d %02d:%02d:%02d\n\n"
			"Exeption Code: %s\nException At Address: 0x%08X\n\n",
			tm.wDay, tm.wMonth, tm.wYear, tm.wHour, tm.wMinute, tm.wSecond,
			GetExpCodeAsStr(pExpRec->ExceptionCode), (DWORD)pExpRec->ExceptionAddress);

		char s[130] = { 0 };
		if (pCtx->ContextFlags & CONTEXT_CONTROL)
			strcat_s(s, "CONTEXT_CONTROL\n");
		if (pCtx->ContextFlags & CONTEXT_INTEGER)
			strcat_s(s, "CONTEXT_INTEGER\n");
		if (pCtx->ContextFlags & CONTEXT_SEGMENTS)
			strcat_s(s, "CONTEXT_SEGMENTS\n");
		if (pCtx->ContextFlags & CONTEXT_FLOATING_POINT)
			strcat_s(s, "CONTEXT_FLOATING_POINT\n");
		if (pCtx->ContextFlags & CONTEXT_DEBUG_REGISTERS)
			strcat_s(s, "CONTEXT_DEBUG_REGISTERS\n");
		if (pCtx->ContextFlags & CONTEXT_EXTENDED_REGISTERS)
			strcat_s(s, "CONTEXT_EXTENDED_REGISTERS\n");

		fprintf_s(f,
			"Context Flags:\n%s\n"
			"Dr0: 0x%08X Dr1: 0x%08X Dr2: 0x%08X\n"
			"Dr3: 0x%08X Dr6: 0x%08X Dr6: 0x%08X\n"
			"SegGs: 0x%08X SegFs: 0x%08X\n"
			"SegEs: 0x%08X SegDs: 0x%08X\n"
			"Edi: 0x%08X Esi: 0x%08X Ebx: 0x%08X\n"
			"Edx: 0x%08X Ecx: 0x%08X Eax: 0x%08X\n"
			"SegCs: 0x%08X Ebp: 0x%08X Esp: 0x%08X\n"
			"SegSs: 0x%08X Eip: 0x%08X EFlags: 0x%08X\n",
			s,
			pCtx->Dr0, pCtx->Dr1, pCtx->Dr2,
			pCtx->Dr3, pCtx->Dr6, pCtx->Dr7,
			pCtx->SegGs, pCtx->SegFs,
			pCtx->SegEs, pCtx->SegDs,
			pCtx->Edi,pCtx->Esi,pCtx->Ebx,
			pCtx->Edx,pCtx->Ecx,pCtx->Eax,
			pCtx->SegCs,pCtx->Ebp,pCtx->Esp,
			pCtx->SegSs,pCtx->Eip,pCtx->EFlags);

		fclose(f);
	}
}

LONG WINAPI exc_handler(_EXCEPTION_POINTERS* exc_inf)
{
	DumpCrashInfo(exc_inf);
	return EXCEPTION_CONTINUE_SEARCH;
}

#endif