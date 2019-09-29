/*

	SA:MP Multiplayer Modification
	Copyright 2004-2005 SA:MP Team

    Version: $Id: vehiclepool.cpp,v 1.10 2006/04/12 19:26:45 mike Exp $

*/

#include "main.h"

//----------------------------------------------------

CVehiclePool::CVehiclePool()
{
	// loop through and initialize all net players to null and slot states to false
	for(VEHICLEID VehicleID = 0; VehicleID != MAX_VEHICLES; VehicleID++) {
		m_bVehicleSlotState[VehicleID] = FALSE;
		m_pVehicles[VehicleID] = NULL;
	}
}

//----------------------------------------------------

CVehiclePool::~CVehiclePool()
{	
	for(VEHICLEID VehicleID = 0; VehicleID != MAX_VEHICLES; VehicleID++) {
		Delete(VehicleID);
	}
}

//----------------------------------------------------

VEHICLEID CVehiclePool::New(int iVehicleType,
					   VECTOR * vecPos, float fRotation,
					   int iColor1, int iColor2, int iRespawnDelay)
{
	VEHICLEID VehicleID;

	for(VehicleID=1; VehicleID != MAX_VEHICLES; VehicleID++)
	{
		if(m_bVehicleSlotState[VehicleID] == FALSE) break;
	}

	if(VehicleID == MAX_VEHICLES) return 0xFFFF;		

	m_pVehicles[VehicleID] = new CVehicle(iVehicleType,vecPos,fRotation,iColor1,iColor2,iRespawnDelay);

	if(m_pVehicles[VehicleID])
	{
		m_pVehicles[VehicleID]->SetID(VehicleID);
		m_bVehicleSlotState[VehicleID] = TRUE;
		m_byteVirtualWorld[VehicleID] = 0;

		return VehicleID;
	}
	else
	{
		return 0xFFFF;
	}
}

//----------------------------------------------------

BOOL CVehiclePool::Delete(VEHICLEID VehicleID)
{
	if(!GetSlotState(VehicleID) || !m_pVehicles[VehicleID])
	{
		return FALSE; // Vehicle already deleted or not used.
	}

	m_bVehicleSlotState[VehicleID] = FALSE;
	delete m_pVehicles[VehicleID];
	m_pVehicles[VehicleID] = NULL;

	return TRUE;
}

//----------------------------------------------------

void CVehiclePool::Process(float fElapsedTime)
{
	for (int i=0; i<MAX_VEHICLES; i++)
	{
		if (GetSlotState(i) == TRUE)
		{
			GetAt(i)->Process(fElapsedTime);
		}
	}
}

//----------------------------------------------------

void CVehiclePool::InitForPlayer(BYTE bytePlayerID)
{	
	// Spawn all existing vehicles for player.
	CVehicle *pVehicle;
	VEHICLEID x=0;

	while(x!=MAX_VEHICLES) {
		if(GetSlotState(x) == TRUE) {
			pVehicle = GetAt(x);
			if(pVehicle->IsActive()) pVehicle->SpawnForPlayer(bytePlayerID);
		}
		x++;
	}
}

//----------------------------------------------------

void CVehiclePool::SetVehicleVirtualWorld(VEHICLEID VehicleID, BYTE byteVirtualWorld)
{
	if (VehicleID >= MAX_VEHICLES) return;
	
	m_byteVirtualWorld[VehicleID] = byteVirtualWorld;
	// Tell existing players it's changed
	RakNet::BitStream bsData;
	bsData.Write(VehicleID); // player id
	bsData.Write(byteVirtualWorld); // vw id
	RakServerInterface *pRak = pNetGame->GetRakServer();
	pRak->RPC(RPC_ScrSetVehicleVirtualWorld , &bsData, HIGH_PRIORITY, RELIABLE, 0, UNASSIGNED_PLAYER_ID, true, false);
}
	
//----------------------------------------------------
