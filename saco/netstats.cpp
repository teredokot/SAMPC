//
// Version: $Id: netstats.cpp,v 1.4 2006/04/15 22:43:03 kyeman Exp $
//

#include "main.h"
#include <stdio.h>

extern CGame* pGame;
extern CNetGame* pNetGame;
extern GAME_SETTINGS tSettings;
extern CChatWindow *pChatWindow;
extern CFontRender *pDefaultFont;

char szDispBuf[16384];
char szStatBuf[16384];
char szDrawLine[1024];

CNetStats::CNetStats(IDirect3DDevice9 *pD3DDevice)
{
	m_dwLastUpdateTick = GetTickCount();
	m_dwLastTotalBytesRecv = 0;
	m_dwLastTotalBytesSent = 0;
	m_dwBPSDownload = 0;
	m_dwBPSUpload = 0;
	m_pD3DDevice = pD3DDevice;
}

void CNetStats::Draw()
{
	RakNetStatisticsStruct *pRakStats = pNetGame->GetRakClient()->GetStatistics();
	float fDown,fUp;

	if((GetTickCount() - m_dwLastUpdateTick) > 1000) {
		m_dwLastUpdateTick = GetTickCount();
		
		m_dwBPSDownload = ((UINT)(pRakStats->bitsReceived / 8)) - m_dwLastTotalBytesRecv;
		m_dwLastTotalBytesRecv = (UINT)(pRakStats->bitsReceived / 8);

		m_dwBPSUpload = ((UINT)(pRakStats->totalBitsSent / 8)) - m_dwLastTotalBytesSent;
		m_dwLastTotalBytesSent = (UINT)(pRakStats->totalBitsSent / 8);
	}

	if(m_dwBPSDownload != 0) {
		fDown = (float)m_dwBPSDownload / 1024;
	} else {
		fDown = 0.0f;
	}

	if(m_dwBPSUpload != 0) {
		fUp = (float)m_dwBPSUpload / 1024;
	} else {
		fUp = 0.0f;
	}

	CLocalPlayer *pLocalPlayer = pNetGame->GetPlayerPool()->GetLocalPlayer();
	CPlayerPed *pPlayerPed = pLocalPlayer->GetPlayerPed();
	int iPlayersInRange = pLocalPlayer->DetermineNumberOfPlayersInLocalRange();
	if(!iPlayersInRange) iPlayersInRange = 20;

	sprintf(szDispBuf,"Download Rate: %.2f kbps\nUpload Rate: %.2f kbps\n",
		fDown,fUp);

	if(pPlayerPed) {
		if(!pPlayerPed->IsInVehicle()) {
			sprintf(szStatBuf,"OnFoot Send Rate: %u\n",pLocalPlayer->GetOptimumOnFootSendRate(iPlayersInRange));
		} else {
			sprintf(szStatBuf,"InCar Send Rate: %u\n",pLocalPlayer->GetOptimumInCarSendRate(iPlayersInRange));
		}
		strcat(szDispBuf,szStatBuf);
	}

#ifdef _DEBUG
	StatisticsToString(pRakStats,szStatBuf,2);
#else
	StatisticsToString(pRakStats,szStatBuf,1);
#endif
	strcat(szDispBuf,szStatBuf);

	//m_pD3DDevice->GetDisplayMode(0,&dDisplayMode);

	SIZE size = pDefaultFont->MeasureText(szDispBuf);

	RECT rect;
	rect.top		= 10;
	rect.right		= pGame->GetScreenWidth() - 150;
	rect.left		= 10;
	rect.bottom		= rect.top + 16;

	PCHAR pBuf = szDispBuf;
	
	int x=0;

	pDefaultFont->RenderText("Client Network Stats",rect,0xFF0000AA);
	rect.top += 16;
	rect.bottom += 16;

	while(*pBuf) {
		szDrawLine[x] = *pBuf;
		if(szDrawLine[x] == '\n') {
			szDrawLine[x] = '\0';
			pDefaultFont->RenderText(szDrawLine,rect,0xFFFFFFFF);
			rect.top += 16;
			rect.bottom += 16;
			x=0;
		} else {
			x++;
		}
		pBuf++;
	}
}
