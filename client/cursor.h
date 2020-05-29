
#pragma once

class CCursor
{
public:
	CCursor();
	//~CCursor();

	void Init();
	void DeleteDeviceObjects();
	void RestoreDeviceObjects();
	void Process();

	IDirect3DTexture9* m_pTexture;
	//ID3DXFont* m_pFont;
	ID3DXSprite* m_pSprite;
	bool m_bVisible;
	int m_iSize;
};