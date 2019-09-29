//----------------------------------------------------------
//
// SA:MP Multiplayer Modification For GTA:SA
// Copyright 2004-2005 SA:MP team
//
// Version: $Id: vehicle.cpp,v 1.32 2006/05/08 20:33:58 kyeman Exp $
//
//----------------------------------------------------------


#include <windows.h>
#include <math.h>
#include <stdio.h>

#include "../main.h"
#include "util.h"
#include "keystuff.h"

extern CGame		*pGame;
extern CNetGame		*pNetGame;
extern CChatWindow  *pChatWindow;
extern BOOL			bAllowVehicleCreation;

DWORD	dwLastCreatedVehicleID=0;
DWORD	dwNumVehicles=0;

//-----------------------------------------------------------
// CONSTRUCTOR

CVehicle::CVehicle( int iType, float fPosX, float fPosY,
					float fPosZ, float fRotation, PCHAR szNumberPlate)
{	
	DWORD dwRetID=0;

	m_pVehicle = 0;
	m_dwGTAId = 0;
	m_pTrailer = NULL;

	if( (iType != TRAIN_PASSENGER_LOCO) &&
		(iType != TRAIN_FREIGHT_LOCO) &&
		(iType != TRAIN_PASSENGER) &&
		(iType != TRAIN_FREIGHT) &&
		(iType != TRAIN_TRAM)) {

		// NORMAL VEHICLE
		if(!pGame->IsModelLoaded(iType)) {
			pGame->RequestModel(iType);
			pGame->LoadRequestedModels();
			while(!pGame->IsModelLoaded(iType)) Sleep(5);
		}

		if (szNumberPlate && szNumberPlate[0]) 
			ScriptCommand(&set_car_numberplate, iType, szNumberPlate);

		ScriptCommand(&create_car,iType,fPosX,fPosY,fPosZ+0.5f,&dwRetID);
		ScriptCommand(&set_car_z_angle,dwRetID,fRotation);
		ScriptCommand(&car_gas_tank_explosion,dwRetID,0);
		ScriptCommand(&set_car_hydraulics,dwRetID,0);
		ScriptCommand(&toggle_car_tires_vulnerable,dwRetID,0);
		
		//LinkToInterior(m_byteInterior);

		m_pVehicle = GamePool_Vehicle_GetAt(dwRetID);
		m_pEntity = (ENTITY_TYPE *)m_pVehicle; 
		m_dwGTAId = dwRetID;
		dwLastCreatedVehicleID = dwRetID;
		m_pVehicle->dwDoorsLocked = 0;
		m_bIsLocked = FALSE;
		
		Remove(); // They'll be added manually during pool processing.
		pGame->RemoveModel(iType);						
	}
	else if( (iType == TRAIN_PASSENGER_LOCO) || 
		(iType == TRAIN_FREIGHT_LOCO) ||
		(iType == TRAIN_TRAM)) {

		// TRAIN LOCOMOTIVES

		if(iType == TRAIN_PASSENGER_LOCO) iType = 5;
		else if(iType == TRAIN_FREIGHT_LOCO) iType = 3;
		else if(iType == TRAIN_TRAM) iType = 9;

		DWORD dwDirection=0;
		if(fRotation != 0.0f) {
			dwDirection = 1;
		}
		pGame->RequestModel(TRAIN_PASSENGER_LOCO);
		pGame->RequestModel(TRAIN_PASSENGER);
		pGame->RequestModel(TRAIN_FREIGHT_LOCO);
		pGame->RequestModel(TRAIN_FREIGHT);
		pGame->RequestModel(TRAIN_TRAM);
		pGame->LoadRequestedModels();
		while(!pGame->IsModelLoaded(TRAIN_PASSENGER_LOCO)) Sleep(1);
		while(!pGame->IsModelLoaded(TRAIN_PASSENGER)) Sleep(1);
		while(!pGame->IsModelLoaded(TRAIN_FREIGHT_LOCO)) Sleep(1);
		while(!pGame->IsModelLoaded(TRAIN_FREIGHT)) Sleep(1);
		while(!pGame->IsModelLoaded(TRAIN_TRAM)) Sleep(1);
	
		ScriptCommand(&create_train,iType,fPosX,fPosY,fPosZ,dwDirection,&dwRetID);

		m_pVehicle = GamePool_Vehicle_GetAt(dwRetID);
		m_pEntity = (ENTITY_TYPE *)m_pVehicle; 
		m_dwGTAId = dwRetID;
		dwLastCreatedVehicleID = dwRetID;

		GamePrepareTrain(m_pVehicle);
		//ScriptCommand(&set_train_flag, &dwRetID, 0);

		//pGame->RemoveModel(TRAIN_PASSENGER_LOCO);
		//pGame->RemoveModel(TRAIN_PASSENGER);
		//pGame->RemoveModel(TRAIN_FREIGHT_LOCO);
		//pGame->RemoveModel(TRAIN_FREIGHT);
	}
	else if((iType == TRAIN_PASSENGER) ||
			(iType == TRAIN_FREIGHT) ) {

		dwRetID = (((dwLastCreatedVehicleID >> 8) + 1) << 8) + 1; // holy shift
		m_pVehicle = GamePool_Vehicle_GetAt(dwRetID);
		m_pEntity = (ENTITY_TYPE *)m_pVehicle; 
		m_dwGTAId = dwRetID;
		///
		
		dwLastCreatedVehicleID = dwRetID;
		//ScriptCommand(&set_train_flag, &dwRetID, 0);
	}
		
	m_bIsInvulnerable = FALSE;
	m_byteObjectiveVehicle = 0;
	m_bSpecialMarkerEnabled = FALSE;
	m_dwMarkerID = 0;
	m_bHasBeenDriven = FALSE;
	m_dwTimeSinceLastDriven = GetTickCount();
	m_bDoorsLocked = FALSE;
}

//-----------------------------------------------------------
// DESTRUCTOR

CVehicle::~CVehicle() 
{
	m_pVehicle = GamePool_Vehicle_GetAt(m_dwGTAId);
	if(m_pVehicle) {
		if(m_dwMarkerID) {
			ScriptCommand(&disable_marker, m_dwMarkerID);
			m_dwMarkerID = 0;
		}
		RemoveEveryoneFromVehicle();
		ScriptCommand(&destroy_car,m_dwGTAId);
	}
}

//-----------------------------------------------------------
// OVERLOADED ADD

void CVehicle::Add()
{
	if (!IsAdded()) {
		// Call underlying Add
		CEntity::Add();

		// Process stuff for trailers
		CVehicle *pTrailer = this->GetTrailer();
		if(pTrailer) pTrailer->Add();
	}
}

//-----------------------------------------------------------
// OVERLOADED REMOVE

void CVehicle::Remove()
{
	if (IsAdded()) {
		// Process stuff for trailers
		CVehicle *pTrailer = this->GetTrailer();
		if(pTrailer) pTrailer->Remove();

		// Call underlying Remove
		CEntity::Remove();
	}
}

//-----------------------------------------------------------

void CVehicle::ProcessEngineAudio(BYTE byteDriverID)
{
	DWORD dwVehicle = (DWORD)m_pVehicle;

	if(byteDriverID != 0) {
		// We need to context switch the keys
		GameStoreLocalPlayerKeys();
		GameSetRemotePlayerKeys(byteDriverID);
	}

	if(m_pVehicle && IsAdded()) {
		_asm mov esi, dwVehicle
		_asm lea ecx, [esi+312]
		_asm mov edx, 0x502280
		_asm call edx
	}

	if(byteDriverID != 0) {
		GameSetLocalPlayerKeys();
	}
}

//-----------------------------------------------------------

void CVehicle::LinkToInterior(int iInterior)
{
	if(GamePool_Vehicle_GetAt(m_dwGTAId)) {
		ScriptCommand(&link_vehicle_to_interior, m_dwGTAId, iInterior);
		//m_byteInterior = iInterior;
	}
}

//-----------------------------------------------------------
// If the game has internally destroyed the vehicle
// during this frame, the vehicle pointer should become 0

void CVehicle::ResetPointers()
{
	m_pVehicle = GamePool_Vehicle_GetAt(m_dwGTAId);
	m_pEntity = (ENTITY_TYPE *)m_pVehicle;
}

//-----------------------------------------------------------
// RECREATE

void CVehicle::Recreate()
{
	UINT		uiType;
	MATRIX4X4   mat;
	BYTE		byteColor1,byteColor2;
	DWORD		dwRetID;

	if(m_pVehicle) {
		// Save the existing info.
		GetMatrix(&mat);
		uiType = GetModelIndex();
		byteColor1 = m_pVehicle->byteColor1;
		byteColor2 = m_pVehicle->byteColor1;

		ScriptCommand(&destroy_car,m_dwGTAId);

		if(!pGame->IsModelLoaded(uiType)) {
			pGame->RequestModel(uiType);
			pGame->LoadRequestedModels();
			while(!pGame->IsModelLoaded(uiType)) Sleep(5);
		}

		ScriptCommand(&create_car,uiType,mat.pos.X,mat.pos.Y,mat.pos.Z,&dwRetID);
		ScriptCommand(&car_gas_tank_explosion,dwRetID,0);
		

		m_pVehicle = GamePool_Vehicle_GetAt(dwRetID);
		m_pEntity = (ENTITY_TYPE *)m_pVehicle; 
		m_dwGTAId = dwRetID;
		dwLastCreatedVehicleID = dwRetID;
		m_pVehicle->dwDoorsLocked = 0;
		m_bIsLocked = FALSE;
		//LinkToInterior(m_byteInterior);

		SetMatrix(mat);
		SetColor(byteColor1,byteColor2);

		//pGame->RemoveModel(uiType);
	}
	
}

//-----------------------------------------------------------

BOOL CVehicle::IsOccupied()
{
	if(m_pVehicle) {
		if(m_pVehicle->pDriver) return TRUE;
		if(m_pVehicle->pPassengers[0]) return TRUE;
		if(m_pVehicle->pPassengers[1]) return TRUE;
		if(m_pVehicle->pPassengers[2]) return TRUE;
		if(m_pVehicle->pPassengers[3]) return TRUE;
		if(m_pVehicle->pPassengers[4]) return TRUE;
		if(m_pVehicle->pPassengers[5]) return TRUE;
		if(m_pVehicle->pPassengers[6]) return TRUE;
	}
	return FALSE;
}

//-----------------------------------------------------------

void CVehicle::SetInvulnerable(BOOL bInv)
{
	if(!m_pVehicle) return;
	if(!GamePool_Vehicle_GetAt(m_dwGTAId)) return;

	if(bInv) {
		ScriptCommand(&set_car_immunities,m_dwGTAId,1,1,1,1,1);
		ScriptCommand(&toggle_car_tires_vulnerable,m_dwGTAId,0);
		m_bIsInvulnerable = TRUE;
	} else { 
		ScriptCommand(&set_car_immunities,m_dwGTAId,0,0,0,0,0);
		if (pNetGame && pNetGame->m_bTirePopping)
			ScriptCommand(&toggle_car_tires_vulnerable,m_dwGTAId,1);
		m_bIsInvulnerable = FALSE;
	}
}
//-----------------------------------------------------------

void CVehicle::SetLockedState(int iLocked)
{
	if(!m_pVehicle) return;

	if(iLocked) {
		ScriptCommand(&lock_car,m_dwGTAId,1);
	} else {
		ScriptCommand(&lock_car,m_dwGTAId,0);
	}
}

//-----------------------------------------------------------

void CVehicle::SetEngineState(BOOL bState)
{
	if(!m_pVehicle) return;

	if(!bState) {
		m_pVehicle->byteFlags &= 0xEF;
	} else {
		m_pVehicle->byteFlags |= 0x10;
	}
}
//-----------------------------------------------------------

float CVehicle::GetHealth()
{	
	if(m_pVehicle) return m_pVehicle->fHealth;
	else return 0.0f;
}

//-----------------------------------------------------------

void CVehicle::SetHealth(float fHealth)
{	
	if(m_pVehicle) {
		m_pVehicle->fHealth = fHealth;
	}
}	

//-----------------------------------------------------------

void CVehicle::SetColor(int iColor1, int iColor2)
{
	if(m_pVehicle)  {
		ScriptCommand(&set_car_color,m_dwGTAId,iColor1,iColor2);
	}
}

//-----------------------------------------------------------
 
UINT CVehicle::GetVehicleSubtype()
{
	if(m_pVehicle) {
		if(m_pVehicle->entity.vtable == 0x871120) {
			return VEHICLE_SUBTYPE_CAR;
		}
		else if(m_pVehicle->entity.vtable == 0x8721A0) {
			return VEHICLE_SUBTYPE_BOAT;
		}
		else if(m_pVehicle->entity.vtable == 0x871360) {
			return VEHICLE_SUBTYPE_BIKE;
		}
		else if(m_pVehicle->entity.vtable == 0x871948) {
			return VEHICLE_SUBTYPE_PLANE;
		}
		else if(m_pVehicle->entity.vtable == 0x871680) {
			return VEHICLE_SUBTYPE_HELI;
		}
		else if(m_pVehicle->entity.vtable == 0x871528) {
			return VEHICLE_SUBTYPE_PUSHBIKE;
		}
		else if(m_pVehicle->entity.vtable == 0x872370) {
			return VEHICLE_SUBTYPE_TRAIN;
		}
	}
	return 0;
}

//-----------------------------------------------------------

BOOL CVehicle::HasSunk()
{	
	if(!m_pVehicle) return FALSE;

	return ScriptCommand(&has_car_sunk,m_dwGTAId);
}

//-----------------------------------------------------------

BOOL CVehicle::IsWrecked()
{	
	if(!m_pVehicle) return FALSE;

	return ScriptCommand(&is_car_wrecked,m_dwGTAId);
}

//-----------------------------------------------------------

BOOL CVehicle::IsDriverLocalPlayer()
{
	if(m_pVehicle) {
		if((PED_TYPE *)m_pVehicle->pDriver == GamePool_FindPlayerPed()) {
			return TRUE;
		}
	}
	return FALSE;
}

//-----------------------------------------------------------

BOOL CVehicle::IsATrainPart()
{
	int nModel;
	if(m_pVehicle) {
		nModel = m_pVehicle->entity.nModelIndex;
		if(nModel == TRAIN_PASSENGER_LOCO) return TRUE;
		if(nModel == TRAIN_PASSENGER) return TRUE;
		if(nModel == TRAIN_FREIGHT_LOCO) return TRUE;
		if(nModel == TRAIN_FREIGHT) return TRUE;
		if(nModel == TRAIN_TRAM) return TRUE;
	}
	return FALSE;
}

//-----------------------------------------------------------

BOOL CVehicle::HasTurret()
{
	int nModel = GetModelIndex();
	return (nModel == 432 ||	// Tank
			nModel == 564 ||	// RC Tank
			nModel == 407 ||	// Firetruck
			nModel == 601		// Swatvan
			);
}

//-----------------------------------------------------------

void CVehicle::SetSirenOn(BOOL state)
{
	m_pVehicle->bSirenOn = state;
}

//-----------------------------------------------------------

BOOL CVehicle::IsSirenOn()
{
	return (m_pVehicle->bSirenOn == 1);
}

//-----------------------------------------------------------

void CVehicle::SetLandingGearState(eLandingGearState state)
{
	if (state == LGS_UP) 
		m_pVehicle->fPlaneLandingGear = 0.0f;
	else if (state == LGS_DOWN)
		m_pVehicle->fPlaneLandingGear = 1.0f;
}

//-----------------------------------------------------------

eLandingGearState CVehicle::GetLandingGearState()
{
	if (m_pVehicle->fPlaneLandingGear == 0.0f)
		return LGS_UP;
	else if (m_pVehicle->fPlaneLandingGear == 1.0f)
		return LGS_DOWN;
	else
		return LGS_CHANGING;
}

//-----------------------------------------------------------

UINT CVehicle::GetPassengersMax()
{
	return 0;
}

//-----------------------------------------------------------

void CVehicle::SetHydraThrusters(DWORD dwDirection)
{
	if(m_pVehicle) m_pVehicle->dwHydraThrusters = dwDirection; // 0x00 - 0x80 // byte
}

//-----------------------------------------------------------

DWORD CVehicle::GetHydraThrusters()
{
	if(m_pVehicle) return m_pVehicle->dwHydraThrusters;
	return 0UL;
}

//-----------------------------------------------------------

void CVehicle::ProcessMarkers()
{
	if(!m_pVehicle) return;

	if(m_byteObjectiveVehicle) {
		// SHOW ALWAYS
		if(!m_bSpecialMarkerEnabled) {
			if(m_dwMarkerID) {
				ScriptCommand(&disable_marker, m_dwMarkerID);
				m_dwMarkerID = 0;
			}
			ScriptCommand(&tie_marker_to_car, m_dwGTAId, 1, 3, &m_dwMarkerID);
			ScriptCommand(&set_marker_color,m_dwMarkerID,202);
			ScriptCommand(&show_on_radar,m_dwMarkerID,3);
			m_bSpecialMarkerEnabled = TRUE;
		}
		return;
	}

	// Disable the special marker if it has been deactivated
	if(!m_byteObjectiveVehicle && m_bSpecialMarkerEnabled) {
		if(m_dwMarkerID) {
			ScriptCommand(&disable_marker, m_dwMarkerID);
			m_bSpecialMarkerEnabled = FALSE;
			m_dwMarkerID = 0;
		}
	}

	// Add or remove car scanning markers.
	if(GetDistanceFromLocalPlayerPed() < CSCANNER_DISTANCE && !IsOccupied()) {
		// SHOW IT
		if(!m_dwMarkerID)  {
			ScriptCommand(&tie_marker_to_car, m_dwGTAId, 1, 2, &m_dwMarkerID);
			ScriptCommand(&set_marker_color,m_dwMarkerID,200);
		}	
	} 
	else if(IsOccupied() || GetDistanceFromLocalPlayerPed() >= CSCANNER_DISTANCE) {
		// REMOVE IT	
		if(m_dwMarkerID) {
			ScriptCommand(&disable_marker, m_dwMarkerID);
			m_dwMarkerID = 0;
		}
	}
}

//-----------------------------------------------------------

void CVehicle::SetDoorState(int iState)
{
	if(iState) {
		m_pVehicle->dwDoorsLocked = 2;
		m_bDoorsLocked = TRUE;
	} else {
		m_pVehicle->dwDoorsLocked = 0;
		m_bDoorsLocked = FALSE;
	}
}

//-----------------------------------------------------------

BOOL CVehicle::UpdateLastDrivenTime()
{
	if(m_pVehicle) {
		if(m_pVehicle->pDriver) {
			m_bHasBeenDriven = TRUE;
			m_dwTimeSinceLastDriven = GetTickCount();
			return TRUE;
		}
	}
	return FALSE;
	// Tell the system this vehicle has been used so it can reset the timer to not be based on remaining delay
}

//-----------------------------------------------------------

void CVehicle::SetTankRot(float X, float Y)
{
	m_pVehicle->fTankRotX = X;
	m_pVehicle->fTankRotY = Y;
}

//-----------------------------------------------------------

float CVehicle::GetTankRotX()
{
	return m_pVehicle->fTankRotX;
}

//-----------------------------------------------------------

float CVehicle::GetTankRotY()
{
	return m_pVehicle->fTankRotY;
}

//-----------------------------------------------------------

float CVehicle::GetTrainSpeed()
{
	return m_pVehicle->fTrainSpeed;
}

//-----------------------------------------------------------

void CVehicle::SetTrainSpeed(float fSpeed)
{
	m_pVehicle->fTrainSpeed = fSpeed;
}

//-----------------------------------------------------------

void CVehicle::SetWheelPopped(DWORD wheelid, DWORD popped)
{
	if (GetVehicleSubtype() == VEHICLE_SUBTYPE_CAR)
		m_pVehicle->bCarWheelPopped[wheelid] = (BYTE)popped;
	else if (GetVehicleSubtype() == VEHICLE_SUBTYPE_BIKE)
		m_pVehicle->bBikeWheelPopped[wheelid] = (BYTE)popped;
}

//-----------------------------------------------------------

BYTE CVehicle::GetWheelPopped(DWORD wheelid)
{
	if (GetVehicleSubtype() == VEHICLE_SUBTYPE_CAR)
		return m_pVehicle->bCarWheelPopped[wheelid];
	else if (GetVehicleSubtype() == VEHICLE_SUBTYPE_BIKE)
		return m_pVehicle->bBikeWheelPopped[wheelid];
	return 0;
}

//-----------------------------------------------------------

void CVehicle::AttachTrailer()
{
	if (m_pTrailer)
		ScriptCommand(&put_trailer_on_cab, m_pTrailer->m_dwGTAId, m_dwGTAId);
}

//-----------------------------------------------------------

void CVehicle::DetachTrailer()
{
	if (m_pTrailer)
		ScriptCommand(&detach_trailer_from_cab, m_pTrailer->m_dwGTAId, m_dwGTAId);
}

//-----------------------------------------------------------

void CVehicle::SetTrailer(CVehicle *pTrailer)
{
	m_pTrailer = pTrailer;
}

//-----------------------------------------------------------

CVehicle* CVehicle::GetTrailer()
{
	if (!m_pVehicle) return NULL;

	// Try to find associated trailer
	DWORD dwTrailerGTAPtr = m_pVehicle->dwTrailer;

	if(pNetGame && dwTrailerGTAPtr) {
		CVehiclePool* pVehiclePool = pNetGame->GetVehiclePool();
		VEHICLEID TrailerID = (VEHICLEID)pVehiclePool->FindIDFromGtaPtr((VEHICLE_TYPE*)dwTrailerGTAPtr);
		if(TrailerID < MAX_VEHICLES && pVehiclePool->GetSlotState(TrailerID)) {
			return pVehiclePool->GetAt(TrailerID);
		}
	}

	return NULL;
}

//-----------------------------------------------------------
//441, 	rcbandit
//464, 	rcbaron
//465, 	rcraider
//594, 	rccam
//564, 	rctiger 
//501, 	rcgoblin

BOOL CVehicle::IsRCVehicle()
{
	if(m_pVehicle) {
		if( m_pVehicle->entity.nModelIndex == 441 || 
			m_pVehicle->entity.nModelIndex == 464 ||
			m_pVehicle->entity.nModelIndex == 465 || 
			m_pVehicle->entity.nModelIndex == 594 ||
			m_pVehicle->entity.nModelIndex == 501 || 
			m_pVehicle->entity.nModelIndex == 564 )
		{
			return TRUE;
		}
	}
	return FALSE;
}

//-----------------------------------------------------------
// This can currently only be used for setting the alternate
// siren. The way it's coded internally doesn't seem to allow
// us to modify the horn alone.

void CVehicle::SetHornState(BYTE byteState)
{
	if(!m_pVehicle) return;
	if(!GamePool_Vehicle_GetAt(m_dwGTAId)) return;

	if( GetVehicleSubtype() != VEHICLE_SUBTYPE_BOAT &&
		GetVehicleSubtype() != VEHICLE_SUBTYPE_PLANE &&
		GetVehicleSubtype() != VEHICLE_SUBTYPE_HELI )
	{
	
		m_pVehicle->byteHorn = byteState;
		m_pVehicle->byteHorn2 = byteState;
	}
}

//-----------------------------------------------------------

BOOL CVehicle::HasADriver()
{	
	if(!m_pVehicle) return FALSE;
	if(!GamePool_Vehicle_GetAt(m_dwGTAId)) return FALSE;

	if(m_pVehicle->pDriver && IN_VEHICLE(m_pVehicle->pDriver)) {
		return TRUE;
	}

	return FALSE;
}

//-----------------------------------------------------------

void CVehicle::RemoveEveryoneFromVehicle()
{
	if (!m_pVehicle) return;
	if (!GamePool_Vehicle_GetAt(m_dwGTAId)) return;

	float fPosX = m_pVehicle->entity.mat->pos.X;
	float fPosY = m_pVehicle->entity.mat->pos.Y;
	float fPosZ = m_pVehicle->entity.mat->pos.Z;

	int iPlayerID = 0;
	if (m_pVehicle->pDriver) {
		iPlayerID = GamePool_Ped_GetIndex( m_pVehicle->pDriver );
		ScriptCommand( &remove_actor_from_car_and_put_at, iPlayerID, fPosX, fPosY, fPosZ + 2 );
	}

	for (int i = 0; i < 7; i++) {
		if (m_pVehicle->pPassengers[i] != NULL) {
			iPlayerID = GamePool_Ped_GetIndex( m_pVehicle->pPassengers[i] );
			ScriptCommand( &remove_actor_from_car_and_put_at, iPlayerID, fPosX, fPosY, fPosZ + 2 );
		}
	}
}

//-----------------------------------------------------------

BOOL CVehicle::VerifyInstance()
{
	if(GamePool_Vehicle_GetAt(m_dwGTAId)) {
		return TRUE;
	}
	return FALSE;
}
//-----------------------------------------------------------
// EOF