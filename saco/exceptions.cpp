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
#include "httpclient.h"
#include "runutil.h"

#include <Tlhelp32.h>

PCONTEXT pContextRecord;
extern	 HANDLE		hInstance;
extern   CGame		*pGame;
extern   CNetGame	*pNetGame;
extern   BOOL		bDisableGTAWndProc;
extern   DWORD		dwScmOpcodeDebug;
extern   BOOL       bScmLocalDebug;
extern	 BYTE		*pbyteCurrentPlayer;//internal GTA FindPlayerPed() number
extern   GAME_SCRIPT_THREAD *gst;
extern   int		iGtaVersion;

CHAR szErrorString[16384];

//----------------------------------------------------

void DumpNetworkStateInformation(PCHAR sz)
{
	CPlayerPool   *pPlayerPool = pNetGame->GetPlayerPool();
	CRemotePlayer *pThisPlayer;
	BYTE bytePlayerId=0;

	char tmp[16384];

	sz[0] = '\0';

	int x=0;
	while(x!=MAX_PLAYERS) {
		if(pPlayerPool->GetSlotState(x)) {
			pThisPlayer = pPlayerPool->GetAt(x);
			bytePlayerId = pThisPlayer->GetID();
		}
		x++;
	}

	sprintf(sz,"\r\nState Information: L: %u  N: %u\r\n",*pbyteCurrentPlayer,bytePlayerId);

	x=0;
	while(x!=MAX_PLAYERS) {
		if(pPlayerPool->GetSlotState(x)) {
			pThisPlayer = pPlayerPool->GetAt(x);
			sprintf(tmp,"P%u (%u,%u) ",x,pThisPlayer->GetState(),pThisPlayer->m_VehicleID);
			strcat(sz,tmp);
			if (x > 0 && ((x+1) % 4) == 0)
				strcat(sz, "\r\n");
		}
		x++;
	}
}

//----------------------------------------------------

void DumpMemory(BYTE *pData, DWORD dwCount, PCHAR sz, BOOL bAsDWords = FALSE)
{
	char s[16384];

	if (bAsDWords)
	{
		for(int i=0; i<(int)dwCount; i += 16)
		{
			sprintf(s, "+%04X: 0x%08X   0x%08X   0x%08X   0x%08X\r\n", i,
					*(DWORD*)(pData+i+0), *(DWORD*)(pData+i+4),
					*(DWORD*)(pData+i+8), *(DWORD*)(pData+i+12)
				);
			strcat(sz,s);
		}
	}
	else
	{
		for(int i=0; i<(int)dwCount; i += 16)
		{
			sprintf(s, "+%04X: %02X %02X %02X %02X   %02X %02X %02X %02X   "
					"%02X %02X %02X %02X   %02X %02X %02X %02X\r\n", i,
					pData[i+0], pData[i+1], pData[i+2], pData[i+3],
					pData[i+4], pData[i+5], pData[i+6], pData[i+7],
					pData[i+8], pData[i+9], pData[i+10], pData[i+11],
					pData[i+12], pData[i+13], pData[i+14], pData[i+15]
				);
			strcat(sz,s);
		}
	}
}

//----------------------------------------------------

void DumpLoadedModules(PCHAR sz)
{
    HANDLE        hModuleSnap = NULL; 
	MODULEENTRY32 me32;
	char s[16384];
	
	hModuleSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, GetCurrentProcessId()); 
	
	strcpy(sz, "\r\nLoaded Modules:\r\n");

	if(hModuleSnap == INVALID_HANDLE_VALUE) 
	{
		strcat(sz, "-FailedCreate-\r\n");
		return;
	}

	me32.dwSize = sizeof( MODULEENTRY32 );

	if( !Module32First( hModuleSnap, &me32 ) )
	{
		strcat(sz, "-FailedFirst-" );  // Show cause of failure
		CloseHandle( hModuleSnap );     // Must clean up the snapshot object!
		return;
	}

	do
	{
		if (me32.szModule[0] != 'f' && me32.szModule[1] != 'l' && me32.szModule[2] != 'a')
		{
			sprintf(s, "%s\tB: 0x%08X\tS: 0x%08X\t(%s)\r\n",
				me32.szModule, me32.modBaseAddr, me32.modBaseSize, me32.szExePath);
			strcat(sz, s);
		}
	} while( Module32Next( hModuleSnap, &me32 ) );

	CloseHandle( hModuleSnap );
	
	return;

}

//----------------------------------------------------

void DumpMain(BOOL bIncModules)
{
	CHAR s[16384];
	DWORD *pdwStack;
	int x=0;

	sprintf(szErrorString,
		"Exception At Address: 0x%08X\r\n\r\n"
		"Registers:\r\n"
		"EAX: 0x%08X\tEBX: 0x%08X\tECX: 0x%08X\tEDX: 0x%08X\r\n"
		"ESI: 0x%08X\tEDI: 0x%08X\tEBP: 0x%08X\tESP: 0x%08X\r\n"
		"EFS: 0x%08X\tEFLAGS: 0x%08X\r\n\r\nStack:\r\n",
		pContextRecord->Eip,
		pContextRecord->Eax,
		pContextRecord->Ebx,
		pContextRecord->Ecx,
		pContextRecord->Edx,
		pContextRecord->Esi,
		pContextRecord->Edi,
		pContextRecord->Ebp,
		pContextRecord->Esp,
		hInstance,
		pContextRecord->EFlags);

	// grab the last 40 stack values
	pdwStack = (DWORD *)pContextRecord->Esp;
	DumpMemory(reinterpret_cast<BYTE*>(pdwStack), 320, szErrorString, TRUE);

	sprintf(s,"\r\nSCM Op: 0x%X, L: %d, Dump:\r\n", dwScmOpcodeDebug, bScmLocalDebug);
	strcat(szErrorString,s);
	DumpMemory(reinterpret_cast<BYTE*>(gst), sizeof(GAME_SCRIPT_THREAD), szErrorString);

	const char *szGameVersion = (iGtaVersion == GTASA_VERSION_USA10) ? "US 1.0" :
								(iGtaVersion == GTASA_VERSION_EU10) ? "EU 1.0" :
								"UNKNOWN";
	sprintf(s, "\r\nGame Version: %s\r\n", szGameVersion);
	strcat(szErrorString,s);

	if(pNetGame) 
	{
		DumpNetworkStateInformation(s);
		strcat(szErrorString,s);
	}

	if (bIncModules)
	{
		DumpLoadedModules(s);
		strcat(szErrorString,s);
	}

}

//----------------------------------------------------

void DoCrashReportingStuff()
{
	CHttpClient pHttp;
	char szURL[256];
	char szBase64[16384];

	DumpMain(TRUE);

	strcpy(szBase64,"data=");

	Util_Base64Encode(szErrorString,&szBase64[5]);
	
	sprintf(szURL,"team.sa-mp.com/report_02X_u1.php?addr=0x%X",pContextRecord->Eip);
	pHttp.ProcessURL(HTTP_POST,szURL,szBase64,"www.sa-mp.com");

	return;
}

//----------------------------------------------------

BOOL CALLBACK GuiDlgProcMain(HWND hDlg,UINT uMsg,WPARAM wParam,LPARAM lParam)
{	
	
	SetCursor(0);
	ShowCursor(TRUE);

	switch(uMsg)
	{
		case WM_MOUSEMOVE:
			ShowCursor(TRUE);
			break;

		case WM_INITDIALOG:		

			DumpMain(FALSE);

			SetDlgItemText(hDlg,IDC_EDIT1,szErrorString);
			SetForegroundWindow(GetDlgItem(hDlg,IDD_DIALOG1));
			SetFocus(GetDlgItem(hDlg,IDC_BUTTON1));
			break;

		case WM_DESTROY:
			EndDialog(hDlg,TRUE);
			//ExitProcess(1);
			break;

		case WM_COMMAND:
			switch(LOWORD(wParam))
			{
			case IDC_BUTTON1:
				EndDialog(hDlg,TRUE);
				//ExitProcess(1);
				break;
			case IDC_BUTTON2:
				DoCrashReportingStuff();
				EnableWindow(GetDlgItem(hDlg,IDC_BUTTON2),FALSE);
				SetDlgItemText(hDlg,IDC_EDIT1,"Thanks for reporting this problem.");
				break;
			}
			break;
	}

	return FALSE;
}

//----------------------------------------------------

LONG WINAPI exc_handler(_EXCEPTION_POINTERS* exc_inf)
{
	pContextRecord = exc_inf->ContextRecord;
	
	ShowWindow(pGame->GetMainWindowHwnd(),SW_MINIMIZE);
	DialogBox((HINSTANCE)hInstance,MAKEINTRESOURCE(IDD_DIALOG1),pGame->GetMainWindowHwnd(),(DLGPROC)GuiDlgProcMain);

//#ifdef _DEBUG
	return EXCEPTION_CONTINUE_SEARCH;
//#else
	//return EXCEPTION_EXECUTE_HANDLER;
//#endif
}

//----------------------------------------------------