/*

	SA:MP Multiplayer Modification
	Copyright 2004-2005 SA:MP Team

    Version: $Id: netrpc.cpp,v 1.52 2006/06/02 13:24:21 mike Exp $

*/

#include "main.h"
#include "vehmods.h"
//#include "anticheat.h"

RakServerInterface		*pRak=0;

// Removed for RakNet upgrade
//#define REGISTER_STATIC_RPC REGISTER_AS_REMOTE_PROCEDURE_CALL
//#define UNREGISTER_STATIC_RPC UNREGISTER_AS_REMOTE_PROCEDURE_CALL

//----------------------------------------------------
// Sent by a client who's wishing to join us in our
// multiplayer-like activities.

void ClientJoin(RPCParameters *rpcParams)
{
	PlayerID sender = rpcParams->sender;

	RakNet::BitStream bsData(rpcParams);
	RakNet::BitStream bsReject;
	CPlayerPool *pPlayerPool = pNetGame->GetPlayerPool();

	CHAR szPlayerName[256];
	BYTE bytePlayerID;
	int  iVersion;
	//BYTE byteMod;
	BYTE byteNickLen;
	BYTE byteRejectReason;
	size_t uiVersionLen = 0;
	char szVersion[12];
	unsigned int uiChallengeResponse=0;
	PlayerID otherPlayerId = UNASSIGNED_PLAYER_ID;
	size_t uiCount = 0;

	bytePlayerID = pRak->GetIndexFromPlayerID(sender);
	PlayerID MyPlayerID = pRak->GetPlayerIDFromIndex(bytePlayerID);

	for (size_t i = 0; i < MAX_PLAYERS; i++) {
		if (pPlayerPool->GetSlotState((BYTE)i)) {
			otherPlayerId = pRak->GetPlayerIDFromIndex(i);
			if (otherPlayerId != UNASSIGNED_PLAYER_ID &&
				otherPlayerId.binaryAddress == MyPlayerID.binaryAddress)
				uiCount++;
		}
	}

	if (pConsole->GetIntVariable("maxplayerperip") < (int)uiCount)
	{
		byteRejectReason = REJECT_REASON_IP_LIMIT_REACHED;
		bsReject.Write(byteRejectReason);
		pRak->RPC(RPC_ConnectionRejected, &bsReject, HIGH_PRIORITY, RELIABLE, 0, sender, false, false);
		pRak->Kick(sender);
		return;
	}

	in_addr in;

	memset(szPlayerName,0,256);

	bsData.Read(iVersion);
	//bsData.Read(byteMod);
	bsData.Read(byteNickLen);
	bsData.Read(szPlayerName,byteNickLen);
	szPlayerName[byteNickLen] = '\0';
	bsData.Read(uiChallengeResponse);

	bsData.Read(uiVersionLen);
	if (uiVersionLen < 1 || 11 < uiVersionLen)
	{
		byteRejectReason = REJECT_REASON_BAD_VERSION;
		bsReject.Write(byteRejectReason);
		pRak->RPC(RPC_ConnectionRejected, &bsReject, HIGH_PRIORITY, RELIABLE, 0, sender, false, false);
		pRak->Kick(sender);
		return;
	}
	bsData.Read(szVersion, uiVersionLen);
	szVersion[uiVersionLen] = '\0';

	if(UNASSIGNED_PLAYER_ID == MyPlayerID) {
		in.s_addr = sender.binaryAddress;
		logprintf("Detected possible bot from (%s)",inet_ntoa(in));
		pRak->Kick(MyPlayerID);
		return;
	}

	if( !pRak->IsActivePlayerID(sender) || 
		bytePlayerID > MAX_PLAYERS ) {
		byteRejectReason = REJECT_REASON_BAD_PLAYERID;
		bsReject.Write(byteRejectReason);
		pRak->RPC(RPC_ConnectionRejected,&bsReject,HIGH_PRIORITY,RELIABLE,0,sender,false,false);
		pRak->Kick(sender);
		return;
	}	

	if(iVersion != NETGAME_VERSION || _uiRndSrvChallenge != (uiChallengeResponse ^ NETGAME_VERSION)) {
		byteRejectReason = REJECT_REASON_BAD_VERSION;
		bsReject.Write(byteRejectReason);
		pRak->RPC(RPC_ConnectionRejected,&bsReject,HIGH_PRIORITY,RELIABLE,0,sender,false,false);
		pRak->Kick(sender);
		return;
	}
	
	/*if(byteMod != pNetGame->m_byteMod) {
		byteRejectReason = REJECT_REASON_BAD_MOD;
		bsReject.Write(byteRejectReason);
		pRak->RPC(RPC_ConnectionRejected,&bsReject,HIGH_PRIORITY,RELIABLE,0,sender,FALSE,FALSE);
		pRak->Kick(sender);
		return;
	}*/

	if(ContainsInvalidNickChars(szPlayerName) ||
		byteNickLen < 3 || byteNickLen > MAX_PLAYER_NAME || pPlayerPool->IsNickInUse(szPlayerName)) {
		byteRejectReason = REJECT_REASON_BAD_NICKNAME;
		bsReject.Write(byteRejectReason);
		pRak->RPC(RPC_ConnectionRejected,&bsReject,HIGH_PRIORITY,RELIABLE,0,sender,false,false);
		pRak->Kick(sender);
		return;
	}

	// Add this client to the player pool.
	if(!pPlayerPool->New(bytePlayerID, szPlayerName, szVersion)) {
		pRak->Kick(sender);
		return;
	}

	pNetGame->ProcessClientJoin(bytePlayerID);
}

//----------------------------------------------------
// Sent by client with global chat text

void Chat(RPCParameters *rpcParams)
{
	PlayerID sender = rpcParams->sender;

	unsigned char szText[256];
	memset(szText,0,256);

	size_t uiTextLen;

	CPlayerPool *pPool = pNetGame->GetPlayerPool();

	RakNet::BitStream bsData(rpcParams);
	bsData.Read(uiTextLen);

	if(uiTextLen > MAX_CMD_INPUT) return;

	bsData.Read((char *)szText, uiTextLen);
	szText[uiTextLen] = '\0';

	if (!pPool->GetSlotState(pRak->GetIndexFromPlayerID(sender))) return;	

	ReplaceBadChars((char *)szText);

	if(pConsole->GetIntVariable("chatlogging"))
		logprintf("[chat] [%s]: %s",
			pPool->GetPlayerName(pRak->GetIndexFromPlayerID(sender)),
			szText);

	BYTE bytePlayerID = pRak->GetIndexFromPlayerID(sender);

/*#ifdef RAKRCON
	RakNet::BitStream bsSend;

	bsSend.Write( bytePlayerID );
	bsSend.Write( byteTextLen );
	bsSend.Write( szText, byteTextLen );

	pRcon->GetRakServer()->RPC( RPC_Chat, &bsSend, HIGH_PRIORITY, RELIABLE, 0, UNASSIGNED_PLAYER_ID, true, false );
#endif*/

	CPlayer *pPlayer = pNetGame->GetPlayerPool()->GetAt(bytePlayerID);
	CGameMode *pGameMode = pNetGame->GetGameMode();
	
	if (pPlayer)
	{	
		// Send OnPlayerText callback to the GameMode script.
		if (pNetGame->GetFilterScripts()->OnPlayerText((cell)bytePlayerID, szText)) {
			if (pGameMode)
			{
				// Comment by spookie:
				//   The CPlayer::Say() call has moved to CGameMode::OnPlayerText(),
				//   when a gamemode is available. This is due to filter scripts.
				pGameMode->OnPlayerText((cell)bytePlayerID, szText);
			} else {
				// No pGameMode
				pPlayer->Say(szText, uiTextLen);
			}
		}
	}
}

//----------------------------------------------------
// Sent by client who wishes to request a class from
// the gamelogic handler.

void RequestClass(RPCParameters *rpcParams)
{
	PlayerID sender = rpcParams->sender;

	RakNet::BitStream bsData(rpcParams);

	if(pNetGame->GetGameState() != GAMESTATE_RUNNING) return;
	if(!pNetGame->GetPlayerPool()->GetSlotState(pRak->GetIndexFromPlayerID(sender))) return;
	
	int iRequestedClass = 1;
	BYTE byteRequestOutcome = 0;
	BYTE bytePlayerID = pRak->GetIndexFromPlayerID(sender);
	bsData.Read(iRequestedClass);

	if(iRequestedClass >= MAX_SPAWNS) return;
	if(iRequestedClass >= pNetGame->m_iSpawnsAvailable) return;
    
	CPlayer *pPlayer = pNetGame->GetPlayerPool()->GetAt(bytePlayerID);
	if(pPlayer && (iRequestedClass <= (pNetGame->m_iSpawnsAvailable - 1)))
	{
		pPlayer->SetSpawnInfo(&pNetGame->m_AvailableSpawns[iRequestedClass]);
		//logprintf("SetSpawnInfo - iSkin = %d", pNetGame->m_AvailableSpawns[iRequestedClass].iSkin);
	}

	pNetGame->GetFilterScripts()->OnPlayerRequestClass((cell)bytePlayerID, (cell)iRequestedClass);
	byteRequestOutcome = 1;
	if (pNetGame->GetGameMode()) {
		byteRequestOutcome = pNetGame->GetGameMode()->OnPlayerRequestClass((cell)bytePlayerID, (cell)iRequestedClass);
	}
	
	RakNet::BitStream bsSpawnRequestReply;
	PLAYER_SPAWN_INFO *pSpawnInfo = pPlayer->GetSpawnInfo();

	bsSpawnRequestReply.Write(byteRequestOutcome);
	bsSpawnRequestReply.Write((PCHAR)pSpawnInfo,sizeof(PLAYER_SPAWN_INFO));
	pRak->RPC(RPC_RequestClass,&bsSpawnRequestReply,HIGH_PRIORITY,RELIABLE,0,sender,false,false);
}

//----------------------------------------------------
// Client wants to spawn

void RequestSpawn(RPCParameters *rpcParams)
{
	PlayerID sender = rpcParams->sender;

	BYTE bytePlayerID = pRak->GetIndexFromPlayerID(sender);
	BYTE byteRequestOutcome = 1;

	if(pNetGame->GetGameState() != GAMESTATE_RUNNING) return;
	if(!pNetGame->GetPlayerPool()->GetSlotState(pRak->GetIndexFromPlayerID(sender))) return;

	if (!pNetGame->GetFilterScripts()->OnPlayerRequestSpawn((cell)bytePlayerID)) byteRequestOutcome = 0;
	if (pNetGame->GetGameMode() && byteRequestOutcome) {
		if (!pNetGame->GetGameMode()->OnPlayerRequestSpawn((cell)bytePlayerID)) byteRequestOutcome = 0;
	}
	
	RakNet::BitStream bsSpawnRequestReply;
	bsSpawnRequestReply.Write(byteRequestOutcome);
	pRak->RPC(RPC_RequestSpawn,&bsSpawnRequestReply,HIGH_PRIORITY,RELIABLE,0,sender,false,false);
}


//----------------------------------------------------
// Sent by client when they're spawning/respawning.

void Spawn(RPCParameters *rpcParams)
{
	PlayerID sender = rpcParams->sender;

	if(pNetGame->GetGameState() != GAMESTATE_RUNNING) return;
	
	BYTE bytePlayerID = pRak->GetIndexFromPlayerID(sender);
	if(!pNetGame->GetPlayerPool()->GetSlotState(bytePlayerID)) return;
	CPlayer	*pPlayer = pNetGame->GetPlayerPool()->GetAt(bytePlayerID);

	// More sanity checks for crashers.
	if(!pPlayer->m_bHasSpawnInfo) return;
	int iSpawnClass = pPlayer->m_SpawnInfo.iSkin;
	if(iSpawnClass < 0 || iSpawnClass > 300) return;

	pPlayer->Spawn();
}

//----------------------------------------------------
// Sent by client when they die.

void Death(RPCParameters *rpcParams)
{
	PlayerID sender = rpcParams->sender;

	RakNet::BitStream bsData(rpcParams);

	if(pNetGame->GetGameState() != GAMESTATE_RUNNING) return;

	BYTE bytePlayerID = pRak->GetIndexFromPlayerID(sender);
	if(!pNetGame->GetPlayerPool()->GetSlotState(bytePlayerID)) return;
	CPlayer	*pPlayer = pNetGame->GetPlayerPool()->GetAt(bytePlayerID);

	BYTE byteDeathReason;
	BYTE byteWhoWasResponsible;

	bsData.Read(byteDeathReason);
	bsData.Read(byteWhoWasResponsible);

	if(pPlayer) {
		pPlayer->HandleDeath(byteDeathReason,byteWhoWasResponsible);
	}
}

//----------------------------------------------------
// Sent by client when they want to enter a
// vehicle gracefully.

void EnterVehicle(RPCParameters *rpcParams)
{
	PlayerID sender = rpcParams->sender;

	RakNet::BitStream bsData(rpcParams);

	if(pNetGame->GetGameState() != GAMESTATE_RUNNING) return;
	if(!pNetGame->GetPlayerPool()->GetSlotState(pRak->GetIndexFromPlayerID(sender))) return;

	BYTE bytePlayerID = pRak->GetIndexFromPlayerID(sender);
	CPlayer	*pPlayer = pNetGame->GetPlayerPool()->GetAt(bytePlayerID);
	
	VEHICLEID VehicleID=0;
	BYTE bytePassenger=0;

	bsData.Read(VehicleID);
	bsData.Read(bytePassenger);

	if(pPlayer) {
		if(VehicleID == 0xFFFF) {
			pNetGame->KickPlayer(bytePlayerID);
			return;
		}
		pPlayer->EnterVehicle(VehicleID,bytePassenger);
	}

	//logprintf("%u enters vehicle %u",bytePlayerID,byteVehicleID);
}

//----------------------------------------------------
// Sent by client when they want to exit a
// vehicle gracefully.

void ExitVehicle(RPCParameters *rpcParams)
{
	PlayerID sender = rpcParams->sender;

	RakNet::BitStream bsData(rpcParams);

	if(pNetGame->GetGameState() != GAMESTATE_RUNNING) return;
	if(!pNetGame->GetPlayerPool()->GetSlotState(pRak->GetIndexFromPlayerID(sender))) return;

	BYTE bytePlayerID = pRak->GetIndexFromPlayerID(sender);
	CPlayer	*pPlayer = pNetGame->GetPlayerPool()->GetAt(bytePlayerID);

	VEHICLEID VehicleID;
	bsData.Read(VehicleID);
	
	if(pPlayer) {
		if(VehicleID == 0xFFFF) {
			pNetGame->KickPlayer(bytePlayerID);
			return;
		}
		pPlayer->ExitVehicle(VehicleID);
	}

	// HACK by spookie - this gonna cause probs, or are they defo out of the car now?
	// comment by kyeman - No they're not, it's just an advisory for the anims.
	//pNetGame->GetVehiclePool()->GetAt(byteVehicleID)->m_byteDriverID = INVALID_ID;

	//logprintf("%u exits vehicle %u",bytePlayerID,byteVehicleID);
}

//----------------------------------------------------

void ServerCommand(RPCParameters *rpcParams)
{
	PlayerID sender = rpcParams->sender;
	int iStrLen=0;
	unsigned char* szCommand=NULL;

	RakNet::BitStream bsData(rpcParams);

	if(pNetGame->GetGameState() != GAMESTATE_RUNNING) return;

	BYTE bytePlayerID = pRak->GetIndexFromPlayerID(sender);
		
	bsData.Read(iStrLen);

	if(iStrLen < 1) return;
	if(iStrLen > MAX_CMD_INPUT) return;

	szCommand = (unsigned char*)calloc(iStrLen+1,1);
	bsData.Read((char*)szCommand, iStrLen);
	szCommand[iStrLen] = '\0';

	ReplaceBadChars((char *)szCommand);

	if (!pNetGame->GetFilterScripts()->OnPlayerCommandText(bytePlayerID, szCommand))
	{
		if (pNetGame->GetGameMode())
		{
			if (!pNetGame->GetGameMode()->OnPlayerCommandText(bytePlayerID, szCommand))
			{
				pNetGame->SendClientMessage(sender, 0xFFFFFFFF, "SERVER: Unknown command.");
			}
		}
	}

	free(szCommand);
}

//----------------------------------------------------

void UpdateScoresPingsIPs(RPCParameters *rpcParams)
{
	PlayerID sender = rpcParams->sender;

	RakNet::BitStream bsData(rpcParams);

	RakNet::BitStream bsParams;
	CPlayerPool *pPlayerPool = pNetGame->GetPlayerPool();
	BYTE bytePlayerId = pRak->GetIndexFromPlayerID(sender);

	if(!pPlayerPool->GetSlotState(bytePlayerId)) return;

	for (BYTE i=0; i<MAX_PLAYERS; i++)
	{
		if (pPlayerPool->GetSlotState(i))
		{
			bsParams.Write(i);
			bsParams.Write((DWORD)pPlayerPool->GetPlayerScore(i));
			bsParams.Write((DWORD)pRak->GetLastPing(pRak->GetPlayerIDFromIndex(i)));
		}
	}

	pRak->RPC(RPC_UpdateScoresPingsIPs, &bsParams, HIGH_PRIORITY, RELIABLE, 0, sender, false, false);
}

//----------------------------------------------------

void SvrStats(RPCParameters *rpcParams)
{
	PlayerID sender = rpcParams->sender;

	RakNet::BitStream bsData(rpcParams);

	RakNet::BitStream bsParams;
	CPlayerPool *pPlayerPool = pNetGame->GetPlayerPool();
	BYTE bytePlayerId = pRak->GetIndexFromPlayerID(sender);

	if(!pPlayerPool->GetSlotState(bytePlayerId)) return;
	if(!pPlayerPool->IsAdmin(bytePlayerId)) return;

	bsParams.Write((const char *)pRak->GetStatistics(UNASSIGNED_PLAYER_ID),sizeof(RakNetStatisticsStruct));
	pRak->RPC(RPC_SvrStats, &bsParams, HIGH_PRIORITY, RELIABLE, 0, sender, false, false);
}

//----------------------------------------------------

void SetInteriorId(RPCParameters *rpcParams)
{
	PlayerID sender = rpcParams->sender;

	RakNet::BitStream bsData(rpcParams);

	BYTE byteInteriorId;
	bsData.Read(byteInteriorId);

	CPlayerPool* pPlayerPool = pNetGame->GetPlayerPool();
	BYTE bytePlayerId = pRak->GetIndexFromPlayerID(sender);

	if (pPlayerPool->GetSlotState(bytePlayerId))
	{
		CGameMode *pGameMode = pNetGame->GetGameMode();
		CFilterScripts *pFilters = pNetGame->GetFilterScripts();

		CPlayer *pPlayer = pPlayerPool->GetAt(bytePlayerId);
		int iOldInteriorId=pPlayer->m_iInteriorId;
		pPlayer->m_iInteriorId = (int)byteInteriorId;

		if(pGameMode) pGameMode->OnPlayerInteriorChange(
			bytePlayerId,pPlayer->m_iInteriorId,iOldInteriorId);

		if(pFilters) pFilters->OnPlayerInteriorChange(
			bytePlayerId,pPlayer->m_iInteriorId,iOldInteriorId);
	}
}

//----------------------------------------------------

void ScmEvent(RPCParameters *rpcParams)
{
	PlayerID sender = rpcParams->sender;

	RakNet::BitStream bsData(rpcParams);
	RakNet::BitStream bsSend;
	BYTE bytePlayerID;
	int iEvent;
	DWORD dwParams1;
	DWORD dwParams2;
	DWORD dwParams3;
	
	bytePlayerID = pNetGame->GetRakServer()->GetIndexFromPlayerID(sender);
	bsData.Read(iEvent);
	bsData.Read(dwParams1);
	bsData.Read(dwParams2);
	bsData.Read(dwParams3);
	
	bool bSend = true;

	//printf("ScmEvent: %u %u %u %u\n",iEvent,dwParams1,dwParams2,dwParams3);
    
	if(pNetGame->GetGameState() != GAMESTATE_RUNNING) return;

	if (iEvent == EVENT_TYPE_CARCOMPONENT)
	{
		CVehicle* pVehicle = pNetGame->GetVehiclePool()->GetAt((VEHICLEID)dwParams1);
		if (!pVehicle) return;

		if (!pNetGame->GetGameMode()->OnVehicleMod(bytePlayerID, dwParams1, dwParams2) ||
			!pNetGame->GetFilterScripts()->OnVehicleMod(bytePlayerID, dwParams1, dwParams2))
		{
			bSend = false;
		}

		if (bSend == true)
		{
			int iComponentId = Utils::GetTypeByComponentId(dwParams2);
			if (iComponentId == -1)
			{
				bSend = false;
			}
			else
			{
				pVehicle->m_CarModInfo.ucCarMod[iComponentId] = (unsigned char)(dwParams2 - 1000);
			}
		}

		if (bSend)
		{
			bsSend.Write(bytePlayerID);
			bsSend.Write(iEvent);
			bsSend.Write(dwParams1);
			bsSend.Write(dwParams2);
			bsSend.Write(dwParams3);
			pRak->RPC(RPC_ScmEvent, &bsSend, HIGH_PRIORITY, RELIABLE, 0, sender, true, false);
		}
		else
		{
			bsSend.Write((VEHICLEID)dwParams1);
			bsSend.Write(dwParams2);
			pRak->RPC(RPC_ScrRemoveComponent, &bsSend, HIGH_PRIORITY, RELIABLE, 0, sender, false, false);
		}
	}
	else if (iEvent == EVENT_TYPE_PAINTJOB)
	{
		CVehicle*	pVehicle	=	pNetGame->GetVehiclePool()->GetAt((VEHICLEID)dwParams1);
		if (!pVehicle) return;

		if (!pNetGame->GetGameMode()->OnVehiclePaintjob(bytePlayerID, dwParams1, dwParams2) ||
		!pNetGame->GetFilterScripts()->OnVehiclePaintjob(bytePlayerID, dwParams1, dwParams2)) bSend = false;
		if (bSend)
		{
			pVehicle->m_CarModInfo.bytePaintJob = (BYTE)dwParams2;

			bsSend.Write(bytePlayerID);
			bsSend.Write(iEvent);
			bsSend.Write(dwParams1);
			bsSend.Write(dwParams2);
			bsSend.Write(dwParams3);
			pRak->RPC(RPC_ScmEvent, &bsSend, HIGH_PRIORITY, RELIABLE, 0, sender, true, false);
		}
	}
	else if (iEvent == EVENT_TYPE_CARCOLOR)
	{
		CVehicle*	pVehicle	=	pNetGame->GetVehiclePool()->GetAt((VEHICLEID)dwParams1);
		if (!pVehicle)
			return;

		if (!pNetGame->GetGameMode()->OnVehicleRespray(bytePlayerID, dwParams1, dwParams2, dwParams3) ||
		!pNetGame->GetFilterScripts()->OnVehicleRespray(bytePlayerID, dwParams1, dwParams2, dwParams3)) bSend = false;
		if (bSend)
		{
			pVehicle->m_CarModInfo.iColor0 = (int)dwParams2;
			pVehicle->m_CarModInfo.iColor1 = (int)dwParams3;

			bsSend.Write(bytePlayerID);
			bsSend.Write(iEvent);
			bsSend.Write(dwParams1);
			bsSend.Write(dwParams2);
			bsSend.Write(dwParams3);
			pRak->RPC(RPC_ScmEvent, &bsSend, HIGH_PRIORITY, RELIABLE, 0, sender, true, false);
		}
	}
	else if (iEvent == EVENT_ENTEREXIT_MODSHOP)
	{
		if (!pNetGame->GetPlayerPool()->GetSlotState(dwParams1))
			return;

		if (pNetGame->GetFilterScripts())
			pNetGame->GetFilterScripts()->OnEnterExitModShop(bytePlayerID, dwParams2, dwParams3);
		if (pNetGame->GetGameMode())
			pNetGame->GetGameMode()->OnEnterExitModShop(bytePlayerID, dwParams2, dwParams3);

		bsSend.Write(bytePlayerID);
		bsSend.Write(iEvent);
		bsSend.Write(dwParams1);
		bsSend.Write(dwParams2);
		bsSend.Write(dwParams3);
		pRak->RPC(RPC_ScmEvent, &bsSend, HIGH_PRIORITY, RELIABLE, 0, sender, true, false);
	}
	else if (iEvent == EVENT_TYPE_STUNT_JUMP)
	{
		//if (!pNetGame->GetVehiclePool()->GetSlotState(dwParams1))
			//return;

		if (pNetGame->GetFilterScripts())
			pNetGame->GetFilterScripts()->OnPlayerStunt(bytePlayerID, dwParams1);
		if (pNetGame->GetGameMode())
			pNetGame->GetGameMode()->OnPlayerStunt(bytePlayerID, dwParams1);
	}
	/*else 
	{
		bsSend.Write(bytePlayerID);
		bsSend.Write(iEvent);
		bsSend.Write(dwParams1);
		bsSend.Write(dwParams2);
		bsSend.Write(dwParams3);
		pRak->RPC(RPC_ScmEvent, &bsSend, HIGH_PRIORITY, RELIABLE, 0, sender, true, false);
	}*/
}

void AdminMapTeleport(RPCParameters *rpcParams)
{
	PlayerID sender = rpcParams->sender;

	RakNet::BitStream bsData(rpcParams);
	
	VECTOR vecPos;
	bsData.Read(vecPos.X);
	bsData.Read(vecPos.Y);
	bsData.Read(vecPos.Z);

	BYTE bytePlayerId = pRak->GetIndexFromPlayerID(sender);
	CPlayerPool *pPlayerPool = pNetGame->GetPlayerPool();

	if (!pNetGame) return;
	if (pNetGame->GetGameState() != GAMESTATE_RUNNING) return;

	if (pPlayerPool->GetSlotState(bytePlayerId)) {
		CPlayer* pPlayer = pPlayerPool->GetAt(bytePlayerId);
		if (!pPlayer)
			return;

		if (!pNetGame->GetGameMode()->OnPlayerClickMap(
			(cell)bytePlayerId, vecPos.X, vecPos.Y, vecPos.Z))
		{
			pNetGame->GetFilterScripts()->OnPlayerClickMap((cell)bytePlayerId,
				vecPos.X, vecPos.Y, vecPos.Z);
		}

		if (pNetGame->m_bAdminTeleport && pPlayer->m_bCanTeleport && pPlayerPool->IsAdmin(bytePlayerId))
		{
			RakNet::BitStream bsParams;
			bsParams.Write(vecPos.X);	// X
			bsParams.Write(vecPos.Y);	// Y
			bsParams.Write(vecPos.Z);	// Z

			RakServerInterface* pRak = pNetGame->GetRakServer();
			pRak->RPC(RPC_ScrSetPlayerPos, &bsParams, HIGH_PRIORITY, RELIABLE, 0,
				sender, false, false);
		}
	}
}

void VehicleDestroyed(RPCParameters *rpcParams)
{
	PlayerID sender = rpcParams->sender;

	RakNet::BitStream bsData(rpcParams);
	VEHICLEID VehicleID;
	bsData.Read(VehicleID);

	if(!pNetGame) return;
	if(pNetGame->GetGameState() != GAMESTATE_RUNNING) return;

	CPlayerPool *pPlayerPool = pNetGame->GetPlayerPool();
	CVehiclePool *pVehiclePool = pNetGame->GetVehiclePool();
	if(!pPlayerPool || !pVehiclePool) return;

	BYTE bytePlayerId = pRak->GetIndexFromPlayerID(sender);
	if(!pPlayerPool->GetSlotState(bytePlayerId)) return;

	if(pVehiclePool->GetSlotState(VehicleID))
	{
		CVehicle* pVehicle = pVehiclePool->GetAt(VehicleID);
		if(pVehicle) pVehicle->SetDead();
	}
}

void PickedUpWeapon(RPCParameters *rpcParams)
{
	// Tells all other clients to destroy this pickup as it's been got already
	RakNet::BitStream bsData(rpcParams);

	BYTE bytePlayerID;
	bsData.Read(bytePlayerID);

	RakNet::BitStream bsSend;
	bsSend.Write(bytePlayerID);
	
	pRak->RPC(RPC_DestroyWeaponPickup, &bsSend, HIGH_PRIORITY, RELIABLE, 0, UNASSIGNED_PLAYER_ID, true, false);
}

void PickedUpPickup(RPCParameters *rpcParams)
{
	BYTE bytePlayerID = pRak->GetIndexFromPlayerID(rpcParams->sender);

	RakNet::BitStream bsData(rpcParams);

	int iPickup;
	bsData.Read(iPickup);

	CGameMode *pGameMode = pNetGame->GetGameMode();
	CFilterScripts *pFilters = pNetGame->GetFilterScripts();
	
	if(pGameMode) pGameMode->OnPlayerPickedUpPickup(bytePlayerID, iPickup);
	if(pFilters) pFilters->OnPlayerPickedUpPickup(bytePlayerID, iPickup);
}

void MenuSelect(RPCParameters *rpcParams)
{	
	BYTE bytePlayerID = pRak->GetIndexFromPlayerID(rpcParams->sender);

	RakNet::BitStream bsData(rpcParams);

	BYTE byteRow;
	bsData.Read(byteRow);

	CGameMode *pGameMode = pNetGame->GetGameMode();
	CFilterScripts *pFilters = pNetGame->GetFilterScripts();

	if(pGameMode) pGameMode->OnPlayerSelectedMenuRow(bytePlayerID, byteRow);
	if(pFilters) pFilters->OnPlayerSelectedMenuRow(bytePlayerID, byteRow);

	pNetGame->GetMenuPool()->ResetPlayer(bytePlayerID);
}

void MenuQuit(RPCParameters *rpcParams)
{
	BYTE bytePlayerID = pRak->GetIndexFromPlayerID(rpcParams->sender);
		
	CGameMode *pGameMode = pNetGame->GetGameMode();
	CFilterScripts *pFilters = pNetGame->GetFilterScripts();

	if(pGameMode) pGameMode->OnPlayerExitedMenu(bytePlayerID);
	if(pFilters) pFilters->OnPlayerExitedMenu(bytePlayerID);

	pNetGame->GetMenuPool()->ResetPlayer(bytePlayerID);
}

void TypingEvent(RPCParameters* rpcParams)
{
	RakNet::BitStream in(rpcParams);

	int iSenderId = pNetGame->GetRakServer()->GetIndexFromPlayerID(rpcParams->sender);
	CPlayerPool* pPool = pNetGame->GetPlayerPool();
	unsigned char ucType = 0;
	if (in.GetNumberOfUnreadBits() == 1 && pPool && pPool->GetSlotState(iSenderId))
	{
		CFilterScripts* pFS = pNetGame->GetFilterScripts();
		CGameMode* pGM = pNetGame->GetGameMode();
		if (pFS == NULL || pGM == NULL)
			return;

		if (in.ReadBit()) {
			pFS->OnPlayerBeginTyping(iSenderId);
			pGM->OnPlayerBeginTyping(iSenderId);
		} else {
			pFS->OnPlayerEndTyping(iSenderId);
			pGM->OnPlayerEndTyping(iSenderId);
		}
	}
}

//----------------------------------------------------

void RegisterRPCs(RakServerInterface * pRakServer)
{
	pRak = pRakServer;

	REGISTER_STATIC_RPC(pRakServer, ClientJoin);
	REGISTER_STATIC_RPC(pRakServer, Chat);
	REGISTER_STATIC_RPC(pRakServer, RequestClass);
	REGISTER_STATIC_RPC(pRakServer, RequestSpawn);
	REGISTER_STATIC_RPC(pRakServer, Spawn);
	REGISTER_STATIC_RPC(pRakServer, Death);
	REGISTER_STATIC_RPC(pRakServer, EnterVehicle);
	REGISTER_STATIC_RPC(pRakServer, ExitVehicle);
	REGISTER_STATIC_RPC(pRakServer, ServerCommand);
	REGISTER_STATIC_RPC(pRakServer, UpdateScoresPingsIPs);
	REGISTER_STATIC_RPC(pRakServer, SvrStats);
	REGISTER_STATIC_RPC(pRakServer, SetInteriorId);
	REGISTER_STATIC_RPC(pRakServer, ScmEvent);
	REGISTER_STATIC_RPC(pRakServer, AdminMapTeleport);
	REGISTER_STATIC_RPC(pRakServer, VehicleDestroyed);
	REGISTER_STATIC_RPC(pRakServer, PickedUpWeapon);
	REGISTER_STATIC_RPC(pRakServer, PickedUpPickup);
	REGISTER_STATIC_RPC(pRakServer, MenuSelect);
	REGISTER_STATIC_RPC(pRakServer, MenuQuit);
	REGISTER_STATIC_RPC(pRakServer, TypingEvent);
}

//----------------------------------------------------

/*void UnRegisterRPCs(RakServerInterface * pRakServer)
{
	pRak = 0;

	UNREGISTER_STATIC_RPC(pRakServer, ClientJoin);
	UNREGISTER_STATIC_RPC(pRakServer, Chat);
	UNREGISTER_STATIC_RPC(pRakServer, RequestClass);
	UNREGISTER_STATIC_RPC(pRakServer, RequestSpawn);
	UNREGISTER_STATIC_RPC(pRakServer, Spawn);
	UNREGISTER_STATIC_RPC(pRakServer, Death);
	UNREGISTER_STATIC_RPC(pRakServer, EnterVehicle);
	UNREGISTER_STATIC_RPC(pRakServer, ExitVehicle);
	UNREGISTER_STATIC_RPC(pRakServer, ServerCommand);
	UNREGISTER_STATIC_RPC(pRakServer, UpdateScoresPingsIPs);
	UNREGISTER_STATIC_RPC(pRakServer, SvrStats);
	UNREGISTER_STATIC_RPC(pRakServer, SetInteriorId);
	UNREGISTER_STATIC_RPC(pRakServer, ScmEvent);
	UNREGISTER_STATIC_RPC(pRakServer, AdminMapTeleport);
	UNREGISTER_STATIC_RPC(pRakServer, VehicleDestroyed);
	UNREGISTER_STATIC_RPC(pRakServer, PickedUpWeapon);
	UNREGISTER_STATIC_RPC(pRakServer, PickedUpPickup);
	UNREGISTER_STATIC_RPC(pRakServer, MenuSelect);
	UNREGISTER_STATIC_RPC(pRakServer, MenuQuit);
	UNREGISTER_STATIC_RPC(pRakServer, TypingEvent);

}*/

//----------------------------------------------------
