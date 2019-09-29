//----------------------------------------------------------
//
// SA:MP Multiplayer Modification For GTA:SA
// Copyright 2004-2005 SA:MP team
//
// Version: $Id: netrpc.cpp,v 1.40 2006/06/02 13:24:20 mike Exp $
//
//----------------------------------------------------------

#include "../main.h"
using namespace RakNet;
extern CNetGame* pNetGame;

#define REJECT_REASON_BAD_VERSION   1
#define REJECT_REASON_BAD_NICKNAME  2
#define REJECT_REASON_BAD_MOD		3

extern ONFOOT_SYNC_DATA ofSync;
extern bool	bSpawned;

//----------------------------------------------------
// Sent when a client joins the server we're
// currently connected to.

void ServerJoin(RPCParameters *rpcParams)
{
	PCHAR Data = reinterpret_cast<PCHAR>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;
	PlayerID sender = rpcParams->sender;

	RakNet::BitStream bsData(Data,(iBitLength/8)+1,false);
	CPlayerPool *pPlayerPool = pNetGame->GetPlayerPool();
	CHAR szPlayerName[MAX_PLAYER_NAME+1];
	BYTE bytePlayerID;
	BYTE bytePlayerState;

	bsData.Read(bytePlayerID);
	bsData.Read(szPlayerName,MAX_PLAYER_NAME);
	bsData.Read(bytePlayerState);
	szPlayerName[MAX_PLAYER_NAME] = '\0';

	// Add this client to the player pool.
	pPlayerPool->New(bytePlayerID, szPlayerName);
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
	bool bLanMode;

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

	BYTE byteStrLen;
	bsInitGame.Read(byteStrLen);
	if(byteStrLen) {
		memset(pNetGame->m_szHostName,0,sizeof(pNetGame->m_szHostName));
		bsInitGame.Read(pNetGame->m_szHostName, byteStrLen);
	}
	pNetGame->m_szHostName[byteStrLen] = '\0';

	pPlayerPool->SetLocalPlayerID(byteMyPlayerID);
	if(bLanMode) pNetGame->SetLanMode(TRUE);
	pNetGame->SetGameState(GAMESTATE_CONNECTED);

	OutputDebugString("BOT: Got InitGame. Sending class request for 0");
	
	RakNet::BitStream bsSpawnRequest;
	bsSpawnRequest.Write((int)0);
	pNetGame->GetRakClient()->RPC(RPC_RequestClass,&bsSpawnRequest,HIGH_PRIORITY,RELIABLE,0,false);
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

	unsigned char szText[256];
	memset(szText,0,256);

	bsData.Read(bytePlayerID);
	bsData.Read(byteTextLen);
	bsData.Read((char*)szText,byteTextLen);
	szText[byteTextLen] = '\0';

	char s[256];
	sprintf(s,"BOT: Chat From %u: %s",bytePlayerID,szText);
	OutputDebugString(s);
}

//----------------------------------------------------
// Remote player has sent a private chat message.

void Privmsg(RPCParameters *rpcParams)
{
}

//----------------------------------------------------
// Remote player has sent a team chat message.

void TeamPrivmsg(RPCParameters *rpcParams)
{
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
	
	bsData.Read(byteRequestOutcome);
	bsData.Read((PCHAR)&SpawnInfo,sizeof(PLAYER_SPAWN_INFO));

	ofSync.byteHealth = 100;
	ofSync.byteArmour = 100;
	ofSync.byteCurrentWeapon = 0;
	ofSync.byteSpecialAction = 0;
	ofSync.vecPos.X = SpawnInfo.vecPos.X;
	ofSync.vecPos.Y = SpawnInfo.vecPos.Y;
	ofSync.vecPos.Z = SpawnInfo.vecPos.Z;

	if(byteRequestOutcome) {
		OutputDebugString("BOT: RequestClass. Requesting Spawn.");
		RakNet::BitStream bsSpawnRequest;
		pNetGame->GetRakClient()->RPC(RPC_RequestSpawn,&bsSpawnRequest,HIGH_PRIORITY,RELIABLE,0,false);
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

	if(byteRequestOutcome) {
		// Let the rest of the network know we're spawning.
		OutputDebugString("BOT: Spawning!");
		bSpawned = true;
		RakNet::BitStream bsSendSpawn;
		pNetGame->GetRakClient()->RPC(RPC_Spawn,&bsSendSpawn,HIGH_PRIORITY,
			RELIABLE_SEQUENCED,0,false);
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

	OutputDebugString("BOT: Spawn (RemotePlayer)");		
}

//----------------------------------------------------
// Remote client is dead.

void Death(RPCParameters *rpcParams)
{
}

//----------------------------------------------------
// Remote client is trying to enter vehicle gracefully.

void EnterVehicle(RPCParameters *rpcParams)
{
}

//----------------------------------------------------
// Remote client is trying to enter vehicle gracefully.

void ExitVehicle(RPCParameters *rpcParams)
{
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

	CAR_MOD_INFO	m_CarModInfo;
	CHAR cNumberPlate[9];

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
	bsData.Read(cNumberPlate, 9); // Constant size defined by SA
	bsData.Read((PCHAR)&m_CarModInfo, sizeof(m_CarModInfo));

	if (!pVehiclePool->New(VehicleID,iVehicleType, &vecPos,fRotation,iColor1,iColor2,&vecSpawnPos,fSpawnRotation, iInterior, cNumberPlate))
		return;
}

//----------------------------------------------------

void SetCheckpoint(RPCParameters *rpcParams)
{
}

//----------------------------------------------------

void DisableCheckpoint(RPCParameters *rpcParams)
{
}

//----------------------------------------------------

void SetRaceCheckpoint(RPCParameters *rpcParams)
{
}

//----------------------------------------------------

void DisableRaceCheckpoint(RPCParameters *rpcParams)
{
}

//----------------------------------------------------

void UpdateScoresPingsIPs(RPCParameters *rpcParams)
{
}

//----------------------------------------------------
RakNetStatisticsStruct RakServerStats;

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
}

//----------------------------------------------------

void ConnectionRejected(RPCParameters *rpcParams)
{
	PCHAR Data = reinterpret_cast<PCHAR>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;
	RakNet::BitStream bsData(Data,(iBitLength/8)+1,false);
	BYTE byteRejectReason;

	bsData.Read(byteRejectReason);

	if(byteRejectReason==REJECT_REASON_BAD_VERSION) {
		OutputDebugString("BOT: CONNECTION REJECTED. INCORRECT SA-MP VERSION!");
	}
	else if(byteRejectReason==REJECT_REASON_BAD_NICKNAME)
	{
		OutputDebugString("BOT: CONNECTION REJECTED. BAD NICKNAME!");
		OutputDebugString("BOT: Please choose another nick between 3-16 characters");
	}
	else if(byteRejectReason==REJECT_REASON_BAD_MOD)
	{
		OutputDebugString("BOT: CONNECTION REJECTED");
		OutputDebugString("BOT: YOUR'RE USING AN INCORRECT MOD!");
	}

	pNetGame->GetRakClient()->Disconnect(500);
	exit(1);
}

//----------------------------------------------------

void ClientMessage(RPCParameters *rpcParams)
{
	PCHAR Data = reinterpret_cast<PCHAR>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;
	RakNet::BitStream bsData(Data,(iBitLength/8)+1,false);
	DWORD dwStrLen;
	DWORD dwColor;

	bsData.Read(dwColor);
	bsData.Read(dwStrLen);
	char* szMsg = (char*)malloc(dwStrLen+1);
	bsData.Read(szMsg, dwStrLen);
	szMsg[dwStrLen] = 0;
	OutputDebugString(szMsg);
	free(szMsg);
}

//----------------------------------------------------

void WorldTime(RPCParameters *rpcParams)
{
}

//----------------------------------------------------

void Pickup(RPCParameters *rpcParams)
{
}

//----------------------------------------------------

void DestroyPickup(RPCParameters *rpcParams)
{
}

//----------------------------------------------------

void DestroyWeaponPickup(RPCParameters *rpcParams)
{
}

//----------------------------------------------------

void ScmEvent(RPCParameters *rpcParams)
{
}

//----------------------------------------------------

void Weather(RPCParameters *rpcParams)
{
}

//----------------------------------------------------

void SetTimeEx(RPCParameters *rpcParams)
{
}

//----------------------------------------------------

void ToggleClock(RPCParameters *rpcParams)
{
}

//----------------------------------------------------

void VehicleDestroy(RPCParameters *rpcParams)
{
}

//----------------------------------------------------

void Instagib(RPCParameters *rpcParams)
{
}

//----------------------------------------------------

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
