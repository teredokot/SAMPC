/*

SA:MP Multiplayer Modification
Copyright 2004-2005 SA:MP Team

Version: $Id: menu.h,v 1.0 2007/02/13 19:26:45 Y_Less Exp $

*/

#pragma once
#ifndef SAMPSRV_MENU_H
#define SAMPSRV_MENU_H

#define MAX_MENU_ITEMS 12
#define MAX_MENU_LINE 32
#define MAX_COLUMNS 2

//----------------------------------------------------

struct MENU_INT
{
	BOOL bMenu;
	BOOL bRow[MAX_MENU_ITEMS];
	BOOL bPadding[8 - ((MAX_MENU_ITEMS + 1) % 8)]; 
};

class CMenu
{
private:
	
	BYTE m_byteMenuID;
	
	CHAR m_charTitle[MAX_MENU_LINE];
	CHAR m_charItems[MAX_MENU_ITEMS][MAX_COLUMNS][MAX_MENU_LINE];
	CHAR m_charHeader[MAX_COLUMNS][MAX_MENU_LINE];
	
	float m_fXPos;
	float m_fYPos;
	float m_fCol1Width;
	float m_fCol2Width;
	BYTE m_byteColumns;
	MENU_INT m_MenuInteraction;
	//float fHeight
	
	BYTE m_byteColCount[MAX_COLUMNS];
	
	DWORD m_dwPanel;
	
public:

	CMenu(PCHAR pTitle, float fX, float fY, BYTE byteColumns, float fCol1Width, float fCol2Width, MENU_INT* MenuInteraction);
	~CMenu(){};
	
	void AddMenuItem(BYTE byteColumn, BYTE byteRow, PCHAR pText);
	void SetColumnTitle(BYTE byteColumn, PCHAR pText);
	void Show();
	void Hide();
	PCHAR GetMenuItem(BYTE byteColumn, BYTE byteRow);
	PCHAR GetMenuTitle();
	PCHAR GetMenuHeader(BYTE byteColumn);
	PCHAR MS(BYTE byteColumn, BYTE byteRow);
	BYTE GetSelectedRow();
};

#endif