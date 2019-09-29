//----------------------------------------------------------
//
// SA:MP Multiplayer Modification For GTA:SA
// Copyright 2004-2005 SA:MP team
//
// Version: $Id: playerpool.h,v 1.10 2006/04/09 09:54:45 kyeman Exp $
//
//----------------------------------------------------------

#pragma once

#define INVALID_PLAYER_ID 255
#define NO_TEAM 255

//----------------------------------------------------
#pragma pack(1)
class CPlayerPool
{
private:
	
	BOOL			m_bPlayerSlotState[MAX_PLAYERS];
	BYTE			m_byteLocalPlayerID;
	CHAR			m_szLocalPlayerName[MAX_PLAYER_NAME+1];
	CHAR			m_szPlayerNames[MAX_PLAYERS][MAX_PLAYER_NAME+1];

	int	  m_iLocalPlayerScore;
	int	  m_iPlayerScores[MAX_PLAYERS];
	DWORD m_dwLocalPlayerPing;
	DWORD m_dwPlayerPings[MAX_PLAYERS];
	ULONG m_ulIPAddresses[MAX_PLAYERS];
	
public:
	// Process All CPlayers
	BOOL Process();

	void SetLocalPlayerName(PCHAR szName) { strcpy(m_szLocalPlayerName,szName); };
	PCHAR GetLocalPlayerName() { return m_szLocalPlayerName; };
	PCHAR GetPlayerName(BYTE bytePlayerID) { return m_szPlayerNames[bytePlayerID]; };

	void SetPlayerName(BYTE bytePlayerID, PCHAR szName) {
		strcpy(m_szPlayerNames[bytePlayerID], szName);
	}

	BYTE FindRemotePlayerIDFromGtaPtr(PED_TYPE * pActor);

	BOOL New(BYTE bytePlayerID, PCHAR szPlayerName);
	BOOL Delete(BYTE bytePlayerID, BYTE byteReason);

	// Find out if the slot is inuse.
	BOOL GetSlotState(BYTE bytePlayerID) {
		if(bytePlayerID > MAX_PLAYERS) { return FALSE; }
		return m_bPlayerSlotState[bytePlayerID];
	};
	
	void SetLocalPlayerID(BYTE byteID) {
		strcpy(m_szPlayerNames[byteID],m_szLocalPlayerName);
		m_byteLocalPlayerID = byteID;
	};

	BYTE GetLocalPlayerID() { return m_byteLocalPlayerID; };

	BYTE GetCount();

	void UpdateScore(BYTE bytePlayerId, int iScore)
	{ 
		if (bytePlayerId == m_byteLocalPlayerID)
		{
			m_iLocalPlayerScore = iScore;
		} else {
			if (bytePlayerId > MAX_PLAYERS-1) { return; }
			m_iPlayerScores[bytePlayerId] = iScore;
		}
	};

	void UpdatePing(BYTE bytePlayerId, DWORD dwPing) { 
		if (bytePlayerId == m_byteLocalPlayerID)
		{
			m_dwLocalPlayerPing = dwPing;
		} else {
			if (bytePlayerId > MAX_PLAYERS-1) { return; }
			m_dwPlayerPings[bytePlayerId] = dwPing;
		}
	};

	void UpdateIPAddress(BYTE bytePlayerId, ULONG ulIPAddress) {
		if (bytePlayerId > MAX_PLAYERS-1) { return; }
		m_ulIPAddresses[bytePlayerId] = ulIPAddress;
	}

	int GetLocalPlayerScore() {
		return m_iLocalPlayerScore;
	};

	DWORD GetLocalPlayerPing() {
		return m_dwLocalPlayerPing;
	};

	int GetPlayerScore(BYTE bytePlayerId) {
		if (bytePlayerId > MAX_PLAYERS-1) { return 0; }
		return m_iPlayerScores[bytePlayerId];
	};

	DWORD GetPlayerPing(BYTE bytePlayerId)
	{
		if (bytePlayerId > MAX_PLAYERS-1) { return 0; }
		return m_dwPlayerPings[bytePlayerId];
	};

	ULONG GetPlayerIP(BYTE bytePlayerId) {
		if (bytePlayerId > MAX_PLAYERS-1) { return 0; }
		return m_ulIPAddresses[bytePlayerId];
	};

	void DeactivateAll();

	CPlayerPool();
	~CPlayerPool();

};

//----------------------------------------------------