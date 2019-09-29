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

#pragma pack(1)
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

#pragma pack(1)
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

//----------------------------------------------------

#pragma pack(1)
class CVehicle
{
public:

	VEHICLEID				m_VehicleID;
	VEHICLEID				m_TrailerID;
	VEHICLEID				m_CabID;
	BYTE					m_byteDriverID;
	BYTE					m_bytePassengers[7];
	BOOL					m_bIsActive;
	BOOL					m_bIsWasted;
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

	void Process(float fElapsedTime);

	CVehicle(int iModel,VECTOR *vecPos,float fRotation,int iColor1,int iColor2, int iRespawnTime);
	~CVehicle(){};

	BOOL IsActive() { return m_bIsActive; };
	BOOL IsWasted() { return m_bIsWasted; };

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
	BOOL IsOccupied();
	bool IsATrainPart();

	void Update(BYTE bytePlayerID, MATRIX4X4 * matWorld, float fHealth, VEHICLEID TrailerID);
};

#endif
