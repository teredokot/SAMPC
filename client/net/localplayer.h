//----------------------------------------------------------
//
// SA:MP Multiplayer Modification For GTA:SA
// Copyright 2004-2005 SA:MP team
//
// Version: $Id: localplayer.h,v 1.27 2006/05/08 17:35:55 kyeman Exp $
//
//----------------------------------------------------------

#pragma once

enum eWeaponState
{
	WS_NO_BULLETS = 0,
	WS_LAST_BULLET = 1,
	WS_MORE_BULLETS = 2,
	WS_RELOADING = 3,
};

//----------------------------------------------------------

class CLocalPlayer
{
public:

	CPlayerPed				*m_pPlayerPed;
	BOOL					m_bIsActive;
	BOOL					m_bIsWasted;
	BOOL					m_bWantsAnotherClass;
	int						m_iSelectedClass;
	BOOL					m_bWaitingForSpawnRequestReply;
	BYTE					m_byteVirtualWorld;

	BYTE					m_byteSpectateMode;
	BYTE					m_byteSpectateType;
	DWORD					m_SpectateID; // Vehicle or player id
	BOOL					m_bSpectateProcessed;
	VEHICLEID				m_CurrentVehicle;
	VEHICLEID				m_LastVehicle;
	int						m_iDisplayZoneTick;
	
	BYTE					m_byteLastWeapon[13];
	DWORD					m_dwLastAmmo[13];

	short m_sNormalOnfootRate;
	short m_sNormalIncarRate;
private:

	PLAYER_SPAWN_INFO		m_SpawnInfo;
	BOOL					m_bHasSpawnInfo;
	ULONG					m_ulThisSyncFrame;
	ULONG					m_ulLastSyncFrame;
	BOOL					m_bPassengerDriveByMode;
	BYTE					m_byteCurInterior;
	BOOL					m_bInRCMode;

	// SPAWNING STUFF
	BOOL					m_bClearedToSpawn;
	DWORD					m_dwLastSpawnSelectionTick;// delays left and right selection
	DWORD					m_dwInitialSelectionTick;// delays initial selection
	BOOL					m_bIsSpectating;
	BYTE					m_byteTeam;

	BOOL					m_bSurfingMode;
	VECTOR					m_vecLockedSurfingOffsets;
	VEHICLEID				m_SurfingID;

	WORD					m_wLastKeys;
	DWORD					m_dwLastSendTick;
	DWORD					m_dwLastSendSpecTick;
	DWORD					m_dwLastAimSendTick;
	DWORD					m_dwLastStatsUpdateTick;
	DWORD					m_dwLastHeadUpdate;

	CHAR					m_szPlayerName[256];

public:

	CLocalPlayer();
	~CLocalPlayer(){};

	BOOL IsActive() { return m_bIsActive; };
	BOOL IsWasted() { return m_bIsWasted; };

	void HandlePassengerEntry();
	BOOL Process();
	BOOL DestroyPlayer();
	
	BYTE GetSpecialAction();
	void ApplySpecialAction(BYTE byteSpecialAction);

	void UpdateSurfing();

	void SendOnFootFullSyncData();
	void SendInCarFullSyncData();
	void SendPassengerFullSyncData();
	void SendAimSyncData();
	void ResetAllSyncAttributes();

	int  GetOptimumInCarSendRate(int iPlayersEffected);
	int  GetOptimumOnFootSendRate(int iPlayersEffected);
	
	void SendWastedNotification();
	
	void RequestClass(int iClass);
	void RequestSpawn();

	void SetSpawnInfo(PLAYER_SPAWN_INFO *pSpawn);

	BOOL Spawn();

	CPlayerPed * GetPlayerPed() { return m_pPlayerPed; };

	void Say(PCHAR szText);

	void SendExitVehicleNotification(VEHICLEID VehicleID);
	void SendEnterVehicleNotification(VEHICLEID VehicleID,BOOL bPassenger);
	
	void SetPlayerColor(DWORD dwColor);
	DWORD GetPlayerColorAsRGBA();
	DWORD GetPlayerColorAsARGB();
	void ProcessOnFootWorldBounds();
	void ProcessInCarWorldBounds();

	void SendStatsUpdate();
	void UpdateRemoteInterior(BYTE byteInterior);

	void HandleClassSelectionOutcome(BOOL bOutcome);
	void HandleClassSelection();
	void ProcessClassSelection();
	BOOL IsClearedToSpawn() { return m_bClearedToSpawn; };
	
	void CheckWeapons();
	void SetVirtualWorld(BYTE byteWorld) { m_byteVirtualWorld = byteWorld; };
	BYTE GetVirtualWorld() { return m_byteVirtualWorld; };

	void ToggleSpectating(BOOL bToggle);
	void SpectateVehicle(VEHICLEID VehicleID);
	void SpectatePlayer(BYTE bytePlayerID);
	void ProcessSpectating();
	BOOL IsSpectating() { return m_bIsSpectating; };
	void ReturnToClassSelection() { m_bWantsAnotherClass = TRUE; };

	BYTE GetTeam() { return m_byteTeam; };
	void SetTeam(BYTE byteTeam) { m_byteTeam = byteTeam; };

	BOOL IsInRCMode() { return m_bInRCMode; };

	int DetermineNumberOfPlayersInLocalRange();
};

//----------------------------------------------------------
