/*

SA:MP Multiplayer Modification
Copyright 2004-2005 SA:MP Team

Version: $Id: menupool.h,v 1.0 2007/02/13 19:26:45 Y_Less Exp $

*/

#ifndef SAMPSRV_MENUPOOL_H
#define SAMPSRV_MENUPOOL_H

//----------------------------------------------------

class CMenuPool
{
private:

	CMenu *m_pMenus[MAX_MENUS];
	BOOL m_bMenuSlotState[MAX_MENUS];
	BYTE m_bytePlayerMenu[MAX_PLAYERS];

public:
	CMenuPool();
	~CMenuPool();

	BYTE New(PCHAR pTitle, float fX, float fY, BYTE byteColumns, float fCol1Width, float fCol2Width);
	
	BOOL Delete(BYTE byteMenuID);
	
	// Retrieve a menu by id
	CMenu* GetAt(BYTE byteMenuID)
	{
		if(byteMenuID > MAX_MENUS) { return NULL; }
		return m_pMenus[byteMenuID];
	};
	
	// Find out if the slot is inuse.
	BOOL GetSlotState(BYTE byteMenuID)
	{
		if(byteMenuID > MAX_MENUS) { return FALSE; }
		return m_bMenuSlotState[byteMenuID];
	};
	
	void ResetPlayer(BYTE bytePlayerID)
	{
		for (BYTE i = 0; i < MAX_MENUS; i++) if (m_pMenus[i]) m_pMenus[i]->ResetPlayer(bytePlayerID);
	}
	
	BYTE GetPlayerMenu(BYTE bytePlayer)
	{
		if (bytePlayer >= MAX_PLAYERS) return 255;
		return m_bytePlayerMenu[bytePlayer];
	}
	
	void SetPlayerMenu(BYTE bytePlayer, BYTE byteMenu)
	{
		if (bytePlayer < MAX_PLAYERS && byteMenu < MAX_MENUS) m_bytePlayerMenu[bytePlayer] = byteMenu;
	}
};

//----------------------------------------------------

#endif

