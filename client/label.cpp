//
// Version: $Id: label.cpp,v 1.2 2006/03/20 17:44:19 kyeman Exp $
//

#include "label.h"

#ifndef SAFE_RELEASE
	#define SAFE_RELEASE(p)	{ if(p) { (p)->Release(); (p)=NULL; } }
#endif

extern D3DXMATRIX matView, matProj;

CLabel::CLabel(IDirect3DDevice9* pDevice, char* szFontFace, bool bFontBold)
{
	m_pDevice			= pDevice;
	m_pFont				= NULL;

	m_szFontFace		= (char*)malloc(strlen(szFontFace)+1);
	strcpy(m_szFontFace, szFontFace);
	m_bFontBold			= bFontBold;
}

CLabel::~CLabel()
{
	DeleteDeviceObjects();
}

void CLabel::Draw(D3DXVECTOR3* ppos, char* szText, DWORD dwColor)
{
	if (!m_pFont)
	{
		RestoreDeviceObjects();
		if (!m_pFont)
			return;
	}

	D3DVIEWPORT9 Viewport;
	m_pDevice->GetViewport(&Viewport);

	D3DXVECTOR3 Out;
	D3DXMATRIX matIdent;
	D3DXMatrixIdentity(&matIdent);
	D3DXVec3Project(&Out, ppos, &Viewport, &matProj, &matView, &matIdent);

	if (Out.z > 1.0f)
		return;

	RECT rect = {(int)Out.x, (int)Out.y, (int)Out.x+1, (int)Out.y+1};
	m_pFont->DrawText(NULL, szText, -1, &rect, DT_NOCLIP|DT_CENTER, 0xFF000000);
	rect.left -= 1; rect.top -= 1;
	m_pFont->DrawText(NULL, szText, -1, &rect, DT_NOCLIP|DT_CENTER, dwColor);
}

void CLabel::DeleteDeviceObjects()
{
	SAFE_RELEASE(m_pFont);
}

void CLabel::RestoreDeviceObjects()
{
	if (!m_pFont)
	{
		D3DXCreateFont(m_pDevice, 12, 0, m_bFontBold?FW_BOLD:0,
			0, FALSE, ANSI_CHARSET, OUT_DEFAULT_PRECIS, ANTIALIASED_QUALITY, DEFAULT_PITCH,
			m_szFontFace, &m_pFont);
	}
}