//
// Version: $Id: scoreboard.cpp,v 1.15 2006/04/18 11:58:57 kyeman Exp $
//

#include "scoreboard.h"
#include "main.h"
#include <stdio.h>

#define ScoreBoardFVF D3DFVF_XYZRHW|D3DFVF_DIFFUSE

extern CNetGame* pNetGame;
extern CGame* pGame;
extern CChatWindow* pChatWindow;

struct ScoreBoardVertices_s
{
	float x, y, z, rhw;
	D3DCOLOR c;
} ScoreBoardVertices[30] = {
	//  x        y  z     rhw   c
	{20.0f , 20.0f, 0.0f, 1.0f, D3DCOLOR_XRGB(0, 0, 0)},
	{20.0f , 54.0f, 0.0f, 1.0f, D3DCOLOR_XRGB(0, 0, 0)},
	{620.0f, 20.0f, 0.0f, 1.0f, D3DCOLOR_XRGB(0, 0, 0)},

	{20.0f , 54.0f, 0.0f, 1.0f, D3DCOLOR_XRGB(0, 0, 0)},
	{620.0f, 20.0f, 0.0f, 1.0f, D3DCOLOR_XRGB(0, 0, 0)},
	{620.0f, 54.0f, 0.0f, 1.0f, D3DCOLOR_XRGB(0, 0, 0)},

	{20.0f, 54.0f , 0.0f, 1.0f, D3DCOLOR_XRGB(0, 0, 0)},
	{21.0f, 54.0f , 0.0f, 1.0f, D3DCOLOR_XRGB(0, 0, 0)},
	{20.0f, 460.0f, 0.0f, 1.0f, D3DCOLOR_XRGB(0, 0, 0)},

	{21.0f, 54.0f , 0.0f, 1.0f, D3DCOLOR_XRGB(0, 0, 0)},
	{20.0f, 460.0f, 0.0f, 1.0f, D3DCOLOR_XRGB(0, 0, 0)},
	{21.0f, 460.0f, 0.0f, 1.0f, D3DCOLOR_XRGB(0, 0, 0)},

	{619.0f, 54.0f , 0.0f, 1.0f, D3DCOLOR_XRGB(0, 0, 0)},
	{620.0f, 54.0f , 0.0f, 1.0f, D3DCOLOR_XRGB(0, 0, 0)},
	{619.0f, 460.0f, 0.0f, 1.0f, D3DCOLOR_XRGB(0, 0, 0)},

	{620.0f, 54.0f , 0.0f, 1.0f, D3DCOLOR_XRGB(0, 0, 0)},
	{619.0f, 460.0f, 0.0f, 1.0f, D3DCOLOR_XRGB(0, 0, 0)},
	{620.0f, 460.0f, 0.0f, 1.0f, D3DCOLOR_XRGB(0, 0, 0)},

	{21.0f , 460.0f, 0.0f, 1.0f, D3DCOLOR_XRGB(0, 0, 0)},
	{21.0f , 459.0f, 0.0f, 1.0f, D3DCOLOR_XRGB(0, 0, 0)},
	{619.0f, 459.0f, 0.0f, 1.0f, D3DCOLOR_XRGB(0, 0, 0)},

	{21.0f , 459.0f, 0.0f, 1.0f, D3DCOLOR_XRGB(0, 0, 0)},
	{619.0f, 459.0f, 0.0f, 1.0f, D3DCOLOR_XRGB(0, 0, 0)},
	{619.0f, 460.0f, 0.0f, 1.0f, D3DCOLOR_XRGB(0, 0, 0)},

	{21.0f , 54.0f , 0.0f, 1.0f, D3DCOLOR_ARGB(150, 0, 0, 0)},
	{21.0f , 459.0f, 0.0f, 1.0f, D3DCOLOR_ARGB(150, 0, 0, 0)},
	{619.0f, 54.0f , 0.0f, 1.0f, D3DCOLOR_ARGB(150, 0, 0, 0)},

	{21.0f , 459.0f, 0.0f, 1.0f, D3DCOLOR_ARGB(150, 0, 0, 0)},
	{619.0f, 54.0f , 0.0f, 1.0f, D3DCOLOR_ARGB(150, 0, 0, 0)},
	{619.0f, 459.0f, 0.0f, 1.0f, D3DCOLOR_ARGB(150, 0, 0, 0)},
};

CScoreBoard::CScoreBoard(IDirect3DDevice9* pDevice, BOOL bScaleToScreen)
{
	m_pDevice		= pDevice;
	m_pOldStates	= NULL;
	m_pNewStates	= NULL;
	m_pFont			= NULL;
	m_pSprite		= NULL;
	m_iOffset		= 0;
	m_bSorted		= FALSE;

	RECT rect;
	GetClientRect(pGame->GetMainWindowHwnd(), &rect);
	if (bScaleToScreen)
	{
		m_fScalar = (float)rect.right / 640.0f;
		m_fScreenOffsetX = 0.0f;
		m_fScreenOffsetY = 0.0f;
		for (int i=0; i<30; i++)
		{
			ScoreBoardVertices[i].x *= m_fScalar;
			ScoreBoardVertices[i].y *= m_fScalar;
		}
	} else {
		m_fScalar = 1.0f;
		m_fScreenOffsetX = ((float)rect.right / 2) - 320.0f;
		m_fScreenOffsetY = ((float)rect.bottom / 2) - 240.0f;
		for (int i=0; i<30; i++)
		{
			ScoreBoardVertices[i].x += m_fScreenOffsetX;
			ScoreBoardVertices[i].y += m_fScreenOffsetY;
		}
	}
}

CScoreBoard::~CScoreBoard()
{
	if (m_pOldStates)
		m_pOldStates->Release();

	if (m_pNewStates)
		m_pNewStates->Release();

	if (m_pFont)
		m_pFont->Release();

	if (m_pSprite)
		m_pSprite->Release();
}

typedef struct _PLAYER_SCORE_INFO
{
	DWORD dwId;
	char* szName;
	int   iScore;
	DWORD dwPing;
	DWORD dwColor;
	int   iState;
} PLAYER_SCORE_INFO;

void SwapPlayerInfo(PLAYER_SCORE_INFO* psi1, PLAYER_SCORE_INFO* psi2)
{
	PLAYER_SCORE_INFO plrinf;
	memcpy(&plrinf, psi1, sizeof(PLAYER_SCORE_INFO));
	memcpy(psi1, psi2, sizeof(PLAYER_SCORE_INFO));
	memcpy(psi2, &plrinf, sizeof(PLAYER_SCORE_INFO));
}

void CScoreBoard::Draw()
{
	if ((!m_pOldStates) || (!m_pNewStates) || (!m_pFont) || (!m_pSprite))
	{
		RestoreDeviceObjects();
		if ((!m_pOldStates) || (!m_pNewStates) || (!m_pFont) || (!m_pSprite))
			return;
	}

	pNetGame->UpdatePlayerScoresAndPings();

	m_pOldStates->Capture();
	m_pNewStates->Apply();

	m_pDevice->DrawPrimitiveUP(D3DPT_TRIANGLELIST, 10, ScoreBoardVertices, sizeof(ScoreBoardVertices_s));

	m_pOldStates->Apply();

	m_pSprite->Begin(D3DXSPRITE_SORT_TEXTURE|D3DXSPRITE_ALPHABLEND);

	CPlayerPool* pPlayerPool = pNetGame->GetPlayerPool();
	int playercount = pPlayerPool->GetCount()+1;

	if (m_iOffset > (playercount-20))
		m_iOffset = (playercount-20);
	if (m_iOffset < 0)
		m_iOffset = 0;

	PLAYER_SCORE_INFO* Players;
	Players = (PLAYER_SCORE_INFO*)malloc(playercount * sizeof(PLAYER_SCORE_INFO));
	Players[0].dwId = pPlayerPool->GetLocalPlayerID();
	Players[0].szName = pPlayerPool->GetLocalPlayerName();
	Players[0].iScore = pPlayerPool->GetLocalPlayerScore();
	Players[0].dwPing = pPlayerPool->GetLocalPlayerPing();
	Players[0].dwColor = pPlayerPool->GetLocalPlayer()->GetPlayerColorAsARGB();
	int i = 1;
	int x;
	for (x=0; x<MAX_PLAYERS; x++)
	{
		if (pPlayerPool->GetSlotState(x) == TRUE)
		{
			Players[i].dwId = x;
			Players[i].szName = pPlayerPool->GetPlayerName(x);
			Players[i].iScore = pPlayerPool->GetPlayerScore(x);
			Players[i].dwPing = pPlayerPool->GetPlayerPing(x);
			Players[i].dwColor = pPlayerPool->GetAt(x)->GetPlayerColorAsARGB();
			Players[i].iState = (int)pPlayerPool->GetAt(x)->GetState();

			i++;
		}
	}

	if(m_bSorted)
	{
		for (i=0; i<playercount-1; i++)
		{
			for (int j=0; j<playercount-1-i; j++)
			{
				if (Players[j+1].iScore > Players[j].iScore)
				{
					SwapPlayerInfo(&Players[j], &Players[j+1]);
				}
			}
		}
	}

	int endplayer = min(m_iOffset+21, playercount);

	char szServerAddress[255];
	sprintf(szServerAddress, "Players: %d-%d of %d", m_iOffset+1, endplayer, playercount);

	// HOSTNAME AND IP ADDRESS
	RECT rectMain = {(LONG)(24.0f * m_fScalar + m_fScreenOffsetX), (LONG)(21.0f * m_fScalar + m_fScreenOffsetY),
					(LONG)(616.0f * m_fScalar + m_fScreenOffsetX), (LONG)(52.0f * m_fScalar + m_fScreenOffsetY)};
	m_pFont->DrawText(m_pSprite, pNetGame->m_szHostName, -1, &rectMain, DT_SINGLELINE, D3DCOLOR_XRGB(255, 0, 0));
	m_pFont->DrawText(m_pSprite, szServerAddress, -1, &rectMain, DT_SINGLELINE|DT_RIGHT, D3DCOLOR_XRGB(150, 150, 150));

	// PLAYERID LABEL
	rectMain.left = (LONG)((28.0f * m_fScalar) + m_fScreenOffsetX); rectMain.right = (LONG)((60.0f * m_fScalar) + m_fScreenOffsetX);
	rectMain.top = (LONG)((22.0f * m_fScalar) + m_fScreenOffsetY); rectMain.bottom = (LONG)((52.0f * m_fScalar) + m_fScreenOffsetY);
	m_pFont->DrawText(m_pSprite, "id", -1, &rectMain, DT_SINGLELINE|DT_CENTER|DT_BOTTOM, D3DCOLOR_XRGB(100, 150, 200));

	// NAME LABEL
	rectMain.left = (LONG)(60.0f * m_fScalar + m_fScreenOffsetX); rectMain.right = (LONG)(374.0f * m_fScalar + m_fScreenOffsetX);
	rectMain.top = (LONG)(22.0f * m_fScalar + m_fScreenOffsetY); rectMain.bottom = (LONG)(52.0f * m_fScalar + m_fScreenOffsetY);
	m_pFont->DrawText(m_pSprite, "name", -1, &rectMain, DT_SINGLELINE|DT_BOTTOM, D3DCOLOR_XRGB(100, 150, 200));

	// SCORE LABEL
	rectMain.left = (LONG)(374.0f * m_fScalar + m_fScreenOffsetX); rectMain.right = (LONG)(495.0f * m_fScalar + m_fScreenOffsetX);
	rectMain.top = (LONG)(22.0f * m_fScalar + m_fScreenOffsetY); rectMain.bottom = (LONG)(52.0f * m_fScalar + m_fScreenOffsetY);
	m_pFont->DrawText(m_pSprite, "score", -1, &rectMain, DT_SINGLELINE|DT_CENTER|DT_BOTTOM, D3DCOLOR_XRGB(100, 150, 200));

	// PING LABEL
	rectMain.left = (LONG)(495.0f * m_fScalar + m_fScreenOffsetX); rectMain.right = (LONG)(616.0f * m_fScalar + m_fScreenOffsetX);
	rectMain.top = (LONG)(22.0f * m_fScalar + m_fScreenOffsetY); rectMain.bottom = (LONG)(52.0f * m_fScalar + m_fScreenOffsetY);
	m_pFont->DrawText(m_pSprite, "ping", -1, &rectMain, DT_SINGLELINE|DT_CENTER|DT_BOTTOM, D3DCOLOR_XRGB(100, 150, 200));

	RECT rectPlayerId	= {(LONG)(28.0f * m_fScalar + m_fScreenOffsetX), (LONG)(57.0f * m_fScalar + m_fScreenOffsetY),
							(LONG)(60.0f * m_fScalar + m_fScreenOffsetX), (LONG)(456.0f * m_fScalar + m_fScreenOffsetY)};
	RECT rectName		= {(LONG)(60.0f * m_fScalar + m_fScreenOffsetX), (LONG)(57.0f * m_fScalar + m_fScreenOffsetY),
							(LONG)(374.0f * m_fScalar + m_fScreenOffsetX), (LONG)(456.0f * m_fScalar + m_fScreenOffsetY)};
	RECT rectScore		= {(LONG)(374.0f * m_fScalar + m_fScreenOffsetX), (LONG)(57.0f * m_fScalar + m_fScreenOffsetY),
							(LONG)(495.0f * m_fScalar + m_fScreenOffsetX), (LONG)(456.0f * m_fScalar + m_fScreenOffsetY)};
	RECT rectPing		= {(LONG)(495.0f * m_fScalar + m_fScreenOffsetX), (LONG)(57.0f * m_fScalar + m_fScreenOffsetY),
							(LONG)(616.0f * m_fScalar + m_fScreenOffsetX), (LONG)(456.0f * m_fScalar + m_fScreenOffsetY)};

	char szPlayerId[11];
	char szScore[11];
	char szPing[11];
	

	for (x=m_iOffset; x<endplayer; x++)
	{
		sprintf(szPlayerId, "%d", Players[x].dwId);
		sprintf(szScore, "%d", Players[x].iScore);
		sprintf(szPing, "%d", Players[x].dwPing);

		rectPlayerId.left++; rectPlayerId.top++;
		m_pFont->DrawText(m_pSprite, szPlayerId, -1, &rectPlayerId, DT_SINGLELINE|DT_CENTER, D3DCOLOR_XRGB(0, 0, 0));
		rectPlayerId.left--; rectPlayerId.top--;
		m_pFont->DrawText(m_pSprite, szPlayerId, -1, &rectPlayerId, DT_SINGLELINE|DT_CENTER, Players[x].dwColor);

		rectName.left++; rectName.top++;

		char szUsePlayerName[64];

#ifdef _DEBUG
		sprintf(szUsePlayerName,"%s (%u)",Players[x].szName,Players[x].iState);
#else
		strcpy(szUsePlayerName,Players[x].szName);
#endif

		m_pFont->DrawText(m_pSprite, szUsePlayerName, -1, &rectName, DT_SINGLELINE, D3DCOLOR_XRGB(0, 0, 0));
		rectName.left--; rectName.top--;
		m_pFont->DrawText(m_pSprite, szUsePlayerName, -1, &rectName, DT_SINGLELINE, Players[x].dwColor);

		rectScore.left++; rectScore.top++;
		m_pFont->DrawText(m_pSprite, szScore, -1, &rectScore, DT_SINGLELINE|DT_CENTER, D3DCOLOR_XRGB(0, 0, 0));
		rectScore.left--; rectScore.top--;
		m_pFont->DrawText(m_pSprite, szScore, -1, &rectScore, DT_SINGLELINE|DT_CENTER, Players[x].dwColor);

		rectPing.left++; rectPing.top++;
		m_pFont->DrawText(m_pSprite, szPing, -1, &rectPing, DT_SINGLELINE|DT_CENTER, D3DCOLOR_XRGB(0, 0, 0));
		rectPing.left--; rectPing.top--;
		m_pFont->DrawText(m_pSprite, szPing, -1, &rectPing, DT_SINGLELINE|DT_CENTER, Players[x].dwColor);

		rectPlayerId.top = rectName.top = rectScore.top = rectPing.top += (LONG)(20.0f * m_fScalar);
	}

	m_pSprite->End();
	free(Players);
}

void CScoreBoard::DeleteDeviceObjects()
{
	if (m_pOldStates)
	{
		m_pOldStates->Release();
		m_pOldStates = NULL;
	}

	if (m_pNewStates)
	{
		m_pNewStates->Release();
		m_pNewStates = NULL;
	}

	if (m_pFont)
		m_pFont->OnLostDevice();

	if (m_pSprite)
		m_pSprite->OnLostDevice();
}

void CScoreBoard::RestoreDeviceObjects()
{
	if (!m_pOldStates)
	{
		m_pDevice->BeginStateBlock();

		m_pDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
		m_pDevice->SetRenderState(D3DRS_FOGENABLE, FALSE);
		m_pDevice->SetRenderState(D3DRS_ZENABLE, 0);
		m_pDevice->SetRenderState(D3DRS_FILLMODE, 3);
		m_pDevice->SetRenderState(D3DRS_CULLMODE, 1);
		m_pDevice->SetRenderState(D3DRS_WRAP0, 0);
		m_pDevice->SetRenderState(D3DRS_CLIPPING, 1);
		m_pDevice->SetRenderState(D3DRS_VERTEXBLEND, 0);
		m_pDevice->SetRenderState(D3DRS_CLIPPLANEENABLE, 0);
		m_pDevice->SetRenderState(D3DRS_INDEXEDVERTEXBLENDENABLE, 0);
		m_pDevice->SetRenderState(D3DRS_COLORWRITEENABLE, 15);
		m_pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, 1);
		m_pDevice->SetRenderState(D3DRS_SRCBLEND, 5);
		m_pDevice->SetRenderState(D3DRS_DESTBLEND, 6);
		m_pDevice->SetRenderState(D3DRS_BLENDOP, 1);
		m_pDevice->SetRenderState(D3DRS_SHADEMODE, D3DSHADE_GOURAUD);
		m_pDevice->SetFVF(ScoreBoardFVF);
		m_pDevice->SetStreamSource(0, NULL, 0, 0);

		m_pDevice->EndStateBlock(&m_pOldStates);
	}

	if (!m_pNewStates)
	{
		m_pDevice->BeginStateBlock();

		m_pDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
		m_pDevice->SetRenderState(D3DRS_FOGENABLE, FALSE);
		m_pDevice->SetRenderState(D3DRS_ZENABLE, 0);
		m_pDevice->SetRenderState(D3DRS_FILLMODE, 3);
		m_pDevice->SetRenderState(D3DRS_CULLMODE, 1);
		m_pDevice->SetRenderState(D3DRS_WRAP0, 0);
		m_pDevice->SetRenderState(D3DRS_CLIPPING, 1);
		m_pDevice->SetRenderState(D3DRS_VERTEXBLEND, 0);
		m_pDevice->SetRenderState(D3DRS_CLIPPLANEENABLE, 0);
		m_pDevice->SetRenderState(D3DRS_INDEXEDVERTEXBLENDENABLE, 0);
		m_pDevice->SetRenderState(D3DRS_COLORWRITEENABLE, 15);
		m_pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, 1);
		m_pDevice->SetRenderState(D3DRS_SRCBLEND, 5);
		m_pDevice->SetRenderState(D3DRS_DESTBLEND, 6);
		m_pDevice->SetRenderState(D3DRS_BLENDOP, 1);
		m_pDevice->SetRenderState(D3DRS_SHADEMODE, D3DSHADE_GOURAUD);
		m_pDevice->SetFVF(ScoreBoardFVF);
		m_pDevice->SetStreamSource(0, NULL, 0, 0);

		m_pDevice->EndStateBlock(&m_pNewStates);
	}

	if (!m_pFont)
	{
		//             device     h                          w  weight  mip ital   charset       precision           quality        pitch          face       ID3DXFont**
		D3DXCreateFont(m_pDevice, (UINT)(16.0f * m_fScalar), 0, FW_BOLD, 1, FALSE, ANSI_CHARSET, OUT_DEFAULT_PRECIS, PROOF_QUALITY, DEFAULT_PITCH, "Verdana", &m_pFont);
	} else {
		m_pFont->OnResetDevice();
	}

	if (!m_pSprite)
	{
		D3DXCreateSprite(m_pDevice, &m_pSprite);
	} else {
		m_pSprite->OnResetDevice();
	}
}