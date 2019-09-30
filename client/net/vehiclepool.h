//----------------------------------------------------------
//
// SA:MP Multiplayer Modification For GTA:SA
// Copyright 2004-2005 SA:MP team
//
// Version: $Id: vehiclepool.h,v 1.10 2006/05/07 17:32:29 kyeman Exp $
//
//----------------------------------------------------------

#pragma once

#define INVALID_VEHICLE_ID	0xFFFF

#pragma pack(1)
typedef struct _VEHICLE_SPAWN_INFO
{
	int iVehicleType;
	VECTOR vecPos;
	float fRotation;
	int iColor1;
	int iColor2;
	int iObjective;
	int iDoorsLocked;
	int iInterior;
} VEHICLE_SPAWN_INFO;

//----------------------------------------------------

#pragma pack(1)
class CVehiclePool
{
public:
	
	BOOL				m_bVehicleSlotState[MAX_VEHICLES];
	CVehicle			*m_pVehicles[MAX_VEHICLES];
	VEHICLE_TYPE		*m_pGTAVehicles[MAX_VEHICLES]; // pointers to actual ingame vehicles.

	BOOL				m_bIsActive[MAX_VEHICLES];
	BOOL				m_bIsWasted[MAX_VEHICLES];
	VEHICLE_SPAWN_INFO	m_SpawnInfo[MAX_VEHICLES];
	
	int					m_iRespawnDelay[MAX_VEHICLES];
	BYTE				m_byteVirtualWorld[MAX_VEHICLES];
	CHAR				m_charNumberPlate[MAX_VEHICLES][9];

	CVehiclePool();
	~CVehiclePool();

	BOOL New(VEHICLEID VehicleID, int iVehicleType,
			 VECTOR * vecPos, float fRotation,
			 int iColor1, int iColor2,
			 VECTOR * vecSpawnPos, float fSpawnRotation,
			 int iInterior, PCHAR szNumberPlate);

	BOOL Delete(VEHICLEID VehicleID);	
	
	// Retrieve a vehicle
	CVehicle* GetAt(VEHICLEID VehicleID) {
		if(VehicleID >= MAX_VEHICLES || !m_bVehicleSlotState[VehicleID]) { return NULL; }
		return m_pVehicles[VehicleID];
	};

	// Find out if the slot is inuse.
	BOOL GetSlotState(VEHICLEID VehicleID) {
		if(VehicleID >= MAX_VEHICLES) { return FALSE; }
		return m_bVehicleSlotState[VehicleID];
	};

	BOOL Spawn( VEHICLEID VehicleID, int iVehicleType,
				VECTOR * vecPos, float fRotation,
				int iColor1, int iColor2, int iInterior, PCHAR szNumberPlate, 
				int iObjective = 0, int iDoorsLocked = 0 );

		void ProcessForVirtualWorld(VEHICLEID vehicleId, BYTE bytePlayerWorld);
	void Process();
	
	void NotifyVehicleDeath(VEHICLEID VehicleID);

	int FindNearestToLocalPlayerPed();

	void AssignSpecialParamsToVehicle(VEHICLEID VehicleID, BYTE byteObjective, BYTE byteDoorsLocked);
	
	BYTE GetVehicleVirtualWorld(VEHICLEID VehicleID) {
		if (VehicleID >= MAX_VEHICLES) { return 0; }
		return m_byteVirtualWorld[VehicleID];		
	};
	
	void SetVehicleVirtualWorld(VEHICLEID VehicleID, BYTE byteVirtualWorld) {
		if (VehicleID >= MAX_VEHICLES) return;
		m_byteVirtualWorld[VehicleID] = byteVirtualWorld;
	};
	
	void SetForRespawn(VEHICLEID VehicleID, int iRespawnDelay = 1);
	void LinkToInterior(VEHICLEID VehicleID, int iInterior);

	VEHICLEID FindIDFromGtaPtr(VEHICLE_TYPE * pGtaVehicle);
	int FindGtaIDFromID(int iID);
	int FindGtaIDFromGtaPtr(VEHICLE_TYPE * pGtaVehicle);
};

//----------------------------------------------------
