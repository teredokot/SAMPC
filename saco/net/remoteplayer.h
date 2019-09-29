//----------------------------------------------------------
//
// SA:MP Multiplayer Modification For GTA:SA
// Copyright 2004-2005 SA:MP team
//
// Version: $Id: remoteplayer.h,v 1.20 2006/05/07 15:38:36 kyeman Exp $
//
//----------------------------------------------------------

#pragma once

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


#define UPDATE_TYPE_NONE				0
#define UPDATE_TYPE_ONFOOT				1
#define UPDATE_TYPE_INCAR				2
#define UPDATE_TYPE_PASSENGER			3

#define VALID_KILL						1
#define TEAM_KILL						2
#define SELF_KILL						3

//----------------------------------------------------

#pragma pack(1)
class CRemotePlayer
{
private:
	CPlayerPed			*m_pPlayerPed;
	BYTE				m_bytePlayerID;
	BYTE				m_byteUpdateFromNetwork;

	ONFOOT_SYNC_DATA	m_ofSync;
	INCAR_SYNC_DATA		m_icSync;
	PASSENGER_SYNC_DATA m_psSync;
	AIM_SYNC_DATA		m_aimSync;
	TRAILER_SYNC_DATA	m_trSync;
	
public:

	VECTOR				m_vecOnfootTargetPos;
	VECTOR				m_vecOnfootTargetSpeed;
	VECTOR				m_vecIncarTargetPos;
	VECTOR				m_vecIncarTargetSpeed;
	VECTOR				m_vecPositionInaccuracy;
	VECTOR				m_vecReferencePosition;

	CVehicle			*m_pCurrentVehicle;

	BYTE				m_byteState;
	VEHICLEID			m_VehicleID;
	BYTE				m_byteSeatID;
	BYTE				m_byteTeam;
	BOOL				m_bPassengerDriveByMode;
	DWORD				m_dwLastHeadUpdate;
	DWORD				m_dwStreamUpdate;

	BOOL				m_bShowNameTag;
	BOOL				m_bVisible;

	float				m_fReportedHealth;
	float				m_fReportedArmour;
	DWORD				m_dwWaitForEntryExitAnims;
	
	int					m_iIsInAModShop;
	BYTE				m_byteVirtualWorld;

public:

	CRemotePlayer();
	~CRemotePlayer();

	BYTE GetState() { return m_byteState; };

	// Process this player during the server loop.
	void Process(BYTE byteLocalWorld);
	void ProcessSpecialActions(BYTE byteSpecialAction);

	void HandleVehicleEntryExit();
	void HandlePlayerPedStreaming();

	void ForceOutOfCurrentVehicle();

	BOOL IsSurfingOrTurretMode();
	void UpdateSurfing();

	void Say(unsigned char * szText);
	void Privmsg(PCHAR szText);
	void TeamPrivmsg(PCHAR szText);
	void SetID(BYTE bytePlayerID) { m_bytePlayerID = bytePlayerID; };
	BYTE GetID() { return m_bytePlayerID; };
	CPlayerPed * GetPlayerPed() { return m_pPlayerPed; };

	BOOL IsActive() {
		if(m_byteState != PLAYER_STATE_NONE && m_byteState != PLAYER_STATE_SPECTATING) {
			return TRUE;
		}
		return FALSE;
	};

	void Deactivate() {
		m_byteState = PLAYER_STATE_NONE;
		m_bShowNameTag = TRUE;
	};
	
	void UpdateIncarTargetPosition();
	void UpdateOnfootTargetPosition();

	void UpdateOnFootPositionAndSpeed(VECTOR * vecPos, VECTOR *vecMoveSpeed);
	void UpdateInCarMatrixAndSpeed(MATRIX4X4 mat, VECTOR vecMove);
	void UpdateTrainDriverMatrixAndSpeed(MATRIX4X4 *matWorld,VECTOR *vecMoveSpeed, float fTrainSpeed);
	void UpdateAimFromSyncData(AIM_SYNC_DATA *paimSync);
	void UpdateTrailerMatrixAndSpeed(CVehicle* Trailer, MATRIX4X4 mat);
		
	void StoreOnFootFullSyncData(ONFOOT_SYNC_DATA * pofSync);
	void StoreInCarFullSyncData(INCAR_SYNC_DATA * picSync);
	void StorePassengerFullSyncData(PASSENGER_SYNC_DATA *ppsSync);
	void StoreTrailerFullSyncData(TRAILER_SYNC_DATA* trSync);

	BOOL Spawn(BYTE byteTeam, int iSkin, VECTOR * vecPos, float fRotation, DWORD dwColor);

	void HandleDeath();
	void ResetAllSyncAttributes();

	float GetDistanceFromRemotePlayer(CRemotePlayer *pFromPlayer);
	float GetDistanceFromLocalPlayer();

	void SetPlayerColor(DWORD dwColor);
	DWORD GetPlayerColorAsRGBA();
	DWORD GetPlayerColorAsARGB();
	float GetReportedHealth() { return m_fReportedHealth; };
	float GetReportedArmour() { return m_fReportedArmour; };

	void EnterVehicle(VEHICLEID VehicleID, BOOL bPassenger);
	void ExitVehicle();
	
	void SetVirtualWorld(BYTE byteWorld) { m_byteVirtualWorld = byteWorld; };
	
	BYTE GetTeam() { return m_byteTeam; };
	void SetTeam(BYTE byteTeam) { m_byteTeam = byteTeam; };
	void ShowNameTag(BYTE byteShow) { if (byteShow) m_bShowNameTag = TRUE; else m_bShowNameTag = FALSE; };

	void StateChange(BYTE byteNewState, BYTE byteOldState);

	void SetState(BYTE byteState) {	
		if(byteState != m_byteState) {
			StateChange(byteState,m_byteState);
			m_byteState = byteState;	
		}
	};
};

//----------------------------------------------------