#pragma once

/*

SA:MP Multiplayer Modification
Copyright 2004-2005 SA:MP Team

Version: $Id: menupool.h,v 1.0 2007/02/13 19:26:45 Y_Less Exp $

*/

//----------------------------------------------------

class CMenuPool
{
private:

	CMenu *m_pMenus[MAX_MENUS];
	bool m_bMenuSlotState[MAX_MENUS];
	BYTE m_byteCurrentMenu;
	BYTE m_byteExited;

public:
	CMenuPool();
	~CMenuPool();

	CMenu* New(BYTE byteMenuID, PCHAR pTitle, float fX, float fY, BYTE byteColumns, float fCol1Width, float fCol2Width, MENU_INT* MenuInteraction);
	
	bool Delete(BYTE byteMenuID);
	
	// Retrieve a menu by id
	CMenu* GetAt(BYTE byteMenuID)
	{
		if (byteMenuID < MAX_MENUS) {
			return m_pMenus[byteMenuID];
		}
		return NULL;
	};
	
	// Find out if the slot is inuse.
	bool GetSlotState(BYTE byteMenuID)
	{
		if (byteMenuID < MAX_MENUS) {
			return m_bMenuSlotState[byteMenuID];
		}
		return false;
	};
	
	void ShowMenu(BYTE byteMenuID);
	void HideMenu(BYTE byteMenuID);
	
	PCHAR GetTextPointer(PCHAR szName);
	
	void Process();
};

//----------------------------------------------------
