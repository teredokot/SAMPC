//
// Version: $Id: newplayertags.cpp,v 1.12 2006/05/07 15:38:35 kyeman Exp $
//

#include "main.h"
#include "newplayertags.h"
#include <stdio.h>

extern CGame *pGame;
extern CChatWindow *pChatWindow;
extern CFontRender *pDefaultFont;

HealthBarVertices1_s HealthBarBDRVertices1[4] =
{	//x     y      z     rhw   c
	{0.0f,  0.0f,  0.0f, 0.0f, D3DCOLOR_XRGB(0, 0, 0)},  // 1 . . 4
	{0.0f,  10.0f, 0.0f, 0.0f, D3DCOLOR_XRGB(0, 0, 0)},  // 2 . . 3
	{50.0f, 10.0f, 0.0f, 0.0f, D3DCOLOR_XRGB(0, 0, 0)},
	{50.0f, 0.0f,  0.0f, 0.0f, D3DCOLOR_XRGB(0, 0, 0)}
};

HealthBarVertices1_s HealthBarBGVertices1[4] =
{	//x     y     z     rhw   c
	{1.0f,  1.0f, 0.0f, 0.0f, D3DCOLOR_XRGB(75, 11, 20)},  // 1 . . 4
	{1.0f,  9.0f, 0.0f, 0.0f, D3DCOLOR_XRGB(75, 11, 20)},  // 2 . . 3
	{49.0f, 9.0f, 0.0f, 0.0f, D3DCOLOR_XRGB(75, 11, 20)},
	{49.0f, 1.0f, 0.0f, 0.0f, D3DCOLOR_XRGB(75, 11, 20)}
};

HealthBarVertices1_s HealthBarVertices1[4] =
{	//x     y     z     rhw   c
	{1.0f,  1.0f, 0.0f, 0.0f, D3DCOLOR_XRGB(185, 34, 40)},  // 1 . . 4
	{1.0f,  9.0f, 0.0f, 0.0f, D3DCOLOR_XRGB(185, 34, 40)},  // 2 . . 3
	{49.0f, 9.0f, 0.0f, 0.0f, D3DCOLOR_XRGB(185, 34, 40)},
	{49.0f, 1.0f, 0.0f, 0.0f, D3DCOLOR_XRGB(185, 34, 40)}
};

extern D3DXMATRIX matView, matProj;

CNewPlayerTags::CNewPlayerTags(IDirect3DDevice9* pDevice)
{
	m_pDevice			= pDevice;
	m_pOldStates		= NULL;
	m_pNewStates		= NULL;
	m_DrawPlayerIDs		= FALSE;
}

CNewPlayerTags::~CNewPlayerTags()
{
	if (m_pOldStates) m_pOldStates->Release();
	if (m_pNewStates) m_pNewStates->Release();
}

void CNewPlayerTags::Begin()
{
	if ((!m_pOldStates) || (!m_pNewStates))
	{
		RestoreDeviceObjects();
		if ((!m_pOldStates) || (!m_pNewStates))
			return;
	}
	m_pOldStates->Capture();
	m_pNewStates->Apply();
}

void CNewPlayerTags::End()
{
	if (!m_pOldStates)
	{
		RestoreDeviceObjects();
		if (!m_pOldStates)
			return;
	}
	m_pOldStates->Apply();
}

void CNewPlayerTags::Draw(D3DXVECTOR3* pPlayerPos, char* pNameText, DWORD dwColor, float fHealth, float fArmor, float fDistanceFromLocalPlayer)
{
	D3DXVECTOR3 TagPos = *pPlayerPos;
	TagPos.z += 1.0f + (fDistanceFromLocalPlayer * 0.05f);
	
	D3DVIEWPORT9 Viewport;
	m_pDevice->GetViewport(&Viewport);

	D3DXVECTOR3 Out;
	D3DXMATRIX matIdent;
	D3DXMatrixIdentity(&matIdent);
	D3DXVec3Project(&Out, &TagPos, &Viewport, &matProj, &matView, &matIdent);

	if (Out.z > 1.0f)
		return;

	RECT rect = {(int)Out.x, (int)Out.y, (int)Out.x+1, (int)Out.y+1};
	SIZE size = pDefaultFont->MeasureText(pNameText);
	rect.left -= size.cx/2;

	pDefaultFont->RenderText(pNameText,rect,dwColor);

	Out.x = (float)((int)Out.x);
	Out.y = (float)((int)Out.y);

	HealthBarBDRVertices1[0].x = Out.x - 20.0f; // Top left
	HealthBarBDRVertices1[0].y = Out.y + 18.0f;
	HealthBarBDRVertices1[1].x = Out.x - 20.0f; // Bottom left
	HealthBarBDRVertices1[1].y = Out.y + 24.0f;
	HealthBarBDRVertices1[2].x = Out.x + 21.0f; // Bottom right
	HealthBarBDRVertices1[2].y = Out.y + 24.0f;
	HealthBarBDRVertices1[3].x = Out.x + 21.0f; // Top Right
	HealthBarBDRVertices1[3].y = Out.y + 18.0f;

	HealthBarBGVertices1[0].x = HealthBarVertices1[0].x = Out.x - 19.0f; // Top left
	HealthBarBGVertices1[0].y = HealthBarVertices1[0].y = Out.y + 19.0f;
	HealthBarBGVertices1[1].x = HealthBarVertices1[1].x = Out.x - 19.0f; // Bottom left
	HealthBarBGVertices1[1].y = HealthBarVertices1[1].y = Out.y + 23.0f;
	HealthBarBGVertices1[2].x = HealthBarVertices1[2].x = Out.x + 19.0f; // Bottom right
	HealthBarBGVertices1[2].y = HealthBarVertices1[2].y = Out.y + 23.0f;
	HealthBarBGVertices1[3].x = HealthBarVertices1[3].x = Out.x + 19.0f; // Top Right
	HealthBarBGVertices1[3].y = HealthBarVertices1[3].y = Out.y + 19.0f;
	
	if (fHealth > 100.0f)
		fHealth = 100.0f;
	fHealth /= 2.6f;
	fHealth -= 19.0f;

	HealthBarVertices1[2].x = Out.x + fHealth; // Bottom right
	HealthBarVertices1[3].x = Out.x + fHealth; // Top Right

	m_pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	m_pDevice->SetRenderState(D3DRS_TEXTUREFACTOR, 0xFFFFFFFF);
	m_pDevice->SetTexture(0, NULL);
	m_pDevice->SetFVF(HealthBar1FVF);

	if (fArmor > 0.0f)
	{
		for (int i=0; i<4; i++)
		{
			HealthBarBDRVertices1[i].y += 8.0f;
			HealthBarBGVertices1[i].y += 8.0f;
			HealthBarVertices1[i].y += 8.0f;
		}
	}

	m_pDevice->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, 2, HealthBarBDRVertices1, sizeof(HealthBarVertices1_s));
	m_pDevice->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, 2, HealthBarBGVertices1,  sizeof(HealthBarVertices1_s));
	m_pDevice->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, 2, HealthBarVertices1,    sizeof(HealthBarVertices1_s));


	// ARMOUR BAR
	if (fArmor > 0.0f)
	{
		for (int i=0; i<4; i++)
		{
			HealthBarBDRVertices1[i].y -= 8.0f;
			HealthBarBGVertices1[i].y -= 8.0f;
			HealthBarVertices1[i].y -= 8.0f;
		}

		HealthBarVertices1[0].c = HealthBarVertices1[1].c =
		HealthBarVertices1[2].c = HealthBarVertices1[3].c = D3DCOLOR_XRGB(200, 200, 200);

		HealthBarBGVertices1[0].c = HealthBarBGVertices1[1].c =
		HealthBarBGVertices1[2].c = HealthBarBGVertices1[3].c = D3DCOLOR_XRGB(40, 40, 40);

		if (fArmor > 100.0f)
			fArmor = 100.0f;
		fArmor /= 2.6f;
		fArmor -= 19.0f;

		HealthBarVertices1[2].x = Out.x + fArmor; // Bottom right
		HealthBarVertices1[3].x = Out.x + fArmor; // Top Right

		m_pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
		m_pDevice->SetRenderState(D3DRS_TEXTUREFACTOR, 0xFFFFFFFF);
		m_pDevice->SetTexture(0, NULL);
		m_pDevice->SetFVF(HealthBar1FVF);

		m_pDevice->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, 2, HealthBarBDRVertices1, sizeof(HealthBarVertices1_s));
		m_pDevice->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, 2, HealthBarBGVertices1,  sizeof(HealthBarVertices1_s));
		m_pDevice->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, 2, HealthBarVertices1,    sizeof(HealthBarVertices1_s));

		HealthBarVertices1[0].c = HealthBarVertices1[1].c =
		HealthBarVertices1[2].c = HealthBarVertices1[3].c = D3DCOLOR_XRGB(185, 34, 40);

		HealthBarBGVertices1[0].c = HealthBarBGVertices1[1].c =
		HealthBarBGVertices1[2].c = HealthBarBGVertices1[3].c = D3DCOLOR_XRGB(75, 11, 20);
	}
}

void CNewPlayerTags::DeleteDeviceObjects()
{
	SAFE_RELEASE(m_pOldStates);
	SAFE_RELEASE(m_pNewStates);
}

void CNewPlayerTags::RestoreDeviceObjects()
{
	if (!m_pOldStates)
	{
		D3DXMATRIX matTemp;
		D3DXMatrixIdentity(&matTemp);

		m_pDevice->BeginStateBlock();
		m_pDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
		m_pDevice->SetRenderState(D3DRS_FOGENABLE, FALSE);
		m_pDevice->SetRenderState(D3DRS_ZENABLE, 1);
		m_pDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
		m_pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
		m_pDevice->SetRenderState(D3DRS_CLIPPING, 1);
		m_pDevice->SetRenderState(D3DRS_VERTEXBLEND, FALSE);
		m_pDevice->SetRenderState(D3DRS_CLIPPLANEENABLE, 1);
		m_pDevice->SetRenderState(D3DRS_INDEXEDVERTEXBLENDENABLE, FALSE);
		m_pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
		m_pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
		m_pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
		m_pDevice->SetRenderState(D3DRS_SHADEMODE, D3DSHADE_GOURAUD);
		m_pDevice->SetRenderState(D3DRS_TEXTUREFACTOR, 0xFFFFFFFF);
		m_pDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
		m_pDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
		m_pDevice->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_TFACTOR);
		m_pDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
		m_pDevice->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_TFACTOR);
		m_pDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
		m_pDevice->SetTransform(D3DTS_WORLD, &matTemp);
		m_pDevice->SetStreamSource(0, NULL, 0, 0);
		m_pDevice->SetTexture(0, NULL);
		m_pDevice->SetFVF(0);
		m_pDevice->EndStateBlock(&m_pOldStates);
	}

	if (!m_pNewStates)
	{
		m_pDevice->BeginStateBlock();

		// This is the original one

		/*
		m_pDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
		m_pDevice->SetRenderState(D3DRS_FOGENABLE, FALSE);
		m_pDevice->SetRenderState(D3DRS_ZENABLE, 1);
		m_pDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
		m_pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
		m_pDevice->SetRenderState(D3DRS_CLIPPING, 1);
		m_pDevice->SetRenderState(D3DRS_VERTEXBLEND, FALSE);
		m_pDevice->SetRenderState(D3DRS_CLIPPLANEENABLE, 1);
		m_pDevice->SetRenderState(D3DRS_INDEXEDVERTEXBLENDENABLE, FALSE);
		m_pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
		m_pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
		m_pDevice->SetRenderState(D3DRS_SHADEMODE, D3DSHADE_GOURAUD);
		m_pDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
		m_pDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
		m_pDevice->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_TFACTOR);
		m_pDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
		m_pDevice->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_TFACTOR);
		m_pDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
		*/

		// End of original one

		// This was taken and modified from public domain code for healthbar fix
		// Not sure what the root cause of the problem is, but this fixes it.

		m_pDevice->SetVertexShader(NULL);
		m_pDevice->SetPixelShader(NULL);

		m_pDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
		m_pDevice->SetRenderState(D3DRS_FOGENABLE, FALSE);
		m_pDevice->SetRenderState(D3DRS_ZENABLE, 0);
		m_pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE,           TRUE);
		m_pDevice->SetRenderState(D3DRS_SRCBLEND,                   D3DBLEND_SRCALPHA);
		m_pDevice->SetRenderState(D3DRS_DESTBLEND,                  D3DBLEND_INVSRCALPHA);
		m_pDevice->SetRenderState(D3DRS_FILLMODE,                   D3DFILL_SOLID);
		m_pDevice->SetRenderState(D3DRS_CULLMODE,                   D3DCULL_NONE);
		m_pDevice->SetRenderState(D3DRS_STENCILENABLE,              FALSE);
		m_pDevice->SetRenderState(D3DRS_CLIPPING,                   TRUE);
		m_pDevice->SetRenderState(D3DRS_CLIPPLANEENABLE,            FALSE);
		m_pDevice->SetRenderState(D3DRS_VERTEXBLEND,                D3DVBF_DISABLE);
		m_pDevice->SetRenderState(D3DRS_INDEXEDVERTEXBLENDENABLE,   FALSE);
		//m_pDevice->SetRenderState(D3DRS_MULTISAMPLEANTIALIAS,       FALSE);
		//m_pDevice->SetRenderState(D3DRS_COLORWRITEENABLE,           D3DCOLORWRITEENABLE_RED|D3DCOLORWRITEENABLE_GREEN|D3DCOLORWRITEENABLE_BLUE|D3DCOLORWRITEENABLE_ALPHA);

		m_pDevice->SetTextureStageState(0, D3DTSS_COLOROP,                D3DTOP_MODULATE);
		m_pDevice->SetTextureStageState(0, D3DTSS_COLORARG1,              D3DTA_TEXTURE);
		m_pDevice->SetTextureStageState(0, D3DTSS_COLORARG2,              D3DTA_DIFFUSE);
		m_pDevice->SetTextureStageState(0, D3DTSS_ALPHAOP,                D3DTOP_MODULATE);
		m_pDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1,              D3DTA_TEXTURE);
		m_pDevice->SetTextureStageState(0, D3DTSS_ALPHAARG2,              D3DTA_DIFFUSE);
		m_pDevice->SetTextureStageState(0, D3DTSS_TEXCOORDINDEX,          0);
		m_pDevice->SetTextureStageState(0, D3DTSS_TEXTURETRANSFORMFLAGS,  D3DTTFF_DISABLE);
		m_pDevice->SetTextureStageState(1, D3DTSS_COLOROP,                D3DTOP_DISABLE);
		m_pDevice->SetTextureStageState(1, D3DTSS_ALPHAOP,                D3DTOP_DISABLE);

		//m_pDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_POINT);
		//m_pDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_POINT);
		//m_pDevice->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_NONE);

		// End modified public domain code

		m_pDevice->EndStateBlock(&m_pNewStates);
	}
}