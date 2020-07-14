/*

SA:MP Multiplayer Modification
Copyright 2004-2005 SA:MP Team

Version: $Id: menu.h,v 1.0 2007/02/13 19:26:45 Y_Less Exp $

*/

#ifndef SAMPSRV_MENU_H
#define SAMPSRV_MENU_H

class CMenu
{
private:
	
	BYTE m_byteMenuID;
	
	CHAR m_charTitle[MAX_MENU_LINE];
	CHAR m_charItems[MAX_MENU_ITEMS][MAX_COLUMNS][MAX_MENU_LINE];
	CHAR m_charHeader[MAX_COLUMNS][MAX_MENU_LINE];
	
	bool m_bInitedForPlayer[MAX_PLAYERS];
	MENU_INT m_MenuInteraction;
	
	float m_fXPos;
	float m_fYPos;
	float m_fCol1Width;
	float m_fCol2Width;
	BYTE m_byteColumns;
	//float fHeight
	
	BYTE m_byteColCount[MAX_COLUMNS];
	
public:

	CMenu(PCHAR pTitle, float fX, float fY, BYTE byteColumns, float fCol1Width, float fCol2Width);
	~CMenu(){};
	
	BYTE AddMenuItem(BYTE byteColumn, PCHAR pText);
	//void RemoveMenuItem(BYTE byteColumn, BYTE byteItem);
	void SetColumnTitle(BYTE byteColumn, PCHAR pText);
	
	void ResetForAll();
	
	void SetID(BYTE byteMenuID) { m_byteMenuID = byteMenuID; };
	
	void InitForPlayer(BYTE bytePlayerID);
	void ShowForPlayer(BYTE bytePlayerID);
	void HideForPlayer(BYTE bytePlayerID);
	
	void ResetPlayer(BYTE bytePlayerID)
	{
		if (bytePlayerID < MAX_PLAYERS) m_bInitedForPlayer[bytePlayerID] = false;
	};
	void DisableInteraction() { m_MenuInteraction.bMenu = false; };
	void DisableRow(BYTE byteRow) { m_MenuInteraction.bRow[byteRow] = false; };
	
	bool ValidRow(unsigned char ucRow);
};

#endif // SAMPSRV_MENU_H

