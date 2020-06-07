/*

	SA:MP Multiplayer Modification
	Copyright 2004-2005 SA:MP Team

	file:
		vehicle.h
	desc:
		Vehicle handling header file.

    Version: $Id: vehicle.h,v 1.3 2006/04/12 19:26:45 mike Exp $

*/

#ifndef SAMPSRV_VEHICLE_H
#define SAMPSRV_VEHICLE_H

typedef struct _VEHICLE_SPAWN_INFO
{
	int iVehicleType;
	VECTOR vecPos;
	float fRotation;
	int iColor1;
	int iColor2;
	int iRespawnDelay;
	int iInterior;
} VEHICLE_SPAWN_INFO;

//----------------------------------------------------

class CVehicle
{
public:

	VEHICLEID				m_VehicleID;
	VEHICLEID				m_TrailerID;
	VEHICLEID				m_CabID;
	BYTE					m_byteDriverID;
	BYTE					m_bytePassengers[7];
	bool					m_bIsActive;
	bool					m_bIsWasted;
	VEHICLE_SPAWN_INFO		m_SpawnInfo;
	MATRIX4X4				m_matWorld;
	VECTOR					m_vecMoveSpeed;
	VECTOR					m_vecTurnSpeed;
	float					m_fHealth;
	bool					m_bDead;
	_CAR_MOD_INFO			m_CarModInfo;
	CHAR					m_szNumberPlate[9];
	bool					m_bDeathHasBeenNotified;
	bool					m_bHasBeenOccupied;
	DWORD					m_dwLastSeenOccupiedTick;
	DWORD					m_dwLastRespawnedTick;
	int						m_iVirtualWorld;

	bool m_bOnItsSide;
	bool m_bUpsideDown;
	bool m_bSirenOn;
	bool m_bWrecked;
	bool m_bSunked;

	void Process(float fElapsedTime);

	CVehicle(int iModel,VECTOR *vecPos,float fRotation,int iColor1,int iColor2, int iRespawnTime);
	~CVehicle(){};

	bool IsActive() { return m_bIsActive; };
	bool IsWasted() { return m_bIsWasted; };

	void SetID(VEHICLEID VehicleID) { m_VehicleID = VehicleID; };
	void SetCab(VEHICLEID VehicleID) { m_CabID = VehicleID; };
	VEHICLE_SPAWN_INFO * GetSpawnInfo() { return &m_SpawnInfo; };

	void SpawnForPlayer(BYTE byteForPlayerID);
	void SetVehicleInterior(int iIntSet) { m_SpawnInfo.iInterior = iIntSet; };
	void SetDead() { m_bDead = true; }; // Respawns due to death in ~10s
	void SetHealth(float fHealth);
	void SetNumberPlate(PCHAR Plate);
	void CheckForIdleRespawn();
	void Respawn();
	bool IsOccupied();
	bool IsATrainPart();

	void Update(BYTE bytePlayerID, MATRIX4X4 * matWorld, float fHealth, VEHICLEID TrailerID);

	float GetDistanceFromPoint(float fX, float fY, float fZ);

	void SetVirtualWorld(int iVirtualWorld);
};

#endif
