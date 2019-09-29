/*

	SA:MP Multiplayer Modification
	Copyright 2004-2005 SA:MP Team

    Version: $Id: player.cpp,v 1.39 2006/05/07 15:35:32 kyeman Exp $

*/

#include "main.h"

#define NOT_KEY_FIRE 0xFB

extern CNetGame *pNetGame;

//----------------------------------------------------
// Some Util functions for Vector compression

void CompressNormalVector(VECTOR * vec, C_VECTOR1 * c1)
{
	int X = (int)(vec->X * 8388607.5f);
	int Y = (int)(vec->Y * 8388607.5f);
	int Z = (int)(vec->Z * 8388607.5f);

	memcpy(c1->data+0, &X, 3);
	memcpy(c1->data+3, &Y, 3);
	memcpy(c1->data+6, &Z, 3);
}

//----------------------------------------------------

void DecompressNormalVector(VECTOR * vec, C_VECTOR1 * c1)
{
	int X = 0, Y = 0, Z = 0;

	memcpy(&X, c1->data+0, 3);
	if (c1->data[2+0] < 0) 
		X |= 0xFF000000;
	memcpy(&Y, c1->data+3, 3);
	if (c1->data[2+3] < 0) 
		Y |= 0xFF000000;
	memcpy(&Z, c1->data+6, 3);
	if (c1->data[2+6] < 0) 
		Z |= 0xFF000000;

	vec->X = ((float)X / 8388607.5f);
	vec->Y = ((float)Y / 8388607.5f);
	vec->Z = ((float)Z / 8388607.5f);
}

//----------------------------------------------------

CPlayer::CPlayer()
{
	m_byteUpdateFromNetwork = UPDATE_TYPE_NONE;
	m_bytePlayerID = INVALID_PLAYER_ID;
	m_VehicleID = 0;
	m_bHasAimUpdates = FALSE;
	m_bHasTrailerUpdates = FALSE;
	m_byteState = PLAYER_STATE_NONE;
	m_dwColor = 0;
	m_wLastKeys = 0;

	m_vecPos.X = 0.0f;
	m_vecPos.Y = 0.0f;
	m_vecPos.Z = 0.0f;
	m_fRotation = 0.0f;
	m_fHealth = 0.0f;
	m_fArmour = 0.0f;
	m_fGameTime = 0.0f;
	m_byteTime = 0;
	m_byteWantedLevel = 0;
	m_bCanTeleport = false;
	//byteCurWeap = 0;

	m_bCheckpointEnabled = FALSE;
	m_bRaceCheckpointEnabled = FALSE;
	m_iInteriorId = 0;
	m_byteSpectateType = 0;
	m_SpectateID = 0xFFFFFFFF;
	
	BYTE i;
	for (i = 0; i < 13; i++)
	{
		m_byteSlotWeapon[i] = 0;
		m_dwSlotAmmo[i] = 0;
	}
}

//----------------------------------------------------

void CPlayer::SetState(BYTE byteNewState)
{
	BYTE byteOldState;
	if(m_byteState != byteNewState) {
		byteOldState = m_byteState;
		m_byteState = byteNewState;

		pNetGame->GetFilterScripts()->OnPlayerStateChange(m_bytePlayerID,m_byteState,byteOldState);
		CGameMode *pGameMode = pNetGame->GetGameMode();
		if(pGameMode) pGameMode->OnPlayerStateChange(m_bytePlayerID,m_byteState,byteOldState);
	}
}

//----------------------------------------------------

void CPlayer::UpdatePosition(float x, float y, float z)
{
	m_vecPos.X = x; 
	m_vecPos.Y = y; 
	m_vecPos.Z = z;

	if (m_bCheckpointEnabled)
	{
		float fSX = (m_vecPos.X - m_vecCheckpoint.X) * (m_vecPos.X - m_vecCheckpoint.X);
		float fSY = (m_vecPos.Y - m_vecCheckpoint.Y) * (m_vecPos.Y - m_vecCheckpoint.Y);
		float fSZ = (m_vecPos.Z - m_vecCheckpoint.Z) * (m_vecPos.Z - m_vecCheckpoint.Z);

		if((float)sqrt(fSX + fSY + fSZ) < m_fCheckpointSize)
		{
			if(!m_bInCheckpoint) {
				m_bInCheckpoint = TRUE;
				pNetGame->GetFilterScripts()->OnPlayerEnterCheckpoint((cell)m_bytePlayerID);
				CGameMode *pGameMode = pNetGame->GetGameMode();
				if(pGameMode) pGameMode->OnPlayerEnterCheckpoint((cell)m_bytePlayerID);
			}
		} 
		else {
			if(m_bInCheckpoint) {
				m_bInCheckpoint = FALSE;
				pNetGame->GetFilterScripts()->OnPlayerLeaveCheckpoint((cell)m_bytePlayerID);
				CGameMode *pGameMode = pNetGame->GetGameMode();
				if(pGameMode) pGameMode->OnPlayerLeaveCheckpoint((cell)m_bytePlayerID);
			}
		}
	}
	
	if (m_bRaceCheckpointEnabled)
	{
		float fSX = (m_vecPos.X - m_vecRaceCheckpoint.X) * (m_vecPos.X - m_vecRaceCheckpoint.X);
		float fSY = (m_vecPos.Y - m_vecRaceCheckpoint.Y) * (m_vecPos.Y - m_vecRaceCheckpoint.Y);
		float fSZ = (m_vecPos.Z - m_vecRaceCheckpoint.Z) * (m_vecPos.Z - m_vecRaceCheckpoint.Z);

		if((float)sqrt(fSX + fSY + fSZ) < m_fRaceCheckpointSize)
		{
			if(!m_bInRaceCheckpoint) {
				m_bInRaceCheckpoint = TRUE;
				pNetGame->GetFilterScripts()->OnPlayerEnterRaceCheckpoint((cell)m_bytePlayerID);
				CGameMode *pGameMode = pNetGame->GetGameMode();
				if(pGameMode) pGameMode->OnPlayerEnterRaceCheckpoint((cell)m_bytePlayerID);
			}
		} 
		else {
			if(m_bInRaceCheckpoint) {
				m_bInRaceCheckpoint = FALSE;
				pNetGame->GetFilterScripts()->OnPlayerLeaveRaceCheckpoint((cell)m_bytePlayerID);
				CGameMode *pGameMode = pNetGame->GetGameMode();
				if(pGameMode) pGameMode->OnPlayerLeaveRaceCheckpoint((cell)m_bytePlayerID);
			}
		}
	}
}

//----------------------------------------------------

void CPlayer::Process(float fElapsedTime)
{
	static float fLastSync = 0.0;
	m_fGameTime += fElapsedTime;
	if(IsActive()) {
		if(m_byteUpdateFromNetwork != UPDATE_TYPE_NONE)  {
			BroadcastSyncData();
			m_byteUpdateFromNetwork = UPDATE_TYPE_NONE;
		}
		
		if (m_byteTime) // If this player has private time
		{
			if (m_fGameTime - fLastSync >= 300.0f) // Sync to server every 5 minutes (no chance of lagging in there really)
			{
				if (m_fGameTime >= 1440.0f) m_fGameTime -= 1440.0f; // Limit to 24 hours
				fLastSync = m_fGameTime; // Only do it here as it's not going to be far out and it means syncs are correct
				int iTime;
				iTime = (int)m_fGameTime;

				RakNet::BitStream bsPlayerTime;

				bsPlayerTime.Write((BYTE)(iTime / 60));
				bsPlayerTime.Write((BYTE)(iTime % 60));

				pNetGame->GetRakServer()->RPC(RPC_SetTimeEx,&bsPlayerTime,HIGH_PRIORITY,RELIABLE,
					0,pNetGame->GetRakServer()->GetPlayerIDFromIndex(m_bytePlayerID),false,false);
			}
		}
	}
	
}

//----------------------------------------------------

void CPlayer::BroadcastSyncData()
{
	RakNet::BitStream bsSync;
	//RakNet::BitStream bsAim;
	
	WORD wKeys;

	if( GetState() == PLAYER_STATE_ONFOOT &&
		m_byteUpdateFromNetwork == UPDATE_TYPE_ONFOOT )
	{
		bsSync.Write((BYTE)ID_PLAYER_SYNC);
		bsSync.Write(m_bytePlayerID);

		//bsSync.Write((PCHAR)&m_ofSync,sizeof (ONFOOT_SYNC_DATA));

		// LEFT/RIGHT KEYS
		if(m_ofSync.lrAnalog) {
			bsSync.Write(true);
			bsSync.Write(m_ofSync.lrAnalog);
		} else {
			bsSync.Write(false);
		}

		// UP/DOWN KEYS
		if(m_ofSync.udAnalog) {
			bsSync.Write(true);
			bsSync.Write(m_ofSync.udAnalog);
		} else {
			bsSync.Write(false);
		}

		// GENERAL KEYSET
		bsSync.Write(m_ofSync.wKeys);
		
		// VECTOR POSITION
		bsSync.Write((const char*)&m_ofSync.vecPos,sizeof(VECTOR));
	
		// ROTATION
		bsSync.Write(m_ofSync.fRotation);

		// HEALTH/ARMOR
		BYTE byteSyncHealthArmour=0;
		if( m_ofSync.byteHealth > 0 && m_ofSync.byteHealth < 100 ) {
			byteSyncHealthArmour = ((BYTE)(m_ofSync.byteHealth / 7)) << 4;
		} 
		else if(m_ofSync.byteHealth >= 100) {
			byteSyncHealthArmour = 0xF << 4;
		}

		if( m_ofSync.byteArmour > 0 && m_ofSync.byteArmour < 100 ) {
			byteSyncHealthArmour |=  (BYTE)(m_ofSync.byteArmour / 7);
		}
		else if(m_ofSync.byteArmour >= 100) {
			byteSyncHealthArmour |= 0xF;
		}
	
		bsSync.Write(byteSyncHealthArmour);
	
		// CURRENT WEAPON
		bsSync.Write(m_ofSync.byteCurrentWeapon);

		// SPECIAL ACTION
		bsSync.Write(m_ofSync.byteSpecialAction);

		// MOVE SPEED VECTORS
		if(m_ofSync.vecMoveSpeed.X != 0.0f) {
			bsSync.Write(true);
			bsSync.Write(m_ofSync.vecMoveSpeed.X);
		} else bsSync.Write(false);

		if(m_ofSync.vecMoveSpeed.Y != 0.0f) {
			bsSync.Write(true);
			bsSync.Write(m_ofSync.vecMoveSpeed.Y);
		} else bsSync.Write(false);

		if(m_ofSync.vecMoveSpeed.Z != 0.0f) {
			bsSync.Write(true);
			bsSync.Write(m_ofSync.vecMoveSpeed.Z);
		} else bsSync.Write(false);

		// VEHICLE SURFING (POSITION RELATIVE TO CAR SYNC)
		if(m_ofSync.SurfVehicleId) {
			bsSync.Write(true);
			bsSync.Write(m_ofSync.SurfVehicleId);
			bsSync.Write(m_ofSync.vecSurfOffsets.X);
			bsSync.Write(m_ofSync.vecSurfOffsets.Y);
			bsSync.Write(m_ofSync.vecSurfOffsets.Z);
		} else {
			bsSync.Write(false);
		}

		pNetGame->BroadcastData(&bsSync,HIGH_PRIORITY,UNRELIABLE_SEQUENCED,0,m_bytePlayerID);
	}
	else if(GetState() == PLAYER_STATE_DRIVER &&
		m_byteUpdateFromNetwork == UPDATE_TYPE_INCAR )
	{			
		// storing
		bsSync.Write((BYTE)ID_VEHICLE_SYNC);
		bsSync.Write(m_bytePlayerID);
		//bsSync.Write((PCHAR)&m_icSync,sizeof (INCAR_SYNC_DATA));

		// VEHICLEID
		bsSync.Write(m_icSync.VehicleID);

		// KEYS
		bsSync.Write(m_icSync.lrAnalog);
		bsSync.Write(m_icSync.udAnalog);
		bsSync.Write(m_icSync.wKeys);

		// ROLL / DIRECTION / POSITION / MOVE SPEED
		bsSync.Write((const char*)&m_icSync.cvecRoll,sizeof(C_VECTOR1));
		bsSync.Write((const char*)&m_icSync.cvecDirection,sizeof(C_VECTOR1));
		bsSync.Write((const char*)&m_icSync.vecPos,sizeof(VECTOR));
		bsSync.Write((const char*)&m_icSync.vecMoveSpeed,sizeof(VECTOR));

		// CAR HEALTH
		bsSync.Write((WORD)m_icSync.fCarHealth);
	
		// HEALTH/ARMOR
		BYTE byteSyncHealthArmour=0;
		if( m_icSync.bytePlayerHealth > 0 && m_icSync.bytePlayerHealth < 100 ) {
			byteSyncHealthArmour = ((BYTE)(m_icSync.bytePlayerHealth / 7)) << 4;
		} 
		else if(m_icSync.bytePlayerHealth >= 100) {
			byteSyncHealthArmour = 0xF << 4;
		}

		if( m_icSync.bytePlayerArmour > 0 && m_icSync.bytePlayerArmour < 100 ) {
			byteSyncHealthArmour |=  (BYTE)(m_icSync.bytePlayerArmour / 7);
		}
		else if(m_icSync.bytePlayerArmour >= 100) {
			byteSyncHealthArmour |= 0xF;
		}

		bsSync.Write(byteSyncHealthArmour);

		// CURRENT WEAPON
		bsSync.Write(m_icSync.byteCurrentWeapon);

		// SIREN
		if(m_icSync.byteSirenOn) bsSync.Write(true);
		else bsSync.Write(false);

		// PLANE LANDING GEAR
		if(m_icSync.byteLandingGearState) bsSync.Write(true);
		else bsSync.Write(false);

		// POPPED TIRES
		if (pNetGame->m_bTirePopping) {
			if (m_icSync.byteTires[0]) bsSync.Write(true);
			else bsSync.Write(false);
			if (m_icSync.byteTires[1]) bsSync.Write(true);
			else bsSync.Write(false);
			if (m_icSync.byteTires[2]) bsSync.Write(true);
			else bsSync.Write(false);
			if (m_icSync.byteTires[3]) bsSync.Write(true);
			else bsSync.Write(false);
		}

		// HYDRA SPECIAL
		if(m_icSync.dwHydraThrustAngle) {
			bsSync.Write(true);
			bsSync.Write(m_icSync.dwHydraThrustAngle);
		} else {
			bsSync.Write(false);
		}

		// TRAIN SPECIAL
		if(m_icSync.fTrainSpeed != 0.0f) {
			bsSync.Write(true);
			bsSync.Write(m_icSync.fTrainSpeed);
		} else {
			bsSync.Write(false);
		}

		// TRAILER SPECIAL
		if(m_icSync.TrailerID != 0) {
			bsSync.Write(true);
			bsSync.Write(m_icSync.TrailerID);
		} else {
			bsSync.Write(false);
		}

		pNetGame->BroadcastData(&bsSync,HIGH_PRIORITY,UNRELIABLE_SEQUENCED,0,m_bytePlayerID);
	}
	else if(GetState() == PLAYER_STATE_PASSENGER &&
		m_byteUpdateFromNetwork == UPDATE_TYPE_PASSENGER)
	{			
		bsSync.Write((BYTE)ID_PASSENGER_SYNC);
		bsSync.Write(m_bytePlayerID);
		wKeys = m_psSync.wKeys;
		if (m_psSync.byteCurrentWeapon == 43) m_psSync.wKeys &= NOT_KEY_FIRE;
		bsSync.Write((PCHAR)&m_psSync,sizeof (PASSENGER_SYNC_DATA));
		m_psSync.wKeys = wKeys;
		pNetGame->BroadcastData(&bsSync,HIGH_PRIORITY,UNRELIABLE_SEQUENCED,0,m_bytePlayerID);
	}

	if(m_bHasAimUpdates) {
		bsSync.Reset();
		bsSync.Write((BYTE)ID_AIM_SYNC);
		bsSync.Write(m_bytePlayerID);
		bsSync.Write((PCHAR)&m_aimSync,sizeof(AIM_SYNC_DATA));
		pNetGame->BroadcastData(&bsSync, HIGH_PRIORITY, UNRELIABLE_SEQUENCED, 0, m_bytePlayerID, TRUE, TRUE);
		m_bHasAimUpdates = FALSE;
	}
	
	if (m_bHasTrailerUpdates) {
		bsSync.Reset();
		bsSync.Write((BYTE)ID_TRAILER_SYNC);
		bsSync.Write(m_bytePlayerID);
		bsSync.Write((PCHAR)&m_trSync, sizeof (TRAILER_SYNC_DATA));
		pNetGame->BroadcastData(&bsSync, HIGH_PRIORITY,UNRELIABLE_SEQUENCED, 0, m_bytePlayerID, TRUE);
		m_bHasTrailerUpdates = FALSE;
	}
		
}

//----------------------------------------------------

BYTE CPlayer::CheckWeapon(BYTE weapon)
{
	if (!(pNetGame->m_longSynchedWeapons & (long long)(((long long)1) << (weapon))))
	{
		return 0;
	}
	else
	{
		return weapon;
	}
}

//----------------------------------------------------

void CPlayer::StoreOnFootFullSyncData(ONFOOT_SYNC_DATA *pofSync)
{
	m_VehicleID = 0;

	memcpy(&m_ofSync,pofSync,sizeof(ONFOOT_SYNC_DATA));
	
	CFilterScripts * pFilterScripts = pNetGame->GetFilterScripts();
	CGameMode * pGameMode = pNetGame->GetGameMode();

	// Hardcoded prediction of ~200ms compensation.
    //m_ofSync.vecPos.X += (m_ofSync.vecMoveSpeed.X * 8.0f);
	//m_ofSync.vecPos.Y += (m_ofSync.vecMoveSpeed.Y * 8.0f);
	//m_ofSync.vecPos.Z += (m_ofSync.vecMoveSpeed.Z * 8.0f);

	UpdatePosition(m_ofSync.vecPos.X,m_ofSync.vecPos.Y,m_ofSync.vecPos.Z);
	m_byteUpdateFromNetwork = UPDATE_TYPE_ONFOOT;
	m_fRotation = m_ofSync.fRotation;
	m_fHealth = (float)m_ofSync.byteHealth;
	m_fArmour = (float)m_ofSync.byteArmour;
	
	m_ofSync.byteCurrentWeapon = CheckWeapon(m_ofSync.byteCurrentWeapon);
		
	// Determine the slot of the current weapon.
	switch (m_ofSync.byteCurrentWeapon)
	{
		case 0:
		case 1:
			m_byteSlotWeapon[0] = m_ofSync.byteCurrentWeapon;
			break;
		case 2:
		case 3:
		case 4:
		case 5:
		case 6:
		case 7:
		case 8:
		case 9:
			m_byteSlotWeapon[1] = m_ofSync.byteCurrentWeapon;
			break;
		case 22:
		case 23:
		case 24:
			m_byteSlotWeapon[2] = m_ofSync.byteCurrentWeapon;
			break;
		case 25:
		case 26:
		case 27:
			m_byteSlotWeapon[3] = m_ofSync.byteCurrentWeapon;
			break;
		case 28:
		case 29:
		case 32:
			m_byteSlotWeapon[4] = m_ofSync.byteCurrentWeapon;
			break;
		case 30:
		case 31:
			m_byteSlotWeapon[5] = m_ofSync.byteCurrentWeapon;
			break;
		case 33:
		case 34:
			m_byteSlotWeapon[6] = m_ofSync.byteCurrentWeapon;
			break;
		case 35:
		case 36:
		case 37:
		case 38:
			m_byteSlotWeapon[7] = m_ofSync.byteCurrentWeapon;
			break;
		case 16:
		case 17:
		case 18:
		case 39:
			m_byteSlotWeapon[8] = m_ofSync.byteCurrentWeapon;
			break;
		case 41:
		case 42:
		case 43:
			m_byteSlotWeapon[9] = m_ofSync.byteCurrentWeapon;
			break;
		case 10:
		case 11:
		case 12:
		case 13:
		case 14:
		case 15:
			m_byteSlotWeapon[10] = m_ofSync.byteCurrentWeapon;
			break;
		case 44:
		case 45:
		case 46:
			m_byteSlotWeapon[11] = m_ofSync.byteCurrentWeapon;
			break;
		case 40:
			m_byteSlotWeapon[12] = m_ofSync.byteCurrentWeapon;
			break;
		default:
			// If invalid weapon give them fists
			m_byteSlotWeapon[0] = 0;
			m_ofSync.byteCurrentWeapon = 0;
			break;
	}

	CheckKeyUpdatesForScript(m_ofSync.wKeys);
	SetState(PLAYER_STATE_ONFOOT);

	if(pFilterScripts && pGameMode) {
		if(!pFilterScripts->OnPlayerUpdate(m_bytePlayerID) || !pGameMode->OnPlayerUpdate(m_bytePlayerID)) {
			m_byteUpdateFromNetwork = UPDATE_TYPE_NONE;
		}
	}
}

//----------------------------------------------------

void CPlayer::StoreAimSyncData(AIM_SYNC_DATA *paimSync)
{
	memcpy(&m_aimSync,paimSync,sizeof(AIM_SYNC_DATA));
	m_bHasAimUpdates = TRUE;
}

//----------------------------------------------------

void CPlayer::StoreInCarFullSyncData(INCAR_SYNC_DATA *picSync)
{
	m_VehicleID = picSync->VehicleID;
	m_byteSeatID = 0;

	CFilterScripts * pFilterScripts = pNetGame->GetFilterScripts();
	CGameMode * pGameMode = pNetGame->GetGameMode();

	memcpy(&m_icSync,picSync,sizeof(INCAR_SYNC_DATA));

	// Hardcoded prediction of ~100ms compensation.
    //m_icSync.vecPos.X += (m_icSync.vecMoveSpeed.X * 5.0f);
	//m_icSync.vecPos.Y += (m_icSync.vecMoveSpeed.Y * 5.0f);
	//m_icSync.vecPos.Z += (m_icSync.vecMoveSpeed.Z * 5.0f);

	UpdatePosition(m_icSync.vecPos.X,m_icSync.vecPos.Y,m_icSync.vecPos.Z);

	m_fHealth = (float)m_icSync.bytePlayerHealth;
	m_fArmour = (float)m_icSync.bytePlayerArmour;

	m_byteUpdateFromNetwork = UPDATE_TYPE_INCAR;

	CVehicle *pVehicle = pNetGame->GetVehiclePool()->GetAt(m_VehicleID);
	if(pVehicle) 
	{
		//pVehicle->m_byteDriverID = m_bytePlayerID; // Done in CVehicle::Update()

		MATRIX4X4 matWorld;

		float fVehicleHealth = m_icSync.fCarHealth;

		memcpy(&matWorld.pos,&m_icSync.vecPos,sizeof(VECTOR));
		DecompressNormalVector(&matWorld.up, &m_icSync.cvecDirection);
		DecompressNormalVector(&matWorld.right, &m_icSync.cvecRoll);
		
		pVehicle->Update(m_bytePlayerID,&matWorld,fVehicleHealth,m_icSync.TrailerID);

	}

	m_icSync.byteCurrentWeapon = CheckWeapon(m_icSync.byteCurrentWeapon);

	CheckKeyUpdatesForScript(m_icSync.wKeys);
	SetState(PLAYER_STATE_DRIVER);

	if(pFilterScripts && pGameMode) {
		if(!pFilterScripts->OnPlayerUpdate(m_bytePlayerID) || !pGameMode->OnPlayerUpdate(m_bytePlayerID)) {
			m_byteUpdateFromNetwork = UPDATE_TYPE_NONE;
		}
	}	
}

//----------------------------------------------------

void CPlayer::StoreTrailerFullSyncData(TRAILER_SYNC_DATA* trSync)
{
	VEHICLEID TrailerID = m_icSync.TrailerID;
	if (!TrailerID) return;
	
	memcpy(&m_trSync, trSync, sizeof (TRAILER_SYNC_DATA));

	CVehicle *pVehicle = pNetGame->GetVehiclePool()->GetAt(TrailerID);
	if(pVehicle) 
	{
		MATRIX4X4 matWorld;

		memcpy(&matWorld.pos, &m_trSync.vecPos, sizeof(VECTOR));
		DecompressNormalVector(&matWorld.up, &m_trSync.cvecDirection);
		DecompressNormalVector(&matWorld.right, &m_trSync.cvecRoll);
		
		pVehicle->Update(m_bytePlayerID, &matWorld, 1000.0, 0); // Could be used for trailer trains, as I accidentally leaked in #betateam :o
		memcpy(&pVehicle->m_vecMoveSpeed, &m_trSync.vecMoveSpeed, sizeof (VECTOR));
	}
}

//----------------------------------------------------

void CPlayer::StorePassengerFullSyncData(PASSENGER_SYNC_DATA *ppsSync)
{
	m_VehicleID  = ppsSync->VehicleID;
	
	CFilterScripts * pFilterScripts = pNetGame->GetFilterScripts();
	CGameMode * pGameMode = pNetGame->GetGameMode();

	memcpy(&m_psSync,ppsSync,sizeof(PASSENGER_SYNC_DATA));
	UpdatePosition(m_psSync.vecPos.X,m_psSync.vecPos.Y,m_psSync.vecPos.Z);

	m_fHealth = (float)m_psSync.bytePlayerHealth;
	m_fArmour = (float)m_psSync.bytePlayerArmour;

	m_psSync.byteCurrentWeapon = CheckWeapon(m_psSync.byteCurrentWeapon);
	
	m_byteUpdateFromNetwork = UPDATE_TYPE_PASSENGER;

	CheckKeyUpdatesForScript(m_psSync.wKeys);
	SetState(PLAYER_STATE_PASSENGER);

	if(pFilterScripts && pGameMode) {
		if(!pFilterScripts->OnPlayerUpdate(m_bytePlayerID) || !pGameMode->OnPlayerUpdate(m_bytePlayerID)) {
			m_byteUpdateFromNetwork = UPDATE_TYPE_NONE;
		}
	}
}

//----------------------------------------------------

void CPlayer::StoreSpectatorFullSyncData(SPECTATOR_SYNC_DATA *pspSync)
{
	memcpy(&m_spSync,pspSync,sizeof(SPECTATOR_SYNC_DATA));
	UpdatePosition(m_spSync.vecPos.X,m_spSync.vecPos.Y,m_spSync.vecPos.Z);
	
	CheckKeyUpdatesForScript(m_spSync.wKeys);
	
	if (m_byteState != PLAYER_STATE_SPECTATING) {
		RakNet::BitStream bsSend;
		bsSend.Write(m_bytePlayerID);
		
		RakServerInterface *pRak = pNetGame->GetRakServer();
		pRak->RPC(RPC_ScrSetPlayerSpectating, &bsSend, HIGH_PRIORITY, RELIABLE, 0, pRak->GetPlayerIDFromIndex(m_bytePlayerID), true, false);
	}
	SetState(PLAYER_STATE_SPECTATING);
}

//----------------------------------------------------

void CPlayer::Say(unsigned char * szText, BYTE byteTextLen)
{
	PlayerID playerid = pNetGame->GetRakServer()->GetPlayerIDFromIndex(m_bytePlayerID);
	RakNet::BitStream bsSend;

	bsSend.Write(m_bytePlayerID);
	bsSend.Write(byteTextLen);
	bsSend.Write((const char *)szText,byteTextLen);

	// Comment by spookie:
	//   Chat text is now sent to all players, including the player that said it.
	//   This is so the filter scripts can modify the text before it gets displayed.

	if(pNetGame->m_bLimitGlobalChatRadius) {
		// we only send to players within pNetGame->m_fGlobalChatRadius
		// and the player who send it
		CPlayerPool *pPlayerPool = pNetGame->GetPlayerPool();
		int x=0; 
		float fDist; 
		float fAllowedDist = pNetGame->m_fGlobalChatRadius;

		while(x!=MAX_PLAYERS) {
			if((x != m_bytePlayerID) && pPlayerPool->GetSlotState(x)) {
				fDist = pPlayerPool->GetDistanceFromPlayerToPlayer(m_bytePlayerID,(BYTE)x);
				if(fDist <= fAllowedDist) {
					pNetGame->GetRakServer()->RPC(RPC_Chat,&bsSend,HIGH_PRIORITY,RELIABLE,0,
						pNetGame->GetRakServer()->GetPlayerIDFromIndex(x),false,false);
				}
			}
			x++;
		}

		// send to the originating person
		pNetGame->GetRakServer()->RPC(RPC_Chat,&bsSend,HIGH_PRIORITY,RELIABLE,0,
			pNetGame->GetRakServer()->GetPlayerIDFromIndex(m_bytePlayerID),false,false);

	} else {
		pNetGame->GetRakServer()->RPC(RPC_Chat,&bsSend,HIGH_PRIORITY,RELIABLE,0,UNASSIGNED_PLAYER_ID,true,false);
	}
}

//----------------------------------------------------

void CPlayer::Privmsg(BYTE byteToPlayerID, unsigned char * szText, BYTE byteTextLen)
{
	PlayerID playerid = pNetGame->GetRakServer()->GetPlayerIDFromIndex(m_bytePlayerID);
	PlayerID toplayerid = pNetGame->GetRakServer()->GetPlayerIDFromIndex(byteToPlayerID);
	RakNet::BitStream bsSend;

	bsSend.Write(m_bytePlayerID);
	bsSend.Write(byteToPlayerID);
	bsSend.Write(byteTextLen);
	bsSend.Write((const char *)szText,byteTextLen);

	pNetGame->GetRakServer()->RPC(RPC_Privmsg,&bsSend,HIGH_PRIORITY,RELIABLE,0,playerid,false,false);
	pNetGame->GetRakServer()->RPC(RPC_Privmsg,&bsSend,HIGH_PRIORITY,RELIABLE,0,toplayerid,false,false);
}

//----------------------------------------------------

void CPlayer::TeamPrivmsg(unsigned char * szText, BYTE byteTextLen)
{
	PlayerID playerid = pNetGame->GetRakServer()->GetPlayerIDFromIndex(m_bytePlayerID);
	RakNet::BitStream bsSend;

	bsSend.Write(m_bytePlayerID);
	bsSend.Write(byteTextLen);
	bsSend.Write((const char *)szText,byteTextLen);

	CPlayerPool *pPlayerPool = pNetGame->GetPlayerPool();
	
	for (int i=0; i<MAX_PLAYERS; i++) {
		if(pPlayerPool->GetSlotState(i)) {
			if(pPlayerPool->GetAt(m_bytePlayerID)->GetTeam() == pPlayerPool->GetAt(i)->GetTeam()) {
				pNetGame->GetRakServer()->RPC(RPC_TeamPrivmsg,&bsSend,HIGH_PRIORITY,RELIABLE,0,
					pNetGame->GetRakServer()->GetPlayerIDFromIndex(i),false,false);
			}
		}
	}
}

//----------------------------------------------------

void CPlayer::HandleDeath(BYTE byteReason, BYTE byteWhoWasResponsible)
{
	RakNet::BitStream bsPlayerDeath;
	PlayerID playerid = pNetGame->GetRakServer()->GetPlayerIDFromIndex(m_bytePlayerID);

	SetState(PLAYER_STATE_WASTED);
	
	pNetGame->GetFilterScripts()->OnPlayerDeath((cell)m_bytePlayerID, (cell)byteWhoWasResponsible, (cell)byteReason);
	CGameMode *pGameMode = pNetGame->GetGameMode();
	if(pGameMode) pGameMode->OnPlayerDeath((cell)m_bytePlayerID, (cell)byteWhoWasResponsible, (cell)byteReason);

	bsPlayerDeath.Write(m_bytePlayerID);

	pNetGame->GetRakServer()->RPC(RPC_Death,&bsPlayerDeath,
		HIGH_PRIORITY,RELIABLE,0,playerid,true,false);

#ifdef RAKRCON
		// use bsPlayerDeath because it already has the playerid in it, why make another bitstream?
		bsPlayerDeath.Write( byteWhoWasResponsible );
		bsPlayerDeath.Write( byteReason );

		pRcon->GetRakServer()->RPC( RPC_Death, &bsPlayerDeath, HIGH_PRIORITY, RELIABLE, 0, UNASSIGNED_PLAYER_ID, true, false );
#endif

	if ( byteWhoWasResponsible == INVALID_PLAYER_ID )
	{
		logprintf("[death] %s died %d",pNetGame->GetPlayerPool()->GetPlayerName(m_bytePlayerID), byteReason);
	} else {
		logprintf("[kill] %s killed %s %s", pNetGame->GetPlayerPool()->GetPlayerName(byteWhoWasResponsible), pNetGame->GetPlayerPool()->GetPlayerName(m_bytePlayerID), pNetGame->GetWeaponName(byteReason));
	}
}

//----------------------------------------------------

void CPlayer::SetSpawnInfo(PLAYER_SPAWN_INFO *pSpawn)
{
	memcpy(&m_SpawnInfo,pSpawn,sizeof(PLAYER_SPAWN_INFO));
	m_bHasSpawnInfo = TRUE;
}

//----------------------------------------------------
// Spawns a player for the world using its
// SpawnInfo.

void CPlayer::Spawn()
{
	//static bool bDimSent = false;
	if(m_bHasSpawnInfo) {
		
		pNetGame->GetFilterScripts()->OnPlayerSpawn((cell)m_bytePlayerID);
		CGameMode *pGameMode = pNetGame->GetGameMode();
		if(pGameMode) pGameMode->OnPlayerSpawn((cell)m_bytePlayerID);

		// Reset all their sync attributes.
		m_ofSync.vecPos.X = m_SpawnInfo.vecPos.X;
		m_ofSync.vecPos.Y = m_SpawnInfo.vecPos.Y;
		m_ofSync.vecPos.Z = m_SpawnInfo.vecPos.Z;
		m_vecPos.X = m_SpawnInfo.vecPos.X;
		m_vecPos.Y = m_SpawnInfo.vecPos.Y;
		m_vecPos.Z = m_SpawnInfo.vecPos.Z;
		m_ofSync.fRotation = m_SpawnInfo.fRotation;
		m_VehicleID=0;
		
		// Spawn them
		SpawnForWorld(m_SpawnInfo.byteTeam,m_SpawnInfo.iSkin,&m_SpawnInfo.vecPos,m_SpawnInfo.fRotation);
	}
}

//----------------------------------------------------
// This is the method used for respawning.

void CPlayer::SpawnForWorld(BYTE byteTeam, int iSkin, VECTOR * vecPos, float fRotation)
{
	RakNet::BitStream bsPlayerSpawn;
	PlayerID playerid = pNetGame->GetRakServer()->GetPlayerIDFromIndex(m_bytePlayerID);

	bsPlayerSpawn.Write(m_bytePlayerID);
	bsPlayerSpawn.Write(byteTeam);
	bsPlayerSpawn.Write(iSkin);
	bsPlayerSpawn.Write(vecPos->X);
	bsPlayerSpawn.Write(vecPos->Y);
	bsPlayerSpawn.Write(vecPos->Z);
	bsPlayerSpawn.Write(fRotation);
	bsPlayerSpawn.Write(m_dwColor);
	//bsPlayerSpawn.Write(m_byteShow);
	
	pNetGame->GetRakServer()->RPC(RPC_Spawn,&bsPlayerSpawn,
		HIGH_PRIORITY,RELIABLE,0,playerid,true,false);

	SetState(PLAYER_STATE_SPAWNED);

	// Set their initial sync position to their spawn position.
	m_ofSync.vecPos.X = vecPos->X;
	m_ofSync.vecPos.Y = vecPos->Y;
	m_ofSync.vecPos.Z = vecPos->Z;
}

//----------------------------------------------------
// This is the method used for spawning players that
// already exist in the world when the client connects.

void CPlayer::SpawnForPlayer(BYTE byteForPlayerID)
{
	RakNet::BitStream bsPlayerSpawn;

	bsPlayerSpawn.Write(m_bytePlayerID);
	bsPlayerSpawn.Write(m_SpawnInfo.byteTeam);
	bsPlayerSpawn.Write(m_SpawnInfo.iSkin);
	bsPlayerSpawn.Write(m_ofSync.vecPos.X);
	bsPlayerSpawn.Write(m_ofSync.vecPos.Y);
	bsPlayerSpawn.Write(m_ofSync.vecPos.Z);
	bsPlayerSpawn.Write(m_ofSync.fRotation);
	bsPlayerSpawn.Write(m_dwColor);

	pNetGame->GetRakServer()->RPC(RPC_Spawn,&bsPlayerSpawn,HIGH_PRIORITY,RELIABLE,
		0,pNetGame->GetRakServer()->GetPlayerIDFromIndex(byteForPlayerID),false,false);
}

//----------------------------------------------------

void CPlayer::EnterVehicle(VEHICLEID VehicleID, BYTE bytePassenger)
{
	// Just send the notification to all the clients.
	RakNet::BitStream bsVehicle;
	PlayerID playerid = pNetGame->GetRakServer()->GetPlayerIDFromIndex(m_bytePlayerID);

	/*
	if(bytePassenger) {
		SetState(PLAYER_STATE_ENTER_VEHICLE_PASSENGER);
	} else {
		SetState(PLAYER_STATE_ENTER_VEHICLE_DRIVER);
	}*/
	
	pNetGame->GetFilterScripts()->OnPlayerEnterVehicle((cell)m_bytePlayerID, (cell)VehicleID, (cell)bytePassenger);
	CGameMode *pGameMode = pNetGame->GetGameMode();
	if(pGameMode) pGameMode->OnPlayerEnterVehicle((cell)m_bytePlayerID, (cell)VehicleID, (cell)bytePassenger);

	bsVehicle.Write(m_bytePlayerID);
	bsVehicle.Write(VehicleID);
	bsVehicle.Write(bytePassenger);

	pNetGame->GetRakServer()->RPC(RPC_EnterVehicle,&bsVehicle,HIGH_PRIORITY,RELIABLE_ORDERED,0,playerid,true,false);
}

//----------------------------------------------------

void CPlayer::ExitVehicle(VEHICLEID VehicleID)
{
	// Just send the notification to all the clients.
	RakNet::BitStream bsVehicle;
	PlayerID playerid = pNetGame->GetRakServer()->GetPlayerIDFromIndex(m_bytePlayerID);

	//SetState(PLAYER_STATE_EXIT_VEHICLE);

	pNetGame->GetFilterScripts()->OnPlayerExitVehicle((cell)m_bytePlayerID, (cell)VehicleID);
	CGameMode *pGameMode = pNetGame->GetGameMode();
	if(pGameMode) pGameMode->OnPlayerExitVehicle((cell)m_bytePlayerID, (cell)VehicleID);

	bsVehicle.Write(m_bytePlayerID);
	bsVehicle.Write(VehicleID);

	pNetGame->GetRakServer()->RPC(RPC_ExitVehicle,&bsVehicle,HIGH_PRIORITY,RELIABLE_ORDERED,
		0,playerid,true,false);
}

//----------------------------------------------------

void CPlayer::SetPlayerColor(DWORD dwColor)
{
	RakNet::BitStream bsColor;
	
	m_dwColor = dwColor;

	bsColor.Write(m_bytePlayerID);
	bsColor.Write(dwColor);

	pNetGame->GetRakServer()->RPC(RPC_ScrSetPlayerColor,&bsColor,HIGH_PRIORITY,RELIABLE,
		0,UNASSIGNED_PLAYER_ID,true,false);
}

//----------------------------------------------------

void CPlayer::SetCheckpoint(float fX, float fY, float fZ, float fSize)
{
	m_vecCheckpoint.X = fX;
	m_vecCheckpoint.Y = fY;
	m_vecCheckpoint.Z = fZ;
	m_fCheckpointSize = fSize;
	ToggleCheckpoint(TRUE);
}


//----------------------------------------------------

void CPlayer::ToggleCheckpoint(BOOL bEnabled)
{
	m_bCheckpointEnabled = bEnabled;
	m_bInCheckpoint = FALSE;

	RakServerInterface* pRak = pNetGame->GetRakServer();

	RakNet::BitStream bsParams;

	if (bEnabled)
	{
		bsParams.Write(m_vecCheckpoint.X);
		bsParams.Write(m_vecCheckpoint.Y);
		bsParams.Write(m_vecCheckpoint.Z);
		bsParams.Write(m_fCheckpointSize);
		pRak->RPC(RPC_SetCheckpoint, &bsParams, HIGH_PRIORITY, RELIABLE, 0,
			pRak->GetPlayerIDFromIndex(m_bytePlayerID), false, false);
	} else {
		pRak->RPC(RPC_DisableCheckpoint, &bsParams, HIGH_PRIORITY, RELIABLE, 0,
			pRak->GetPlayerIDFromIndex(m_bytePlayerID), false, false);
	}
}

//----------------------------------------------------

void CPlayer::SetRaceCheckpoint(int iType, float fX, float fY, float fZ, float fNX, float fNY, float fNZ, float fSize)
{
	m_vecRaceCheckpoint.X = fX;
	m_vecRaceCheckpoint.Y = fY;
	m_vecRaceCheckpoint.Z = fZ;
	m_vecRaceNextCheckpoint.X = fNX;
	m_vecRaceNextCheckpoint.Y = fNY;
	m_vecRaceNextCheckpoint.Z = fNZ;
	m_fRaceCheckpointSize = fSize;
	m_byteRaceCheckpointType = iType;
	ToggleRaceCheckpoint(TRUE);
}

//----------------------------------------------------

void CPlayer::SetWeaponSlot(BYTE byteSlot, DWORD dwWeapon, DWORD dwAmmo)
{
	if (byteSlot < 13)
	{
		//m_WeaponSlots[byteSlot]->dwType = dwWeapon;
		//m_WeaponSlots[byteSlot]->dwAmmo = dwAmmo;
		m_byteSlotWeapon[byteSlot] = dwWeapon;
		m_dwSlotAmmo[byteSlot] = dwAmmo;
	}
}

//----------------------------------------------------

void CPlayer::SetCurrentWeaponAmmo(DWORD dwAmmo)
{
	BYTE i;
	for (i = 0; i < 13; i++)
	{
		if (m_byteSlotWeapon[i] == m_ofSync.byteCurrentWeapon)
		{
			m_dwSlotAmmo[i] = dwAmmo;
			break;
		}
	}
}

//----------------------------------------------------

void CPlayer::ToggleRaceCheckpoint(BOOL bEnabled)
{
	m_bRaceCheckpointEnabled = bEnabled;
	m_bInRaceCheckpoint = FALSE;

	RakServerInterface* pRak = pNetGame->GetRakServer();

	RakNet::BitStream bsParams;

	if (bEnabled)
	{
		bsParams.Write(m_byteRaceCheckpointType);
		bsParams.Write(m_vecRaceCheckpoint.X);
		bsParams.Write(m_vecRaceCheckpoint.Y);
		bsParams.Write(m_vecRaceCheckpoint.Z);
		bsParams.Write(m_vecRaceNextCheckpoint.X);
		bsParams.Write(m_vecRaceNextCheckpoint.Y);
		bsParams.Write(m_vecRaceNextCheckpoint.Z);
		bsParams.Write(m_fRaceCheckpointSize);
		pRak->RPC(RPC_SetRaceCheckpoint, &bsParams, HIGH_PRIORITY, RELIABLE, 0,
			pRak->GetPlayerIDFromIndex(m_bytePlayerID), false, false);
	} else {
		pRak->RPC(RPC_DisableRaceCheckpoint, &bsParams, HIGH_PRIORITY, RELIABLE, 0,
			pRak->GetPlayerIDFromIndex(m_bytePlayerID), false, false);
	}
}

//----------------------------------------------------

void CPlayer::SetTime(BYTE byteHour, BYTE byteMinute)
{
	
	RakNet::BitStream bsPlayerTime;

	bsPlayerTime.Write(byteHour);
	bsPlayerTime.Write(byteMinute);

	pNetGame->GetRakServer()->RPC(RPC_SetTimeEx,&bsPlayerTime,HIGH_PRIORITY,RELIABLE,
		0,pNetGame->GetRakServer()->GetPlayerIDFromIndex(m_bytePlayerID),false,false);

	m_fGameTime = (float)((byteHour * 60) + byteMinute); // Save server representation

}

//----------------------------------------------------

void CPlayer::SetClock(BYTE byteClock)
{
	m_byteTime = byteClock;

	RakNet::BitStream bsPlayerClock;

	bsPlayerClock.Write(byteClock);

	pNetGame->GetRakServer()->RPC(RPC_ToggleClock,&bsPlayerClock,HIGH_PRIORITY,RELIABLE,
		0,pNetGame->GetRakServer()->GetPlayerIDFromIndex(m_bytePlayerID),false,false);
	
}

//----------------------------------------------------

void CPlayer::CheckKeyUpdatesForScript(WORD wKeys)
{
	if(m_wLastKeys != wKeys) {
		if(pNetGame->GetGameMode()) {
			pNetGame->GetGameMode()->OnPlayerKeyStateChange(m_bytePlayerID,wKeys,m_wLastKeys);
		}
		if(pNetGame->GetFilterScripts()) {
			pNetGame->GetFilterScripts()->OnPlayerKeyStateChange(m_bytePlayerID,wKeys,m_wLastKeys);
		}
		m_wLastKeys = wKeys;
	}
}

//----------------------------------------------------
// EOF
