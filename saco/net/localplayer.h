//----------------------------------------------------------
//
// SA:MP Multiplayer Modification For GTA:SA
// Copyright 2004-2005 SA:MP team
//
// Version: $Id: localplayer.h,v 1.27 2006/05/08 17:35:55 kyeman Exp $
//
//----------------------------------------------------------

#pragma once

#define SPECTATE_TYPE_NONE				0
#define SPECTATE_TYPE_PLAYER			1
#define SPECTATE_TYPE_VEHICLE			2

#define SPECIAL_ACTION_NONE				0
#define SPECIAL_ACTION_USEJETPACK		2
#define SPECIAL_ACTION_DANCE1			5
#define SPECIAL_ACTION_DANCE2			6
#define SPECIAL_ACTION_DANCE3			7
#define SPECIAL_ACTION_DANCE4			8
#define SPECIAL_ACTION_HANDSUP			10
#define SPECIAL_ACTION_USECELLPHONE		11
#define SPECIAL_ACTION_SITTING			12
#define SPECIAL_ACTION_STOPUSECELLPHONE 13
#define SPECIAL_ACTION_URINATE			68

#pragma pack(1)
typedef struct _PLAYER_SPAWN_INFO
{
	BYTE byteTeam;
	int iSkin;
	VECTOR vecPos;
	float fRotation;
	int iSpawnWeapons[3];
	int iSpawnWeaponsAmmo[3];
} PLAYER_SPAWN_INFO;

#pragma pack(1)
typedef struct _ONFOOT_SYNC_DATA
{
	WORD lrAnalog;
	WORD udAnalog;
	WORD wKeys;
	VECTOR vecPos;
	float fRotation;
	BYTE byteHealth;
	BYTE byteArmour;
	BYTE byteCurrentWeapon;
	BYTE byteSpecialAction;	
	VECTOR vecMoveSpeed;
	VECTOR vecSurfOffsets;
	VEHICLEID SurfVehicleId;
} ONFOOT_SYNC_DATA;

enum eWeaponState
{
	WS_NO_BULLETS = 0,
	WS_LAST_BULLET = 1,
	WS_MORE_BULLETS = 2,
	WS_RELOADING = 3,
};

#pragma pack(1)
typedef struct _AIM_SYNC_DATA
{
	BYTE byteCamMode;
	BYTE byteCamExtZoom : 6;	// 0-63 normalized
	BYTE byteWeaponState : 2;	// see eWeaponState
	VECTOR vecAimf1;
	VECTOR vecAimf2;
	VECTOR vecAimPos;
	float fAimZ;
} AIM_SYNC_DATA;

#pragma pack(1)
typedef struct _INCAR_SYNC_DATA
{
	VEHICLEID VehicleID;
	WORD lrAnalog;
	WORD udAnalog;
	WORD wKeys;
	C_VECTOR1 cvecRoll;
	C_VECTOR1 cvecDirection;
	VECTOR vecPos;
	VECTOR vecMoveSpeed;
	float fCarHealth;
	BYTE bytePlayerHealth;
	BYTE bytePlayerArmour;
	BYTE byteCurrentWeapon;
	BYTE byteSirenOn;
	BYTE byteLandingGearState;
	BYTE byteTires[4];
	VEHICLEID TrailerID;
	DWORD dwHydraThrustAngle;
	FLOAT fTrainSpeed;
} INCAR_SYNC_DATA;

#pragma pack(1)
typedef struct _PASSENGER_SYNC_DATA
{
	VEHICLEID VehicleID;
	BYTE byteSeatFlags : 7;
	BYTE byteDriveBy : 1;
	BYTE byteCurrentWeapon;
	BYTE bytePlayerHealth;
	BYTE bytePlayerArmour;
	WORD lrAnalog;
	WORD udAnalog;
	WORD wKeys;
	VECTOR vecPos;
} PASSENGER_SYNC_DATA;

#pragma pack(1)
typedef struct _SPECTATOR_SYNC_DATA
{
	WORD lrAnalog;
	WORD udAnalog;
	WORD wKeys;
	VECTOR vecPos;
} SPECTATOR_SYNC_DATA;

#pragma pack(1)
typedef struct _TRAILER_SYNC_DATA
{
	C_VECTOR1 cvecRoll;
	C_VECTOR1 cvecDirection;
	VECTOR vecPos;
	VECTOR vecMoveSpeed;
} TRAILER_SYNC_DATA;

//----------------------------------------------------------

#pragma pack(1)
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
	void Msg(BYTE byteToPlayer, PCHAR szText);
	void TeamMsg(PCHAR szText);

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

typedef struct _CAR_MOD_INFO
{
	BYTE byteCarMod0;
	BYTE byteCarMod1;
	BYTE byteCarMod2;
	BYTE byteCarMod3;
	BYTE byteCarMod4;
	BYTE byteCarMod5;
	BYTE byteCarMod6;
	BYTE byteCarMod7;
	BYTE byteCarMod8;
	BYTE byteCarMod9;
	BYTE byteCarMod10;
	BYTE byteCarMod11;
	BYTE byteCarMod12;
	BYTE byteCarMod13;
	BYTE bytePaintJob;
	int iColor0;
	int iColor1;
} CAR_MOD_INFO;

//----------------------------------------------------------
