//----------------------------------------------------------
//
// SA:MP Multiplayer Modification For GTA:SA
// Copyright 2004-2005 SA:MP team
//
// Version: $Id: playerpool.cpp,v 1.14 2006/05/07 15:38:36 kyeman Exp $
//
//----------------------------------------------------------

#include "../main.h"

char szQuitReasons[][32] = {
"Timeout",
"Leaving",
"Kicked"
};

//----------------------------------------------------

CPlayerPool::CPlayerPool()
{
	m_iLocalPlayerScore = 0;
	m_dwLocalPlayerPing = 0;

	// loop through and initialize all net players to null and slot states to false
	for(BYTE bytePlayerID = 0; bytePlayerID < MAX_PLAYERS; bytePlayerID++) {
		m_bPlayerSlotState[bytePlayerID] = FALSE;
		m_iPlayerScores[bytePlayerID] = 0;
		m_dwPlayerPings[bytePlayerID] = 0;
	}
}

//----------------------------------------------------

CPlayerPool::~CPlayerPool()
{
	for(BYTE bytePlayerID = 0; bytePlayerID < MAX_PLAYERS; bytePlayerID++) {
		Delete(bytePlayerID,0);
	}
}

//----------------------------------------------------

BOOL CPlayerPool::New(BYTE bytePlayerID, PCHAR szPlayerName)
{
	return TRUE;
}

//----------------------------------------------------

BOOL CPlayerPool::Delete(BYTE bytePlayerID, BYTE byteReason)
{
	m_bPlayerSlotState[bytePlayerID] = FALSE;
	return TRUE;
}

//----------------------------------------------------

BOOL CPlayerPool::Process()
{
	return TRUE;
}

//----------------------------------------------------

BYTE CPlayerPool::FindRemotePlayerIDFromGtaPtr(PED_TYPE * pActor)
{
	return INVALID_PLAYER_ID;	
}

//----------------------------------------------------

BYTE CPlayerPool::GetCount()
{
	BYTE byteCount=0;
	for(BYTE bytePlayerID = 0; bytePlayerID < MAX_PLAYERS; bytePlayerID++) {
		if(TRUE == m_bPlayerSlotState[bytePlayerID]) {
			byteCount++;
		}
	}
	return byteCount;
}

//----------------------------------------------------

void CPlayerPool::DeactivateAll()
{
}

//----------------------------------------------------
// EOF