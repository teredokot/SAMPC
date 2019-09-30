//----------------------------------------------------------
//
// SA:MP Multiplayer Modification For GTA:SA
// Copyright 2004-2005 SA:MP team
//
//----------------------------------------------------------

#pragma once

#define INVALID_PLAYER_ID 255

//----------------------------------------------------

class CPlayerPool
{
private:
	
	BOOL m_bPlayerSlotState[MAX_PLAYERS];
	CRemotePlayer *m_pPlayers[MAX_PLAYERS];
	CHAR	m_szPlayerName[MAX_PLAYERS][MAX_PLAYER_NAME+1];

public:
	
	CPlayerPool();
	~CPlayerPool();

	// Process All CNetPlayers
	BOOL Process();

	// Add a new CNetPlayerRemote.
	BOOL AddPlayer(BYTE bytePlayerID, PCHAR szPlayerName);

	// Delete a CNetPlayerRemote.
	BOOL DeletePlayer(BYTE bytePlayerID);	
		
	// Retrieve a player
	CPlayer* GetPlayer(BYTE bytePlayerID) {
		if(bytePlayerID > MAX_PLAYERS) { return NULL; }
		return m_pPlayers[bytePlayerID];
	};

	// Find out if the slot is inuse.
	BOOL GetPlayerSlotState(BYTE bytePlayerID) {
		if(bytePlayerID > MAX_PLAYERS) { return FALSE; }
		return m_bPlayerSlotState[bytePlayerID];
	};
};

//----------------------------------------------------