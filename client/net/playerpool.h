//----------------------------------------------------------
//
// SA:MP Multiplayer Modification For GTA:SA
// Copyright 2004-2005 SA:MP team
//
// Version: $Id: playerpool.h,v 1.10 2006/04/09 09:54:45 kyeman Exp $
//
//----------------------------------------------------------

#pragma once

//----------------------------------------------------

class CPlayerPool
{
private:

	CLocalPlayer	*m_pLocalPlayer;
	BYTE			m_byteLocalPlayerID;

	bool			m_bPlayerSlotState[MAX_PLAYERS];
	CRemotePlayer	*m_pPlayers[MAX_PLAYERS];

public:
	// Process All CPlayers
	bool Process();

	CLocalPlayer * GetLocalPlayer() { return m_pLocalPlayer; };
	BYTE FindRemotePlayerIDFromGtaPtr(PED_TYPE * pActor);

	bool New(BYTE bytePlayerID, PCHAR szPlayerName);
	bool Delete(BYTE bytePlayerID, BYTE byteReason);

	CRemotePlayer* GetAt(unsigned short usPlayerID) {
		if (usPlayerID < MAX_PLAYERS) {
			return m_pPlayers[usPlayerID];
		}
		return NULL;
	};

	// Find out if the slot is inuse.
	bool GetSlotState(unsigned short usPlayerID) {
		if (usPlayerID < MAX_PLAYERS) {
			return m_bPlayerSlotState[usPlayerID];
		}
		return false;
	};
	
	void SetLocalPlayerID(BYTE byteID) {
		m_byteLocalPlayerID = byteID;
	};

	BYTE GetLocalPlayerID() { return m_byteLocalPlayerID; };

	BYTE GetCount();

	int GetLocalPlayerScore() {
		return m_pLocalPlayer->m_iScore;
	};

	// Used in scoreboard.cpp, and array index gets checked by GetSlotState()
	int GetPlayerScore(BYTE bytePlayerId) {
		return m_pPlayers[bytePlayerId]->m_iScore;
	};

	void UpdateScore(unsigned short usPlayerId, int iScore)
	{ 
		if (usPlayerId > MAX_PLAYERS - 1)
			return;

		if (usPlayerId == m_byteLocalPlayerID) {
			m_pLocalPlayer->m_iScore = iScore;
		} else {
			if (m_pPlayers[usPlayerId] != NULL)
				m_pPlayers[usPlayerId]->m_iScore = iScore;
		}
	};

	unsigned short GetLocalPlayerPing() {
		return m_pLocalPlayer->m_usPing;
	};

	// Used in netrpc.cpp at UpdatePings(), and player index is checked by GetSlotState()
	void UpdatePing(unsigned short usPlayerId, unsigned short usPing) {
		if (usPlayerId == m_byteLocalPlayerID) {
			m_pLocalPlayer->m_usPing = usPing;
		} else {
			if (m_pPlayers[usPlayerId] != NULL)
				m_pPlayers[usPlayerId]->m_usPing = usPing;
		}
	};

	// Used in scoreboard.cpp, and player index gets checked by GetSlotState()
	unsigned short GetPlayerPing(BYTE bytePlayerId)
	{
		return m_pPlayers[bytePlayerId]->m_usPing;
	};

	void DeactivateAll();

	CPlayerPool();
	~CPlayerPool();
};

//----------------------------------------------------