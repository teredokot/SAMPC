//
// Version: $Id: helpdialog.cpp,v 1.4 2006/05/20 08:52:04 mike Exp $
//

#include "main.h"
#include <stdio.h>

extern CGame* pGame;
extern GAME_SETTINGS tSettings;
extern CChatWindow *pChatWindow;
extern CFontRender *pDefaultFont;

CHelpDialog::CHelpDialog(IDirect3DDevice9 *pD3DDevice)
{
	m_pD3DDevice = pD3DDevice;
}

void CHelpDialog::Draw()
{
	RECT rect;
	rect.top		= 10;
	rect.right		= pGame->GetScreenWidth() - 150;
	rect.left		= 10;
	rect.bottom		= rect.top + 16;

	pDefaultFont->RenderText("--- SA:MP Key Binds ---",rect,0xFFFFFFFF); rect.top += 16; rect.bottom += 16;
	pDefaultFont->RenderText("F1: Display this help dialog",rect,0xFFFFFFFF); rect.top += 16; rect.bottom += 16;
	pDefaultFont->RenderText("TAB: Display the scoreboard",rect,0xFFFFFFFF); rect.top += 16; rect.bottom += 16;
	pDefaultFont->RenderText("F4: Allows you to change class next time you respawn",rect,0xFFFFFFFF); rect.top += 16; rect.bottom += 16;
	pDefaultFont->RenderText("F5: Show bandwidth statistics",rect,0xFFFFFFFF); rect.top += 16; rect.bottom += 16;
	pDefaultFont->RenderText("F7: Toggle the chat box",rect,0xFFFFFFFF); rect.top += 16; rect.bottom += 16;
	pDefaultFont->RenderText("F8: Take a screenshot",rect,0xFFFFFFFF); rect.top += 16; rect.bottom += 16;
	pDefaultFont->RenderText("F9: Toggle the deathwindow",rect,0xFFFFFFFF); rect.top += 16; rect.bottom += 16;
	pDefaultFont->RenderText("T/F6: Allows you to enter a chat message",rect,0xFFFFFFFF); rect.top += 16; rect.bottom += 16;
	pDefaultFont->RenderText("Recruit Key: Enter vehicle as passenger",rect,0xFFFFFFFF); rect.top += 16; rect.bottom += 16;
}
