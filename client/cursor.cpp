
#include "main.h"

CCursor::CCursor()
{
	m_bVisible = false;
	//m_pFont = NULL;
	m_pTexture = NULL;
	m_pSprite = NULL;
	m_iSize = 20;
}

/*CCursor::~CCursor()
{

}*/

void CCursor::Init()
{
	//SAFE_RELEASE(m_pFont);
	SAFE_RELEASE(m_pTexture);
	SAFE_RELEASE(m_pSprite);

	/*HRESULT hResult = D3DXCreateFontA(pD3DDevice, m_iSize, 0, FW_NORMAL, 1, FALSE,
		SYMBOL_CHARSET, OUT_TT_ONLY_PRECIS, ANTIALIASED_QUALITY, DEFAULT_PITCH, "Cursors", &m_pFont);
	if (hResult != D3D_OK)
		return;*/

	D3DXCreateTextureFromFile(pD3DDevice, "mouse.png", &m_pTexture);
	D3DXCreateSprite(pD3DDevice, &m_pSprite);
}

void CCursor::DeleteDeviceObjects()
{
	//if(m_pFont)
		//m_pFont->OnLostDevice();
	if (m_pSprite)
		m_pSprite->OnLostDevice();
}

void CCursor::RestoreDeviceObjects()
{
	//if(m_pFont)
		//m_pFont->OnResetDevice();
	if (m_pSprite)
		m_pSprite->OnResetDevice();
}

void CCursor::Process()
{
	if (!m_bVisible || !m_pSprite || !m_pTexture)
		return;

	D3DXMATRIX mat;
	POINT lPoint;
	D3DXVECTOR2	scale;
	D3DXVECTOR2	pos;

	GetCursorPos(&lPoint);

	pos = { (float)lPoint.x, (float)lPoint.y };
	scale = { 1.f, 1.f };

	D3DXMatrixTransformation2D(&mat, NULL, 0.0f, &scale, NULL, 0.0f, &pos);
	m_pSprite->Begin(D3DXSPRITE_ALPHABLEND);
	m_pSprite->SetTransform(&mat);
	m_pSprite->Draw(m_pTexture, NULL, NULL, NULL, 0xFFFFFFFF);
	m_pSprite->End();

	//pChatWindow->AddDebugMessage("Drawing cursor");

	/*m_pSprite->Begin(D3DXSPRITE_ALPHABLEND | D3DXSPRITE_SORT_TEXTURE);

	POINT lPoint;
	GetCursorPos(&lPoint);

	RECT rect = { lPoint.x, lPoint.y, lPoint.x + m_iSize, lPoint.y + m_iSize };
	m_pFont->DrawText(m_pSprite, "d", -1, &rect, DT_NOCLIP | DT_SINGLELINE | DT_LEFT, 0xFFFFFFFF);

	m_pSprite->End();*/
}
