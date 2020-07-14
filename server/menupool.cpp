/*

SA:MP Multiplayer Modification
Copyright 2004-2005 SA:MP Team

Version: $Id: menupool.cpp,v 1.0 2007/02/13 19:26:45 Y_Less Exp $

*/

#include "main.h"

//----------------------------------------------------

CMenuPool::CMenuPool()
{
	// loop through and initialize all net players to null and slot states to false
	for (BYTE byteMenuID = 0; byteMenuID < MAX_MENUS; byteMenuID++)
	{
		m_bMenuSlotState[byteMenuID] = false;
		m_pMenus[byteMenuID] = NULL;
	}
	for (BYTE bytePlayer = 0; bytePlayer < MAX_PLAYERS; bytePlayer++)
	{
		m_bytePlayerMenu[bytePlayer] = INVALID_MENU_ID;
	}
}

//----------------------------------------------------

CMenuPool::~CMenuPool()
{	
	for (BYTE byteMenuID = 0; byteMenuID < MAX_MENUS; byteMenuID++)
	{
		//m_bMenuSlotState[byteMenuID] = FALSE;
		if (m_pMenus[byteMenuID])
		{
			delete m_pMenus[byteMenuID];
			m_pMenus[byteMenuID] = NULL;
		}
	}
}

//----------------------------------------------------

BYTE CMenuPool::New(PCHAR pTitle, float fX, float fY, BYTE byteColumns, float fCol1Width, float fCol2Width)
{
	BYTE byteMenuID;
	
	for (byteMenuID = 1; byteMenuID < MAX_MENUS; byteMenuID++)
	{
		if (m_bMenuSlotState[byteMenuID] == false) break;
	}
	if (byteMenuID == MAX_MENUS) return INVALID_MENU_ID;

	CMenu* pMenu = new CMenu(pTitle, fX, fY, byteColumns, fCol1Width, fCol2Width);
	//m_pMenus[byteMenuID] = new CMenu();
	
	if (pMenu)
	{
		//m_pMenu
		m_bMenuSlotState[byteMenuID] = true;
		pMenu->SetID(byteMenuID);
		m_pMenus[byteMenuID] = pMenu;
		return byteMenuID;
	}
	return INVALID_MENU_ID;
}

//----------------------------------------------------

bool CMenuPool::Delete(BYTE byteMenuID)
{
	if (m_bMenuSlotState[byteMenuID] == false || !m_pMenus[byteMenuID])
	{
		return false;
	}
	m_bMenuSlotState[byteMenuID] = false;
	delete m_pMenus[byteMenuID];
	m_pMenus[byteMenuID] = NULL;

	return true;
}
