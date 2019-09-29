//----------------------------------------------------------
//
// SA:MP Multiplayer Modification For GTA:SA
// Copyright 2004-2005 SA:MP team
//
// Version: $Id: cheats.cpp,v 1.4 2006/05/09 12:41:36 kyeman Exp $
//
//----------------------------------------------------------

#include "../main.h"
#include "util.h"
#include "../runutil.h"

#include <process.h>
#include <Tlhelp32.h>

#define WEAPON_DATA_SIZE (28*13)

int iMoneyStored;
BYTE byteWeaponData[WEAPON_DATA_SIZE];
DWORD dwHealth;
DWORD dwArmour;
MATRIX4X4 matStore;

DWORD dwCamTargetEnt1;
DWORD dwCamTargetEnt2;

extern CGame *pGame;
extern CChatWindow *pChatWindow;

int iTimesDataModified = 0;
extern BOOL bFirstSpawn;

void UnFuck(DWORD,int);

extern CChatWindow *pChatWindow;
extern CNetGame *pNetGame;

int iDupeD3D9DllsWasVerified=0;

DWORD dwD3D9DllBaseAddr=0;
DWORD dwD3D9DllSize=0;

typedef HANDLE (WINAPI *CREATETOOLHELP32SNAPSHOT)(DWORD, DWORD);
typedef BOOL (WINAPI *CLOSETOOLHELP32SNAPSHOT)(HANDLE);
typedef BOOL (WINAPI *MODULE32FIRST)(HANDLE, LPMODULEENTRY32);
typedef BOOL (WINAPI *MODULE32NEXT)(HANDLE, LPMODULEENTRY32);

//----------------------------------------------------------

void DoCheatDataStoring()
{
	if(!pNetGame) return;

	PED_TYPE *pPed = pGame->FindPlayerPed()->m_pPed;

	// Store the current money
	iMoneyStored = pGame->GetLocalMoney();
	_asm mov edx, iMoneyStored
	_asm ror edx, 5
	_asm mov iMoneyStored, edx

	// Store the weapon data
	memcpy(byteWeaponData,&pPed->WeaponSlots[0],WEAPON_DATA_SIZE);

	// Store the health and armour
	memcpy(&dwHealth,&pPed->fHealth,4);
	_asm mov edx, dwHealth
	_asm ror edx, 5
	_asm mov dwHealth, edx

	memcpy(&dwArmour,&pPed->fArmour,4);
	_asm mov edx, dwArmour
	_asm ror edx, 5
	_asm mov dwArmour, edx

	// Store the player matrix
	memcpy(&matStore,pPed->entity.mat,sizeof(MATRIX4X4));

	// Fuck with the Camera's target ent since
	// cheats use it to obtain the player addr.
	dwCamTargetEnt1 = *(PDWORD)0xB6F3B8;
	*(PDWORD)0xB6F3B8 = 0;

	dwCamTargetEnt2 = *(PDWORD)0xB6F5F0;
	*(PDWORD)0xB6F5F0 = 0;
}

//----------------------------------------------------------

void CheckDuplicateD3D9Dlls()
{
	CREATETOOLHELP32SNAPSHOT fnCreateToolhelp32Snapshot = NULL;
	MODULE32FIRST fnModule32First = NULL;
	MODULE32NEXT fnModule32Next = NULL;
    HANDLE hModuleSnap = NULL;
	MODULEENTRY32 me32;

	int iFoundD3D9Dll=0;
	char szModule[MAX_PATH+1];

	DWORD dwCurImageSize=0;
	DWORD dwCurDllBase=0;

	BYTE szD3D9Enc[9] = {0x8C,0x66,0x8C,0x27,0xC5,0x8C,0x8D,0x8D,0}; // d3d9.dll
	char *szD3D9Dec = K_DecodeString(szD3D9Enc);

	BYTE szKernel32Enc[13] = {0x6D,0xAC,0x4E,0xCD,0xAC,0x8D,0x66,0x46,0xC5,0x8C,0x8D,0x8D,0}; // kernel32.dll
    char *szKernel32Dec = K_DecodeString(szKernel32Enc);

    BYTE szCreateToolhelp32SnapshotEnc[25] = {0x68,0x4E,0xAC,0x2C,0x8E,0xAC,0x8A,0xED,0xED,0x8D,0xD,0xAC,0x8D,0xE,0x66,0x46,0x6A,0xCD,0x2C,0xE,0x6E,0xD,0xED,0x8E,0}; // CreateToolhelp32Snapshot
	BYTE szModule32FirstEnc[14] = {0xA9,0xED,0x8C,0xAE,0x8D,0xAC,0x66,0x46,0xC8,0x2D,0x4E,0x6E,0x8E,0}; // Module32First
	BYTE szModule32NextEnc[13] = {0xA9,0xED,0x8C,0xAE,0x8D,0xAC,0x66,0x46,0xC9,0xAC,0xF,0x8E,0}; // Module32Next

	HMODULE kernel32 = GetModuleHandle(szKernel32Dec);

	fnCreateToolhelp32Snapshot = (CREATETOOLHELP32SNAPSHOT)GetProcAddress(kernel32,K_DecodeString(szCreateToolhelp32SnapshotEnc));
	fnModule32First = (MODULE32FIRST)GetProcAddress(kernel32,K_DecodeString(szModule32FirstEnc));
	fnModule32Next = (MODULE32NEXT)GetProcAddress(kernel32,K_DecodeString(szModule32NextEnc));
    
	hModuleSnap = fnCreateToolhelp32Snapshot(TH32CS_SNAPMODULE, GetCurrentProcessId()); 

	if(hModuleSnap == INVALID_HANDLE_VALUE)	return;

	me32.dwSize = sizeof(MODULEENTRY32);

	if(!fnModule32First(hModuleSnap,&me32))
	{
		CloseHandle( hModuleSnap );
		return;
	}

	do
	{
		strcpy(szModule,me32.szModule);
		dwCurImageSize = me32.modBaseSize;
		dwCurDllBase = (DWORD)me32.modBaseAddr;
		
		if(!strcmp(szD3D9Dec,szModule))
		{
			dwD3D9DllBaseAddr = dwCurDllBase ^ 0xACACACAC;
			dwD3D9DllSize = dwCurImageSize;
			iFoundD3D9Dll++;
		}

	} while( fnModule32Next( hModuleSnap, &me32 ) );

	iDupeD3D9DllsWasVerified = 1;
	
	// If there is more than 1 d3d9.dll or its image size is less than
	// 0x00100005 FORCE_EXIT.
	if(iFoundD3D9Dll > 1 || dwD3D9DllSize < 0x00100005) {
		FORCE_EXIT( 11 );
	}

	CloseHandle( hModuleSnap );
	return;
}

//----------------------------------------------------------

void DoCheatDataComparing()
{
	if(!pNetGame) return;

	PED_TYPE *pPed = pGame->FindPlayerPed()->m_pPed;
	DWORD dwTempHealth;
	DWORD dwTempArmour;

	// Repair the camera's target entity.
	*(PDWORD)0xB6F3B8 = dwCamTargetEnt1;
	*(PDWORD)0xB6F5F0 = dwCamTargetEnt2;

	if(!bFirstSpawn && !pGame->IsMenuActive()) {

		// MONEY CHECK
		_asm mov edx, iMoneyStored
		_asm rol edx, 5
		_asm mov iMoneyStored, edx

		if(pGame->GetLocalMoney() != iMoneyStored) {
			//if(pChatWindow) pChatWindow->AddDebugMessage("Money modified");
			iTimesDataModified++;
		}

		// WEAPON DATA CHECK
		if(memcmp(&pPed->WeaponSlots[0],byteWeaponData,WEAPON_DATA_SIZE) != 0) {
			//if(pChatWindow) pChatWindow->AddDebugMessage("Weapon Data modified");
			iTimesDataModified++;
		}

		// HEALTH CHECK
		memcpy(&dwTempHealth,&pPed->fHealth,4);

		_asm mov edx, dwHealth
		_asm rol edx, 5
		_asm mov dwHealth, edx

		if(dwTempHealth != dwHealth) {
			//if(pChatWindow) pChatWindow->AddDebugMessage("Health modified");
			iTimesDataModified++;
		}
	
		// ARMOUR CHECK
		memcpy(&dwTempArmour,&pPed->fArmour,4);

		_asm mov edx, dwArmour
		_asm rol edx, 5
		_asm mov dwArmour, edx

		if(dwTempArmour != dwArmour) {
			//if(pChatWindow) pChatWindow->AddDebugMessage("Armour modified");
			iTimesDataModified++;
		}

		// PLAYER MATRIX CHECK
		if(memcmp(&matStore,pPed->entity.mat,sizeof(MATRIX4X4)) != 0) {
			//if(pChatWindow) pChatWindow->AddDebugMessage("Matrix modified");
			iTimesDataModified++;
		}

	}
}

//----------------------------------------------------------
