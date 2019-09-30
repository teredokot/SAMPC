//----------------------------------------------------------
//
//   SA:MP Multiplayer Modification For GTA:SA
//   Copyright 2004-2006 SA:MP team
//
//----------------------------------------------------------

#include "main.h"
#include "fontrender.h"

extern CGame *pGame;

//----------------------------------------------------

CFontRender::CFontRender(IDirect3DDevice9* pD3DDevice)
{
	m_pD3DDevice = pD3DDevice;
	m_pD3DFont = NULL;

	CreateFonts();
}

CFontRender::~CFontRender()
{
	if(m_pD3DFont) SAFE_DELETE(m_pD3DFont);
}

void CFontRender::CreateFonts()
{	
	if(!m_pD3DDevice) return;
	if(m_pD3DFont) SAFE_DELETE(m_pD3DFont);

	int iFontSize;

	if(pGame->GetScreenWidth() < 1024) {
		iFontSize = 14;
	} else if(pGame->GetScreenWidth() == 1024) {
		iFontSize = 16;
	} else {
		iFontSize = 18;
	}

	D3DXCreateFont(m_pD3DDevice, iFontSize, 0, FW_BOLD, 1, FALSE,
		DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, "Arial", &m_pD3DFont);
}


void CFontRender::DeleteDeviceObjects() 
{
	m_pD3DFont->OnLostDevice();
}
void CFontRender::RestoreDeviceObjects()
{
	m_pD3DFont->OnResetDevice();
}

SIZE CFontRender::MeasureText(char * szString)
{
	RECT rect;
	SIZE ret;

	m_pD3DFont->DrawText(0,szString,-1,&rect,DT_CALCRECT|DT_LEFT,0xFF000000);
	ret.cx = rect.right - rect.left;
	ret.cy = rect.bottom - rect.top;

	return ret;
}

void CFontRender::RenderText(char * sz, RECT rect, DWORD dwColor)
{
	rect.left += 1;
	rect.top += 1;
	m_pD3DFont->DrawText(0,sz,-1,&rect,DT_NOCLIP|DT_LEFT,0xFF000000);
	rect.left -= 1;
	rect.top -= 1;
    
	// the text
	m_pD3DFont->DrawText(0,sz,-1,&rect,DT_NOCLIP|DT_LEFT,dwColor);	
}

//----------------------------------------------------