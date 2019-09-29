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

#pragma pack(1)
typedef struct _DEATH_WINDOW_ENTRY
{
	CHAR szKiller[MAX_PLAYER_NAME+1];
	CHAR szKillee[MAX_PLAYER_NAME+1];
	DWORD dwKillerColor;
	DWORD dwKilleeColor;
	BYTE  byteWeaponType;
} DEATH_WINDOW_ENTRY;

#define SPECIAL_ENTRY_CONNECT 200
#define SPECIAL_ENTRY_DISCONNECT 201

//----------------------------------------------------

#pragma pack(1)
class CDeathWindow
{
private:

	BOOL				m_bEnabled;
	DEATH_WINDOW_ENTRY	m_DeathWindowEntries[MAX_DISP_DEATH_MESSAGES];
	int					m_iLongestNickLength; // In screen units, longest nick length;

	void PushBack();
	void AddToDeathWindowBuffer(CHAR *szKiller,CHAR *szKillee,DWORD dwKillerColor,DWORD dwKilleeColor,BYTE byteWeaponID);
		
public:
	void Draw();
	void AddMessage(CHAR *szKiller, CHAR *szKillee, DWORD dwKillerColor, DWORD dwKilleeColor, BYTE byteWeaponID);
		
	void ToggleEnabled() { 
		if(m_bEnabled) m_bEnabled = FALSE;
		else m_bEnabled = TRUE;
	};

	void RenderText(CHAR *sz,RECT rect,DWORD dwColor,DWORD dwParams);
	void RenderWeaponSprite(CHAR *WeaponChar,RECT rect,DWORD dwColor);
	PCHAR SpriteIDForWeapon(BYTE byteWeaponID);

	CDeathWindow(IDirect3DDevice9 *pD3DDevice);
	~CDeathWindow();

	void CreateFonts();

	ID3DXFont		    *m_pD3DFont;
	ID3DXFont			*m_pWeaponFont;
	ID3DXSprite			*m_pSprite;
	IDirect3DDevice9	*m_pD3DDevice;
};

//----------------------------------------------------
// EOF