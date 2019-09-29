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
#include "runutil.h"
#include <Tlhelp32.h>

PCONTEXT pContextRecord;
HANDLE	 hInstance;
CHAR	 szErrorString[16384];

//----------------------------------------------------

void DumpCrashInfo(char * szFileName)
{
	int x=0;

	FILE *f = fopen(szFileName,"a");
	if(!f) return; // nothing we can do

	fputs("\r\n--------------------------\r\n",f);

	sprintf(szErrorString,
		"Exception At Address: 0x%08X\r\n\r\n"
		"Registers:\r\n"
		"EAX: 0x%08X\tEBX: 0x%08X\tECX: 0x%08X\tEDX: 0x%08X\r\n"
		"ESI: 0x%08X\tEDI: 0x%08X\tEBP: 0x%08X\tESP: 0x%08X\r\n"
		"EFLAGS: 0x%08X\r\n\r\nStack:\r\n",
		pContextRecord->Eip,
		pContextRecord->Eax,
		pContextRecord->Ebx,
		pContextRecord->Ecx,
		pContextRecord->Edx,
		pContextRecord->Esi,
		pContextRecord->Edi,
		pContextRecord->Ebp,
		pContextRecord->Esp,
		pContextRecord->EFlags);

	fputs(szErrorString,f);
	fclose(f);
}

//----------------------------------------------------

LONG WINAPI exc_handler(_EXCEPTION_POINTERS* exc_inf)
{
	pContextRecord = exc_inf->ContextRecord;
	
	DumpCrashInfo("crashinfo.txt");

	return EXCEPTION_CONTINUE_SEARCH;
}

//----------------------------------------------------

#endif