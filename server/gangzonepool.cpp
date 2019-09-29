/*

SA:MP Multiplayer Modification
Copyright 2004-2005 SA:MP Team

Version: $Id: gangzonepool.cpp,v 1.0 2007/05/25 19:26:45 Y_Less Exp $

Based on original hook by Peter

*/

#include "main.h"
extern CNetGame *pNetGame;

#define RGBA_ABGR(n) (((n >> 24) & 0x000000FF) | ((n >> 8) & 0x0000FF00) | ((n << 8) & 0x00FF0000) | ((n << 24) & 0xFF000000))

//----------------------------------------------------

CGangZonePool::CGangZonePool()
{
	for (WORD wZone = 0; wZone < MAX_GANG_ZONES; wZone++)
	{
		m_bSlotState[wZone] = FALSE;
	}
}

WORD CGangZonePool::New(float fMinX, float fMinY, float fMaxX, float fMaxY)
{
	WORD wZone = 0;
	while (wZone < MAX_GANG_ZONES)
	{
		if (!m_bSlotState[wZone]) break;
		wZone++;
	}
	if (wZone == MAX_GANG_ZONES) return 0xFFFF;
	m_fGangZone[wZone][0] = fMinX;
	m_fGangZone[wZone][1] = fMinY;
	m_fGangZone[wZone][2] = fMaxX;
	m_fGangZone[wZone][3] = fMaxY;
	m_bSlotState[wZone] = TRUE;
	return wZone;
}

void CGangZonePool::Delete(WORD wZone)
{
	m_bSlotState[wZone] = FALSE;
	RakNet::BitStream bsParams;
	bsParams.Write(wZone);
	pNetGame->GetRakServer()->RPC(RPC_ScrRemoveGangZone, &bsParams, HIGH_PRIORITY, RELIABLE, 0, UNASSIGNED_PLAYER_ID, true, false);
}

void CGangZonePool::ShowForPlayer(BYTE bytePlayer, WORD wZone, DWORD dwColor)
{
	RakServerInterface* pRak = pNetGame->GetRakServer();
	RakNet::BitStream bsParams;
	bsParams.Write(wZone);
	bsParams.Write(m_fGangZone[wZone][0]);
	bsParams.Write(m_fGangZone[wZone][1]);
	bsParams.Write(m_fGangZone[wZone][2]);
	bsParams.Write(m_fGangZone[wZone][3]);
	dwColor = RGBA_ABGR(dwColor);
	bsParams.Write(dwColor);
	pRak->RPC(RPC_ScrAddGangZone, &bsParams, HIGH_PRIORITY, RELIABLE, 0, pRak->GetPlayerIDFromIndex(bytePlayer), false, false);
}

void CGangZonePool::ShowForAll(WORD wZone, DWORD dwColor)
{
	RakNet::BitStream bsParams;
	bsParams.Write(wZone);
	bsParams.Write(m_fGangZone[wZone][0]);
	bsParams.Write(m_fGangZone[wZone][1]);
	bsParams.Write(m_fGangZone[wZone][2]);
	bsParams.Write(m_fGangZone[wZone][3]);
	dwColor = RGBA_ABGR(dwColor);
	bsParams.Write(dwColor);
	pNetGame->GetRakServer()->RPC(RPC_ScrAddGangZone, &bsParams, HIGH_PRIORITY, RELIABLE, 0, UNASSIGNED_PLAYER_ID, true, false);
}

void CGangZonePool::HideForPlayer(BYTE bytePlayer, WORD wZone)
{
	RakServerInterface* pRak = pNetGame->GetRakServer();
	RakNet::BitStream bsParams;
	bsParams.Write(wZone);
	pRak->RPC(RPC_ScrRemoveGangZone, &bsParams, HIGH_PRIORITY, RELIABLE, 0, pRak->GetPlayerIDFromIndex(bytePlayer), false, false);
}

void CGangZonePool::HideForAll(WORD wZone)
{
	RakNet::BitStream bsParams;
	bsParams.Write(wZone);
	pNetGame->GetRakServer()->RPC(RPC_ScrRemoveGangZone, &bsParams, HIGH_PRIORITY, RELIABLE, 0, UNASSIGNED_PLAYER_ID, true, false);
}

void CGangZonePool::FlashForPlayer(BYTE bytePlayer, WORD wZone, DWORD dwColor)
{
	RakServerInterface* pRak = pNetGame->GetRakServer();
	RakNet::BitStream bsParams;
	bsParams.Write(wZone);
	dwColor = RGBA_ABGR(dwColor);
	bsParams.Write(dwColor);
	pRak->RPC(RPC_ScrFlashGangZone, &bsParams, HIGH_PRIORITY, RELIABLE, 0, pRak->GetPlayerIDFromIndex(bytePlayer), false, false);
}

void CGangZonePool::FlashForAll(WORD wZone, DWORD dwColor)
{
	RakNet::BitStream bsParams;
	bsParams.Write(wZone);
	dwColor = RGBA_ABGR(dwColor);
	bsParams.Write(dwColor);
	pNetGame->GetRakServer()->RPC(RPC_ScrFlashGangZone, &bsParams, HIGH_PRIORITY, RELIABLE, 0, UNASSIGNED_PLAYER_ID, true, false);
}

void CGangZonePool::StopFlashForPlayer(BYTE bytePlayer, WORD wZone)
{
	RakServerInterface* pRak = pNetGame->GetRakServer();
	RakNet::BitStream bsParams;
	bsParams.Write(wZone);
	pRak->RPC(RPC_ScrStopFlashGangZone, &bsParams, HIGH_PRIORITY, RELIABLE, 0, pRak->GetPlayerIDFromIndex(bytePlayer), false, false);
}

void CGangZonePool::StopFlashForAll(WORD wZone)
{
	RakNet::BitStream bsParams;
	bsParams.Write(wZone);
	pNetGame->GetRakServer()->RPC(RPC_ScrStopFlashGangZone, &bsParams, HIGH_PRIORITY, RELIABLE, 0, UNASSIGNED_PLAYER_ID, true, false);
}
