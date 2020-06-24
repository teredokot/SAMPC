//----------------------------------------------------------
//
// SA:MP Multiplayer Modification For GTA:SA
// Copyright 2004-2005 SA:MP team
//
// Version: $Id: remoteplayer.h,v 1.20 2006/05/07 15:38:36 kyeman Exp $
//
//----------------------------------------------------------

#pragma once

//----------------------------------------------------

class CRemotePlayer
{
private:
	CPlayerPed			*m_pPlayerPed;
	char				m_szName[MAX_PLAYER_NAME];
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
	bool				m_bPassengerDriveByMode;
	DWORD				m_dwLastHeadUpdate;
	DWORD				m_dwStreamUpdate;

	bool				m_bShowNameTag;
	bool				m_bVisible;

	float				m_fReportedHealth;
	float				m_fReportedArmour;
	DWORD				m_dwWaitForEntryExitAnims;
	
	int					m_iIsInAModShop;
	int					m_iVirtualWorld;
	int					m_iScore;
	unsigned short		m_usPing;

public:

	CRemotePlayer();
	~CRemotePlayer();

	void SetName(const char* szName);
	const char* GetName();

	BYTE GetState() { return m_byteState; };

	// Process this player during the server loop.
	void Process(int iLocalWorld);
	void ProcessSpecialActions(BYTE byteSpecialAction);

	void HandleVehicleEntryExit();
	void HandlePlayerPedStreaming();

	void ForceOutOfCurrentVehicle();

	bool IsSurfingOrTurretMode();
	void UpdateSurfing();

	void Say(unsigned char * szText);

	void SetID(BYTE bytePlayerID) { m_bytePlayerID = bytePlayerID; };
	BYTE GetID() { return m_bytePlayerID; };
	CPlayerPed * GetPlayerPed() { return m_pPlayerPed; };

	bool IsActive() {
		if(m_byteState != PLAYER_STATE_NONE && m_byteState != PLAYER_STATE_SPECTATING) {
			return true;
		}
		return false;
	};

	void Deactivate() {
		m_byteState = PLAYER_STATE_NONE;
		m_bShowNameTag = true;
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

	bool Spawn(BYTE byteTeam, int iSkin, VECTOR * vecPos, float fRotation, DWORD dwColor);

	void HandleDeath();
	void ResetAllSyncAttributes();

	float GetDistanceFromRemotePlayer(CRemotePlayer *pFromPlayer);
	float GetDistanceFromLocalPlayer();

	void SetPlayerColor(DWORD dwColor);
	DWORD GetPlayerColorAsRGBA();
	DWORD GetPlayerColorAsARGB();
	float GetReportedHealth() { return m_fReportedHealth; };
	float GetReportedArmour() { return m_fReportedArmour; };

	void EnterVehicle(VEHICLEID VehicleID, bool bPassenger);
	void ExitVehicle();
	
	void SetVirtualWorld(int iWorld) { m_iVirtualWorld = iWorld; };
	
	BYTE GetTeam() { return m_byteTeam; };
	void SetTeam(BYTE byteTeam) { m_byteTeam = byteTeam; };
	void ShowNameTag(BYTE byteShow) { if (byteShow) m_bShowNameTag = true; else m_bShowNameTag = false; };

	void StateChange(BYTE byteNewState, BYTE byteOldState);

	void SetState(BYTE byteState) {	
		if(byteState != m_byteState) {
			StateChange(byteState,m_byteState);
			m_byteState = byteState;	
		}
	};
};

//----------------------------------------------------