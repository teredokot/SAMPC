/*

	SA:MP Multiplayer Modification
	Copyright 2004-2005 SA:MP Team

    Version: $Id: player.h,v 1.30 2006/05/07 15:35:32 kyeman Exp $

*/

#ifndef SAMPSRV_PLAYER_H
#define SAMPSRV_PLAYER_H

#define PLAYER_STATE_NONE						0
#define PLAYER_STATE_ONFOOT						1
#define PLAYER_STATE_DRIVER						2
#define PLAYER_STATE_PASSENGER					3
#define PLAYER_STATE_EXIT_VEHICLE				4
#define PLAYER_STATE_ENTER_VEHICLE_DRIVER		5
#define PLAYER_STATE_ENTER_VEHICLE_PASSENGER	6
#define PLAYER_STATE_WASTED						7
#define PLAYER_STATE_SPAWNED					8
#define PLAYER_STATE_SPECTATING					9

#define UPDATE_TYPE_NONE		0
#define UPDATE_TYPE_ONFOOT		1
#define UPDATE_TYPE_INCAR		2
#define UPDATE_TYPE_PASSENGER	3

#define SPECTATE_TYPE_NONE		0
#define SPECTATE_TYPE_PLAYER	1
#define SPECTATE_TYPE_VEHICLE	2

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

// Packs vehicle health into a byte
#define PACK_VEHICLE_HEALTH(f)		(BYTE)(f / 4)
#define UNPACK_VEHICLE_HEALTH(b)	(float)b * 4

#pragma pack(1)
typedef struct _C_VECTOR1 {
	// New format, 24 bits for each of X, Y, Z = 72 bits/9 bytes
	char data[9];

	// Old format
	// short X,Y,Z;
} C_VECTOR1;

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

//----------------------------------------------------

#pragma pack(1)
class CPlayer
{
private:
	BYTE					m_bytePlayerID;
	BYTE					m_byteUpdateFromNetwork;

	// Information that is synced.
	ONFOOT_SYNC_DATA		m_ofSync;
	INCAR_SYNC_DATA			m_icSync;
	PASSENGER_SYNC_DATA		m_psSync;
	AIM_SYNC_DATA			m_aimSync;
	SPECTATOR_SYNC_DATA		m_spSync;
	TRAILER_SYNC_DATA		m_trSync;

	WORD					m_wLastKeys;

	BOOL					m_bHasAimUpdates;
	BOOL					m_bHasTrailerUpdates;
	BYTE					m_byteSeatID;
	BYTE					m_byteState;

	VECTOR					m_vecCheckpoint;
	float					m_fCheckpointSize;
	BOOL					m_bInCheckpoint;

	VECTOR					m_vecRaceCheckpoint;
	VECTOR					m_vecRaceNextCheckpoint;
	BYTE					m_byteRaceCheckpointType;
	float					m_fRaceCheckpointSize;
	BOOL					m_bInRaceCheckpoint;
public:
	PLAYER_SPAWN_INFO		m_SpawnInfo;
	BOOL					m_bHasSpawnInfo;
	BYTE					m_byteWantedLevel;

	VEHICLEID				m_VehicleID;
	DWORD					m_dwColor;
	BOOL					m_bCheckpointEnabled;
	BOOL					m_bRaceCheckpointEnabled;
	int						m_iInteriorId;
		
	// Weapon data
	DWORD					m_dwSlotAmmo[13];
	BYTE					m_byteSlotWeapon[13];
	
	BYTE					m_byteTime; // Uses
	float					m_fGameTime; // Time in seconds (game minutes)

	BYTE					m_byteSpectateType;
	DWORD					m_SpectateID; // Vehicle or player id

	ONFOOT_SYNC_DATA* GetOnFootSyncData() { return &m_ofSync; }
	INCAR_SYNC_DATA* GetInCarSyncData() { return &m_icSync; }
	PASSENGER_SYNC_DATA* GetPassengerSyncData() { return &m_psSync; }
	AIM_SYNC_DATA* GetAimSyncData() { return &m_aimSync; }
	SPECTATOR_SYNC_DATA* GetSpectatorSyncData() { return &m_spSync; }

	void SetState(BYTE byteState);
	BYTE GetState() { return m_byteState; };

	CPlayer();
	~CPlayer() {};

	float	m_fHealth;
	float	m_fArmour;
	VECTOR  m_vecPos;
	float	m_fRotation;
	BOOL	m_bCanTeleport;

	BOOL IsActive() { 
		if( m_byteState != PLAYER_STATE_NONE && m_byteState != PLAYER_STATE_SPECTATING ) { return TRUE; }
		return FALSE;
	};
	
	void Deactivate() {	
		m_bHasSpawnInfo = FALSE;
		memset(&m_SpawnInfo,0,sizeof(PLAYER_SPAWN_INFO));
		m_dwColor = 0;
		m_byteState = PLAYER_STATE_NONE;
		m_byteTime = 0;
		m_fGameTime = 720.0f; // 12 o'clock in minutes	
		m_fHealth = 0.0f;
		m_fArmour = 0.0f;
	};

	void UpdatePosition(float x, float y, float z);

	// Process this player during the server loop.
	void Process(float fElapsedTime);
	void BroadcastSyncData();
	void Say(unsigned char * szText, BYTE byteTextLength);
	void Privmsg(BYTE byteToPlayerID, unsigned char * szText, BYTE byteTextLength);
	void TeamPrivmsg(unsigned char * szText, BYTE byteTextLength);
	void SetID(BYTE bytePlayerID) { m_bytePlayerID = bytePlayerID; };
	
	void StoreOnFootFullSyncData(ONFOOT_SYNC_DATA * pofSync);
	void StoreInCarFullSyncData(INCAR_SYNC_DATA * picSync);
	void StorePassengerFullSyncData(PASSENGER_SYNC_DATA *ppsSync);
	void StoreSpectatorFullSyncData(SPECTATOR_SYNC_DATA *pspSync);
	void StoreAimSyncData(AIM_SYNC_DATA *paimSync);
	void StoreTrailerFullSyncData(TRAILER_SYNC_DATA* trSync);
	void SetSpawnInfo(PLAYER_SPAWN_INFO *pSpawn);

	PLAYER_SPAWN_INFO * GetSpawnInfo() { return &m_SpawnInfo; };

	void HandleDeath(BYTE byteReason, BYTE byteWhoWasResponsible);
	void Spawn();
	void SpawnForWorld(BYTE byteTeam, int iSkin, VECTOR * vecPos, float fRotation);
	void SpawnForPlayer(BYTE byteForPlayerID);

	void EnterVehicle(VEHICLEID VehicleID,BYTE bytePassenger);
	void ExitVehicle(VEHICLEID VehicleID);

	void SetPlayerColor(DWORD dwColor);
	DWORD GetPlayerColor() { return m_dwColor; };

	void SetCheckpoint(float fX, float fY, float fZ, float fSize);
	void ToggleCheckpoint(BOOL bEnabled);
	void SetRaceCheckpoint(int iType, float fX, float fY, float fZ, float fNX, float fNY, float fNZ, float fSize);
	void ToggleRaceCheckpoint(BOOL bEnabled);

	BOOL IsInCheckpoint() { return m_bInCheckpoint; };
	BOOL IsInRaceCheckpoint() { return m_bInRaceCheckpoint; };
	BYTE GetTeam() { return m_SpawnInfo.byteTeam; };
	BYTE GetCurrentWeapon() { return m_ofSync.byteCurrentWeapon; };
	
	//WEAPON_SLOT_TYPE* GetWeaponSlotsData();
	void SetWeaponSlot(BYTE byteSlot, DWORD dwWeapon, DWORD dwAmmo);
	
	DWORD GetSlotWeapon(BYTE bSlot) { return m_byteSlotWeapon[bSlot]; };
	DWORD GetSlotAmmo(BYTE bSlot) { return m_dwSlotAmmo[bSlot]; };
	void SetCurrentWeaponAmmo(DWORD dwAmmo);
	void SetWantedLevel(BYTE byteLevel) { m_byteWantedLevel = byteLevel; };
	BYTE GetWantedLevel() { return m_byteWantedLevel; };
	
	void SetTime(BYTE byteHour, BYTE byteMinute);
	void SetClock(BYTE byteClock);

	BYTE CheckWeapon(BYTE weapon);
	void CheckKeyUpdatesForScript(WORD wKeys);

	BYTE GetSpecialAction() {
		if(GetState() == PLAYER_STATE_ONFOOT) return m_ofSync.byteSpecialAction;
		return SPECIAL_ACTION_NONE;
	};
};

#endif

//----------------------------------------------------
// EOF

