/*

	SA:MP Multiplayer Modification
	Copyright 2004-2005 SA:MP Team

	file:
		playerpool.h
	desc:
		Player pool handling header file.

    Version: $Id: playerpool.h,v 1.12 2006/04/09 09:54:46 kyeman Exp $

*/

#ifndef SAMPSRV_PLAYERPOOL_H
#define SAMPSRV_PLAYERPOOL_H

#define INVALID_PLAYER_ID 255
#define NO_TEAM 255

#define VALID_KILL		  1
#define TEAM_KILL		  2
#define SELF_KILL		  3


//----------------------------------------------------
#pragma pack(1)
class CPlayerPool
{
private:
	
	BOOL	m_bPlayerSlotState[MAX_PLAYERS];
	CPlayer *m_pPlayers[MAX_PLAYERS];
	CHAR	m_szPlayerName[MAX_PLAYERS][MAX_PLAYER_NAME+1];
	int 	m_iPlayerScore[MAX_PLAYERS];
	int		m_iPlayerMoney[MAX_PLAYERS];
	DWORD	m_dwPlayerAmmo[MAX_PLAYERS];
	BOOL	m_bIsAnAdmin[MAX_PLAYERS];
	BYTE	m_byteVirtualWorld[MAX_PLAYERS];
	int		m_iPlayerCount;

public:
	
	CPlayerPool();
	~CPlayerPool();

	BOOL Process(float fElapsedTime);
	BOOL New(BYTE bytePlayerID, PCHAR szPlayerName);
	BOOL Delete(BYTE bytePlayerID, BYTE byteReason);
		
	// Retrieve a player
	CPlayer* GetAt(BYTE bytePlayerID) {
		if (bytePlayerID >= MAX_PLAYERS) { return NULL; }
		return m_pPlayers[bytePlayerID];
	};

	// Find out if the slot is inuse.
	BOOL GetSlotState(BYTE bytePlayerID) {
		if(bytePlayerID >= MAX_PLAYERS) { return FALSE; }
		return m_bPlayerSlotState[bytePlayerID];
	};

	PCHAR GetPlayerName(BYTE bytePlayerID) {
		if(bytePlayerID >= MAX_PLAYERS) { return FALSE; }
		return m_szPlayerName[bytePlayerID];
	};

	int GetPlayerScore(BYTE bytePlayerID) {
		if(bytePlayerID >= MAX_PLAYERS) { return FALSE; }
		return m_iPlayerScore[bytePlayerID];
	};

	void SetPlayerScore(BYTE bytePlayerID, int iScore) {
		if(bytePlayerID >= MAX_PLAYERS) return;
		m_iPlayerScore[bytePlayerID] = iScore;
	};

	void SetPlayerName(BYTE bytePlayerID, PCHAR szName) {
		strcpy(m_szPlayerName[bytePlayerID], szName);
	}

	int GetPlayerMoney(BYTE bytePlayerID) {
		if(bytePlayerID >= MAX_PLAYERS) { return FALSE; }
		return m_iPlayerMoney[bytePlayerID];
	};

	void SetPlayerMoney(BYTE bytePlayerID, int iMoney) {
		if(bytePlayerID >= MAX_PLAYERS) return;
		m_iPlayerMoney[bytePlayerID] = iMoney;
	};

	DWORD GetPlayerAmmo(BYTE bytePlayerID) {
		if(bytePlayerID >= MAX_PLAYERS) { return FALSE; }
		return m_dwPlayerAmmo[bytePlayerID];
	};

	void SetPlayerAmmo(BYTE bytePlayerID, DWORD dwAmmo) {
		if(bytePlayerID >= MAX_PLAYERS) return;
		m_dwPlayerAmmo[bytePlayerID] = dwAmmo;
	};

	void ResetPlayerScoresAndMoney() {
		memset(&m_iPlayerScore[0],0,sizeof(int) * MAX_PLAYERS);
		memset(&m_iPlayerMoney[0],0,sizeof(int) * MAX_PLAYERS);	
		memset(&m_byteVirtualWorld[0],0,sizeof(BYTE) * MAX_PLAYERS);	
	};
	
	void SetPlayerVirtualWorld(BYTE bytePlayerID, BYTE byteVirtualWorld);
	
	BYTE GetPlayerVirtualWorld(BYTE bytePlayerID) {
		if (bytePlayerID >= MAX_PLAYERS) { return 0; }
		return m_byteVirtualWorld[bytePlayerID];		
	};

	void SetAdmin(BYTE bytePlayerID) { m_bIsAnAdmin[bytePlayerID] = TRUE; };
	BOOL IsAdmin(BYTE bytePlayerID) { return m_bIsAnAdmin[bytePlayerID]; };

	void InitPlayersForPlayer(BYTE bytePlayerID);
	void InitSpawnsForPlayer(BYTE bytePlayerID);

	BYTE GetKillType(BYTE byteWhoKilled, BYTE byteWhoDied);

	float GetDistanceFromPlayerToPlayer(BYTE bytePlayer1, BYTE bytePlayer2);
	float GetDistanceSquaredFromPlayerToPlayer(BYTE bytePlayer1, BYTE bytePlayer2);
	BOOL  IsNickInUse(PCHAR szNick);

	int GetPlayerCount() { return m_iPlayerCount; };

	void DeactivateAll();

};

//----------------------------------------------------

#endif