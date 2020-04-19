//----------------------------------------------------------
//
// SA:MP Multiplayer Modification For GTA:SA
// Copyright 2004-2005 SA:MP team
//
// Version: $Id: deathwindow.cpp,v 1.9 2006/05/08 17:35:55 kyeman Exp $
//
//----------------------------------------------------------

#include "main.h"

static CHAR SpriteIDForWeapon(BYTE byteWeaponID)
{
	switch (byteWeaponID)
	{
	case WEAPON_FIST:
		return '%';
	case WEAPON_BRASSKNUCKLE:
		return 'B';
	case WEAPON_GOLFCLUB:
		return '>';
	case WEAPON_NITESTICK:
		return '(';
	case WEAPON_KNIFE:
		return 'C';
	case WEAPON_BAT:
		return '?';
	case WEAPON_SHOVEL:
		return '&';
	case WEAPON_POOLSTICK:
		return '\\';
	case WEAPON_KATANA:
		return '!';
	case WEAPON_CHAINSAW:
		return '1';
	case WEAPON_DILDO:
		return 'E';
	case WEAPON_DILDO2:
		return 'E';
	case WEAPON_VIBRATOR:
		return 'E';
	case WEAPON_VIBRATOR2:
		return 'E';
	case WEAPON_FLOWER:
		return '$';
	case WEAPON_CANE:
		return '#';
	case WEAPON_GRENADE:
		return '@';
	case WEAPON_TEARGAS:
		return 'D';
	case WEAPON_COLT45:
		return '6';
	case WEAPON_SILENCED:
		return '2';
	case WEAPON_DEAGLE:
		return '3';
	case WEAPON_SHOTGUN:
		return '=';
	case WEAPON_SAWEDOFF:
		return '0';
	case WEAPON_SHOTGSPA:
		return '+';
	case WEAPON_UZI:
		return 'I';
	case WEAPON_MP5:
		return '8';
	case WEAPON_AK47:
		return 'H';
	case WEAPON_M4:
		return '5';
	case WEAPON_TEC9:
		return '7';
	case WEAPON_RIFLE:
		return '.';
	case WEAPON_SNIPER:
		return 'A';
	case WEAPON_ROCKETLAUNCHER:
		return '4';
	case WEAPON_HEATSEEKER:
		return ')';
	case WEAPON_FLAMETHROWER:
		return 'P';
	case WEAPON_MINIGUN:
		return 'F';
	case WEAPON_SATCHEL:
		return '<';
	case WEAPON_BOMB:
		return ';';
	case WEAPON_SPRAYCAN:
		return '/';
	case WEAPON_FIREEXTINGUISHER:
		return ',';
	case WEAPON_PARACHUTE:
		return ':';
	case WEAPON_VEHICLE:
		return 'L';
	case WEAPON_DROWN:
		return 'J';
	case WEAPON_HELIBLADES:
		return 'R';
	case WEAPON_EXPLOSION:
		return 'Q';
	case WEAPON_COLLISION:
		return 'K';
	case SPECIAL_ENTRY_CONNECT:
		return 'N';
	case SPECIAL_ENTRY_DISCONNECT:
		return 'N';
	}
	return 'J';
}

CDeathWindow::CDeathWindow(IDirect3DDevice9 *pD3DDevice)
{
	m_bFailed = TRUE; 
	m_bEnabled = TRUE;
	m_pD3DFont = NULL;
	m_pWeaponFont = NULL;
	m_pSprite = NULL;

	m_pD3DDevice = pD3DDevice;

	ClearWindow();
}

CDeathWindow::~CDeathWindow()
{
	SAFE_RELEASE(m_pSprite);
	SAFE_RELEASE(m_pD3DFont);
	SAFE_RELEASE(m_pWeaponFont);
}

void CDeathWindow::CreateFonts()
{
	if(!m_pD3DDevice) return;
	
	HRESULT hResult = S_OK;
	RECT rectLongestNick = {0,0,0,0};
	int	iFontSize;

	SAFE_RELEASE(m_pSprite);
	SAFE_RELEASE(m_pD3DFont);
	SAFE_RELEASE(m_pWeaponFont);

	// Create a sprite to use when drawing text
	hResult = D3DXCreateSprite(m_pD3DDevice, &m_pSprite);
	if (hResult != S_OK)
		return;

	int iWidth = pGame->GetScreenWidth();
	if (iWidth < 1024)
		iFontSize = 14;
	else if (iWidth == 1024)
		iFontSize = 16;
	else
		iFontSize = 18;

	hResult = D3DXCreateFontA(m_pD3DDevice, iFontSize, 0, FW_BOLD, 1, FALSE,
		DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, ANTIALIASED_QUALITY, DEFAULT_PITCH, "Arial", &m_pD3DFont);
	if (hResult != S_OK)
		return;

	// Store the rect for right aligned name (DT_RIGHT fucks the text)
	if(m_pD3DFont)
		m_pD3DFont->DrawTextA(0,"LONGESTNICKNICKK",-1,&rectLongestNick,DT_CALCRECT|DT_LEFT,0xFF000000);
		
	m_iLongestNickLength = rectLongestNick.right - rectLongestNick.left;
	
	hResult = D3DXCreateFontA(m_pD3DDevice, 20, 0, FW_NORMAL, 1, FALSE,
		SYMBOL_CHARSET, OUT_DEFAULT_PRECIS, ANTIALIASED_QUALITY, DEFAULT_PITCH, "GTAWEAPON3", &m_pWeaponFont);
	if (hResult != S_OK)
		return;

	m_bFailed = FALSE;
}

void CDeathWindow::OnLostDevice()
{
	if (m_pSprite)
		m_pSprite->OnLostDevice();
	if (m_pD3DFont)
		m_pD3DFont->OnLostDevice();
	if (m_pWeaponFont)
		m_pWeaponFont->OnLostDevice();
}

void CDeathWindow::OnResetDevice()
{
	if (m_pSprite)
		m_pSprite->OnResetDevice();
	if (m_pD3DFont)
		m_pD3DFont->OnResetDevice();
	if (m_pWeaponFont)
		m_pWeaponFont->OnResetDevice();
}

//----------------------------------------------------

void CDeathWindow::Draw()
{
	if (m_bEnabled && !m_bFailed)
	{
		RECT
			rect,
			rectNickSize;
		int
			iScreenHeight = pGame->GetScreenHeight(),
			iScreenWidth = pGame->GetScreenWidth(),
			iVerticalBase = (int)(iScreenHeight * 0.3f),
			iHorizontalBase = (int)(iScreenWidth * 0.75f),
			x = 0;

		if (iScreenWidth <= 800)
			iHorizontalBase = (int)(iScreenWidth * 0.65f);

		rect.top = iVerticalBase;
		rect.left = iHorizontalBase;
		rect.bottom = rect.top + 30;
		rect.right = rect.left + 60;

		m_pSprite->Begin(D3DXSPRITE_ALPHABLEND | D3DXSPRITE_SORT_TEXTURE);

		while (x != MAX_DISP_DEATH_MESSAGES)
		{
			if (m_DeathWindowEntries[x].szKiller[0] != '\0' && m_DeathWindowEntries[x].szKillee[0] != '\0')
			{
				// Get the rect length of the killee's nick so we can right justify.
				m_pD3DFont->DrawText(0, m_DeathWindowEntries[x].szKiller, -1,
					&rectNickSize, DT_CALCRECT | DT_LEFT, 0xFFFFFFFF);

				// Move in so it's right justified. (DT_RIGHT fucks the text)
				rect.left += m_iLongestNickLength - (rectNickSize.right - rectNickSize.left);

				RenderText(m_DeathWindowEntries[x].szKiller, rect,
					m_DeathWindowEntries[x].dwKillerColor, DT_LEFT);

				rect.left = iHorizontalBase + m_iLongestNickLength + 5;
				rect.right = rect.left + 30;

				rect.top -= 3;
				RenderWeaponSprite(SpriteIDForWeapon(m_DeathWindowEntries[x].byteWeaponType),
					rect, 0xFFFFFFFF);
				rect.top += 3;

				rect.left += 30;
				rect.right += 30;

				RenderText(m_DeathWindowEntries[x].szKillee, rect, m_DeathWindowEntries[x].dwKilleeColor, DT_LEFT);
			}
			else if(m_DeathWindowEntries[x].szKiller[0] == '\0' && m_DeathWindowEntries[x].szKillee[0] != '\0')
			{
				DWORD dwColor = 0xFFFFFFFF;

				// Get the rect length of the killee's nick so we can right justify.
				m_pD3DFont->DrawText(0, m_DeathWindowEntries[x].szKillee, -1,
					&rectNickSize, DT_CALCRECT | DT_LEFT, 0xFF000000);

				// Move in so it's right justified. (DT_RIGHT fucks the text)
				rect.left += m_iLongestNickLength - (rectNickSize.right - rectNickSize.left);

				RenderText(m_DeathWindowEntries[x].szKillee, rect,
					m_DeathWindowEntries[x].dwKilleeColor, DT_LEFT);

				rect.left = iHorizontalBase + m_iLongestNickLength + 5;
				rect.right = rect.left + 30;

				if (m_DeathWindowEntries[x].byteWeaponType == SPECIAL_ENTRY_CONNECT)
					dwColor = 0xFF000044;
				else if (m_DeathWindowEntries[x].byteWeaponType == SPECIAL_ENTRY_DISCONNECT)
					dwColor = 0xFF440000;

				rect.top -= 3;
				RenderWeaponSprite(SpriteIDForWeapon(m_DeathWindowEntries[x].byteWeaponType), rect, dwColor);
				rect.top += 3;
			}

			rect.top += 30;
			rect.bottom += 30;
			rect.left = iHorizontalBase;
			rect.right = rect.left + 60;

			x++;
		}

		m_pSprite->End();
	}
}

void CDeathWindow::AddMessage(PCHAR szKiller, PCHAR szKillee,
	DWORD dwKillerColor, DWORD dwKilleeColor, BYTE byteWeaponID)
{
	int n = MAX_DISP_DEATH_MESSAGES-1;

	PushBack();

	m_DeathWindowEntries[n].byteWeaponType = byteWeaponID;
	m_DeathWindowEntries[n].dwKilleeColor = dwKilleeColor;
	m_DeathWindowEntries[n].dwKillerColor = dwKillerColor;

	if(szKiller) strcpy_s(m_DeathWindowEntries[n].szKiller, szKiller);
	else m_DeathWindowEntries[n].szKiller[0] = '\0';

	if(szKillee) strcpy_s(m_DeathWindowEntries[n].szKillee, szKillee);	
	else m_DeathWindowEntries[n].szKillee[0] = '\0';
}

void CDeathWindow::PushBack()
{
	int x=0;
	while(x!=(MAX_DISP_DEATH_MESSAGES - 1)) {
		memcpy(&m_DeathWindowEntries[x],&m_DeathWindowEntries[x+1],sizeof(DEATH_WINDOW_ENTRY));
		x++;
	}
}

void CDeathWindow::RenderText(PCHAR sz,RECT rect,DWORD dwColor,DWORD dwParams)
{
	rect.left += 1;
	m_pD3DFont->DrawText(m_pSprite, sz, -1, &rect, DT_NOCLIP | dwParams, 0xFF000000);
	rect.left -= 2;
	m_pD3DFont->DrawText(m_pSprite, sz, -1, &rect, DT_NOCLIP | dwParams, 0xFF000000);
	rect.left += 1;
	rect.top += 1;
	m_pD3DFont->DrawText(m_pSprite, sz, -1, &rect, DT_NOCLIP | dwParams, 0xFF000000);
	rect.top -= 2;
	m_pD3DFont->DrawText(m_pSprite, sz, -1, &rect, DT_NOCLIP | dwParams, 0xFF000000);
	rect.top += 1;

	m_pD3DFont->DrawText(m_pSprite,sz,-1,&rect,DT_NOCLIP|dwParams,dwColor);	
}

void CDeathWindow::RenderWeaponSprite(CHAR WeaponChar,RECT rect,DWORD dwColor)
{
	rect.top += 1;
	rect.left -= 1;
	m_pWeaponFont->DrawTextA(m_pSprite,"G",-1,&rect,DT_NOCLIP|DT_LEFT,0xFF000000);
	rect.top -= 1;
	rect.left += 1;

	m_pWeaponFont->DrawText(m_pSprite,&WeaponChar,-1,&rect,DT_NOCLIP|DT_LEFT,dwColor);
}

void CDeathWindow::ClearWindow()
{
	SecureZeroMemory(m_DeathWindowEntries, sizeof(m_DeathWindowEntries));
}

void CDeathWindow::ChangeNick(PCHAR szOldNick, PCHAR szNewNick)
{
	int x = 0;
	while (x != MAX_DISP_DEATH_MESSAGES) {
		if (m_DeathWindowEntries[x].szKiller[0] != '\0' &&
			strcmp(m_DeathWindowEntries[x].szKiller, szOldNick) == 0)
			strcpy_s(m_DeathWindowEntries[x].szKiller, szNewNick);

		if (m_DeathWindowEntries[x].szKillee[0] != '\0' && 
			strcmp(m_DeathWindowEntries[x].szKillee, szOldNick) == 0)
			strcpy_s(m_DeathWindowEntries[x].szKillee, szNewNick);
	}
}

void CDeathWindow::ToggleEnabled()
{
	m_bEnabled = (m_bEnabled == TRUE);
}
