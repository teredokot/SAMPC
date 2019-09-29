//
// Version: $Id: newplayertags.h,v 1.6 2006/05/07 15:38:35 kyeman Exp $
//

#pragma once
#ifndef NEWPLAYERTAGS_H
#define NEWPLAYERTAGS_H

#include <d3d9.h>
#include <d3dx9.h>

#define HealthBar1FVF D3DFVF_XYZRHW|D3DFVF_DIFFUSE

struct HealthBarVertices1_s
{
	float x, y, z, rhw;
	D3DCOLOR c;
};

class CNewPlayerTags
{
private:
	IDirect3DDevice9* m_pDevice;
	IDirect3DStateBlock9* m_pOldStates;
	IDirect3DStateBlock9* m_pNewStates;
public:
	CNewPlayerTags(IDirect3DDevice9* pDevice);
	~CNewPlayerTags();

	BOOL m_DrawPlayerIDs;

	void Begin();
	void End();
	void Draw(D3DXVECTOR3* pPlayerPos, char* pNameText, DWORD dwColor, float fHealth, float fArmor, float fDistanceFromLocalPlayer);
	void DeleteDeviceObjects();
	void RestoreDeviceObjects();
};

#endif