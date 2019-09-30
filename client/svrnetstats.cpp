//
// Version: $Id: svrnetstats.cpp $
//

#include "main.h"
#include <stdio.h>

extern CGame* pGame;
extern CNetGame* pNetGame;
extern GAME_SETTINGS tSettings;
extern CChatWindow *pChatWindow;
extern CFontRender *pDefaultFont;

char szSvrDispBuf[16384];
char szSvrStatBuf[16384];
char szSvrDrawLine[1024];
RakNetStatisticsStruct RakServerStats;

CSvrNetStats::CSvrNetStats(IDirect3DDevice9 *pD3DDevice)
{
	m_dwLastUpdateTick = GetTickCount();
	m_dwLastTotalBytesRecv = 0;
	m_dwLastTotalBytesSent = 0;
	m_dwBPSDownload = 0;
	m_dwBPSUpload = 0;
	m_pD3DDevice = pD3DDevice;
	memset(&RakServerStats,0,sizeof(RakNetStatisticsStruct));
}

void CSvrNetStats::Draw()
{
	return;

	float fDown,fUp;

	if((GetTickCount() - m_dwLastUpdateTick) > 1000) {
		m_dwLastUpdateTick = GetTickCount();

		m_dwBPSDownload = ((UINT)(RakServerStats.bitsReceived / 8)) - m_dwLastTotalBytesRecv;
		m_dwLastTotalBytesRecv = (UINT)(RakServerStats.bitsReceived / 8);
		m_dwBPSUpload = ((UINT)(RakServerStats.totalBitsSent / 8)) - m_dwLastTotalBytesSent;
		m_dwLastTotalBytesSent = (UINT)(RakServerStats.totalBitsSent / 8);

		RakNet::BitStream bsParams;
		pNetGame->GetRakClient()->RPC(RPC_SvrStats,&bsParams,HIGH_PRIORITY,UNRELIABLE,0,false);
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

	sprintf(szSvrDispBuf,"Download Rate: %.2f kbps\nUpload Rate: %.2f kbps\n",fDown,fUp);
	StatisticsToString(&RakServerStats,szSvrStatBuf,1);
	strcat(szSvrDispBuf,szSvrStatBuf);

	RECT rect;
	rect.top		= 10;
	rect.right		= pGame->GetScreenWidth() - 150;
	rect.left		= 10;
	rect.bottom		= rect.top + 16;

	PCHAR pBuf = szSvrDispBuf;
	
	int x=0;

	pDefaultFont->RenderText("Server Net Stats (Admin only)",rect,0xFFAA0000);
	rect.top += 16;
	rect.bottom += 16;

	while(*pBuf) {
		szSvrDrawLine[x] = *pBuf;
		if(szSvrDrawLine[x] == '\n') {
			szSvrDrawLine[x] = '\0';
			pDefaultFont->RenderText(szSvrDrawLine,rect,0xFFFFFFFF);
			rect.top += 16;
			rect.bottom += 16;
			x=0;
		} else {
			x++;
		}
		pBuf++;
	}
}
