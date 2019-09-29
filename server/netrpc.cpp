/*

	SA:MP Multiplayer Modification
	Copyright 2004-2005 SA:MP Team

    Version: $Id: netrpc.cpp,v 1.52 2006/06/02 13:24:21 mike Exp $

*/

#include "main.h"
#include "vehmods.h"
#include "anticheat.h"

extern CNetGame *pNetGame;

RakServerInterface		*pRak=0;

// Removed for RakNet upgrade
//#define REGISTER_STATIC_RPC REGISTER_AS_REMOTE_PROCEDURE_CALL
//#define UNREGISTER_STATIC_RPC UNREGISTER_AS_REMOTE_PROCEDURE_CALL

#define REJECT_REASON_BAD_VERSION	1
#define REJECT_REASON_BAD_NICKNAME	2
#define REJECT_REASON_BAD_MOD		3
#define REJECT_REASON_BAD_PLAYERID	4

bool ContainsInvalidNickChars(char * szString);
void ReplaceBadChars(char * szString);

#define NETGAME_VERSION 8866

extern unsigned int _uiRndSrvChallenge;

//----------------------------------------------------
// Sent by a client who's wishing to join us in our
// multiplayer-like activities.

void ClientJoin(RPCParameters *rpcParams)
{
	PCHAR Data = reinterpret_cast<PCHAR>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;
	PlayerID sender = rpcParams->sender;

	RakNet::BitStream bsData(Data,(iBitLength/8)+1,false);
	RakNet::BitStream bsReject;
	CPlayerPool *pPlayerPool = pNetGame->GetPlayerPool();

	CHAR szPlayerName[256];
	BYTE bytePlayerID;
	int  iVersion;
	BYTE byteMod;
	BYTE byteNickLen;
	BYTE byteRejectReason;
	unsigned int uiChallengeResponse=0;

	bytePlayerID = pRak->GetIndexFromPlayerID(sender);
	PlayerID MyPlayerID = pRak->GetPlayerIDFromIndex(bytePlayerID);
	in_addr in;

	memset(szPlayerName,0,256);

	bsData.Read(iVersion);
	bsData.Read(byteMod);
	bsData.Read(byteNickLen);
	bsData.Read(szPlayerName,byteNickLen);
	szPlayerName[byteNickLen] = '\0';
	bsData.Read(uiChallengeResponse);

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
		pRak->RPC(RPC_ConnectionRejected,&bsReject,HIGH_PRIORITY,RELIABLE,0,sender,FALSE,FALSE);
		pRak->Kick(sender);
		return;
	}	

	if(iVersion != NETGAME_VERSION || _uiRndSrvChallenge != (uiChallengeResponse ^ NETGAME_VERSION)) {
		byteRejectReason = REJECT_REASON_BAD_VERSION;
		bsReject.Write(byteRejectReason);
		pRak->RPC(RPC_ConnectionRejected,&bsReject,HIGH_PRIORITY,RELIABLE,0,sender,FALSE,FALSE);
		pRak->Kick(sender);
		return;
	}
	
	if(byteMod != pNetGame->m_byteMod) {
		byteRejectReason = REJECT_REASON_BAD_MOD;
		bsReject.Write(byteRejectReason);
		pRak->RPC(RPC_ConnectionRejected,&bsReject,HIGH_PRIORITY,RELIABLE,0,sender,FALSE,FALSE);
		pRak->Kick(sender);
		return;
	}

	if(ContainsInvalidNickChars(szPlayerName) ||
		byteNickLen < 3 || byteNickLen > 16 || pPlayerPool->IsNickInUse(szPlayerName)) {
		byteRejectReason = REJECT_REASON_BAD_NICKNAME;
		bsReject.Write(byteRejectReason);
		pRak->RPC(RPC_ConnectionRejected,&bsReject,HIGH_PRIORITY,RELIABLE,0,sender,FALSE,FALSE);
		pRak->Kick(sender);
		return;
	}

	// Add this client to the player pool.
	if(!pPlayerPool->New(bytePlayerID, szPlayerName)) {
		pRak->Kick(sender);
		return;
	}

	pNetGame->ProcessClientJoin(bytePlayerID);
}

//----------------------------------------------------
// Sent by client with global chat text

void Chat(RPCParameters *rpcParams)
{
	PCHAR Data = reinterpret_cast<PCHAR>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;
	PlayerID sender = rpcParams->sender;

	unsigned char szText[256];
	memset(szText,0,256);

	BYTE byteTextLen;

	CPlayerPool *pPool = pNetGame->GetPlayerPool();

	RakNet::BitStream bsData(Data,(iBitLength/8)+1,false);
	bsData.Read(byteTextLen);

	if(byteTextLen > MAX_CMD_INPUT) return;

	bsData.Read((char *)szText,byteTextLen);
	szText[byteTextLen] = '\0';

	if (!pPool->GetSlotState(pRak->GetIndexFromPlayerID(sender))) return;	

	ReplaceBadChars((char *)szText);

	logprintf("[chat] [%s]: %s",
		pPool->GetPlayerName(pRak->GetIndexFromPlayerID(sender)),
		szText);

	BYTE bytePlayerID = pRak->GetIndexFromPlayerID(sender);

#ifdef RAKRCON
	RakNet::BitStream bsSend;

	bsSend.Write( bytePlayerID );
	bsSend.Write( byteTextLen );
	bsSend.Write( szText, byteTextLen );

	pRcon->GetRakServer()->RPC( RPC_Chat, &bsSend, HIGH_PRIORITY, RELIABLE, 0, UNASSIGNED_PLAYER_ID, true, false );
#endif

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
				pPlayer->Say(szText,byteTextLen);
			}
		}
	}
}

//----------------------------------------------------
// Sent by client with private message text

void Privmsg(RPCParameters *rpcParams)
{
	PCHAR Data = reinterpret_cast<PCHAR>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;
	PlayerID sender = rpcParams->sender;

	BYTE byteToPlayerID;
	unsigned char szText[256];
	memset(szText,0,256);

	BYTE byteTextLen;
	CPlayerPool *pPool = pNetGame->GetPlayerPool();

	RakNet::BitStream bsData(Data,(iBitLength/8)+1,false);
	bsData.Read(byteToPlayerID);
	bsData.Read(byteTextLen);

	if(byteTextLen > MAX_CMD_INPUT) return;

	bsData.Read((char*)szText,byteTextLen);
	szText[byteTextLen] = '\0';

	if (!pPool->GetSlotState(pRak->GetIndexFromPlayerID(sender))) return;

	ReplaceBadChars((char *)szText);

	logprintf("[pm] [%s to %s]: %s",
		pPool->GetPlayerName(pRak->GetIndexFromPlayerID(sender)),
		pPool->GetPlayerName(byteToPlayerID),
		szText);

#ifdef RAKRCON
	/*char szPm[255];
	sprintf(szPm, "*** PM from %s to %s: %s.", pPool->GetPlayerName(pRak->GetIndexFromPlayerID(sender)), pPool->GetPlayerName(byteToPlayerID), szText);
	pRcon->SendEventString(szPm);*/

	RakNet::BitStream bsSend;

	bsData.Write( (BYTE)pRak->GetIndexFromPlayerID( sender ) );
	bsSend.Write( byteToPlayerID );
	bsData.Write( byteTextLen );
	bsData.Write( szText, byteTextLen );

	pRcon->GetRakServer()->RPC( RPC_Privmsg, &bsSend, HIGH_PRIORITY, RELIABLE, 0, UNASSIGNED_PLAYER_ID, true, false );

#endif

	BYTE bytePlayerID = pRak->GetIndexFromPlayerID(sender);

	CPlayer *pPlayer = pNetGame->GetPlayerPool()->GetAt(bytePlayerID);
	CGameMode *pGameMode = pNetGame->GetGameMode();
	
	if (pPlayer)
	{	
		 pNetGame->GetFilterScripts()->OnPlayerPrivmsg((cell)bytePlayerID, (cell)byteToPlayerID, szText);
		// Send OnPlayerText callback to the GameMode script.
		if (pGameMode)
		{
			pGameMode->OnPlayerPrivmsg((cell)bytePlayerID, (cell)byteToPlayerID, szText);
		} else {
			// No pGameMode
			pPlayer->Privmsg(byteToPlayerID, szText,byteTextLen);
		}	
	}
}

//----------------------------------------------------
// Sent by client with team chat text

void TeamPrivmsg(RPCParameters *rpcParams)
{
	PCHAR Data = reinterpret_cast<PCHAR>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;
	PlayerID sender = rpcParams->sender;

	unsigned char szText[256];
	BYTE byteTextLen;
	memset(szText,0,256);

	CPlayerPool *pPool = pNetGame->GetPlayerPool();

	RakNet::BitStream bsData(Data,(iBitLength/8)+1,false);
	bsData.Read(byteTextLen);

	if(byteTextLen > MAX_CMD_INPUT) return;

	bsData.Read((char*)szText,byteTextLen);
	szText[byteTextLen] = '\0';

	if(!pPool->GetSlotState(pRak->GetIndexFromPlayerID(sender))) return;
	
	ReplaceBadChars((char *)szText);

	if(pPool->GetAt(pRak->GetIndexFromPlayerID(sender))->GetTeam() == NO_TEAM) {
		pNetGame->SendClientMessage(sender, 0xDC181AFF, "You are not on a team!");
		return;
	}

	logprintf("[pm] [%s to team]: %s",
		pPool->GetPlayerName(pRak->GetIndexFromPlayerID(sender)),
		szText);

#ifdef RAKRCON
	char szTeamPm[255];
	sprintf(szTeamPm, "*** PM from %s to team: %s.", pPool->GetPlayerName(pRak->GetIndexFromPlayerID(sender)), szText);
	pRcon->SendEventString(szTeamPm);
#endif

	BYTE bytePlayerID = pRak->GetIndexFromPlayerID(sender);

	CPlayer *pPlayer = pNetGame->GetPlayerPool()->GetAt(bytePlayerID);
	CGameMode *pGameMode = pNetGame->GetGameMode();
	
	if (pPlayer)
	{	
		pNetGame->GetFilterScripts()->OnPlayerTeamPrivmsg((cell)bytePlayerID, szText);
		// Send OnPlayerText callback to the GameMode script.
		if (pGameMode)
		{
			pGameMode->OnPlayerTeamPrivmsg((cell)bytePlayerID, szText);
		} else {
			// No pGameMode
			pPlayer->TeamPrivmsg(szText,byteTextLen);
		}	
	}
}

//----------------------------------------------------
// Sent by client who wishes to request a class from
// the gamelogic handler.

void RequestClass(RPCParameters *rpcParams)
{
	PCHAR Data = reinterpret_cast<PCHAR>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;
	PlayerID sender = rpcParams->sender;

	RakNet::BitStream bsData(Data,(iBitLength/8)+1,false);

	if(pNetGame->GetGameState() != GAMESTATE_RUNNING) return;
	if(!pNetGame->GetPlayerPool()->GetSlotState(pRak->GetIndexFromPlayerID(sender))) return;
	
	unsigned int uiRequestedClass = 1;
	BYTE byteRequestOutcome = 0;
	BYTE bytePlayerID = pRak->GetIndexFromPlayerID(sender);
	bsData.Read(uiRequestedClass);

	if(uiRequestedClass >= MAX_SPAWNS) return;
	if(uiRequestedClass >= (unsigned int)pNetGame->m_iSpawnsAvailable) return;
    
	CPlayer *pPlayer = pNetGame->GetPlayerPool()->GetAt(bytePlayerID);
	if(pPlayer && (uiRequestedClass <= (pNetGame->m_iSpawnsAvailable - 1)))
	{
		pPlayer->SetSpawnInfo(&pNetGame->m_AvailableSpawns[uiRequestedClass]);
		//logprintf("SetSpawnInfo - iSkin = %d", pNetGame->m_AvailableSpawns[iRequestedClass].iSkin);
	}

	pNetGame->GetFilterScripts()->OnPlayerRequestClass((cell)bytePlayerID, (cell)uiRequestedClass);
	byteRequestOutcome = 1;
	if (pNetGame->GetGameMode()) {
		byteRequestOutcome = pNetGame->GetGameMode()->OnPlayerRequestClass((cell)bytePlayerID, (cell)uiRequestedClass);
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
	PCHAR Data = reinterpret_cast<PCHAR>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;
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
	PCHAR Data = reinterpret_cast<PCHAR>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;
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
	PCHAR Data = reinterpret_cast<PCHAR>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;
	PlayerID sender = rpcParams->sender;

	RakNet::BitStream bsData(Data,(iBitLength/8)+1,false);

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
	PCHAR Data = reinterpret_cast<PCHAR>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;
	PlayerID sender = rpcParams->sender;

	RakNet::BitStream bsData(Data,(iBitLength/8)+1,false);

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
	PCHAR Data = reinterpret_cast<PCHAR>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;
	PlayerID sender = rpcParams->sender;

	RakNet::BitStream bsData(Data,(iBitLength/8)+1,false);

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
	PCHAR Data = reinterpret_cast<PCHAR>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;
	PlayerID sender = rpcParams->sender;
	int iStrLen=0;
	unsigned char* szCommand=NULL;

	RakNet::BitStream bsData(Data,(iBitLength/8)+1,false);

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
	PCHAR Data = reinterpret_cast<PCHAR>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;
	PlayerID sender = rpcParams->sender;

	RakNet::BitStream bsData(Data,(iBitLength/8)+1,false);

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
	PCHAR Data = reinterpret_cast<PCHAR>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;
	PlayerID sender = rpcParams->sender;

	RakNet::BitStream bsData(Data,(iBitLength/8)+1,false);

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
	PCHAR Data = reinterpret_cast<PCHAR>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;
	PlayerID sender = rpcParams->sender;

	RakNet::BitStream bsData(Data,(iBitLength/8)+1,false);

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
	PCHAR Data = reinterpret_cast<PCHAR>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;
	PlayerID sender = rpcParams->sender;

	RakNet::BitStream bsData(Data,(iBitLength/8)+1,false);
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
	
	BOOL bSend = TRUE;

	//printf("ScmEvent: %u %u %u %u\n",iEvent,dwParams1,dwParams2,dwParams3);
    
	if(pNetGame->GetGameState() != GAMESTATE_RUNNING) return;

	if (iEvent == EVENT_TYPE_CARCOMPONENT)
	{
		CVehicle*	pVehicle	=	pNetGame->GetVehiclePool()->GetAt((VEHICLEID)dwParams1);
		if (!pVehicle) return;

		BYTE*	pDataStart	= (BYTE*)&pVehicle->m_CarModInfo.byteCarMod0;
		for(int i = 0; i < 14; i++)
		{
			DWORD data = pDataStart[i];

			if (data == 0)
			{
				if (!pNetGame->GetGameMode()->OnVehicleMod(bytePlayerID, dwParams1, dwParams2) ||
				!pNetGame->GetFilterScripts()->OnVehicleMod(bytePlayerID, dwParams1, dwParams2))
				{
					bSend = FALSE;
				}

				if (bSend)
				{
					BYTE byteMod = (BYTE)(dwParams2 - 1000);

					if (byteMod >= sizeof (c_byteVehMods) ||
						(c_byteVehMods[byteMod] != NOV && 
						c_byteVehMods[byteMod] != (BYTE)(pVehicle->m_SpawnInfo.iVehicleType - 400)))	{
						bSend = FALSE;

						//printf("Setting bSend false because: %u %u\n",byteMod,dwParams1);

					}
					else {
						pDataStart[i] = byteMod;
					}
				}
				break;
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
		!pNetGame->GetFilterScripts()->OnVehiclePaintjob(bytePlayerID, dwParams1, dwParams2)) bSend = FALSE;
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
		!pNetGame->GetFilterScripts()->OnVehicleRespray(bytePlayerID, dwParams1, dwParams2, dwParams3)) bSend = FALSE;
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
	else 
	{
		bsSend.Write(bytePlayerID);
		bsSend.Write(iEvent);
		bsSend.Write(dwParams1);
		bsSend.Write(dwParams2);
		bsSend.Write(dwParams3);
		pRak->RPC(RPC_ScmEvent, &bsSend, HIGH_PRIORITY, RELIABLE, 0, sender, true, false);
	}
}

void AdminMapTeleport(RPCParameters *rpcParams)
{
	PCHAR Data = reinterpret_cast<PCHAR>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;
	PlayerID sender = rpcParams->sender;

	RakNet::BitStream bsData(Data,(iBitLength/8)+1,false);
	
	VECTOR vecPos;
	bsData.Read(vecPos.X);
	bsData.Read(vecPos.Y);
	bsData.Read(vecPos.Z);

	BYTE bytePlayerId = pRak->GetIndexFromPlayerID(sender);
    CPlayerPool *pPlayerPool = pNetGame->GetPlayerPool();

	if(!pNetGame) return;
	if(pNetGame->GetGameState() != GAMESTATE_RUNNING) return;
	if(!pNetGame->m_bAdminTeleport) return;
	
	if(pPlayerPool->GetSlotState(bytePlayerId)) {
		CPlayer *pPlayer = pPlayerPool->GetAt(bytePlayerId);
		if(pPlayer && pPlayer->m_bCanTeleport && pPlayerPool->IsAdmin(bytePlayerId)) 
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
	PCHAR Data = reinterpret_cast<PCHAR>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;
	PlayerID sender = rpcParams->sender;

	RakNet::BitStream bsData(Data,(iBitLength/8)+1,false);
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
	PCHAR Data = reinterpret_cast<PCHAR>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;
	PlayerID sender = rpcParams->sender;

	RakNet::BitStream bsData(Data,(iBitLength/8)+1,false);

	BYTE bytePlayerID;
	bsData.Read(bytePlayerID);

	RakNet::BitStream bsSend;
	bsSend.Write(bytePlayerID);
	
	pRak->RPC(RPC_DestroyWeaponPickup, &bsSend, HIGH_PRIORITY, RELIABLE, 0, UNASSIGNED_PLAYER_ID, true, false);
}

void PickedUpPickup(RPCParameters *rpcParams)
{
	PCHAR Data = reinterpret_cast<PCHAR>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	BYTE bytePlayerID = pRak->GetIndexFromPlayerID(rpcParams->sender);

	RakNet::BitStream bsData(Data,(iBitLength/8)+1,false);

	int iPickup;
	bsData.Read(iPickup);

	CGameMode *pGameMode = pNetGame->GetGameMode();
	CFilterScripts *pFilters = pNetGame->GetFilterScripts();
	
	if(pGameMode) pGameMode->OnPlayerPickedUpPickup(bytePlayerID, iPickup);
	if(pFilters) pFilters->OnPlayerPickedUpPickup(bytePlayerID, iPickup);
}

void MenuSelect(RPCParameters *rpcParams)
{
	PCHAR Data = reinterpret_cast<PCHAR>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;
	
	BYTE bytePlayerID = pRak->GetIndexFromPlayerID(rpcParams->sender);

	RakNet::BitStream bsData(Data,(iBitLength/8)+1,false);

	BYTE byteRow;
	bsData.Read(byteRow);

	CGameMode *pGameMode = pNetGame->GetGameMode();
	CFilterScripts *pFilters = pNetGame->GetFilterScripts();

	if(pGameMode) pGameMode->OnPlayerSelectedMenuRow(bytePlayerID, byteRow);
	if(pFilters) pFilters->OnPlayerSelectedMenuRow(bytePlayerID, byteRow);
}

void MenuQuit(RPCParameters *rpcParams)
{
	BYTE bytePlayerID = pRak->GetIndexFromPlayerID(rpcParams->sender);
		
	CGameMode *pGameMode = pNetGame->GetGameMode();
	CFilterScripts *pFilters = pNetGame->GetFilterScripts();

	if(pGameMode) pGameMode->OnPlayerExitedMenu(bytePlayerID);
	if(pFilters) pFilters->OnPlayerExitedMenu(bytePlayerID);
}

//----------------------------------------------------

void RegisterRPCs(RakServerInterface * pRakServer)
{
	pRak = pRakServer;

	REGISTER_STATIC_RPC(pRakServer, ClientJoin);
	REGISTER_STATIC_RPC(pRakServer, Chat);
	REGISTER_STATIC_RPC(pRakServer, Privmsg);
	REGISTER_STATIC_RPC(pRakServer, TeamPrivmsg);
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
}

//----------------------------------------------------

void UnRegisterRPCs(RakServerInterface * pRakServer)
{
	pRak = 0;

	UNREGISTER_STATIC_RPC(pRakServer, ClientJoin);
	UNREGISTER_STATIC_RPC(pRakServer, Chat);
	UNREGISTER_STATIC_RPC(pRakServer, Privmsg);
	UNREGISTER_STATIC_RPC(pRakServer, TeamPrivmsg);
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
}

//----------------------------------------------------
