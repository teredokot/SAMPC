//----------------------------------------------------------
//
// SA:MP Multiplayer Modification For GTA:SA
// Copyright 2004-2005 SA:MP team
//
// Version: $Id: playerpool.cpp,v 1.14 2006/05/07 15:38:36 kyeman Exp $
//
//----------------------------------------------------------

#include "../main.h"

extern CGame		 *pGame;
extern CChatWindow   *pChatWindow;
extern CCmdWindow	 *pCmdWindow;

char szQuitReasons[][32] = {
"Timeout",
"Leaving",
"Kicked"
};

int iExceptPlayerMessageDisplayed=0;

//----------------------------------------------------

CPlayerPool::CPlayerPool()
{
	m_pLocalPlayer = new CLocalPlayer();

	m_iLocalPlayerScore = 0;
	m_dwLocalPlayerPing = 0;

	// loop through and initialize all net players to null and slot states to false
	for(BYTE bytePlayerID = 0; bytePlayerID < MAX_PLAYERS; bytePlayerID++) {
		m_bPlayerSlotState[bytePlayerID] = FALSE;
		m_pPlayers[bytePlayerID] = NULL;
		m_iPlayerScores[bytePlayerID] = 0;
		m_dwPlayerPings[bytePlayerID] = 0;
	}
}

//----------------------------------------------------

CPlayerPool::~CPlayerPool()
{
	delete m_pLocalPlayer;
	m_pLocalPlayer = NULL;

	for(BYTE bytePlayerID = 0; bytePlayerID < MAX_PLAYERS; bytePlayerID++) {
		Delete(bytePlayerID,0);
	}
}

//----------------------------------------------------

BOOL CPlayerPool::New(BYTE bytePlayerID, PCHAR szPlayerName)
{
	m_pPlayers[bytePlayerID] = new CRemotePlayer();

	if(m_pPlayers[bytePlayerID])
	{
		strcpy(m_szPlayerNames[bytePlayerID],szPlayerName);
		m_pPlayers[bytePlayerID]->SetID(bytePlayerID);
		m_bPlayerSlotState[bytePlayerID] = TRUE;
		//if(pChatWindow) 
			//pChatWindow->AddInfoMessage("*** %s joined the server.",szPlayerName);
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

//----------------------------------------------------

BOOL CPlayerPool::Delete(BYTE bytePlayerID, BYTE byteReason)
{
	if(!GetSlotState(bytePlayerID) || !m_pPlayers[bytePlayerID]) {
		return FALSE; // Player already deleted or not used.
	}

	if (m_pLocalPlayer && m_pLocalPlayer->IsSpectating() && m_pLocalPlayer->m_SpectateID == bytePlayerID) {
		m_pLocalPlayer->ToggleSpectating(FALSE);
	}

	m_bPlayerSlotState[bytePlayerID] = FALSE;
	delete m_pPlayers[bytePlayerID];
	m_pPlayers[bytePlayerID] = NULL;

	//if(pChatWindow) {
		//pChatWindow->AddInfoMessage("*** %s left the server. (%s)",
		//m_szPlayerNames[bytePlayerID],szQuitReasons[byteReason]);
	//}

	return TRUE;
}

//----------------------------------------------------

BOOL CPlayerPool::Process()
{
	// Process all CRemotePlayers
	BYTE localVW = 0;
	if (m_pLocalPlayer) localVW = m_pLocalPlayer->GetVirtualWorld();
	for(BYTE bytePlayerID = 0; bytePlayerID < MAX_PLAYERS; bytePlayerID++) {
		if(TRUE == m_bPlayerSlotState[bytePlayerID]) {
			
			try {
				m_pPlayers[bytePlayerID]->Process(localVW);
			} catch(...) {
				if(!iExceptPlayerMessageDisplayed) {
					pChatWindow->AddDebugMessage("Warning: Error Processing Player(%u)",bytePlayerID);
					//Delete(bytePlayerID,0);
					iExceptPlayerMessageDisplayed++;
				}
			}

			/*if (m_byteVirtualWorld[bytePlayerID] != localVW || m_pPlayers[bytePlayerID]->GetState() == PLAYER_STATE_SPECTATING){
				m_pPlayers[bytePlayerID]->HideForLocal();
			}
			else {
				// Just shows the radar marker if required
				m_pPlayers[bytePlayerID]->ShowForLocal();
			}*/
		}
	}

	// Process the LocalPlayer
	try {
		m_pLocalPlayer->Process();
	} catch(...) {
		if(!iExceptPlayerMessageDisplayed) {
			pChatWindow->AddDebugMessage("Warning: Error Processing Player");
			iExceptPlayerMessageDisplayed++;
		}
	}
	
	return TRUE;
}

//----------------------------------------------------

BYTE CPlayerPool::FindRemotePlayerIDFromGtaPtr(PED_TYPE * pActor)
{
	CPlayerPed *pPlayerPed;

	for(BYTE bytePlayerID = 0; bytePlayerID < MAX_PLAYERS; bytePlayerID++)
	{
		if(TRUE == m_bPlayerSlotState[bytePlayerID])
		{
			pPlayerPed = m_pPlayers[bytePlayerID]->GetPlayerPed();

			if(pPlayerPed) {
				PED_TYPE *pTestActor = pPlayerPed->GetGtaActor();
				if((pTestActor != NULL) && (pActor == pTestActor)) // found it
					return (BYTE)m_pPlayers[bytePlayerID]->GetID();
			}
		}
	}

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
	m_pLocalPlayer->m_bIsActive = FALSE;
	m_pLocalPlayer->m_iSelectedClass = 0;

	for(BYTE bytePlayerID = 0; bytePlayerID < MAX_PLAYERS; bytePlayerID++) {
		if(TRUE == m_bPlayerSlotState[bytePlayerID]) {
			m_pPlayers[bytePlayerID]->Deactivate();
		}
	}
}

//----------------------------------------------------
// EOF