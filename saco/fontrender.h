//----------------------------------------------------------
//
//   SA:MP Multiplayer Modification For GTA:SA
//   Copyright 2004-2006 SA:MP team
//
//----------------------------------------------------------

#pragma once

class CFontRender
{
public:
	ID3DXFont		    *m_pD3DFont;
	IDirect3DDevice9	*m_pD3DDevice;

	CFontRender(IDirect3DDevice9* pD3DDevice);
	~CFontRender();

	void CreateFonts();

	void DeleteDeviceObjects();
	void RestoreDeviceObjects();

	SIZE MeasureText(char * szString);

	void RenderText(char * sz, DWORD x, DWORD y, DWORD dwColor) {
		RECT rect;

		rect.left = x; 
		rect.top = y; 
		rect.right = rect.left + 400; 
		rect.bottom = rect.top + 400;

		RenderText(sz,rect,dwColor);
	};

	void RenderText(char * sz, RECT rect, DWORD dwColor);

};