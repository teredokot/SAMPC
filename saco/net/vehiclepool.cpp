//----------------------------------------------------------
//
// SA:MP Multiplayer Modification For GTA:SA
// Copyright 2004-2005 SA:MP team
//
// Version: $Id: vehiclepool.cpp,v 1.29 2006/05/07 17:32:29 kyeman Exp $
//
//----------------------------------------------------------

#include "../main.h"
#include "../game/util.h"

extern CNetGame *pNetGame;
extern CGame *pGame;
extern CChatWindow *pChatWindow;
//----------------------------------------------------

CVehiclePool::CVehiclePool()
{
	// loop through and initialize all vehicle properties to 0
	for(VEHICLEID VehicleID = 0; VehicleID < MAX_VEHICLES; VehicleID++) {
		m_bVehicleSlotState[VehicleID] = FALSE;
		m_pVehicles[VehicleID] = NULL;
		m_pGTAVehicles[VehicleID] = NULL;
		m_byteVirtualWorld[VehicleID] = 0;
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

BOOL CVehiclePool::Spawn( VEHICLEID VehicleID, int iVehicleType,
					      VECTOR * vecPos, float fRotation,
					      int iColor1, int iColor2, int iInterior, PCHAR szNumberPlate, int iObjective,
						  int iDoorsLocked )
{	

	if(m_pVehicles[VehicleID] != NULL) {
		Delete(VehicleID);
	}

	m_pVehicles[VehicleID] = pGame->NewVehicle(iVehicleType,
		vecPos->X,vecPos->Y,vecPos->Z,fRotation, szNumberPlate);

	if(m_pVehicles[VehicleID])
	{	
		if(iColor1 != -1 || iColor2 != -1) {
			m_pVehicles[VehicleID]->SetColor(iColor1,iColor2);
		}

		m_pGTAVehicles[VehicleID] = m_pVehicles[VehicleID]->m_pVehicle;
		m_bVehicleSlotState[VehicleID] = TRUE;

		if(iObjective) m_pVehicles[VehicleID]->m_byteObjectiveVehicle = 1;
		if(iDoorsLocked) m_pVehicles[VehicleID]->SetDoorState(1);
		if (iInterior > 0)
		{
			LinkToInterior(VehicleID, iInterior);
		}

		m_bIsActive[VehicleID] = TRUE;
		m_bIsWasted[VehicleID] = FALSE;
		m_charNumberPlate[VehicleID][0] = 0;

		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

void CVehiclePool::LinkToInterior(VEHICLEID VehicleID, int iInterior)
{
	if(m_bVehicleSlotState[VehicleID]) {
		m_SpawnInfo[VehicleID].iInterior = iInterior;
		m_pVehicles[VehicleID]->LinkToInterior(iInterior);
	}
}

//----------------------------------------------------

void CVehiclePool::AssignSpecialParamsToVehicle(VEHICLEID VehicleID, BYTE byteObjective, BYTE byteDoorsLocked)
{
	if(!GetSlotState(VehicleID)) return;

	m_SpawnInfo[VehicleID].iObjective = byteObjective;
	m_SpawnInfo[VehicleID].iDoorsLocked = byteDoorsLocked;
	
	CVehicle *pVehicle = m_pVehicles[VehicleID];

	if(pVehicle && m_bIsActive[VehicleID]) {
		if (byteObjective)
		{
			pVehicle->m_byteObjectiveVehicle = 1;
			pVehicle->m_bSpecialMarkerEnabled = false;
		}
		pVehicle->SetDoorState(byteDoorsLocked);
	}
}

//----------------------------------------------------

VEHICLEID CVehiclePool::FindIDFromGtaPtr(VEHICLE_TYPE * pGtaVehicle)
{
	int x=1;
	
	while(x!=MAX_VEHICLES) {
		if(pGtaVehicle == m_pGTAVehicles[x]) return x;
		x++;
	}

	return INVALID_VEHICLE_ID;
}

//----------------------------------------------------

int CVehiclePool::FindGtaIDFromID(int iID)
{
	return GamePool_Vehicle_GetIndex(m_pGTAVehicles[iID]);
}

//----------------------------------------------------

int CVehiclePool::FindGtaIDFromGtaPtr(VEHICLE_TYPE * pGtaVehicle)
{
	return GamePool_Vehicle_GetIndex(pGtaVehicle);
}

//----------------------------------------------------

void CVehiclePool::ProcessForVirtualWorld(VEHICLEID vehicleId, BYTE bytePlayerWorld)
{
	BYTE byteVehicleVW = m_byteVirtualWorld[vehicleId];
	if (bytePlayerWorld != byteVehicleVW)
	{
		if(m_pVehicles[vehicleId]->m_dwMarkerID)
		{
			ScriptCommand(&disable_marker, m_pVehicles[vehicleId]->m_dwMarkerID);
			m_pVehicles[vehicleId]->m_dwMarkerID = 0;
		}
	}
}

//-----------------------------------------------------------

void CVehiclePool::Process()
{
	// Process all vehicles in the vehicle pool.
	CVehicle *pVehicle;
	DWORD dwThisTime = GetTickCount();
	CPlayerPool* pPlayerPool = pNetGame->GetPlayerPool();
	CLocalPlayer* pLocalPlayer = pPlayerPool->GetLocalPlayer();

	//if(!pLocalPlayer->IsActive()) return;
	
	BYTE localVW = 0;
	if (pLocalPlayer) localVW = pLocalPlayer->GetVirtualWorld();

	for(VEHICLEID x = 0; x != MAX_VEHICLES; x++)
	{
		if(GetSlotState(x) == TRUE)
		{
			// It's in use.
			pVehicle = m_pVehicles[x];

			if(m_bIsActive[x])
			{
				/*
				if(!pVehicle->IsOccupied()) {
					pVehicle->ProcessEngineAudio(0);
				}*/

				if(pVehicle->IsDriverLocalPlayer()) {
					pVehicle->SetInvulnerable(FALSE);
				} else {
					pVehicle->SetInvulnerable(TRUE);
				}

				if (pVehicle->GetHealth() == 0.0f) // || pVehicle->IsWrecked()) // It's dead
				{
					if (pLocalPlayer->m_LastVehicle == x) // Notify server of death
					{
						NotifyVehicleDeath(x);
					}
					continue;
				}
				
				// Peter: This caused every vehicle outside the worldbounds
				// that's not occupied to respawn every time this is called.

				/*if(pVehicle->HasExceededWorldBoundries(
					pNetGame->m_WorldBounds[0],pNetGame->m_WorldBounds[1],
					pNetGame->m_WorldBounds[2],pNetGame->m_WorldBounds[3]))
				{
					if (!pVehicle->IsOccupied()) {
						SetForRespawn(x);
						continue;
					}
				}*/

				if( pVehicle->GetVehicleSubtype() != VEHICLE_SUBTYPE_BOAT &&
					pVehicle->HasSunk() ) // Not boat and has sunk.
				{
					if (pLocalPlayer->m_LastVehicle == x) {
						NotifyVehicleDeath(x);
					}
					continue;
				}
				
				// Code to respawn vehicle after it has been idle for the amount of time specified
				pVehicle->UpdateLastDrivenTime();

				// Active and in world.

/*		
#ifdef _DEBUG
				CHAR szBuffer2[1024];
				if (!pVehicle->IsAdded() && pVehicle->GetDistanceFromLocalPlayerPed() < LOCKING_DISTANCE) {
					sprintf(szBuffer2, "Vehicle streamed into locking distance: %d:%u\n", x,m_byteVirtualWorld[x]);
					OutputDebugString(szBuffer2);
				}
				if (pVehicle->IsAdded() && pVehicle->GetDistanceFromLocalPlayerPed() >= LOCKING_DISTANCE) {
					sprintf(szBuffer2, "Vehicle streamed out of locking distance: %d:%u\n", x,m_byteVirtualWorld[x]);				
					OutputDebugString(szBuffer2);
				}
#endif */
				// Remove or Add vehicles as they leave/enter a radius around the player
				if( (pVehicle->GetDistanceFromLocalPlayerPed() < LOCKING_DISTANCE)
					&& m_byteVirtualWorld[x] == localVW ) {

					pVehicle->Add();
					//pVehicle->SetLockedState(0);
					

					CVehicle* pTrailer = pVehicle->GetTrailer();
					if (pTrailer && !pTrailer->IsAdded())
					{
						MATRIX4X4 matPos;
						pVehicle->GetMatrix(&matPos);
						pTrailer->TeleportTo(matPos.pos.X, matPos.pos.Y, matPos.pos.Z);
						pTrailer->Add();
					}

				} else {
					//pVehicle->SetLockedState(1);
					pVehicle->Remove();					
				}

				pVehicle->ProcessMarkers(); // car scanning shit

				/*
				if( (pVehicle->GetVehicleSubtype() == VEHICLE_SUBTYPE_PLANE ||
					pVehicle->GetVehicleSubtype() == VEHICLE_SUBTYPE_HELI) &&
					!pVehicle->IsOccupied() ) {
					pVehicle->SetEngineState(FALSE);
				}*/

				if(!pVehicle->HasADriver()) {
					pVehicle->SetHornState(0);
					pVehicle->SetEngineState(FALSE);
				}

				// Update the actual ingame pointer if it's not
				// the same as the one we have listed.
				if(pVehicle->m_pVehicle != m_pGTAVehicles[x]) {
					m_pGTAVehicles[x] = pVehicle->m_pVehicle;
				}
				// Put at the END so other processing is still done!
				ProcessForVirtualWorld(x, localVW);
			}
			else // !m_bIsActive
			{
				if(!pVehicle->IsOccupied()) {
					if(m_iRespawnDelay[x] > 0) {
						m_iRespawnDelay[x]--;
					}
					else {
#ifdef _DEBUG
						CHAR szBuffer2[1024];
						sprintf(szBuffer2, "Inactive vehicle getting respawned: %d\n", x);
						OutputDebugString(szBuffer2);
#endif
						Spawn(x,m_SpawnInfo[x].iVehicleType,&m_SpawnInfo[x].vecPos, m_SpawnInfo[x].fRotation,
							m_SpawnInfo[x].iColor1,m_SpawnInfo[x].iColor2,m_SpawnInfo[x].iInterior,m_charNumberPlate[x],m_SpawnInfo[x].iObjective,m_SpawnInfo[x].iDoorsLocked);
					}
				}	
			}			
		}
	} // end for each vehicle
}

//----------------------------------------------------

void CVehiclePool::SetForRespawn(VEHICLEID VehicleID, int iRespawnDelay)
{
	CVehicle *pVehicle = m_pVehicles[VehicleID];

	if(pVehicle) {
		m_bIsActive[VehicleID] = FALSE;
		m_bIsWasted[VehicleID] = TRUE;
		m_iRespawnDelay[VehicleID] = iRespawnDelay;
	}
}

//----------------------------------------------------

void CVehiclePool::NotifyVehicleDeath(VEHICLEID VehicleID)
{
	RakNet::BitStream bsDeath;
	bsDeath.Write(VehicleID);
	pNetGame->GetRakClient()->RPC(RPC_VehicleDestroyed, &bsDeath, HIGH_PRIORITY, RELIABLE_SEQUENCED, 0, false);
	pNetGame->GetPlayerPool()->GetLocalPlayer()->m_LastVehicle = 0xFFFF; // Mark as notification sent
}

//----------------------------------------------------

int CVehiclePool::FindNearestToLocalPlayerPed()
{
	float fLeastDistance=10000.0f;
	float fThisDistance;
	VEHICLEID ClosestSoFar=INVALID_VEHICLE_ID;

	VEHICLEID x=0;
	while(x < MAX_VEHICLES) {
		if(GetSlotState(x) && m_bIsActive[x]) {
			fThisDistance = m_pVehicles[x]->GetDistanceFromLocalPlayerPed();
			if(fThisDistance < fLeastDistance) {
				fLeastDistance = fThisDistance;
				ClosestSoFar = x;
			}
		}
		x++;
	}

	return ClosestSoFar;
}

//----------------------------------------------------