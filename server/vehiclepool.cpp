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
		m_bVehicleSlotState[VehicleID] = false;
		m_pVehicles[VehicleID] = NULL;
		if (VehicleID < 212)
			m_usVehicleModelsUsed[VehicleID] = 0;
	}
	m_iLastVehicleId = -1;
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

	if (!IsVehicleModelIdValid(iVehicleType))
		return 0xFFFF;

	for(VehicleID=1; VehicleID != MAX_VEHICLES; VehicleID++)
	{
		if(m_bVehicleSlotState[VehicleID] == false) break;
	}

	if(VehicleID == MAX_VEHICLES) return 0xFFFF;		

	m_pVehicles[VehicleID] = new CVehicle(iVehicleType,vecPos,fRotation,iColor1,iColor2,iRespawnDelay);

	if(m_pVehicles[VehicleID])
	{
		m_pVehicles[VehicleID]->SetID(VehicleID);
		m_bVehicleSlotState[VehicleID] = true;
		//m_byteVirtualWorld[VehicleID] = 0;

		m_iLastVehicleId = -1;
		for (int i = 0; i < MAX_VEHICLES; i++)
		{
			if (m_bVehicleSlotState[i])
			{
				m_iLastVehicleId = i;
			}
		}

		m_usVehicleModelsUsed[iVehicleType - 400]++;

		return VehicleID;
	}
	else
	{
		return 0xFFFF;
	}
}

//----------------------------------------------------

bool CVehiclePool::Delete(VEHICLEID VehicleID)
{
	if(!GetSlotState(VehicleID) || !m_pVehicles[VehicleID])
	{
		return false; // Vehicle already deleted or not used.
	}

	m_usVehicleModelsUsed[m_pVehicles[VehicleID]->m_SpawnInfo.iVehicleType - 400]--;

	m_bVehicleSlotState[VehicleID] = false;
	delete m_pVehicles[VehicleID];
	m_pVehicles[VehicleID] = NULL;

	m_iLastVehicleId = -1;
	for (int i = 0; i < MAX_VEHICLES; i++)
	{
		if (m_bVehicleSlotState[i])
		{
			m_iLastVehicleId = i;
		}
	}
	return true;
}

//----------------------------------------------------

void CVehiclePool::Process(float fElapsedTime)
{
	for (int i=0; i<MAX_VEHICLES; i++)
	{
		if (GetSlotState(i))
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
		if(GetSlotState(x)) {
			pVehicle = GetAt(x);
			if(pVehicle->IsActive()) pVehicle->SpawnForPlayer(bytePlayerID);
		}
		x++;
	}
}

//----------------------------------------------------

/*void CVehiclePool::SetVehicleVirtualWorld(VEHICLEID VehicleID, BYTE byteVirtualWorld)
{
	if (VehicleID >= MAX_VEHICLES) return;
	
	m_byteVirtualWorld[VehicleID] = byteVirtualWorld;
	// Tell existing players it's changed
	RakNet::BitStream bsData;
	bsData.Write(VehicleID); // player id
	bsData.Write(byteVirtualWorld); // vw id
	RakServerInterface *pRak = pNetGame->GetRakServer();
	pRak->RPC(RPC_ScrSetVehicleVirtualWorld , &bsData, HIGH_PRIORITY, RELIABLE, 0, UNASSIGNED_PLAYER_ID, true, false);
}*/
	
//----------------------------------------------------
