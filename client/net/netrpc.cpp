//----------------------------------------------------------
//
// SA:MP Multiplayer Modification For GTA:SA
// Copyright 2004-2005 SA:MP team
//
// Version: $Id: netrpc.cpp,v 1.40 2006/06/02 13:24:20 mike Exp $
//
//----------------------------------------------------------

#include "../main.h"

extern CGame		 *pGame;
extern CChatWindow   *pChatWindow;
extern CCmdWindow	 *pCmdWindow;

using namespace RakNet;
extern CNetGame* pNetGame;

#define REJECT_REASON_BAD_VERSION   1
#define REJECT_REASON_BAD_NICKNAME  2
#define REJECT_REASON_BAD_MOD		3
#define REJECT_REASON_BAD_PLAYERID	4

// No longer needed in new RakNet
//#define REGISTER_STATIC_RPC REGISTER_AS_REMOTE_PROCEDURE_CALL 
//#define UNREGISTER_STATIC_RPC UNREGISTER_AS_REMOTE_PROCEDURE_CALL

void ProcessIncommingEvent(BYTE bytePlayerID, int iEventType, DWORD dwParam1, DWORD dwParam2, DWORD dwParam3);

//----------------------------------------------------
// Sent when a client joins the server we're
// currently connected to.

void ServerJoin(RPCParameters *rpcParams)
{
	PCHAR Data = reinterpret_cast<PCHAR>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;
	PlayerID sender = rpcParams->sender;

	CHAR szPlayerName[MAX_PLAYER_NAME+1];
	BYTE bytePlayerID;
	BYTE byteNameLen=0;

	RakNet::BitStream bsData(Data,(iBitLength/8)+1,false);
	CPlayerPool *pPlayerPool = pNetGame->GetPlayerPool();
	
	bsData.Read(bytePlayerID);
	bsData.Read(byteNameLen);
	bsData.Read(szPlayerName,byteNameLen);
	szPlayerName[byteNameLen] = '\0';

	// Add this client to the player pool.
	pPlayerPool->New(bytePlayerID, szPlayerName);

	/*
	switch (bytePlayerState) {
		case PLAYER_STATE_SPECTATING:
			pPlayerPool->GetAt(bytePlayerID)->SetState(PLAYER_STATE_SPECTATING);
	}*/
}

//----------------------------------------------------
// Sent when a client joins the server we're
// currently connected to.

void ServerQuit(RPCParameters *rpcParams)
{
	PCHAR Data = reinterpret_cast<PCHAR>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;
	PlayerID sender = rpcParams->sender;

	RakNet::BitStream bsData(Data,(iBitLength/8)+1,false);
	CPlayerPool *pPlayerPool = pNetGame->GetPlayerPool();
	BYTE bytePlayerID;
	BYTE byteReason;

	bsData.Read(bytePlayerID);
	bsData.Read(byteReason);

	// Delete this client from the player pool.
	pPlayerPool->Delete(bytePlayerID,byteReason);
}

//----------------------------------------------------
// Server is giving us basic init information.

void InitGame(RPCParameters *rpcParams)
{
	PCHAR Data = reinterpret_cast<PCHAR>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;
	PlayerID sender = rpcParams->sender;

	RakNet::BitStream bsInitGame(Data,(iBitLength/8)+1,false);
	CPlayerPool *pPlayerPool = pNetGame->GetPlayerPool();
	BYTE byteMyPlayerID;
	bool bLanMode, bStuntBonus;

	bsInitGame.Read(pNetGame->m_iSpawnsAvailable);
	bsInitGame.Read(byteMyPlayerID);
	bsInitGame.Read(pNetGame->m_bShowPlayerTags);
	bsInitGame.Read(pNetGame->m_bShowPlayerMarkers);
	bsInitGame.Read(pNetGame->m_bTirePopping);
	bsInitGame.Read(pNetGame->m_byteWorldTime);
	bsInitGame.Read(pNetGame->m_byteWeather);
	bsInitGame.Read(pNetGame->m_fGravity);
	bsInitGame.Read(bLanMode);
	bsInitGame.Read((int)pNetGame->m_iDeathDropMoney);
	bsInitGame.Read(pNetGame->m_bInstagib);
	bsInitGame.Read(pNetGame->m_bZoneNames);
	bsInitGame.Read(pNetGame->m_bUseCJWalk);	
	bsInitGame.Read(pNetGame->m_bAllowWeapons);
	bsInitGame.Read(pNetGame->m_bLimitGlobalChatRadius);
	bsInitGame.Read(pNetGame->m_fGlobalChatRadius);
	bsInitGame.Read(bStuntBonus);
	bsInitGame.Read(pNetGame->m_fNameTagDrawDistance);
	bsInitGame.Read(pNetGame->m_bDisableEnterExits);

	BYTE byteStrLen;
	bsInitGame.Read(byteStrLen);
	if(byteStrLen) {
		memset(pNetGame->m_szHostName,0,sizeof(pNetGame->m_szHostName));
		bsInitGame.Read(pNetGame->m_szHostName, byteStrLen);
	}
	pNetGame->m_szHostName[byteStrLen] = '\0';

	pPlayerPool->SetLocalPlayerID(byteMyPlayerID);
	pGame->EnableStuntBonus(bStuntBonus);
	if (bLanMode) pNetGame->SetLanMode(TRUE);

	pNetGame->InitGameLogic();

	// Set the gravity now
	pGame->SetGravity(pNetGame->m_fGravity);

	// Disable the enter/exits if needed.
	if(pNetGame->m_bDisableEnterExits) {
		pGame->DisableEnterExits();
	}
	
	pNetGame->SetGameState(GAMESTATE_CONNECTED);
	pPlayerPool->GetLocalPlayer()->HandleClassSelection();

	pChatWindow->AddDebugMessage("Connected to %.64s",pNetGame->m_szHostName);

}

//----------------------------------------------------
// Remote player has sent a chat message.

void Chat(RPCParameters *rpcParams)
{
	PCHAR Data = reinterpret_cast<PCHAR>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;
	PlayerID sender = rpcParams->sender;

	RakNet::BitStream bsData(Data,(iBitLength/8)+1,false);
	BYTE bytePlayerID;
	BYTE byteTextLen;

	if(pNetGame->GetGameState() != GAMESTATE_CONNECTED)	return;

	unsigned char szText[256];
	memset(szText,0,256);

	bsData.Read(bytePlayerID);
	bsData.Read(byteTextLen);

	if(byteTextLen > MAX_CMD_INPUT) return;

	bsData.Read((char*)szText,byteTextLen);

	szText[byteTextLen] = '\0';

	CPlayerPool* pPlayerPool = pNetGame->GetPlayerPool();
	if (bytePlayerID == pPlayerPool->GetLocalPlayerID())
	{
		pChatWindow->AddChatMessage(pPlayerPool->GetLocalPlayerName(),
			pPlayerPool->GetLocalPlayer()->GetPlayerColorAsARGB(), (char*)szText);
	} else {
		CRemotePlayer *pRemotePlayer = pNetGame->GetPlayerPool()->GetAt(bytePlayerID);
		if(pRemotePlayer) {
			pRemotePlayer->Say(szText);	
		}
	}
}

//----------------------------------------------------
// Remote player has sent a private chat message.

void Privmsg(RPCParameters *rpcParams)
{
	PCHAR Data = reinterpret_cast<PCHAR>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;
	PlayerID sender = rpcParams->sender;

	RakNet::BitStream bsData(Data,(iBitLength/8)+1,false);

	if(pNetGame->GetGameState() != GAMESTATE_CONNECTED)	return;

	BYTE bytePlayerID;
	BYTE byteToPlayerID;
	BYTE byteTextLen;
	CHAR szText[256];
	CHAR szStr[256];

	bsData.Read(bytePlayerID);
	bsData.Read(byteToPlayerID);
	bsData.Read(byteTextLen);

	if(byteTextLen > MAX_CMD_INPUT) return;

	bsData.Read(szText,byteTextLen);

	szText[byteTextLen] = '\0';

	CPlayerPool* pPlayerPool = pNetGame->GetPlayerPool();
	if (bytePlayerID == pPlayerPool->GetLocalPlayerID())
	{
		sprintf(szStr, "PM sent to %s: %s", pPlayerPool->GetPlayerName(byteToPlayerID), szText);
		pChatWindow->AddClientMessage(D3DCOLOR_ARGB(255,220,24,26), szStr);
	} else {
		CRemotePlayer *pRemotePlayer = pNetGame->GetPlayerPool()->GetAt(bytePlayerID);
		if(pRemotePlayer) {
			pRemotePlayer->Privmsg(szText);	
		}
	}
}

//----------------------------------------------------
// Remote player has sent a team chat message.

void TeamPrivmsg(RPCParameters *rpcParams)
{
	PCHAR Data = reinterpret_cast<PCHAR>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;
	PlayerID sender = rpcParams->sender;

	RakNet::BitStream bsData(Data,(iBitLength/8)+1,false);

	if(pNetGame->GetGameState() != GAMESTATE_CONNECTED)	return;

    BYTE bytePlayerID;
	BYTE byteTextLen;
	CHAR szText[256];

	bsData.Read(bytePlayerID);
	bsData.Read(byteTextLen);

	if(byteTextLen > MAX_CMD_INPUT) return;

	bsData.Read(szText,byteTextLen);

	szText[byteTextLen] = '\0';

	CPlayerPool* pPlayerPool = pNetGame->GetPlayerPool();
	if (bytePlayerID == pPlayerPool->GetLocalPlayerID())
	{
		char szTempBuffer[256];
		sprintf(szTempBuffer, "Team PM sent: %s", szText);
		pChatWindow->AddClientMessage(D3DCOLOR_ARGB(255,220,24,26), szTempBuffer);
	} else {
		CRemotePlayer *pRemotePlayer = pNetGame->GetPlayerPool()->GetAt(bytePlayerID);
		if(pRemotePlayer) {
			pRemotePlayer->TeamPrivmsg(szText);
		}
	}
}

//----------------------------------------------------
// Reply to our class request from the server.

void RequestClass(RPCParameters *rpcParams)
{
	PCHAR Data = reinterpret_cast<PCHAR>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;
	PlayerID sender = rpcParams->sender;

	RakNet::BitStream bsData(Data,(iBitLength/8)+1,false);
	BYTE byteRequestOutcome=0;
	PLAYER_SPAWN_INFO SpawnInfo;
	CLocalPlayer *pPlayer = pNetGame->GetPlayerPool()->GetLocalPlayer();

	bsData.Read(byteRequestOutcome);
	bsData.Read((PCHAR)&SpawnInfo,sizeof(PLAYER_SPAWN_INFO));

	if(byteRequestOutcome) {
		pPlayer->SetSpawnInfo(&SpawnInfo);
		pPlayer->HandleClassSelectionOutcome(TRUE);
	}
	else {
		pPlayer->HandleClassSelectionOutcome(FALSE);
	}
}

//----------------------------------------------------
// The server has allowed us to spawn!

void RequestSpawn(RPCParameters *rpcParams)
{
	PCHAR Data = reinterpret_cast<PCHAR>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;
	PlayerID sender = rpcParams->sender;

	BYTE byteRequestOutcome=0;
	RakNet::BitStream bsData(Data,(iBitLength/8)+1,false);
	bsData.Read(byteRequestOutcome);
	CLocalPlayer *pPlayer = pNetGame->GetPlayerPool()->GetLocalPlayer();
	if (byteRequestOutcome == 2 || (byteRequestOutcome && pPlayer->m_bWaitingForSpawnRequestReply)) {
		pPlayer->Spawn();
	}
	else {
		pPlayer->m_bWaitingForSpawnRequestReply = false;
	}
}

//----------------------------------------------------
// Remote client is spawning.

void Spawn(RPCParameters *rpcParams)
{
	PCHAR Data = reinterpret_cast<PCHAR>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;
	PlayerID sender = rpcParams->sender;

	RakNet::BitStream bsData(Data,(iBitLength/8)+1,false);
	CRemotePlayer *pRemotePlayer;

	BYTE bytePlayerID=0;
	BYTE byteTeam=0;
	int iSkin=0;
	VECTOR vecPos;
	float fRotation=0;
	DWORD dwColor=0;

	bsData.Read(bytePlayerID);
	bsData.Read(byteTeam);
	bsData.Read(iSkin);
	bsData.Read(vecPos.X);
	bsData.Read(vecPos.Y);
	bsData.Read(vecPos.Z);
	bsData.Read(fRotation);
	bsData.Read(dwColor);

	pRemotePlayer = pNetGame->GetPlayerPool()->GetAt(bytePlayerID);

	if(pRemotePlayer) 
		pRemotePlayer->Spawn(byteTeam,iSkin,&vecPos,fRotation,dwColor);
		
}

//----------------------------------------------------
// Remote client is dead.

void Death(RPCParameters *rpcParams)
{
	PCHAR Data = reinterpret_cast<PCHAR>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;
	PlayerID sender = rpcParams->sender;

	RakNet::BitStream bsData(Data,(iBitLength/8)+1,false);

	BYTE bytePlayerID=0;

	bsData.Read(bytePlayerID);

	CRemotePlayer *pRemotePlayer = pNetGame->GetPlayerPool()->GetAt(bytePlayerID);
	if(pRemotePlayer) {
		CPlayerPed* pRemotePed = pRemotePlayer->GetPlayerPed();
		#ifdef WANT_PICKUP
		if (pRemotePed->GetDistanceFromLocalPlayerPed() <= 100.0f)
		{
			MATRIX4X4 matPlayer;
			pRemotePed->GetMatrix(&matPlayer);

			WEAPON_SLOT_TYPE * WeaponSlot = pRemotePlayer->GetCurrentWeaponSlot()
			if (WeaponSlot->dwType != 0 &&
				WeaponSlot->dwType != WEAPON_GRENADE &&
				WeaponSlot->dwType != WEAPON_TEARGAS &&
				WeaponSlot->dwType != WEAPON_MOLTOV)
			{
				//DWORD hnd = 0;
				//ScriptCommand(&create_pickup_with_ammo, pGame->GetWeaponModelIDFromWeapon(pRemotePed->m_pPed->WeaponSlots[i].dwType), 4, pRemotePed->m_pPed->WeaponSlots[i].dwAmmoInClip, matPlayer.pos.X + rand()%4, matPlayer.pos.Y + rand()%4, matPlayer.pos.Z, &hnd);	
				pNetGame->GetPickupPool()->New(pGame->GetWeaponModelIDFromWeapon(WeaponSlot->dwType), matPlayer.pos.X, matPlayer.pos.Y, matPlayer.pos.Z, WeaponSlot->dwAmmoInClip, bytePlayerID);
			}
		}
		#endif
		pRemotePlayer->HandleDeath();
	}
}

//----------------------------------------------------
// Remote client is trying to enter vehicle gracefully.

void EnterVehicle(RPCParameters *rpcParams)
{
	PCHAR Data = reinterpret_cast<PCHAR>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;
	PlayerID sender = rpcParams->sender;

	RakNet::BitStream bsData(Data,(iBitLength/8)+1,false);
	BYTE bytePlayerID=0;
	VEHICLEID VehicleID=0;
	BYTE bytePassenger=0;
	BOOL bPassenger=FALSE;

	bsData.Read(bytePlayerID);
	bsData.Read(VehicleID);
	bsData.Read(bytePassenger);

	if(bytePassenger) bPassenger = TRUE;

	CRemotePlayer *pRemotePlayer = pNetGame->GetPlayerPool()->GetAt(bytePlayerID);
	CVehiclePool *pVehiclePool = pNetGame->GetVehiclePool();

	if(pRemotePlayer) {
		pRemotePlayer->EnterVehicle(VehicleID,bPassenger);
	}

	//pChatWindow->AddDebugMessage("Player(%u)::EnterVehicle(%u)",bytePlayerID,byteVehicleID);
}

//----------------------------------------------------
// Remote client is trying to enter vehicle gracefully.

void ExitVehicle(RPCParameters *rpcParams)
{
	PCHAR Data = reinterpret_cast<PCHAR>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;
	PlayerID sender = rpcParams->sender;

	RakNet::BitStream bsData(Data,(iBitLength/8)+1,false);
	BYTE bytePlayerID=0;
	VEHICLEID VehicleID=0;

	bsData.Read(bytePlayerID);
	bsData.Read(VehicleID);

	CRemotePlayer *pRemotePlayer = pNetGame->GetPlayerPool()->GetAt(bytePlayerID);
	CVehiclePool *pVehiclePool = pNetGame->GetVehiclePool();

	if(pRemotePlayer) {
		pRemotePlayer->ExitVehicle();
	}

	//pChatWindow->AddDebugMessage("Player(%u)::ExitVehicle(%u)",bytePlayerID,byteVehicleID);
}

//----------------------------------------------------

void VehicleSpawn(RPCParameters *rpcParams)
{
	PCHAR Data = reinterpret_cast<PCHAR>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;
	PlayerID sender = rpcParams->sender;

	CVehiclePool *pVehiclePool = pNetGame->GetVehiclePool();
	RakNet::BitStream bsData(Data,(iBitLength/8)+1,false);
	VEHICLEID VehicleID=0;
	int iVehicleType;
	VECTOR vecPos;
	VECTOR vecSpawnPos;
	float fRotation;
	float fSpawnRotation;
	float fHealth;
	int iColor1, iColor2;
	int iInterior;
	bool bHasNumberPlate = false;
	bool bHasCarModInfo = false;

	CAR_MOD_INFO	m_CarModInfo;
	CHAR cNumberPlate[9];

	memset(&m_CarModInfo,0,sizeof(CAR_MOD_INFO));
	memset(&cNumberPlate[0],0,9);

	bsData.Read(VehicleID);
	bsData.Read(iVehicleType);
	bsData.Read(vecPos.X);
	bsData.Read(vecPos.Y);
	bsData.Read(vecPos.Z);
	bsData.Read(fRotation);
	bsData.Read(iColor1);
	bsData.Read(iColor2);
	bsData.Read(fHealth);
	bsData.Read(vecSpawnPos.X);
	bsData.Read(vecSpawnPos.Y);
	bsData.Read(vecSpawnPos.Z);
	bsData.Read(fSpawnRotation);
	bsData.Read(iInterior);

	bsData.Read(bHasNumberPlate);
	if(bHasNumberPlate) {
		bsData.Read(cNumberPlate, 9); // Constant size defined by SA
	}

	bsData.Read(bHasCarModInfo);
	if(bHasCarModInfo) {
		bsData.Read((PCHAR)&m_CarModInfo, sizeof(m_CarModInfo));
	} else {
		m_CarModInfo.iColor0 = -1;
		m_CarModInfo.iColor1 = -1;
	}

	if (!pVehiclePool->New(VehicleID,iVehicleType, &vecPos,fRotation,iColor1,iColor2,&vecSpawnPos,fSpawnRotation, iInterior, cNumberPlate))
		return;
		
	int iVehicle = pVehiclePool->FindGtaIDFromID(VehicleID);

	if (iVehicle)
	{
		BYTE*	pDataStart	= (BYTE*)&m_CarModInfo.byteCarMod0;
		for(int i = 0; i < 14; i++)
		{
			DWORD data = pDataStart[i];
			DWORD v = 0;

			if (data == 0)
				continue;

			data += 1000;

			pGame->RequestModel(data);
			pGame->LoadRequestedModels();
			ScriptCommand(&request_car_component,data);

			int iWait = 10;
			while(!ScriptCommand(&is_component_available,data) && iWait) {
				Sleep(5);
				iWait--;
			}
			if(!iWait) {
				//pChatWindow->AddDebugMessage("Timeout on car component.");
				continue;
			}
			//pChatWindow->AddDebugMessage("CarComponent: %u,%u,%u",VehicleID,data,iVehicleType);
			ScriptCommand(&add_car_component,iVehicle,data,&v);
		}

		if(m_CarModInfo.bytePaintJob)
			ScriptCommand(&change_car_skin,iVehicle,m_CarModInfo.bytePaintJob);

		if(m_CarModInfo.iColor0 != -1 || m_CarModInfo.iColor1 != -1) 
			pVehiclePool->GetAt(VehicleID)->SetColor(m_CarModInfo.iColor0,m_CarModInfo.iColor1);
	}


}

//----------------------------------------------------

void SetCheckpoint(RPCParameters *rpcParams)
{
	PCHAR Data = reinterpret_cast<PCHAR>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;
	//PlayerID sender = rpcParams->sender;

	RakNet::BitStream bsData(Data,(iBitLength/8)+1,false);
	float fX, fY, fZ, fSize;

	bsData.Read(fX);
	bsData.Read(fY);
	bsData.Read(fZ);
	bsData.Read(fSize);

	VECTOR Pos,Extent;

	Pos.X = fX;
	Pos.Y = fY;
	Pos.Z = fZ;
	Extent.X = fSize;
	Extent.Y = fSize;
	Extent.Z = fSize;

	pGame->SetCheckpointInformation(&Pos, &Extent);
	pGame->ToggleCheckpoints(TRUE);
}

//----------------------------------------------------

void DisableCheckpoint(RPCParameters *rpcParams)
{
	//PCHAR Data = reinterpret_cast<PCHAR>(rpcParams->input);
	//int iBitLength = rpcParams->numberOfBitsOfData;
	//PlayerID sender = rpcParams->sender;

	pGame->ToggleCheckpoints(FALSE);
}

//----------------------------------------------------

void SetRaceCheckpoint(RPCParameters *rpcParams)
{
	PCHAR Data = reinterpret_cast<PCHAR>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;
	//PlayerID sender = rpcParams->sender;

	RakNet::BitStream bsData(Data,(iBitLength/8)+1,false);
	float fX, fY, fZ;
	BYTE byteType; //, byteSize;
	VECTOR Pos,Next;

	bsData.Read(byteType);
	bsData.Read(fX);
	bsData.Read(fY);
	bsData.Read(fZ);
	Pos.X = fX;
	Pos.Y = fY;
	Pos.Z = fZ;

	bsData.Read(fX);
	bsData.Read(fY);
	bsData.Read(fZ);
	Next.X = fX;
	Next.Y = fY;
	Next.Z = fZ;

	bsData.Read(fX);

	pGame->SetRaceCheckpointInformation(byteType, &Pos, &Next, fX);
	pGame->ToggleRaceCheckpoints(TRUE);
}

//----------------------------------------------------

void DisableRaceCheckpoint(RPCParameters *rpcParams)
{
	//PCHAR Data = reinterpret_cast<PCHAR>(rpcParams->input);
	//int iBitLength = rpcParams->numberOfBitsOfData;
	//PlayerID sender = rpcParams->sender;

	pGame->ToggleRaceCheckpoints(FALSE);
}

//----------------------------------------------------

void UpdateScoresPingsIPs(RPCParameters *rpcParams)
{
	PCHAR Data = reinterpret_cast<PCHAR>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;
	//PlayerID sender = rpcParams->sender;

	RakNet::BitStream bsData(Data,(iBitLength/8)+1,false);

	BYTE bytePlayerId;
	int  iPlayerScore;
	DWORD dwPlayerPing;

	CPlayerPool* pPlayerPool = pNetGame->GetPlayerPool();

	for (BYTE i=0; i<(iBitLength/8)/9; i++)
	{
		bsData.Read(bytePlayerId);
		bsData.Read(iPlayerScore);
		bsData.Read(dwPlayerPing);

		pPlayerPool->UpdateScore(bytePlayerId, iPlayerScore);
		pPlayerPool->UpdatePing(bytePlayerId, dwPlayerPing);
	}
}

//----------------------------------------------------
extern RakNetStatisticsStruct RakServerStats;

void SvrStats(RPCParameters *rpcParams)
{
	PCHAR Data = reinterpret_cast<PCHAR>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;
	RakNet::BitStream bsData(Data,(iBitLength/8)+1,false);
	bsData.Read((char *)&RakServerStats,sizeof(RakNetStatisticsStruct));
}

//----------------------------------------------------

void GameModeRestart(RPCParameters *rpcParams)
{
	//PCHAR Data = reinterpret_cast<PCHAR>(rpcParams->input);
	//int iBitLength = rpcParams->numberOfBitsOfData;
	//PlayerID sender = rpcParams->sender;

	pNetGame->ShutdownForGameModeRestart();
}

//----------------------------------------------------

void ConnectionRejected(RPCParameters *rpcParams)
{
	PCHAR Data = reinterpret_cast<PCHAR>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;
	//PlayerID sender = rpcParams->sender;

	RakNet::BitStream bsData(Data,(iBitLength/8)+1,false);
	BYTE byteRejectReason;

	bsData.Read(byteRejectReason);

	if(byteRejectReason==REJECT_REASON_BAD_VERSION) {
		pChatWindow->AddInfoMessage("DISCONNECTED. THE SERVER DID NOT ALLOW A CONNECTION.");
	}
	else if(byteRejectReason==REJECT_REASON_BAD_NICKNAME)
	{
		pChatWindow->AddInfoMessage("CONNECTION REJECTED. BAD NICKNAME!");
		pChatWindow->AddInfoMessage("Please choose another nick between 3-16 characters");
		pChatWindow->AddInfoMessage("containing only A-Z a-z 0-9 [ ] or _");
		pChatWindow->AddInfoMessage("Use /quit to exit or press ESC and select Quit Game");
	}
	else if(byteRejectReason==REJECT_REASON_BAD_MOD)
	{
		pChatWindow->AddInfoMessage("CONNECTION REJECTED");
		pChatWindow->AddInfoMessage("YOUR'RE USING AN INCORRECT MOD!");
	}
	else if(byteRejectReason==REJECT_REASON_BAD_PLAYERID)
	{
		pChatWindow->AddInfoMessage("Connection was closed by the server.");
		pChatWindow->AddInfoMessage("Unable to allocate a player slot. Try again.");
	}

	pNetGame->GetRakClient()->Disconnect(500);
}

//----------------------------------------------------

void ClientMessage(RPCParameters *rpcParams)
{
	PCHAR Data = reinterpret_cast<PCHAR>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;
	//PlayerID sender = rpcParams->sender;

	RakNet::BitStream bsData(Data,(iBitLength/8)+1,false);
	DWORD dwStrLen;
	DWORD dwColor;

	bsData.Read(dwColor);
	bsData.Read(dwStrLen);
	char* szMsg = (char*)malloc(dwStrLen+1);
	bsData.Read(szMsg, dwStrLen);
	szMsg[dwStrLen] = 0;

	pChatWindow->AddClientMessage(dwColor,szMsg);

	free(szMsg);
}

//----------------------------------------------------

void WorldTime(RPCParameters *rpcParams)
{
	PCHAR Data = reinterpret_cast<PCHAR>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;
	//PlayerID sender = rpcParams->sender;

	RakNet::BitStream bsData(Data,(iBitLength/8)+1,false);
	BYTE byteWorldTime;
	bsData.Read(byteWorldTime);
	pNetGame->m_byteWorldTime = byteWorldTime;	
}

//----------------------------------------------------

void Pickup(RPCParameters *rpcParams)
{
	PCHAR Data = reinterpret_cast<PCHAR>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;
	//PlayerID sender = rpcParams->sender;

	RakNet::BitStream bsData(Data,(iBitLength/8)+1,false);

	PICKUP Pickup;
	int iIndex;

	bsData.Read(iIndex);
	bsData.Read((PCHAR)&Pickup, sizeof (PICKUP));

	//pChatWindow->AddDebugMessage("Pickup: %d %d %f %f %f",iModel,iType,x,y,z);

	pNetGame->GetPickupPool()->New(&Pickup, iIndex);
}

//----------------------------------------------------

void DestroyPickup(RPCParameters *rpcParams)
{
	PCHAR Data = reinterpret_cast<PCHAR>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;
	//PlayerID sender = rpcParams->sender;

	RakNet::BitStream bsData(Data,(iBitLength/8)+1,false);

	int iIndex;

	bsData.Read(iIndex);

	pNetGame->GetPickupPool()->Destroy(iIndex);
}

//----------------------------------------------------

void DestroyWeaponPickup(RPCParameters *rpcParams)
{
	PCHAR Data = reinterpret_cast<PCHAR>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;
	//PlayerID sender = rpcParams->sender;

	RakNet::BitStream bsData(Data,(iBitLength/8)+1,false);

	BYTE byteIndex;

	bsData.Read(byteIndex);

	pNetGame->GetPickupPool()->DestroyDropped(byteIndex);
}

//----------------------------------------------------

void ScmEvent(RPCParameters *rpcParams)
{
	PCHAR Data = reinterpret_cast<PCHAR>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;
	//PlayerID sender = rpcParams->sender;

	RakNet::BitStream bsData(Data,(iBitLength/8)+1,false);
	
	BYTE bytePlayerID;
	int iEvent;
	DWORD dwParam1,dwParam2,dwParam3;

	bsData.Read(bytePlayerID);
	bsData.Read(iEvent);
	bsData.Read(dwParam1);
	bsData.Read(dwParam2);
	bsData.Read(dwParam3);

	ProcessIncommingEvent(bytePlayerID,iEvent,dwParam1,dwParam2,dwParam3);
}

//----------------------------------------------------

void Weather(RPCParameters *rpcParams)
{
	PCHAR Data = reinterpret_cast<PCHAR>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;
	//PlayerID sender = rpcParams->sender;

	RakNet::BitStream bsData(Data,(iBitLength/8)+1,false);
	BYTE byteWeather;
	bsData.Read(byteWeather);
	pNetGame->m_byteWeather = byteWeather;	
}

//----------------------------------------------------

void SetTimeEx(RPCParameters *rpcParams)
{
	PCHAR Data = reinterpret_cast<PCHAR>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;
	//PlayerID sender = rpcParams->sender;

	RakNet::BitStream bsData(Data,(iBitLength/8)+1,false);
	BYTE byteHour;
	BYTE byteMinute;
	bsData.Read(byteHour);
	bsData.Read(byteMinute);
	//pNetGame->m_byteHoldTime = 0;
	pGame->SetWorldTime(byteHour, byteMinute);
	pNetGame->m_byteWorldTime = byteHour;
	pNetGame->m_byteWorldMinute = byteMinute;
}

//----------------------------------------------------

void ToggleClock(RPCParameters *rpcParams)
{
	PCHAR Data = reinterpret_cast<PCHAR>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;
	//PlayerID sender = rpcParams->sender;

	RakNet::BitStream bsData(Data,(iBitLength/8)+1,false);
	BYTE byteClock;
	bsData.Read(byteClock);
	pGame->EnableClock(byteClock);	
	if (byteClock)
	{
		pNetGame->m_byteHoldTime = 0;
	}
	else
	{
		pNetGame->m_byteHoldTime = 1;
		pGame->GetWorldTime((int*)&pNetGame->m_byteWorldTime, (int*)&pNetGame->m_byteWorldMinute);
	}
}

//----------------------------------------------------

void VehicleDestroy(RPCParameters *rpcParams)
{
	PCHAR Data = reinterpret_cast<PCHAR>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;
	PlayerID sender = rpcParams->sender;

	CVehiclePool *pVehiclePool = pNetGame->GetVehiclePool();
	RakNet::BitStream bsData(Data,(iBitLength/8)+1,false);

	VEHICLEID VehicleID;
	VEHICLEID MyVehicleID;

	bsData.Read(VehicleID);

	CVehicle *pVehicle = pVehiclePool->GetAt(VehicleID);
	CRemotePlayer *pRemotePlayer;
	CPlayerPool *pPlayerPool = pNetGame->GetPlayerPool();

	if(!pVehicle) return;
			
	MyVehicleID = pVehiclePool->FindIDFromGtaPtr(pGame->FindPlayerPed()->GetGtaVehicle());

	if (MyVehicleID == VehicleID)
	{
		MATRIX4X4 mat;
		pGame->FindPlayerPed()->GetMatrix(&mat);
		pGame->FindPlayerPed()->RemoveFromVehicleAndPutAt(mat.pos.X,mat.pos.Y,mat.pos.Z);
	}

	// we'll also need to remove any remote player that
	// is in this vehicle.
	BYTE x=0;
	while(x!=MAX_PLAYERS) {
		if(pPlayerPool->GetSlotState(x)) {
			pRemotePlayer = pPlayerPool->GetAt(x);
			if( pRemotePlayer->IsActive() && 
				pRemotePlayer->m_pCurrentVehicle == pVehicle )
			{
				pRemotePlayer->ForceOutOfCurrentVehicle();
			}
		}
		x++;
	}
	pNetGame->GetVehiclePool()->Delete(VehicleID);
}

//----------------------------------------------------

void Instagib(RPCParameters *rpcParams)
{
	PCHAR Data = reinterpret_cast<PCHAR>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;
	PlayerID sender = rpcParams->sender;

	RakNet::BitStream bsData(Data,(iBitLength/8)+1,false);
	bsData.Read(pNetGame->m_bInstagib);
}

//----------------------------------------------------

// AntiCheat
void ACServerProtected(RPCParameters *rpcParams)
{
	
}

//----------------------------------------------------

void RegisterRPCs(RakClientInterface * pRakClient)
{
	REGISTER_STATIC_RPC(pRakClient,ServerJoin);
	REGISTER_STATIC_RPC(pRakClient,ServerQuit);	
	REGISTER_STATIC_RPC(pRakClient,InitGame);
	REGISTER_STATIC_RPC(pRakClient,Chat);
	REGISTER_STATIC_RPC(pRakClient,Privmsg);
	REGISTER_STATIC_RPC(pRakClient,TeamPrivmsg);
	REGISTER_STATIC_RPC(pRakClient,RequestClass);
	REGISTER_STATIC_RPC(pRakClient,RequestSpawn);
	REGISTER_STATIC_RPC(pRakClient,Spawn);
	REGISTER_STATIC_RPC(pRakClient,Death);
	REGISTER_STATIC_RPC(pRakClient,EnterVehicle);
	REGISTER_STATIC_RPC(pRakClient,ExitVehicle);
	REGISTER_STATIC_RPC(pRakClient,VehicleSpawn);
	REGISTER_STATIC_RPC(pRakClient,VehicleDestroy);
	REGISTER_STATIC_RPC(pRakClient,SetCheckpoint); //14
	REGISTER_STATIC_RPC(pRakClient,DisableCheckpoint);
	REGISTER_STATIC_RPC(pRakClient,SetRaceCheckpoint);
	REGISTER_STATIC_RPC(pRakClient,DisableRaceCheckpoint);
	REGISTER_STATIC_RPC(pRakClient,UpdateScoresPingsIPs);
	REGISTER_STATIC_RPC(pRakClient,SvrStats);
	REGISTER_STATIC_RPC(pRakClient,GameModeRestart);
	REGISTER_STATIC_RPC(pRakClient,ConnectionRejected);
	REGISTER_STATIC_RPC(pRakClient,ClientMessage);
	REGISTER_STATIC_RPC(pRakClient,WorldTime);
	REGISTER_STATIC_RPC(pRakClient,Pickup);
	REGISTER_STATIC_RPC(pRakClient,DestroyPickup);
	REGISTER_STATIC_RPC(pRakClient,DestroyWeaponPickup);
	REGISTER_STATIC_RPC(pRakClient,ScmEvent);
	REGISTER_STATIC_RPC(pRakClient,Weather);
	REGISTER_STATIC_RPC(pRakClient,Instagib);
	REGISTER_STATIC_RPC(pRakClient,SetTimeEx);
	REGISTER_STATIC_RPC(pRakClient,ToggleClock); // 31
	REGISTER_STATIC_RPC(pRakClient,ACServerProtected);
}

//----------------------------------------------------

void UnRegisterRPCs(RakClientInterface * pRakClient)
{
	UNREGISTER_STATIC_RPC(pRakClient,ServerJoin);
	UNREGISTER_STATIC_RPC(pRakClient,ServerQuit);
	UNREGISTER_STATIC_RPC(pRakClient,InitGame);
	UNREGISTER_STATIC_RPC(pRakClient,Chat);
	UNREGISTER_STATIC_RPC(pRakClient,Privmsg);
	UNREGISTER_STATIC_RPC(pRakClient,TeamPrivmsg);
	UNREGISTER_STATIC_RPC(pRakClient,RequestClass);
	UNREGISTER_STATIC_RPC(pRakClient,RequestSpawn);
	UNREGISTER_STATIC_RPC(pRakClient,Spawn);
	UNREGISTER_STATIC_RPC(pRakClient,Death);
	UNREGISTER_STATIC_RPC(pRakClient,EnterVehicle);
	UNREGISTER_STATIC_RPC(pRakClient,ExitVehicle);
	UNREGISTER_STATIC_RPC(pRakClient,VehicleSpawn);
	UNREGISTER_STATIC_RPC(pRakClient,VehicleDestroy);
	UNREGISTER_STATIC_RPC(pRakClient,SetCheckpoint);
	UNREGISTER_STATIC_RPC(pRakClient,DisableCheckpoint);
	UNREGISTER_STATIC_RPC(pRakClient,SetRaceCheckpoint);
	UNREGISTER_STATIC_RPC(pRakClient,DisableRaceCheckpoint);
	UNREGISTER_STATIC_RPC(pRakClient,UpdateScoresPingsIPs);
	UNREGISTER_STATIC_RPC(pRakClient,SvrStats);
	UNREGISTER_STATIC_RPC(pRakClient,GameModeRestart);
	UNREGISTER_STATIC_RPC(pRakClient,ConnectionRejected);
	UNREGISTER_STATIC_RPC(pRakClient,ClientMessage);
	UNREGISTER_STATIC_RPC(pRakClient,WorldTime);
	UNREGISTER_STATIC_RPC(pRakClient,Pickup);
	UNREGISTER_STATIC_RPC(pRakClient,DestroyPickup);
	UNREGISTER_STATIC_RPC(pRakClient,DestroyWeaponPickup);
	UNREGISTER_STATIC_RPC(pRakClient,ScmEvent);
	UNREGISTER_STATIC_RPC(pRakClient,Weather);
	UNREGISTER_STATIC_RPC(pRakClient,Instagib);
	UNREGISTER_STATIC_RPC(pRakClient,SetTimeEx);
	UNREGISTER_STATIC_RPC(pRakClient,ToggleClock);
	UNREGISTER_STATIC_RPC(pRakClient,ACServerProtected);
}

//----------------------------------------------------
