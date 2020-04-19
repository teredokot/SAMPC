//----------------------------------------------------------
//
// SA:MP Multiplayer Modification For GTA:SA
// Copyright 2004-2005 SA:MP team
//
// Version: $Id: deathwindow.h,v 1.2 2006/04/26 17:31:37 kyeman Exp $
//
//----------------------------------------------------------

#pragma once

#define MAX_DISP_DEATH_MESSAGES	5

#define SPECIAL_ENTRY_CONNECT 200
#define SPECIAL_ENTRY_DISCONNECT 201

class CDeathWindow
{
private:
	typedef struct
	{
		CHAR szKiller[MAX_PLAYER_NAME];
		CHAR szKillee[MAX_PLAYER_NAME];
		DWORD dwKillerColor;
		DWORD dwKilleeColor;
		BYTE byteWeaponType;
	} DEATH_WINDOW_ENTRY;

	IDirect3DDevice9* m_pD3DDevice;
	ID3DXFont* m_pD3DFont;
	ID3DXFont* m_pWeaponFont;
	ID3DXSprite* m_pSprite;

	BOOL m_bFailed;
	BOOL m_bEnabled;
	
	DEATH_WINDOW_ENTRY m_DeathWindowEntries[MAX_DISP_DEATH_MESSAGES];
	int m_iLongestNickLength; // In screen units, longest nick length;

	void RenderText(PCHAR sz, RECT rect, DWORD dwColor, DWORD dwParams);
	void RenderWeaponSprite(CHAR WeaponChar, RECT rect, DWORD dwColor);

	void PushBack();

public:
	CDeathWindow(IDirect3DDevice9* pD3DDevice);
	~CDeathWindow();

	void CreateFonts();
	void OnLostDevice();
	void OnResetDevice();
	void Draw();
	void ChangeNick(PCHAR szOldNick, PCHAR szNewNick);
	void ClearWindow();
	void AddMessage(PCHAR szKiller, PCHAR szKillee, DWORD dwKillerColor, DWORD dwKilleeColor, BYTE byteWeaponID);
	void ToggleEnabled();
};
