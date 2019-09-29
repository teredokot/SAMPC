//----------------------------------------------------------
//
// SA:MP Multiplayer Modification For GTA:SA
// Copyright 2004-2005 SA:MP team
//
//
//----------------------------------------------------------

#include "../main.h"
extern CNetGame *pNetGame;

//----------------------------------------------------

CVehiclePool::CVehiclePool()
{
	// loop through and initialize all net players to null and slot states to false
	for(VEHICLEID VehicleID = 0; VehicleID < MAX_VEHICLES; VehicleID++) {
		m_bVehicleSlotState[VehicleID] = FALSE;
	}
}

//----------------------------------------------------

CVehiclePool::~CVehiclePool()
{
	for(VEHICLEID VehicleID = 0; VehicleID < MAX_VEHICLES; VehicleID++) {
		Delete(VehicleID);
	}
}

//----------------------------------------------------

BOOL CVehiclePool::New( VEHICLEID VehicleID, int iVehicleType,
					    VECTOR * vecPos, float fRotation,
					    int iColor1, int iColor2,
					    VECTOR * vecSpawnPos, float fSpawnRotation, /*int iRespawnDelay,*/
						int iInterior, PCHAR szNumberPlate )
{
	memset(&m_SpawnInfo[VehicleID],0,sizeof(VEHICLE_SPAWN_INFO));

	// Setup the spawninfo for the next respawn.
	m_SpawnInfo[VehicleID].iVehicleType = iVehicleType;
	m_SpawnInfo[VehicleID].vecPos.X = vecSpawnPos->X;
	m_SpawnInfo[VehicleID].vecPos.Y = vecSpawnPos->Y;
	m_SpawnInfo[VehicleID].vecPos.Z = vecSpawnPos->Z;
	m_SpawnInfo[VehicleID].fRotation = fSpawnRotation;
	m_SpawnInfo[VehicleID].iColor1 = iColor1;
	m_SpawnInfo[VehicleID].iColor2 = iColor2;
	m_byteVirtualWorld[VehicleID] = 0;

	// Now go ahead and spawn it at the location we got passed.
	return Spawn(VehicleID,iVehicleType,vecPos,fRotation,iColor1,iColor2,iInterior,szNumberPlate);
}

//----------------------------------------------------

BOOL CVehiclePool::Delete(VEHICLEID VehicleID)
{
	return TRUE;
}

//----------------------------------------------------

BOOL CVehiclePool::Spawn( VEHICLEID VehicleID, int iVehicleType,
					      VECTOR * vecPos, float fRotation,
					      int iColor1, int iColor2, int iInterior, PCHAR szNumberPlate, int iObjective,
						  int iDoorsLocked )
{	
	return TRUE;
}

void CVehiclePool::LinkToInterior(VEHICLEID VehicleID, int iInterior)
{
}

//----------------------------------------------------

void CVehiclePool::AssignSpecialParamsToVehicle(VEHICLEID VehicleID, BYTE byteObjective, BYTE byteDoorsLocked)
{
}

//----------------------------------------------------

VEHICLEID CVehiclePool::FindIDFromGtaPtr(VEHICLE_TYPE * pGtaVehicle)
{
	return INVALID_VEHICLE_ID;
}

//----------------------------------------------------

int CVehiclePool::FindGtaIDFromID(int iID)
{
	return 0;
}

//----------------------------------------------------

int CVehiclePool::FindGtaIDFromGtaPtr(VEHICLE_TYPE * pGtaVehicle)
{
	return 0;
}


//-----------------------------------------------------------

void CVehiclePool::Process()
{	
}

//----------------------------------------------------

void CVehiclePool::SetForRespawn(VEHICLEID VehicleID, int iRespawnDelay)
{
}

//----------------------------------------------------

void CVehiclePool::NotifyVehicleDeath(VEHICLEID VehicleID)
{
}

//----------------------------------------------------

int CVehiclePool::FindNearestToLocalPlayerPed()
{
	return 0;
}

//----------------------------------------------------