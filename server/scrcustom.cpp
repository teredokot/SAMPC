/*

	SA:MP Multiplayer Modification
	Copyright 2004-2005 SA:MP Team

	file:
		scrcore.cpp
	desc:
		Scripting custom functions

    Version: $Id: scrcustom.cpp,v 1.60 2006/05/20 08:28:04 kyeman Exp $

*/

#include "main.h"

#define CHECK_PARAMS(n) { if (params[0] != (n * sizeof(cell))) { logprintf("SCRIPT: Bad parameter count (Count is %d, Should be %d): ", params[0] / sizeof(cell), n); return 0; } }

char* format_amxstring(AMX *amx, cell *params, int parm, int &len);
int set_amxstring(AMX *amx,cell amx_addr,const char *source,int max);
bool ContainsInvalidNickChars(PCHAR szString);

extern BOOL bGameModeFinished;
extern CNetGame* pNetGame;

//----------------------------------------------------------------------------------

static cell AMX_NATIVE_CALL n_GameModeExit(AMX *amx, cell *params)
{
	if(pNetGame->SetNextScriptFile(NULL)) {
		bGameModeFinished = TRUE;
	} else {
		logprintf("The gamemode finished and I couldn't start another script.");
		fcloseall();
		exit(1);
	}
	return 0;
}


//----------------------------------------------------------------------------------

static cell AMX_NATIVE_CALL n_SetGameModeText(AMX *amx, cell *params)
{
	CHECK_PARAMS(1);

	char* szGameModeText;
	amx_StrParam(amx, params[1], szGameModeText);
	pConsole->SetStringVariable("gamemodetext", szGameModeText);

	return 0;
}

//----------------------------------------------------------------------------------

static cell AMX_NATIVE_CALL n_SetTeamCount(AMX *amx, cell *params)
{
	return 0;
}

//----------------------------------------------------------------------------------

static cell AMX_NATIVE_CALL n_AddPlayerClass(AMX *amx, cell *params)
{
	CHECK_PARAMS(11);
	PLAYER_SPAWN_INFO Spawn;

	Spawn.byteTeam = 255; // Auto team assignment for the old AddPlayerClass
	Spawn.iSkin = (int)params[1];
	Spawn.vecPos.X = amx_ctof(params[2]);
	Spawn.vecPos.Y = amx_ctof(params[3]);
	Spawn.vecPos.Z = amx_ctof(params[4]);
	Spawn.fRotation = amx_ctof(params[5]);

	// WEAPONS 
	Spawn.iSpawnWeapons[0] = (int)params[6];
	Spawn.iSpawnWeaponsAmmo[0] = (int)params[7];
	Spawn.iSpawnWeapons[1] = (int)params[8];
	Spawn.iSpawnWeaponsAmmo[1] = (int)params[9];
	Spawn.iSpawnWeapons[2] = (int)params[10];
	Spawn.iSpawnWeaponsAmmo[2] = (int)params[11];

	return pNetGame->AddSpawn(&Spawn);
}

//----------------------------------------------------------------------------------

static cell AMX_NATIVE_CALL n_AddPlayerClassEx(AMX *amx, cell *params)
{
	CHECK_PARAMS(12);
	PLAYER_SPAWN_INFO Spawn;

	// BASE INFO
	Spawn.byteTeam = (BYTE)params[1];
	Spawn.iSkin = (int)params[2];
	Spawn.vecPos.X = amx_ctof(params[3]);
	Spawn.vecPos.Y = amx_ctof(params[4]);
	Spawn.vecPos.Z = amx_ctof(params[5]);
	Spawn.fRotation = amx_ctof(params[6]);

	// WEAPONS 
	Spawn.iSpawnWeapons[0] = (int)params[7];
	Spawn.iSpawnWeaponsAmmo[0] = (int)params[8];
	Spawn.iSpawnWeapons[1] = (int)params[9];
	Spawn.iSpawnWeaponsAmmo[1] = (int)params[10];
	Spawn.iSpawnWeapons[2] = (int)params[11];
	Spawn.iSpawnWeaponsAmmo[2] = (int)params[12];

	return pNetGame->AddSpawn(&Spawn);
}

//----------------------------------------------------------------------------------

static cell AMX_NATIVE_CALL n_AddStaticVehicle(AMX *amx, cell *params)
{
	CHECK_PARAMS(7);

	VECTOR vecPos;
	vecPos.X = amx_ctof(params[2]);
	vecPos.Y = amx_ctof(params[3]);
	vecPos.Z = amx_ctof(params[4]);

	VEHICLEID ret = pNetGame->GetVehiclePool()->New((int)params[1], &vecPos, amx_ctof(params[5]),
		(int)params[6], (int)params[7], 120000);

	return ret;
}

//----------------------------------------------------------------------------------

static cell AMX_NATIVE_CALL n_AddStaticVehicleEx(AMX *amx, cell *params)
{
	CHECK_PARAMS(8);

	VECTOR vecPos;
	vecPos.X = amx_ctof(params[2]);
	vecPos.Y = amx_ctof(params[3]);
	vecPos.Z = amx_ctof(params[4]);

	VEHICLEID ret = pNetGame->GetVehiclePool()->New((int)params[1], &vecPos, amx_ctof(params[5]),
		(int)params[6], (int)params[7], ((int)params[8]) * 1000);

	return ret;
}

//----------------------------------------------------------------------------------

static cell AMX_NATIVE_CALL n_SetVehicleToRespawn(AMX *amx, cell *params)
{
	CHECK_PARAMS(1);

	CVehicle* pVehicle;
	pVehicle = pNetGame->GetVehiclePool()->GetAt((VEHICLEID)params[1]);

	if (pVehicle != NULL) {
		pVehicle->Respawn();
		RakNet::BitStream bsVehicle;
		bsVehicle.Write((VEHICLEID)params[1]);
		pNetGame->GetRakServer()->RPC(RPC_ScrRespawnVehicle , &bsVehicle, HIGH_PRIORITY, 
			RELIABLE, 0, UNASSIGNED_PLAYER_ID, true, false);
		return 1;
	}
	
	return 0;
}

//----------------------------------------------------------------------------------
// native LinkVehicleToInterior
static cell AMX_NATIVE_CALL n_LinkVehicleToInterior(AMX *amx, cell *params)
{
	CHECK_PARAMS(2);
	CVehicle* pVehicle;
	pVehicle = pNetGame->GetVehiclePool()->GetAt((VEHICLEID)params[1]);

	if (pVehicle != NULL)
	{
		pVehicle->SetVehicleInterior(params[2]);
		RakNet::BitStream bsData;
		bsData.Write((VEHICLEID)params[1]);
		bsData.Write((BYTE)params[2]);

		RakServerInterface* pRak = pNetGame->GetRakServer();
		pRak->RPC(RPC_ScrLinkVehicle , &bsData, HIGH_PRIORITY, 
			RELIABLE, 0, UNASSIGNED_PLAYER_ID, true, false);
		return 1;
	}
	return 0;
}

//----------------------------------------------------------------------------------
// native AddVehicleComponent(vehicleid, componentid);
static cell AMX_NATIVE_CALL n_AddVehicleComponent(AMX *amx, cell *params)
{
	CHECK_PARAMS(2);

	CVehicle* pVehicle = pNetGame->GetVehiclePool()->GetAt((VEHICLEID)params[1]);
	if (!pVehicle)
		return 0;

	RakNet::BitStream bsData;
	bsData.Write((BYTE)INVALID_PLAYER_ID);
	bsData.Write((int)EVENT_TYPE_CARCOMPONENT);
	bsData.Write((DWORD)params[1]);
	bsData.Write((DWORD)params[2]);
	bsData.Write((DWORD)0);

	RakServerInterface* pRak = pNetGame->GetRakServer();
	pRak->RPC(RPC_ScmEvent , &bsData, HIGH_PRIORITY, 
		RELIABLE, 0, UNASSIGNED_PLAYER_ID, true, false);

	BYTE*	pDataStart	= (BYTE*)&pVehicle->m_CarModInfo.byteCarMod0;
	for(int i = 0; i < 14; i++)
	{
		DWORD data = pDataStart[i];

		if (data == 0)
		{
			pDataStart[i] = (BYTE)(params[2]-1000);
			break;
		}
	}

	return 1;
}

//----------------------------------------------------------------------------------
// native RemoveVehicleComponent(vehicleid, componentid);
static cell AMX_NATIVE_CALL n_RemoveVehicleComponent(AMX *amx, cell *params)
{
	CHECK_PARAMS(2);

	CVehicle* pVehicle = pNetGame->GetVehiclePool()->GetAt((VEHICLEID)params[1]);
	if (!pVehicle)
		return 0;

	RakNet::BitStream bsData;
	bsData.Write((VEHICLEID)params[1]);
	bsData.Write((DWORD)params[2]);

	RakServerInterface* pRak = pNetGame->GetRakServer();
	pRak->RPC(RPC_ScrRemoveComponent , &bsData, HIGH_PRIORITY, 
		RELIABLE, 0, UNASSIGNED_PLAYER_ID, true, false);

	BYTE*	pDataStart	= (BYTE*)&pVehicle->m_CarModInfo.byteCarMod0;
	BYTE	byteComp = (BYTE)(params[2]-1000);
	for(int i = 0; i < 14; i++)
	{
		DWORD data = pDataStart[i];
		if (data == byteComp)
		{
			pDataStart[i] = 0;
			break;
		}
	}

	return 1;
}

//----------------------------------------------------------------------------------
// native ChangeVehicleColor(vehicleid, color1, color2);
static cell AMX_NATIVE_CALL n_ChangeVehicleColor(AMX *amx, cell *params)
{
	CHECK_PARAMS(3);

	CVehicle* pVehicle = pNetGame->GetVehiclePool()->GetAt((VEHICLEID)params[1]);
	
	if (!pVehicle)
		return 0;

	RakNet::BitStream bsData;
	bsData.Write((BYTE)INVALID_PLAYER_ID);
	bsData.Write((int)EVENT_TYPE_CARCOLOR);
	bsData.Write((DWORD)params[1]);
	bsData.Write((DWORD)params[2]);
	bsData.Write((DWORD)params[3]);

	RakServerInterface* pRak = pNetGame->GetRakServer();
	pRak->RPC(RPC_ScmEvent , &bsData, HIGH_PRIORITY, 
		RELIABLE, 0, UNASSIGNED_PLAYER_ID, true, false);

	pVehicle->m_CarModInfo.iColor0 = (int)params[2];
	pVehicle->m_CarModInfo.iColor1 = (int)params[3];

	return 1;
}

//----------------------------------------------------------------------------------
// native ChangeVehiclePaintjob(vehicleid, paintjobid);
static cell AMX_NATIVE_CALL n_ChangeVehiclePaintjob(AMX *amx, cell *params)
{
	CHECK_PARAMS(2);

	CVehicle* pVehicle = pNetGame->GetVehiclePool()->GetAt((VEHICLEID)params[1]);
	if (!pVehicle)
		return 1;

	RakNet::BitStream bsData;
	bsData.Write((BYTE)INVALID_PLAYER_ID);
	bsData.Write((int)EVENT_TYPE_PAINTJOB);
	bsData.Write((DWORD)params[1]);
	bsData.Write((DWORD)params[2]);
	bsData.Write((DWORD)0);

	RakServerInterface* pRak = pNetGame->GetRakServer();
	pRak->RPC(RPC_ScmEvent , &bsData, HIGH_PRIORITY, 
		RELIABLE, 0, UNASSIGNED_PLAYER_ID, true, false);

	pVehicle->m_CarModInfo.bytePaintJob = (BYTE)params[2];

	return 1;
}

//----------------------------------------------------------------------------------
// native SetVehicleNumberPlate(vehicleid, numberplate[]);
#define VALID(x) (((x) > 0x2F && (x) < 0x3A) || ((x > 0x40) && (x < 0x5B))) ? (x) : (((x > 0x60) && (x < 0x7B)) ? ((x) - 0x20) : ('_'))
// Not sure if that's faster than a bounds check
static cell AMX_NATIVE_CALL n_SetVehicleNumberPlate(AMX *amx, cell *params)
{
	CHECK_PARAMS(2);

	if (pNetGame->GetVehiclePool()->GetSlotState((VEHICLEID)params[1]))
	{
		char *szInput;
		// 10 char buffer to store full plate
		CHAR szPlate[9] = "";
		amx_StrParam(amx, params[2], szInput);
		if (szInput)
		{
			int len = strlen(szInput), i = 0;
			for ( ; i < 8; i++)
			{
				if (i >= len) szPlate[i] = '_'; // Pad if required
				else szPlate[i] = VALID(szInput[i]); // Else store the uppercase version, number or _ if invalid
			}
			szPlate[8] = 0;
		}
		pNetGame->GetVehiclePool()->GetAt((VEHICLEID)params[1])->SetNumberPlate(szPlate);
		return 1;
	}
	return 0;
}

//----------------------------------------------------------------------------------
// native GetVehicleModel(vehicleid);
static cell AMX_NATIVE_CALL n_GetVehicleModel(AMX *amx, cell *params)
{
	CHECK_PARAMS(1);

	CVehiclePool *pVehiclePool = pNetGame->GetVehiclePool();
	if (pVehiclePool && pVehiclePool->GetSlotState((VEHICLEID)params[1])) {
		return pVehiclePool->GetAt((VEHICLEID)params[1])->m_SpawnInfo.iVehicleType;
	}
	return 0;
}

//----------------------------------------------------------------------------------
// native AddStaticPickup(model,type,Float:X,Float:Y,Float:Z);

static cell AMX_NATIVE_CALL n_AddStaticPickup(AMX *amx, cell *params)
{
	CHECK_PARAMS(5);
	VECTOR vecPos;
	vecPos.X = amx_ctof(params[3]);
	vecPos.Y = amx_ctof(params[4]);
	vecPos.Z = amx_ctof(params[5]);

	if (pNetGame->GetPickupPool()->New(params[1],params[2],vecPos.X,vecPos.Y,vecPos.Z,1) != -1) return 1;
	return 0;
}

//----------------------------------------------------------------------------------
// native CreatePickup(model, type, Float:X, Float:Y, Float:Z);

static cell AMX_NATIVE_CALL n_CreatePickup(AMX *amx, cell *params)
{
	CHECK_PARAMS(5);
	VECTOR vecPos;
	vecPos.X = amx_ctof(params[3]);
	vecPos.Y = amx_ctof(params[4]);
	vecPos.Z = amx_ctof(params[5]);

	return pNetGame->GetPickupPool()->New(params[1],params[2],vecPos.X,vecPos.Y,vecPos.Z);
}

//----------------------------------------------------------------------------------
// native DestroyPickup(pickup);

static cell AMX_NATIVE_CALL n_DestroyPickup(AMX *amx, cell *params)
{
	CHECK_PARAMS(1);
	return pNetGame->GetPickupPool()->Destroy(params[1]);
}

//----------------------------------------------------------------------------------
// native SetPlayerWorldBounds(playerid,Float:x_max,Float:y_max,Float:x_min,Float:y_min);

static cell AMX_NATIVE_CALL n_SetPlayerWorldBounds(AMX *amx, cell *params)
{
	CHECK_PARAMS(5);
	RakNet::BitStream bsBounds;
	float fBounds[4];
	
	CPlayer* pPlayer = pNetGame->GetPlayerPool()->GetAt((BYTE)params[1]);
	if(!pPlayer) return 0;

	fBounds[0] = amx_ctof(params[2]);
	fBounds[1] = amx_ctof(params[3]);
	fBounds[2] = amx_ctof(params[4]);
	fBounds[3] = amx_ctof(params[5]);
	
	bsBounds.Write(fBounds[0]);
	bsBounds.Write(fBounds[1]);
	bsBounds.Write(fBounds[2]);
	bsBounds.Write(fBounds[3]);

	RakServerInterface* pRak = pNetGame->GetRakServer();
	pRak->RPC(RPC_ScrSetWorldBounds , &bsBounds, HIGH_PRIORITY, 
		RELIABLE, 0, pRak->GetPlayerIDFromIndex(params[1]), false, false);
	
	return 1;
}

//----------------------------------------------------------------------------------

// native Kick(playerid)
static cell AMX_NATIVE_CALL n_Kick(AMX *amx, cell *params)
{
	CHECK_PARAMS(1);

	if (pNetGame->GetPlayerPool()->GetSlotState((BYTE)params[1])) {
		pNetGame->KickPlayer((BYTE)params[1]);
		return 1;
	}
	return 0;
}

//----------------------------------------------------------------------------------

// native Ban(playerid)
static cell AMX_NATIVE_CALL n_Ban(AMX *amx, cell *params)
{
	CHECK_PARAMS(1);

	if (pNetGame->GetPlayerPool()->GetSlotState((BYTE)params[1])) {
		RakServerInterface* pRak = pNetGame->GetRakServer();
		PlayerID Player = pRak->GetPlayerIDFromIndex(params[1]);

		in_addr in;
		in.s_addr = Player.binaryAddress;
		pNetGame->AddBan(pNetGame->GetPlayerPool()->GetPlayerName((BYTE)params[1]), inet_ntoa(in), "INGAME BAN");
		pNetGame->KickPlayer((BYTE)params[1]);
		return 1;
	}
	return 0;
}

//----------------------------------------------------------------------------------

// native BanEx(playerid, reason)
static cell AMX_NATIVE_CALL n_BanEx(AMX *amx, cell *params)
{
	CHECK_PARAMS(2);

	if (pNetGame->GetPlayerPool()->GetSlotState((BYTE)params[1])) {
		RakServerInterface* pRak = pNetGame->GetRakServer();
		PlayerID Player = pRak->GetPlayerIDFromIndex(params[1]);

		in_addr in;
		in.s_addr = Player.binaryAddress;

		char *szReason;
		amx_StrParam(amx, params[2], szReason);

		pNetGame->AddBan(pNetGame->GetPlayerPool()->GetPlayerName((BYTE)params[1]), inet_ntoa(in), szReason);
		pNetGame->KickPlayer((BYTE)params[1]);
		return 1;
	}
	return 0;
}

//----------------------------------------------------------------------------------
// native IsPlayerAdmin(playerid)
static cell AMX_NATIVE_CALL n_IsPlayerAdmin(AMX *amx, cell *params)
{
	CHECK_PARAMS(1);
	CPlayerPool* pPlayerPool = pNetGame->GetPlayerPool();

	if (pPlayerPool->GetSlotState((BYTE)params[1]))
	{
		return pPlayerPool->IsAdmin((BYTE)params[1]);
	}
	return 0;
}

//----------------------------------------------------------------------------------
// native SetSpawnInfo(playerid, team, skin, Float:x, Float:y, Float:z, Float:rotation, weapon1, weapon1_ammo, weapon2, weapon2_ammo, weapon3, weapon3_ammo)
static cell AMX_NATIVE_CALL n_SetSpawnInfo(AMX *amx, cell *params)
{
	CHECK_PARAMS(13);

	CPlayer *pPlayer = pNetGame->GetPlayerPool()->GetAt((BYTE)params[1]);
	if (pPlayer)
	{
		PLAYER_SPAWN_INFO SpawnInfo;
		SpawnInfo.byteTeam = (BYTE)params[2];
		SpawnInfo.iSkin = (int)params[3];
		SpawnInfo.vecPos.X = amx_ctof(params[4]);
		SpawnInfo.vecPos.Y = amx_ctof(params[5]);
		SpawnInfo.vecPos.Z = amx_ctof(params[6]);
		SpawnInfo.fRotation = amx_ctof(params[7]);
		SpawnInfo.iSpawnWeapons[0] = (int)params[8];
		SpawnInfo.iSpawnWeaponsAmmo[0] = (int)params[9];
		SpawnInfo.iSpawnWeapons[1] = (int)params[10];
		SpawnInfo.iSpawnWeaponsAmmo[1] = (int)params[11];
		SpawnInfo.iSpawnWeapons[2] = (int)params[12];
		SpawnInfo.iSpawnWeaponsAmmo[2] = (int)params[13];

		pPlayer->SetSpawnInfo(&SpawnInfo);
		RakNet::BitStream bsData;
		bsData.Write((PCHAR)&SpawnInfo, sizeof(PLAYER_SPAWN_INFO));
		RakServerInterface *pRak = pNetGame->GetRakServer();
		pRak->RPC(RPC_ScrSetSpawnInfo , &bsData, HIGH_PRIORITY, RELIABLE, 0, pRak->GetPlayerIDFromIndex((int)params[1]), false, false);
		return 1;
	} else {
		return 0;
	}
}

//----------------------------------------------------------------------------------
// native SetSpawnInfo(playerid)
static cell AMX_NATIVE_CALL n_SpawnPlayer(AMX *amx, cell *params)
{
	CHECK_PARAMS(1);

	if (pNetGame->GetPlayerPool()->GetSlotState((BYTE)params[1]))
	{

		RakNet::BitStream bsData;
		RakServerInterface *pRak = pNetGame->GetRakServer();
		bsData.Write(2); // 2 - overwrite default behaviour
		pRak->RPC(RPC_RequestSpawn , &bsData, HIGH_PRIORITY, RELIABLE, 0, pRak->GetPlayerIDFromIndex((int)params[1]), false, false);
		return 1;
	} else {
		return 0;
	}
}

// native ForceClassSelection(playerid);
static cell AMX_NATIVE_CALL n_ForceClassSelection(AMX *amx, cell *params)
{
	CHECK_PARAMS(1);
	if (pNetGame->GetPlayerPool()->GetSlotState((BYTE)params[1]))
	{
		RakNet::BitStream bsData;
		RakServerInterface *pRak = pNetGame->GetRakServer();
		pRak->RPC(RPC_ScrForceSpawnSelection , &bsData, HIGH_PRIORITY, RELIABLE, 0, pRak->GetPlayerIDFromIndex((int)params[1]), false, false);
		return 1;
	} else {
		return 0;
	}
}

//----------------------------------------------------------------------------------
// native GetPlayerTeam(playerid)
static cell AMX_NATIVE_CALL n_GetPlayerTeam(AMX *amx, cell *params)
{
	CHECK_PARAMS(1);

	CPlayerPool* pPlayerPool = pNetGame->GetPlayerPool();

	if(pPlayerPool->GetSlotState(BYTE(params[1])))
	{
		return pPlayerPool->GetAt(BYTE(params[1]))->GetTeam();
	} else {
		return -1;
	}
}

//----------------------------------------------------------------------------------

//native SetPlayerName(playerid, name[])
static cell AMX_NATIVE_CALL n_SetPlayerName(AMX *amx, cell *params)
{
	CHECK_PARAMS(2);

	if (pNetGame->GetPlayerPool()->GetSlotState(BYTE(params[1])))
	{
		char *szNewNick;
		char szOldNick[MAX_PLAYER_NAME+1];
		amx_StrParam(amx, params[2], szNewNick);

		if(ContainsInvalidNickChars(szNewNick)) return -1;

		BYTE bytePlayerID = (BYTE)params[1];
		BYTE byteNickLen = strlen(szNewNick);
		BYTE byteSuccess;

		if(byteNickLen > MAX_PLAYER_NAME) return -1;

		strncpy(szOldNick,pNetGame->GetPlayerPool()->GetPlayerName(bytePlayerID),MAX_PLAYER_NAME);
		
		if (byteNickLen == 0 || pNetGame->GetPlayerPool()->IsNickInUse(szNewNick)) byteSuccess = 0;
		else byteSuccess = 1;

		RakNet::BitStream bsData;
		bsData.Write(bytePlayerID); // player id
		bsData.Write(byteNickLen); // nick length
		bsData.Write(szNewNick, byteNickLen); // name
		bsData.Write(byteSuccess); // if the nickname was rejected

		if (byteSuccess != 0)
		{
			pNetGame->GetPlayerPool()->SetPlayerName(bytePlayerID, szNewNick);
			logprintf("[nick] %s nick changed to %s", szOldNick, szNewNick);
			pNetGame->GetRakServer()->RPC(RPC_ScrSetPlayerName , &bsData, HIGH_PRIORITY, RELIABLE, 0, UNASSIGNED_PLAYER_ID, true, false);

		}

		return byteSuccess;
	}
	else
	{
		return 0;
	}
}

//----------------------------------------------------------------------------------

// native SetPlayerSkin(playerid, skin)
static cell AMX_NATIVE_CALL n_SetPlayerSkin(AMX *amx, cell *params)
{
	CHECK_PARAMS(2);

	if(pNetGame->GetPlayerPool()->GetSlotState(BYTE(params[1])))
	{
		CPlayer *pPlayer = pNetGame->GetPlayerPool()->GetAt((BYTE)params[1]);

		// Check to make sure the player has spawned before setting the skin remotely
		if (pPlayer->m_bHasSpawnInfo)
		{
			RakNet::BitStream bsData;
			bsData.Write((int)params[1]); // player id
			bsData.Write((int)params[2]); // skin id
			RakServerInterface *pRak = pNetGame->GetRakServer();
			pRak->RPC(RPC_ScrSetPlayerSkin , &bsData, HIGH_PRIORITY, RELIABLE, 0, UNASSIGNED_PLAYER_ID, true, false);
		} 
		pPlayer->m_SpawnInfo.iSkin = (int)params[2];

		return 1;
	}
	else
	{
		return 0;
	}
}

//----------------------------------------------------------------------------------

// native GetPlayerSkin(playerid)
static cell AMX_NATIVE_CALL n_GetPlayerSkin(AMX *amx, cell *params)
{
	CHECK_PARAMS(1);

	CPlayer *pPlayer = pNetGame->GetPlayerPool()->GetAt((BYTE)params[1]);
	if (pPlayer)
	{
		return pPlayer->m_SpawnInfo.iSkin;
	}
	return 0;
}

//----------------------------------------------------------------------------------

// native SetPlayerVirtualWorld(playerid, worldid)

static cell AMX_NATIVE_CALL n_SetPlayerVirtualWorld(AMX *amx, cell *params)
{
	CHECK_PARAMS(2);

	if (pNetGame->GetPlayerPool()->GetSlotState((BYTE)params[1]))
	{
		pNetGame->GetPlayerPool()->SetPlayerVirtualWorld((BYTE)params[1], (BYTE)params[2]);
		return 1;
	}
	return 0;
}

//----------------------------------------------------------------------------------

// native GetPlayerVirtualWorld(playerid)

static cell AMX_NATIVE_CALL n_GetPlayerVirtualWorld(AMX *amx, cell *params)
{
	CHECK_PARAMS(1);

	if (pNetGame->GetPlayerPool()->GetSlotState((BYTE)params[1]))
	{
		return pNetGame->GetPlayerPool()->GetPlayerVirtualWorld((BYTE)params[1]);
	}
	return 0;
}

//----------------------------------------------------------------------------------

// native SetVehicleVirtualWorld(vehicleid, worldid)

static cell AMX_NATIVE_CALL n_SetVehicleVirtualWorld(AMX *amx, cell *params)
{
	CHECK_PARAMS(2);

	if (pNetGame->GetVehiclePool()->GetSlotState((VEHICLEID)params[1]))
	{
		pNetGame->GetVehiclePool()->SetVehicleVirtualWorld((VEHICLEID)params[1], (BYTE)params[2]);
		return 1;
	}
	return 0;
}

//----------------------------------------------------------------------------------

// native GetVehicleVirtualWorld(vehicleid)

static cell AMX_NATIVE_CALL n_GetVehicleVirtualWorld(AMX *amx, cell *params)
{
	CHECK_PARAMS(1);

	if (pNetGame->GetVehiclePool()->GetSlotState((VEHICLEID)params[1]))
	{
		return pNetGame->GetVehiclePool()->GetVehicleVirtualWorld((VEHICLEID)params[1]);
	}
	return 0;
}


//----------------------------------------------------------------------------------
// native TogglePlayerSpectating(playerid, toggle);
static cell AMX_NATIVE_CALL n_TogglePlayerSpectating(AMX *amx, cell *params)
{
	CHECK_PARAMS(2);
	if (pNetGame->GetPlayerPool()->GetSlotState((BYTE)params[1]))
	{
		CPlayer *pPlayer = pNetGame->GetPlayerPool()->GetAt((BYTE)params[1]);
		
		pPlayer->m_SpectateID = 0xFFFFFFFF;
		pPlayer->m_byteSpectateType = SPECTATE_TYPE_NONE;

		RakNet::BitStream bsParams;
		bsParams.Write((BOOL)params[2]); // toggle
		RakServerInterface* pRak = pNetGame->GetRakServer();
		pRak->RPC(RPC_ScrTogglePlayerSpectating , &bsParams, HIGH_PRIORITY, RELIABLE, 0, pRak->GetPlayerIDFromIndex((BYTE)params[1]), false, false);
		return 1;
	}
	return 0;
}

//----------------------------------------------------------------------------------
// native PlayerSpectateVehicle(playerid, vehicleid, mode = SPECTATE_MODE_NORMAL);
static cell AMX_NATIVE_CALL n_PlayerSpectateVehicle(AMX *amx, cell *params)
{
	CHECK_PARAMS(3);

	//printf("n_PlayerSpectateVehicle(%u,%u,%u)",params[1],params[2],params[3]);

	if (pNetGame->GetPlayerPool()->GetSlotState((BYTE)params[1])
		&& pNetGame->GetVehiclePool()->GetSlotState((VEHICLEID)params[2]))
	{
		CPlayer *pPlayer = pNetGame->GetPlayerPool()->GetAt((BYTE)params[1]);
		pPlayer->m_SpectateID = (VEHICLEID)params[2];
		pPlayer->m_byteSpectateType = SPECTATE_TYPE_VEHICLE;

		RakNet::BitStream bsParams;
		bsParams.Write((VEHICLEID)params[2]); // vehicleid
		bsParams.Write((BYTE)params[3]); // mode
		RakServerInterface* pRak = pNetGame->GetRakServer();
		pRak->RPC(RPC_ScrPlayerSpectateVehicle , &bsParams, HIGH_PRIORITY, RELIABLE, 0, pRak->GetPlayerIDFromIndex((BYTE)params[1]), false, false);
		return 1;
	}
	return 0;
}

//----------------------------------------------------------------------------------
// native PlayerSpectatePlayer(playerid, vehicleid, mode = SPECTATE_MODE_NORMAL);
static cell AMX_NATIVE_CALL n_PlayerSpectatePlayer(AMX *amx, cell *params)
{
	CHECK_PARAMS(3);
	if (pNetGame->GetPlayerPool()->GetSlotState((BYTE)params[1])
		&& pNetGame->GetPlayerPool()->GetSlotState((BYTE)params[2]))
	{
		CPlayer *pPlayer = pNetGame->GetPlayerPool()->GetAt((BYTE)params[1]);
		pPlayer->m_SpectateID = (BYTE)params[2];
		pPlayer->m_byteSpectateType = SPECTATE_TYPE_PLAYER;

		RakNet::BitStream bsParams;
		bsParams.Write((BYTE)params[2]); // playerid
		bsParams.Write((BYTE)params[3]); // mode
		RakServerInterface* pRak = pNetGame->GetRakServer();
		pRak->RPC(RPC_ScrPlayerSpectatePlayer , &bsParams, HIGH_PRIORITY, RELIABLE, 0, pRak->GetPlayerIDFromIndex((BYTE)params[1]), false, false);
		return 1;
	}
	return 0;
}

//----------------------------------------------------------------------------------

// native SetPlayerTeam(playerid, team)
static cell AMX_NATIVE_CALL n_SetPlayerTeam(AMX *amx, cell *params)
{
	CHECK_PARAMS(2);

	//CPlayerPool* pPlayerPool = pNetGame->GetPlayerPool();

	if(pNetGame->GetPlayerPool()->GetSlotState(BYTE(params[1])))
	{
		//pPlayerPool->SetTeam(params[1], params[2]);

		RakNet::BitStream bsParams;
		bsParams.Write((BYTE)params[1]); // playerid
		bsParams.Write((BYTE)params[2]); // team id
		RakServerInterface* pRak = pNetGame->GetRakServer();
		pRak->RPC(RPC_ScrSetPlayerTeam , &bsParams, HIGH_PRIORITY, RELIABLE, 0, UNASSIGNED_PLAYER_ID, true, false);
		return 1;
	} else {
		return 0;
	}
}

//----------------------------------------------------------------------------------

// native GetPlayerPos(playerid, &Float:x, &Float:y, &Float:z)
static cell AMX_NATIVE_CALL n_GetPlayerPos(AMX *amx, cell *params)
{
	CHECK_PARAMS(4);

	CPlayer* pPlayer = pNetGame->GetPlayerPool()->GetAt((BYTE)params[1]);

	if (pPlayer)
	{
		cell* cptr;
		amx_GetAddr(amx, params[2], &cptr);
		*cptr = amx_ftoc(pPlayer->m_vecPos.X);
		amx_GetAddr(amx, params[3], &cptr);
		*cptr = amx_ftoc(pPlayer->m_vecPos.Y);
		amx_GetAddr(amx, params[4], &cptr);
		*cptr = amx_ftoc(pPlayer->m_vecPos.Z);

		return 1;
	} else {
		return 0;
	}
}

//----------------------------------------------------------------------------------

// native SetPlayerPos(playerid, Float:x, Float:y, Float:z)
static cell AMX_NATIVE_CALL n_SetPlayerPos(AMX *amx, cell *params)
{
	CHECK_PARAMS(4);

	if(pNetGame->GetPlayerPool()->GetSlotState(BYTE(params[1])))
	{
		VECTOR vecPos;
		vecPos.X = amx_ctof(params[2]);
		vecPos.Y = amx_ctof(params[3]);
		vecPos.Z = amx_ctof(params[4]);

		RakNet::BitStream bsParams;
		bsParams.Write(vecPos.X);	// X
		bsParams.Write(vecPos.Y);	// Y
		bsParams.Write(vecPos.Z);	// Z

		RakServerInterface* pRak = pNetGame->GetRakServer();
		pRak->RPC(RPC_ScrSetPlayerPos , &bsParams, HIGH_PRIORITY, RELIABLE, 0,
			pRak->GetPlayerIDFromIndex(params[1]), false, false);

		return 1;
	}
	else
	{
		return 0;
	}
}

//----------------------------------------------------------------------------------

// native SetPlayerPosFindZ(playerid, Float:x, Float:y, Float:z)
static cell AMX_NATIVE_CALL n_SetPlayerPosFindZ(AMX *amx, cell *params)
{
	CHECK_PARAMS(4);

	if(pNetGame->GetPlayerPool()->GetSlotState(BYTE(params[1])))
	{
		VECTOR vecPos;
		vecPos.X = amx_ctof(params[2]);
		vecPos.Y = amx_ctof(params[3]);
		vecPos.Z = amx_ctof(params[4]);

		RakNet::BitStream bsParams;
		bsParams.Write(vecPos.X);	// X
		bsParams.Write(vecPos.Y);	// Y
		bsParams.Write(vecPos.Z);	// Z

		RakServerInterface* pRak = pNetGame->GetRakServer();
		pRak->RPC(RPC_ScrSetPlayerPosFindZ , &bsParams, HIGH_PRIORITY, RELIABLE, 0,
			pRak->GetPlayerIDFromIndex(params[1]), false, false);

		return 1;
	}
	else
	{
		return 0;
	}
}

//----------------------------------------------------------------------------------
// native GetPlayerHealth(playerid, &Float:health)
static cell AMX_NATIVE_CALL n_GetPlayerHealth(AMX *amx, cell *params)
{
	CHECK_PARAMS(2);

	CPlayer* pPlayer = pNetGame->GetPlayerPool()->GetAt((BYTE)params[1]);

	if (pPlayer)
	{
		cell* cptr;
		amx_GetAddr(amx, params[2], &cptr);
		*cptr = amx_ftoc(pPlayer->m_fHealth);

		return 1;
	} else {
		return 0;
	}
}

//----------------------------------------------------------------------------------
// native SetPlayerHealth(playerid,Float:health)

static cell AMX_NATIVE_CALL n_SetPlayerHealth(AMX *amx, cell *params)
{
	CHECK_PARAMS(2);

	if(pNetGame->GetPlayerPool()->GetSlotState(BYTE(params[1])))
	{
		float fHealth = amx_ctof(params[2]);

		RakNet::BitStream bsHealth;
		bsHealth.Write(fHealth);

		//logprintf("Setting health of %d to %f:", params[1], fHealth);
		
		pNetGame->GetRakServer()->RPC(RPC_ScrSetPlayerHealth , &bsHealth, HIGH_PRIORITY, 
			RELIABLE, 0, pNetGame->GetRakServer()->GetPlayerIDFromIndex(params[1]), false, false);

		return 1;
	}
	else
	{
		return 0;
	}
}

//----------------------------------------------------------------------------------

// native PutPlayerInVehicle(playerid, vehicleid, seatid)
static cell AMX_NATIVE_CALL n_PutPlayerInVehicle(AMX *amx, cell *params)
{
	CHECK_PARAMS(3);

	if(pNetGame->GetPlayerPool()->GetSlotState(params[1]) && pNetGame->GetVehiclePool()->GetSlotState((VEHICLEID)params[2]))
	{
		RakNet::BitStream bsParams;
		bsParams.Write((VEHICLEID)params[2]);	// vehicleid
		bsParams.Write((BYTE)params[3]);	// seatid

		if((BYTE)params[3] == 0) { // driver
			pNetGame->GetVehiclePool()->GetAt((VEHICLEID)params[2])->m_byteDriverID = (BYTE)params[1];
		}

		RakServerInterface *pRak = pNetGame->GetRakServer();
		pRak->RPC(RPC_ScrPutPlayerInVehicle , &bsParams, HIGH_PRIORITY,
			RELIABLE_ORDERED, 0, pRak->GetPlayerIDFromIndex(params[1]), false, false);

		return 1;
	}
	else
	{
		return 0;
	}
}


//----------------------------------------------------------------------------------

// native RemovePlayerFromVehicle(playerid)
static cell AMX_NATIVE_CALL n_RemovePlayerFromVehicle(AMX *amx, cell *params)
{
	CHECK_PARAMS(1);

	if (!pNetGame->GetPlayerPool()->GetSlotState(params[1])) return 0;
	RakNet::BitStream bsParams;

	RakServerInterface *pRak = pNetGame->GetRakServer();
	pNetGame->GetRakServer()->RPC(RPC_ScrRemovePlayerFromVehicle , &bsParams, HIGH_PRIORITY,
		RELIABLE, 0, pRak->GetPlayerIDFromIndex(params[1]), false, false);

	return 1;
}

//----------------------------------------------------------------------------------

// native IsPlayerInVehicle(playerid, vehicleid)
static cell AMX_NATIVE_CALL n_IsPlayerInVehicle(AMX *amx, cell *params)
{
	CHECK_PARAMS(2);

	CPlayer* pPlayer = pNetGame->GetPlayerPool()->GetAt((BYTE)params[1]);
	if (!pPlayer) return 0;
	BYTE byteState = pPlayer->GetState();
	if ((byteState == PLAYER_STATE_DRIVER) || (byteState == PLAYER_STATE_PASSENGER))
	{
		if (pPlayer->m_VehicleID == params[2])
		{
			return 1;
		}
	}

	return 0;
}

//----------------------------------------------------------------------------------

// native IsPlayerInAnyVehicle(playerid)
static cell AMX_NATIVE_CALL n_IsPlayerInAnyVehicle(AMX *amx, cell *params)
{
	CHECK_PARAMS(1);

	CPlayer* pPlayer = pNetGame->GetPlayerPool()->GetAt((BYTE)params[1]);
	if (!pPlayer) return 0;
	BYTE byteState = pPlayer->GetState();

	if ((byteState == PLAYER_STATE_DRIVER) || (byteState == PLAYER_STATE_PASSENGER))
	{
		return 1;
	}

	return 0;
}

//----------------------------------------------------------------------------------

// native GetPlayerName(playerid, const name[], len)
static cell AMX_NATIVE_CALL n_GetPlayerName(AMX *amx, cell *params)
{
	CHECK_PARAMS(3);
	BYTE bytePlayerID = (BYTE)params[1];
	if (bytePlayerID > MAX_PLAYERS || !pNetGame->GetPlayerPool()->GetSlotState(bytePlayerID)) return 0;
	return set_amxstring(amx, params[2], pNetGame->GetPlayerPool()->
		GetPlayerName(bytePlayerID), params[3]);
}

//----------------------------------------------------------------------------------

// native GetWeaponName(weaponid, const name[], len)
static cell AMX_NATIVE_CALL n_GetWeaponName(AMX *amx, cell *params)
{
	CHECK_PARAMS(3);
	if(params[1] > WEAPON_COLLISION) return 0;

	return set_amxstring(amx,params[2],pNetGame->GetWeaponName(params[1]),params[3]);
}

//----------------------------------------------------------------------------------

// native CreateVehicle(vehicletype, Float:x, Float:y, Float:z, Float:rotation, color1, color2, respawndelay)
static cell AMX_NATIVE_CALL n_CreateVehicle(AMX *amx, cell *params)
{
	CHECK_PARAMS(8);

	VECTOR vecPos;
	vecPos.X = amx_ctof(params[2]);
	vecPos.Y = amx_ctof(params[3]);
	vecPos.Z = amx_ctof(params[4]);

	VEHICLEID VehicleID = pNetGame->GetVehiclePool()->New((int)params[1], &vecPos, 
		amx_ctof(params[5]), (int)params[6], (int)params[7], ((int)params[8]) * 1000);

	if (VehicleID != 0xFFFF)
	{
		for(int x = 0; x < MAX_PLAYERS;x++) {	
			if (pNetGame->GetPlayerPool()->GetSlotState(x)) 	{
				pNetGame->GetVehiclePool()->GetAt(VehicleID)->SpawnForPlayer(x);
			}
		}
	}

	return VehicleID;
}

//----------------------------------------------------

// native DestroyVehicle(vehicleid)

static cell AMX_NATIVE_CALL n_DestroyVehicle(AMX *amx, cell *params)
{
	CHECK_PARAMS(1);

	CVehiclePool*	pVehiclePool = pNetGame->GetVehiclePool();
	if (pVehiclePool->GetAt((VEHICLEID)params[1]))
	{
		pVehiclePool->Delete((VEHICLEID)params[1]);
		RakNet::BitStream bsParams;

		bsParams.Write((VEHICLEID)params[1]);

		pNetGame->GetRakServer()->RPC(RPC_VehicleDestroy , &bsParams, HIGH_PRIORITY, RELIABLE, 0, UNASSIGNED_PLAYER_ID, true, false);
		return 1;
	}
	return 0;
}

//----------------------------------------------------------------------------------

// native GetVehiclePos(vehicleid, &Float:x, &Float:y, &Float:z)
static cell AMX_NATIVE_CALL n_GetVehiclePos(AMX *amx, cell *params)
{
	CHECK_PARAMS(4);

	CVehicle* pVehicle = pNetGame->GetVehiclePool()->GetAt((VEHICLEID)params[1]);

	if (pVehicle)
	{
		cell* cptr;
		amx_GetAddr(amx, params[2], &cptr);
		*cptr = amx_ftoc(pVehicle->m_matWorld.pos.X);
		amx_GetAddr(amx, params[3], &cptr);
		*cptr = amx_ftoc(pVehicle->m_matWorld.pos.Y);
		amx_GetAddr(amx, params[4], &cptr);
		*cptr = amx_ftoc(pVehicle->m_matWorld.pos.Z);

		return 1;
	} else {
		return 0;
	}
}

//----------------------------------------------------------------------------------

// native SetVehiclePos(vehicleid, Float:x, Float:y, Float:z)
static cell AMX_NATIVE_CALL n_SetVehiclePos(AMX *amx, cell *params)
{
	CHECK_PARAMS(4);

	CVehicle* pVehicle = pNetGame->GetVehiclePool()->GetAt((VEHICLEID)params[1]);

	if (pVehicle)
	{
		if (pVehicle->m_byteDriverID != INVALID_ID)
		{
			RakNet::BitStream bsParams;
			bsParams.Write((VEHICLEID)params[1]);
			bsParams.Write(amx_ctof(params[2]));
			bsParams.Write(amx_ctof(params[3]));
			bsParams.Write(amx_ctof(params[4]));
			RakServerInterface* pRak = pNetGame->GetRakServer();
			pRak->RPC(RPC_ScrSetVehiclePos , &bsParams, HIGH_PRIORITY, 
				RELIABLE, 0, pRak->GetPlayerIDFromIndex(pVehicle->m_byteDriverID), false, false);
		}

		pVehicle->m_matWorld.pos.X = amx_ctof(params[2]);
		pVehicle->m_matWorld.pos.Y = amx_ctof(params[3]);
		pVehicle->m_matWorld.pos.Z = amx_ctof(params[4]);

		return 1;
	} else {
		return 0;
	}
}

//----------------------------------------------------------------------------------

// native SendClientMessage(playerid, color, const message[])
static cell AMX_NATIVE_CALL n_SendClientMessage(AMX *amx, cell *params)
{
	CHECK_PARAMS(3);
	if (!pNetGame->GetPlayerPool()->GetSlotState(params[1])) return 0;
	PlayerID pidPlayer = pNetGame->GetRakServer()->GetPlayerIDFromIndex(params[1]);
	char* szMessage;
	amx_StrParam(amx, params[3], szMessage);
	pNetGame->SendClientMessage(pidPlayer,params[2],szMessage);

	return 1;
}

//----------------------------------------------------------------------------------

// native SendPlayerMessageToPlayer(playerid, senderid, const message[])
static cell AMX_NATIVE_CALL n_SendPlayerMessageToPlayer(AMX *amx, cell *params)
{
	CHECK_PARAMS(3);
	if (pNetGame->GetPlayerPool()->GetSlotState((BYTE)params[1])
		&& pNetGame->GetPlayerPool()->GetSlotState((BYTE)params[2]))
	{	
		char* szMessage;
		amx_StrParam(amx, params[3], szMessage);
		BYTE byteTextLen = strlen(szMessage);
		
		RakNet::BitStream bsSend;
		bsSend.Write((BYTE)params[2]);
		bsSend.Write(byteTextLen);
		bsSend.Write(szMessage, byteTextLen);
		pNetGame->GetRakServer()->RPC(RPC_Chat, &bsSend, HIGH_PRIORITY, RELIABLE, 0,
			pNetGame->GetRakServer()->GetPlayerIDFromIndex((BYTE)params[1]), false, false);
		return 1;
	}
	return 0;
}

//----------------------------------------------------------------------------------

// native SendPlayerMessageToAll(senderid, const message[])
static cell AMX_NATIVE_CALL n_SendPlayerMessageToAll(AMX *amx, cell *params)
{
	CHECK_PARAMS(2);
	if (pNetGame->GetPlayerPool()->GetSlotState((BYTE)params[1]))
	{	
		char* szMessage;
		amx_StrParam(amx, params[2], szMessage);
		BYTE byteTextLen = strlen(szMessage);
		
		RakNet::BitStream bsSend;
		bsSend.Write((BYTE)params[1]);
		bsSend.Write(byteTextLen);
		bsSend.Write(szMessage, byteTextLen);
		pNetGame->GetRakServer()->RPC(RPC_Chat, &bsSend, HIGH_PRIORITY, RELIABLE, 0, UNASSIGNED_PLAYER_ID, true, false);
		return 1;
	}
	return 0;
}

//----------------------------------------------------------------------------------

// native SendClientMessageToAll(color, const message[])
static cell AMX_NATIVE_CALL n_SendClientMessageToAll(AMX *amx, cell *params)
{
	char* szMessage;
	amx_StrParam(amx, params[2], szMessage);
	pNetGame->SendClientMessageToAll(params[1],szMessage);

	return 1;
}

//----------------------------------------------------------------------------------

// native SendDeathMessage(killer,killee,weapon)
static cell AMX_NATIVE_CALL n_SendDeathMessage(AMX *amx, cell *params)
{
	CHECK_PARAMS(3);
	RakServerInterface* pRak = pNetGame->GetRakServer();
	RakNet::BitStream bsDM;
	bsDM.Write((BYTE)params[1]);
	bsDM.Write((BYTE)params[2]);
	bsDM.Write((BYTE)params[3]);

	pRak->RPC(RPC_ScrDeathMessage , &bsDM, HIGH_PRIORITY, 
		RELIABLE, 0, UNASSIGNED_PLAYER_ID, true, false);

	return 1;
}

//----------------------------------------------------------------------------------

// native SetPlayerColor(playerid, color)
static cell AMX_NATIVE_CALL n_SetPlayerColor(AMX *amx, cell *params)
{
	CHECK_PARAMS(2);
	CPlayer* pPlayer = pNetGame->GetPlayerPool()->GetAt((BYTE)params[1]);
	if(pPlayer) {
		pPlayer->SetPlayerColor(params[2]);
		return 1;
	}	

	return 0;
}

//----------------------------------------------------------------------------------

// native GetPlayerColor(playerid)
static cell AMX_NATIVE_CALL n_GetPlayerColor(AMX *amx, cell *params)
{
	CHECK_PARAMS(1);

	CPlayer* pPlayer = pNetGame->GetPlayerPool()->GetAt((BYTE)params[1]);
	if(pPlayer) {
		return pPlayer->GetPlayerColor();
	}	

	return 0;
}

//----------------------------------------------------------------------------------

// native GetPlayerVehicleID(playerid)
static cell AMX_NATIVE_CALL n_GetPlayerVehicleID(AMX *amx, cell *params)
{
	CHECK_PARAMS(1);

	CPlayer *pPlayer = pNetGame->GetPlayerPool()->GetAt((BYTE)params[1]);
	if(pPlayer) {
		return pPlayer->m_VehicleID;
	}
	return 0;
}

//----------------------------------------------------------------------------------

// native SetPlayerCheckpoint(playerid, Float:x, Float:y, Float:z, Float:size)
static cell AMX_NATIVE_CALL n_SetPlayerCheckpoint(AMX *amx, cell *params)
{
	CHECK_PARAMS(5);

	CPlayer *pPlayer = pNetGame->GetPlayerPool()->GetAt((BYTE)params[1]);
	if (pPlayer)
	{
		pPlayer->SetCheckpoint(amx_ctof(params[2]), amx_ctof(params[3]),
			amx_ctof(params[4]), amx_ctof(params[5]));

		return 1;
	}
	return 0;
}

//----------------------------------------------------------------------------------

// native DisablePlayerCheckpoint(playerid)
static cell AMX_NATIVE_CALL n_DisablePlayerCheckpoint(AMX *amx, cell *params)
{
	CHECK_PARAMS(1);

	CPlayer *pPlayer = pNetGame->GetPlayerPool()->GetAt((BYTE)params[1]);
	if (pPlayer)
	{
		pPlayer->ToggleCheckpoint(FALSE);

		return 1;
	}
	return 0;
}

//----------------------------------------------------------------------------------

// native IsPlayerInCheckpoint(playerid)
static cell AMX_NATIVE_CALL n_IsPlayerInCheckpoint(AMX *amx, cell *params)
{
	CHECK_PARAMS(1);

	CPlayer *pPlayer = pNetGame->GetPlayerPool()->GetAt((BYTE)params[1]);
	if (pPlayer)
	{
		return pPlayer->IsInCheckpoint();
	}
	return 0;
}

//----------------------------------------------------------------------------------

// native SetPlayerRaceCheckpoint(playerid, tpye, Float:x, Float:y, Float:z, Float:nx, Float:ny, Float:nz, Float:size)
static cell AMX_NATIVE_CALL n_SetPlayerRaceCheckpoint(AMX *amx, cell *params)
{
	CHECK_PARAMS(9);

	CPlayer *pPlayer = pNetGame->GetPlayerPool()->GetAt((BYTE)params[1]);
	if (pPlayer)
	{
		pPlayer->SetRaceCheckpoint(params[2], amx_ctof(params[3]),
			amx_ctof(params[4]), amx_ctof(params[5]), amx_ctof(params[6]),
			amx_ctof(params[7]), amx_ctof(params[8]), amx_ctof(params[9]));

		return 1;
	}
	return 0;
}

//----------------------------------------------------------------------------------

// native DisablePlayerRaceCheckpoint(playerid)
static cell AMX_NATIVE_CALL n_DisablePlayerRaceCheckpoint(AMX *amx, cell *params)
{
	CHECK_PARAMS(1);

	CPlayer *pPlayer = pNetGame->GetPlayerPool()->GetAt((BYTE)params[1]);
	if (pPlayer)
	{
		pPlayer->ToggleRaceCheckpoint(FALSE);

		return 1;
	}
	return 0;
}

//----------------------------------------------------------------------------------

// native IsPlayerInRaceCheckpoint(playerid)
static cell AMX_NATIVE_CALL n_IsPlayerInRaceCheckpoint(AMX *amx, cell *params)
{
	CHECK_PARAMS(1);

	CPlayer *pPlayer = pNetGame->GetPlayerPool()->GetAt((BYTE)params[1]);
	if (pPlayer)
	{
		return pPlayer->IsInRaceCheckpoint();
	}
	return 0;
}

//----------------------------------------------------------------------------------

// native GameTextForAll(strtext,displaytime,style)
static cell AMX_NATIVE_CALL n_GameTextForAll(AMX *amx, cell *params)
{
	CHECK_PARAMS(3);
	char *szMessage;
	int iLength;
	int iTime;
	int iStyle;

	amx_StrParam(amx, params[1], szMessage);
	iTime = params[2];
	iStyle = params[3];
	iLength = strlen(szMessage);

	if(!iLength) return 0;

	RakNet::BitStream bsParams;
	bsParams.Write(iStyle);
	bsParams.Write(iTime);
	bsParams.Write(iLength);
	bsParams.Write(szMessage,iLength);
	pNetGame->GetRakServer()->RPC(RPC_ScrDisplayGameText , &bsParams, HIGH_PRIORITY, 
		RELIABLE, 0, UNASSIGNED_PLAYER_ID, true, false);
	
	return 1;
}

//----------------------------------------------------------------------------------

// native GameTextForPlayer(playerid,strtext,displaytime,style)
static cell AMX_NATIVE_CALL n_GameTextForPlayer(AMX *amx, cell *params)
{
	CHECK_PARAMS(4);
	if (!pNetGame->GetPlayerPool()->GetSlotState((BYTE)params[1])) return 0;
	char *szMessage;
	int iLength;
	int iTime;
	int iStyle;

	amx_StrParam(amx, params[2], szMessage);
	iTime = params[3];
	iStyle = params[4];
	iLength = strlen(szMessage);

	if(!iLength) return 0;

	RakNet::BitStream bsParams;
	bsParams.Write(iStyle);
	bsParams.Write(iTime);
	bsParams.Write(iLength);
	bsParams.Write(szMessage,iLength);
	pNetGame->GetRakServer()->RPC(RPC_ScrDisplayGameText , &bsParams, HIGH_PRIORITY, 
		RELIABLE, 0, pNetGame->GetRakServer()->GetPlayerIDFromIndex(params[1]), false, false);

	return 1;
}

//----------------------------------------------------------------------------------

// native SetPlayerInterior(playerid,interiorid)
static cell AMX_NATIVE_CALL n_SetPlayerInterior(AMX *amx, cell *params)
{	
	CHECK_PARAMS(2);
	if (!pNetGame->GetPlayerPool()->GetSlotState((BYTE)params[1])) return 0;
	RakNet::BitStream bsParams;
	BYTE byteInteriorID = (BYTE)params[2];
	bsParams.Write(byteInteriorID);

	pNetGame->GetRakServer()->RPC(RPC_ScrSetInterior , &bsParams, HIGH_PRIORITY, 
		RELIABLE, 0, pNetGame->GetRakServer()->GetPlayerIDFromIndex(params[1]), false, false);

	return 1;
}

//----------------------------------------------------------------------------------

// native GetPlayerInterior(playerid,interiorid)
static cell AMX_NATIVE_CALL n_GetPlayerInterior(AMX *amx, cell *params)
{
	CHECK_PARAMS(1);

	CPlayer *pPlayer = pNetGame->GetPlayerPool()->GetAt((BYTE)params[1]);
	if (pPlayer)
	{
		return pPlayer->m_iInteriorId;
	}
	return 0;
}

//----------------------------------------------------------------------------------
// native SetPlayerSpecialAction(playerid,actionid)

static cell AMX_NATIVE_CALL n_SetPlayerSpecialAction(AMX *amx, cell *params)
{	
	if (!pNetGame->GetPlayerPool()->GetSlotState((BYTE)params[1])) return 0;
	RakNet::BitStream bsParams;
	bsParams.Write((BYTE)params[2]);
	pNetGame->GetRakServer()->RPC(RPC_ScrSetSpecialAction ,&bsParams, HIGH_PRIORITY, 
		RELIABLE, 0, pNetGame->GetRakServer()->GetPlayerIDFromIndex(params[1]), false, false);
	return 1;
}

//----------------------------------------------------------------------------------
// native GetPlayerSpecialAction(playerid)

static cell AMX_NATIVE_CALL n_GetPlayerSpecialAction(AMX *amx, cell *params)
{	
	if (!pNetGame->GetPlayerPool()->GetSlotState((BYTE)params[1])) return SPECIAL_ACTION_NONE;
	CPlayer *pPlayer = pNetGame->GetPlayerPool()->GetAt((BYTE)params[1]);
	if(pPlayer) {
		if(pPlayer->GetState() == PLAYER_STATE_ONFOOT) {
			return pPlayer->GetSpecialAction();
		}
	}	
	return SPECIAL_ACTION_NONE;
}


//----------------------------------------------------------------------------------

// native SetPlayerCameraPos(playerid,x,y,z)
static cell AMX_NATIVE_CALL n_SetPlayerCameraPos(AMX *amx, cell *params)
{	
	CHECK_PARAMS(4);
	if (!pNetGame->GetPlayerPool()->GetSlotState((BYTE)params[1])) return 0;
	RakNet::BitStream bsParams;

	VECTOR vecPos;
	vecPos.X = amx_ctof(params[2]);
	vecPos.Y = amx_ctof(params[3]);
	vecPos.Z = amx_ctof(params[4]);

	bsParams.Write(vecPos.X);
	bsParams.Write(vecPos.Y);
	bsParams.Write(vecPos.Z);

	pNetGame->GetRakServer()->RPC(RPC_ScrSetCameraPos , &bsParams, HIGH_PRIORITY, 
		RELIABLE, 0, pNetGame->GetRakServer()->GetPlayerIDFromIndex(params[1]), false, false);

	return 1;
}

//----------------------------------------------------------------------------------

// native SetPlayerCameraLookAt(playerid,x,y,z)
static cell AMX_NATIVE_CALL n_SetPlayerCameraLookAt(AMX *amx, cell *params)
{	
	CHECK_PARAMS(4);
	if (!pNetGame->GetPlayerPool()->GetSlotState((BYTE)params[1])) return 0;
	RakNet::BitStream bsParams;

	VECTOR vecPos;
	vecPos.X = amx_ctof(params[2]);
	vecPos.Y = amx_ctof(params[3]);
	vecPos.Z = amx_ctof(params[4]);
	
	bsParams.Write(vecPos.X);
	bsParams.Write(vecPos.Y);
	bsParams.Write(vecPos.Z);

	pNetGame->GetRakServer()->RPC(RPC_ScrSetCameraLookAt , &bsParams, HIGH_PRIORITY, 
		RELIABLE, 0, pNetGame->GetRakServer()->GetPlayerIDFromIndex(params[1]), false, false);

	return 1;
}

//----------------------------------------------------------------------------------

// native SetCameraBehindPlayer(playerid)
static cell AMX_NATIVE_CALL n_SetCameraBehindPlayer(AMX *amx, cell *params)
{	
	CHECK_PARAMS(1);
	if (!pNetGame->GetPlayerPool()->GetSlotState((BYTE)params[1])) return 0;
	RakNet::BitStream bsParams;
	RakServerInterface* pRak = pNetGame->GetRakServer();
	pRak->RPC(RPC_ScrSetCameraBehindPlayer , &bsParams, HIGH_PRIORITY, RELIABLE, 0,
		pRak->GetPlayerIDFromIndex(params[1]), false, false);

	return 1;
}

//----------------------------------------------------------------------------------

// native TogglePlayerControllable(playerid, toggle);
static cell AMX_NATIVE_CALL n_TogglePlayerControllable(AMX *amx, cell *params)
{
	CHECK_PARAMS(2);
	if (!pNetGame->GetPlayerPool()->GetSlotState((BYTE)params[1])) return 0;

	RakNet::BitStream bsParams;
	RakServerInterface* pRak = pNetGame->GetRakServer();
	bsParams.Write((BYTE)params[2]);
	pRak->RPC(RPC_ScrTogglePlayerControllable , &bsParams, HIGH_PRIORITY, RELIABLE, 0,
		pRak->GetPlayerIDFromIndex(params[1]), false, false);

	return 1;
}

//----------------------------------------------------------------------------------

// native SetVehicleParametersForPlayer(vehicleid,playerid,objective,doorslocked)
static cell AMX_NATIVE_CALL n_SetVehicleParamsForPlayer(AMX *amx, cell *params)
{	
	CHECK_PARAMS(4);
	if (!pNetGame->GetPlayerPool()->GetSlotState((BYTE)params[2]) ||
		!pNetGame->GetVehiclePool()->GetSlotState((VEHICLEID)params[1])) return 0;
	RakNet::BitStream bsParams;
	//VehicleID = (VEHICLEID)params[1];
	//byteObjectiveVehicle = (BYTE)params[3];
	//byteDoorsLocked = (BYTE)params[4];

	bsParams.Write((VEHICLEID)params[1]);
	bsParams.Write((BYTE)params[3]);
	bsParams.Write((BYTE)params[4]);

	pNetGame->GetRakServer()->RPC(RPC_ScrVehicleParams , &bsParams, HIGH_PRIORITY, 
		RELIABLE, 0, pNetGame->GetRakServer()->GetPlayerIDFromIndex(params[2]), false, false);

	return 1;
}

//----------------------------------------------------------------------------------
// native SetPlayerScore(playerid,score)

static cell AMX_NATIVE_CALL n_SetPlayerScore(AMX *amx, cell *params)
{	
	CHECK_PARAMS(2);
	BYTE bytePlayerID = (BYTE)params[1];
	int iScore = (int)params[2];

	if(pNetGame->GetPlayerPool()->GetSlotState(bytePlayerID)) {
		pNetGame->GetPlayerPool()->SetPlayerScore(bytePlayerID,iScore);
		return 1;
	}

	return 0;
}

//----------------------------------------------------------------------------------
// native GetPlayerScore(playerid)

static cell AMX_NATIVE_CALL n_GetPlayerScore(AMX *amx, cell *params)
{	
	CHECK_PARAMS(1);
	BYTE bytePlayerID = (BYTE)params[1];
	if (!pNetGame->GetPlayerPool()->GetSlotState(bytePlayerID)) return 0;

	return pNetGame->GetPlayerPool()->GetPlayerScore(bytePlayerID);
}

//----------------------------------------------------------------------------------
// native GivePlayerMoney(playerid,money)

static cell AMX_NATIVE_CALL n_GivePlayerMoney(AMX *amx, cell *params)
{	
	CHECK_PARAMS(2);
	RakNet::BitStream bsMoney;
	bsMoney.Write((int)params[2]);

	CPlayerPool *pPool = pNetGame->GetPlayerPool();

	if( pPool->GetSlotState((BYTE)params[1]) ) {
		pPool->SetPlayerMoney((BYTE)params[1], pPool->GetPlayerMoney((BYTE)params[1]) + params[2]);

		pNetGame->GetRakServer()->RPC(RPC_ScrHaveSomeMoney , &bsMoney, HIGH_PRIORITY, 
			RELIABLE, 0, pNetGame->GetRakServer()->GetPlayerIDFromIndex(params[1]), false, false);
		return 1;
	}

	return 0;
}

//----------------------------------------------------------------------------------
// native GetPlayerMoney(playerid)

static cell AMX_NATIVE_CALL n_GetPlayerMoney(AMX *amx, cell *params)
{	
	CHECK_PARAMS(1);
	BYTE bytePlayerID = (BYTE)params[1];
	CPlayerPool *pPlayerPool = pNetGame->GetPlayerPool();

	if(pPlayerPool->GetSlotState(bytePlayerID)) {
		return pPlayerPool->GetPlayerMoney(bytePlayerID);
	}

	return 0;
}

//----------------------------------------------------------------------------------
// native ResetPlayerMoney(playerid)

static cell AMX_NATIVE_CALL n_ResetPlayerMoney(AMX *amx, cell *params)
{		
	CHECK_PARAMS(1);
	RakNet::BitStream bsMoney;

	BYTE bytePlayerID = (BYTE)params[1];
	CPlayerPool *pPlayerPool = pNetGame->GetPlayerPool();

	if(pPlayerPool->GetSlotState(bytePlayerID)) {
		pPlayerPool->SetPlayerMoney(bytePlayerID,0);
		pNetGame->GetRakServer()->RPC(RPC_ScrResetMoney , &bsMoney, HIGH_PRIORITY, 
			RELIABLE, 0, pNetGame->GetRakServer()->GetPlayerIDFromIndex(bytePlayerID), false, false);
		return 1;
	}

	return 0;
}

//----------------------------------------------------------------------------------
// native GetPlayerAmmo(playerid)

static cell AMX_NATIVE_CALL n_GetPlayerAmmo(AMX *amx, cell *params)
{	
	CHECK_PARAMS(1);
	BYTE bytePlayerID = (BYTE)params[1];
	CPlayerPool *pPlayerPool = pNetGame->GetPlayerPool();

	if(pPlayerPool->GetSlotState(bytePlayerID)) {
		return pPlayerPool->GetPlayerAmmo(bytePlayerID);
	}

	return 0;
}

//----------------------------------------------------------------------------------
// native SetPlayerAmmo(playerid, weaponslot, ammo)

static cell AMX_NATIVE_CALL n_SetPlayerAmmo(AMX *amx, cell *params)
{	
	CHECK_PARAMS(3);
	BYTE bytePlayerID = (BYTE)params[1];
	CPlayerPool *pPlayerPool = pNetGame->GetPlayerPool();

	if(pPlayerPool->GetSlotState(bytePlayerID))
	{	
		RakNet::BitStream bsAmmo;
		bsAmmo.Write((BYTE)params[2]);
		bsAmmo.Write((WORD)params[3]);

		pNetGame->GetRakServer()->RPC(RPC_ScrSetWeaponAmmo , &bsAmmo, HIGH_PRIORITY, 
			RELIABLE, 0, pNetGame->GetRakServer()->GetPlayerIDFromIndex(bytePlayerID), false, false);
		return 1;
	}
	return 0;
}

//----------------------------------------------------------------------------------
// native GetPlayerWeaponData(playerid,slot,  &weapon, &ammo)

static cell AMX_NATIVE_CALL n_GetPlayerWeaponData(AMX *amx, cell *params)
{	
	CHECK_PARAMS(4);
	BYTE bIndex = (BYTE)params[2];
	if (bIndex >= 13) return 0;

	BYTE bytePlayerID = (BYTE)params[1];
	CPlayerPool *pPlayerPool = pNetGame->GetPlayerPool();
	//cell cWeapons[13];
	//cell cAmmo[13];
	

	if(pPlayerPool->GetSlotState(bytePlayerID)) {
		//return pPlayerPool->GetPlayerAmmo(bytePlayerID);
		//WEAPON_SLOT_TYPE* WeaponSlots = pPlayerPool->GetAt(bytePlayerID)->GetWeaponSlotsData();
		CPlayer *pPlayer =  pPlayerPool->GetAt(bytePlayerID);
		/*BYTE i;
		for (i = 0; i < 13; i++)
		{
			cWeapons[i] = (cell)pPlayer->m_dwSlotWeapon[i];
			cAmmo[i] = (cell)pPlayer->m_dwSlotAmmo[i];
		}*/
		cell* cptr;
		
		amx_GetAddr(amx, params[3], &cptr);
		*cptr = (cell)pPlayer->m_byteSlotWeapon[bIndex];
		amx_GetAddr(amx, params[4], &cptr);
		*cptr = (cell)pPlayer->m_dwSlotAmmo[bIndex];
		/*amx_GetAddr(amx, params[4], &cptr);
		*cptr = (int)((in.s_addr & 0x0000FF00) >> 8);
		amx_GetAddr(amx, params[5], &cptr);
		*cptr = (int)((in.s_addr & 0x000000FF));
		params[3] = (cell)pPlayer->m_dwSlotWeapon[bIndex]; //(cell)&cWeapons[0];
		params[4] = (cell)pPlayer->m_dwSlotAmmo[bIndex]; //(cell)&cAmmo[0];*/
		
		return 1;
	}

	return 0;
}

//----------------------------------------------------------------------------------
// native IsPlayerConnected(playerid)

static cell AMX_NATIVE_CALL n_IsPlayerConnected(AMX *amx, cell *params)
{	
	CHECK_PARAMS(1);
	BYTE bytePlayerID = (BYTE)params[1];
	CPlayerPool *pPlayerPool = pNetGame->GetPlayerPool();

	if (pPlayerPool->GetSlotState(bytePlayerID)) {
		return 1;
	}

	return 0;
}

//----------------------------------------------------------------------------------
// native GetPlayerState(playerid)

static cell AMX_NATIVE_CALL n_GetPlayerState(AMX *amx, cell *params)
{	
	CHECK_PARAMS(1);
	BYTE bytePlayerID = (BYTE)params[1];
	CPlayer *pPlayer = pNetGame->GetPlayerPool()->GetAt(bytePlayerID);

	if(pPlayer) {
		return pPlayer->GetState();
	}

	return PLAYER_STATE_NONE;
}

//----------------------------------------------------------------------------------
// native SetPlayerFacingAngle(playerid,Float:ang);

static cell AMX_NATIVE_CALL n_SetPlayerFacingAngle(AMX *amx, cell *params)
{	
	CHECK_PARAMS(2);

	if (!pNetGame->GetPlayerPool()->GetSlotState((BYTE)params[1])) return 0;

	RakNet::BitStream bsFace;
	float fFace = amx_ctof(params[2]);
	bsFace.Write(fFace);
	pNetGame->GetRakServer()->RPC(RPC_ScrSetPlayerFacingAngle , &bsFace, HIGH_PRIORITY, 
		RELIABLE, 0, pNetGame->GetRakServer()->GetPlayerIDFromIndex(params[1]), false, false);

	return 1;
}

//----------------------------------------------------------------------------------
// native GetPlayerFacingAngle(playerid,&Float:ang);

static cell AMX_NATIVE_CALL n_GetPlayerFacingAngle(AMX *amx, cell *params)
{
	CHECK_PARAMS(2);

	CPlayer* pPlayer = pNetGame->GetPlayerPool()->GetAt((BYTE)params[1]);

	if (pPlayer)
	{
		cell* cptr;
		amx_GetAddr(amx, params[2], &cptr);
		*cptr = amx_ftoc(pPlayer->m_fRotation);

		return 1;
	} else {
		return 0;
	}
}

//----------------------------------------------------------------------------------
// native ResetPlayerWeapons(playerid);

static cell AMX_NATIVE_CALL n_ResetPlayerWeapons(AMX *amx, cell *params)
{
	CHECK_PARAMS(1);

	if (!pNetGame->GetPlayerPool()->GetSlotState((BYTE)params[1])) return 0;

	RakNet::BitStream bsData;
	pNetGame->GetRakServer()->RPC(RPC_ScrResetPlayerWeapons , &bsData, HIGH_PRIORITY, 
		RELIABLE, 0, pNetGame->GetRakServer()->GetPlayerIDFromIndex(params[1]), false, false);
	return 1;
}

//----------------------------------------------------------------------------------
// native GivePlayerWeapon(playerid,weaponid,ammo);

static cell AMX_NATIVE_CALL n_GivePlayerWeapon(AMX *amx, cell *params)
{
	CHECK_PARAMS(3);

	if (!pNetGame->GetPlayerPool()->GetSlotState((BYTE)params[1])) return 0;


	RakNet::BitStream bsData;
	bsData.Write((int)params[2]); // weaponid
	bsData.Write((int)params[3]); // ammo
	pNetGame->GetRakServer()->RPC(RPC_ScrGivePlayerWeapon , &bsData, HIGH_PRIORITY, 
		RELIABLE, 0, pNetGame->GetRakServer()->GetPlayerIDFromIndex(params[1]), false, false);
	return 1;
}

//----------------------------------------------------------------------------------
// native SetWorldTime(hour)

static cell AMX_NATIVE_CALL n_SetWorldTime(AMX *amx, cell *params)
{	
	CHECK_PARAMS(1);
	BYTE byteHour = (BYTE)params[1];
	pNetGame->SetWorldTime(byteHour);
	return 1;
}

//----------------------------------------------------------------------------------
// native SetPlayerTime(playerid, hour, min)

static cell AMX_NATIVE_CALL n_SetPlayerTime(AMX *amx, cell *params)
{	
	CHECK_PARAMS(3);

	CPlayer* pPlayer = pNetGame->GetPlayerPool()->GetAt((BYTE)params[1]);
	if(!pPlayer) return 0;	
	
	pPlayer->SetTime((BYTE)params[2], (BYTE)params[3]);

	return 1;
}

//----------------------------------------------------------------------------------
// native GetPlayerTime(playerid, &hour, &minute)

static cell AMX_NATIVE_CALL n_GetPlayerTime(AMX *amx, cell *params)
{
	CHECK_PARAMS(3);

	CPlayer* pPlayer = pNetGame->GetPlayerPool()->GetAt((BYTE)params[1]);

	if (pPlayer)
	{
		int iTime;
		if (pPlayer->m_byteTime)
		{
			iTime = (int)pPlayer->m_fGameTime;
		}
		else
		{
			iTime = ((int)pNetGame->m_byteWorldTime) * 60;
		}

		cell* cptr;
			
		amx_GetAddr(amx, params[2], &cptr);
		*cptr = (cell)(iTime / 60);
		amx_GetAddr(amx, params[3], &cptr);
		*cptr = (cell)(iTime % 60);

		return 1;
	} else {
		return 0;
	}
}

//----------------------------------------------------------------------------------
// native TogglePlayerClock(playerid, toggle)

static cell AMX_NATIVE_CALL n_TogglePlayerClock(AMX *amx, cell *params)
{	
	CHECK_PARAMS(2);

	CPlayer* pPlayer = pNetGame->GetPlayerPool()->GetAt((BYTE)params[1]);
	if(!pPlayer) return 0;	
	
	pPlayer->SetClock((BYTE)params[2]);
	return 1;
}

//----------------------------------------------------------------------------------

// native print(const string[])
static cell AMX_NATIVE_CALL n_print(AMX* amx, cell* params)
{
	CHECK_PARAMS(1);

	char* msg;
	amx_StrParam(amx, params[1], msg);
	logprintf("%s",msg);
	return 0;
}

//----------------------------------------------------------------------------------


// native printf(const format[], {Float,_}:...)
static cell AMX_NATIVE_CALL n_printf(AMX *amx, cell *params)
{
	int len;
	logprintf("%s",format_amxstring(amx, params, 1, len));

	return 0;
}

/*

#define MAX_FORMATSTR 256

int amx_printstring(AMX *amx,cell *cstr,AMX_FMTINFO *info);

static int str_putstr(void *dest,TCHAR *str)
{
	if (_tcslen((TCHAR*)dest)+_tcslen(str)<MAX_FORMATSTR)
		_tcscat((TCHAR*)dest,str);
	return 0;
}

static int str_putchar(void *dest,TCHAR ch)
{
	int len=_tcslen((TCHAR*)dest);
	if (len<MAX_FORMATSTR-1)
	{
		((TCHAR*)dest)[len]   = ch;
		((TCHAR*)dest)[len+1] = '\0';
	}
	return 0;
}

// native strformat(const format[], {Fixed,_}:...)
static cell AMX_NATIVE_CALL n_printf(AMX *amx, cell *params)
{
	cell *cstr;
	AMX_FMTINFO info;
	TCHAR output[MAX_FORMATSTR];

	memset(&info,0,sizeof info);
	info.params=params+5;
	info.numparams=(int)(params[0]/sizeof(cell))-4;
	info.skip=0;
	info.length=MAX_FORMATSTR;  // max. length of the string
	info.f_putstr=str_putstr;
	info.f_putchar=str_putchar;
	info.user=output;
	output[0] = __T('\0');

	amx_GetAddr(amx,params[4],&cstr);
	amx_printstring(amx,cstr,&info);

	// store the output string
	amx_GetAddr(amx,params[1],&cstr);
	amx_SetString(cstr,(char*)output,(int)params[3],sizeof(TCHAR)>1,(int)params[2]);
	return 1;
}
*/

//----------------------------------------------------------------------------------

// native format(output[], len, const format[], {Float,_}:...)
static cell AMX_NATIVE_CALL n_format(AMX *amx, cell *params)
{
  int len;
  return set_amxstring(amx, params[1], format_amxstring(amx, params, 3, len), params[2]);
}

//----------------------------------------------------------------------------------

// native SetTimer(funcname[], interval, repeating)
static cell AMX_NATIVE_CALL n_SetTimer(AMX *amx, cell *params)
{
	CHECK_PARAMS(3);

	char* szFuncName;
	amx_StrParam(amx, params[1], szFuncName);
	return pNetGame->GetTimers()->New(szFuncName, params[2], params[3], amx);
}

//----------------------------------------------------------------------------------

// native KillTimer(timerid)
static cell AMX_NATIVE_CALL n_KillTimer(AMX *amx, cell *params)
{
	CHECK_PARAMS(1);

	pNetGame->GetTimers()->Kill(params[1]);

	return 1;
}

//----------------------------------------------------------------------------------

// native GetTickCount()
static cell AMX_NATIVE_CALL n_GetTickCount(AMX *amx, cell *params)
{
	CHECK_PARAMS(0);

	return (cell)GetTickCount();
}

//----------------------------------------------------------------------------------
// native GetMaxPlayers()
static cell AMX_NATIVE_CALL n_GetMaxPlayers(AMX *amx, cell *params)
{
	CHECK_PARAMS(0);
	
	extern CConsole *pConsole;
	return pConsole->GetIntVariable("maxplayers");
}

//----------------------------------------------------------------------------------
// native SetMaxPlayers(maxplayers)
/*static cell AMX_NATIVE_CALL n_SetMaxPlayers(AMX *amx, cell *params)
{
	CHECK_PARAMS(1);
	extern CConsole *pConsole;
	pConsole->SetIntVariable("maxplayers", (int)params[1]);
    return 1;
}*/

//----------------------------------------------------------------------------------
// native GetMaxPlayers()
static cell AMX_NATIVE_CALL n_LimitGlobalChatRadius(AMX *amx, cell *params)
{
	float fRadius = amx_ctof(params[1]);

	pNetGame->m_bLimitGlobalChatRadius = TRUE;
	pNetGame->m_fGlobalChatRadius = fRadius;
	
	return 1;
}

//----------------------------------------------------------------------------------

// native GetVehicleZAngle(vehicleid, &Float:z_angle)
static cell AMX_NATIVE_CALL n_GetVehicleZAngle(AMX *amx, cell *params)
{
	CHECK_PARAMS(2);

	CVehicle* pVehicle = pNetGame->GetVehiclePool()->GetAt((VEHICLEID)params[1]);

	if (pVehicle)
	{
		float fZAngle = atan2(-pVehicle->m_matWorld.up.X, pVehicle->m_matWorld.up.Y) * 180.0f/PI;
		
		// Bound it to [0, 360)
		while(fZAngle < 0.0f) 
			fZAngle += 360.0f;
		while(fZAngle >= 360.0f) 
			fZAngle -= 360.0f;
		
		cell* cptr;
		amx_GetAddr(amx, params[2], &cptr);
		*cptr = amx_ftoc(fZAngle);
		return 1;
	} else {
		return 0;
	}
}

//----------------------------------------------------------------------------------

// native SetVehicleZAngle(vehicleid, Float:z_angle)
static cell AMX_NATIVE_CALL n_SetVehicleZAngle(AMX *amx, cell *params)
{
	CHECK_PARAMS(2);

	CVehicle* pVehicle = pNetGame->GetVehiclePool()->GetAt((VEHICLEID)params[1]);

	if (pVehicle)
	{
		if (pVehicle->m_byteDriverID != INVALID_ID)
		{
			RakNet::BitStream bsParams;
			bsParams.Write((VEHICLEID)params[1]);
			bsParams.Write(amx_ctof(params[2]));

			RakServerInterface* pRak = pNetGame->GetRakServer();
			pRak->RPC(RPC_ScrSetVehicleZAngle , &bsParams, HIGH_PRIORITY, 
				RELIABLE, 0, pRak->GetPlayerIDFromIndex(pVehicle->m_byteDriverID), false, false);
		}
		return 1;
	} else {
		return 0;
	}
}

//----------------------------------------------------------------------------------

// native PlayerPlaySound(playerid, soundid, Float:x, Float:y, Float:z)
static cell AMX_NATIVE_CALL n_PlayerPlaySound(AMX *amx, cell *params)
{	
	CHECK_PARAMS(5);

	if (!pNetGame->GetPlayerPool()->GetSlotState((BYTE)params[1])) return 0;

	RakNet::BitStream bsParams;
	
	bsParams.Write(params[2]);
	bsParams.Write(amx_ctof(params[3]));
	bsParams.Write(amx_ctof(params[4]));
	bsParams.Write(amx_ctof(params[5]));

	pNetGame->GetRakServer()->RPC(RPC_ScrPlaySound , &bsParams, HIGH_PRIORITY, 
		RELIABLE, 0, pNetGame->GetRakServer()->GetPlayerIDFromIndex(params[1]), false, false);

	return 1;
}

//----------------------------------------------------------------------------------

// native ShowNameTags(show)
static cell AMX_NATIVE_CALL n_ShowNameTags(AMX *amx, cell *params)
{	
	CHECK_PARAMS(1);
	pNetGame->m_bShowNameTags = (bool)params[1];
	return 1;
}

//----------------------------------------------------------------------------------

// native ShowPlayerNameTagForPlayer(playerid, showplayerid, show)
static cell AMX_NATIVE_CALL n_ShowPlayerNameTagForPlayer(AMX *amx, cell *params)
{	
	CHECK_PARAMS(3);

	if (!pNetGame->GetPlayerPool()->GetSlotState((BYTE)params[1]) ||
		!pNetGame->GetPlayerPool()->GetSlotState((BYTE)params[2])) return 0;

	RakNet::BitStream bsParams;
	
	bsParams.Write((BYTE)params[2]);
	bsParams.Write((BYTE)params[3]);

	pNetGame->GetRakServer()->RPC(RPC_ScrShowNameTag , &bsParams, HIGH_PRIORITY, 
		RELIABLE, 0, pNetGame->GetRakServer()->GetPlayerIDFromIndex(params[1]), false, false);
	return 1;
}

//----------------------------------------------------------------------------------

// native ShowPlayerMarkers(show)
static cell AMX_NATIVE_CALL n_ShowPlayerMarkers(AMX *amx, cell *params)
{	
	CHECK_PARAMS(1);
	pNetGame->m_bShowPlayerMarkers = (bool)params[1];
	return 1;
}

//----------------------------------------------------------------------------------

// native AllowInteriorWeapons(allow)
static cell AMX_NATIVE_CALL n_AllowInteriorWeapons(AMX *amx, cell *params)
{
	CHECK_PARAMS(1);
	pNetGame->m_bAllowWeapons = (bool)params[1];
	return 1;
}

//----------------------------------------------------------------------------------

// native UsePlayerPedAnims()
static cell AMX_NATIVE_CALL n_UsePlayerPedAnims(AMX *amx, cell *params)
{
	pNetGame->m_bUseCJWalk = TRUE;
	return 1;
}

//----------------------------------------------------------------------------------

// native GetPlayerIP(playerid, const ip[], len)
static cell AMX_NATIVE_CALL n_GetPlayerIp(AMX *amx, cell *params)
{
	CHECK_PARAMS(3);
	
	if (pNetGame->GetPlayerPool()->GetSlotState((BYTE)params[1])) {
		RakServerInterface* pRak = pNetGame->GetRakServer();
		PlayerID Player = pRak->GetPlayerIDFromIndex(params[1]);

		in_addr in;
		in.s_addr = Player.binaryAddress;
		return set_amxstring(amx, params[2], inet_ntoa(in), params[3]);
	} else { return -1; }
}
//----------------------------------------------------------------------------------

// native GetPlayerPing(playerid)
static cell AMX_NATIVE_CALL n_GetPlayerPing(AMX *amx, cell *params)
{
	CHECK_PARAMS(1);
	
	if (pNetGame->GetPlayerPool()->GetSlotState((BYTE)params[1])) {
		RakServerInterface* pRak = pNetGame->GetRakServer();
		PlayerID Player = pRak->GetPlayerIDFromIndex(params[1]);

		return pRak->GetLastPing(Player);
	} else { return -1; }
}
//----------------------------------------------------------------------------------

// native GetPlayerWeapon(playerid)
static cell AMX_NATIVE_CALL n_GetPlayerWeapon(AMX *amx, cell *params)
{
	CHECK_PARAMS(1);
	CPlayer* pPlayer = pNetGame->GetPlayerPool()->GetAt((BYTE)params[1]);
	if(!pPlayer) return -1;	
	BYTE byteState = pPlayer->GetState();
	if ((byteState != PLAYER_STATE_DRIVER) || (byteState != PLAYER_STATE_PASSENGER))
	{
		return pPlayer->GetCurrentWeapon();
	} else { return 0; }
	
}

// native SetTimerEx(funcname[], interval, repeating, parameter)
static cell AMX_NATIVE_CALL n_SetTimerEx(AMX *amx, cell *params)
{
	if (params[0] < 4 * sizeof (cell))
	{
		logprintf("SCRIPT: Bad parameter count (%d < 4): ", params[0]);
		return 0;
	}
	else if (params[0] > 260 * sizeof (cell))
	{
		logprintf("SCRIPT: Bad parameter count (%d > 260): ", params[0]);
		return 0;
	}

	char* szFuncName;
	amx_StrParam(amx, params[1], szFuncName);
	return pNetGame->GetTimers()->NewEx(szFuncName, params[2], params[3], params, amx);
}
	
//----------------------------------------------------

// native SendRconCommand(command[])
static cell AMX_NATIVE_CALL n_SendRconCommand( AMX *amx, cell *params)
{
	CHECK_PARAMS(1);
	extern CConsole *pConsole;
	char* szCommand;
	amx_StrParam(amx, params[1], szCommand);
	pConsole->Execute(szCommand);
	return 1;
}

//----------------------------------------------------
// native GetPlayerArmour(playerid, &Float:armour)
static cell AMX_NATIVE_CALL n_GetPlayerArmour(AMX *amx, cell *params)
{
	CHECK_PARAMS(2);

	CPlayer* pPlayer = pNetGame->GetPlayerPool()->GetAt((BYTE)params[1]);

	if (pPlayer)
	{
		cell* cptr;
		amx_GetAddr(amx, params[2], &cptr);
		*cptr = amx_ftoc(pPlayer->m_fArmour);

		return 1;
	} else {
		return 0;
	}
}

//----------------------------------------------------
// native SetPlayerArmour(playerid,Float:armour)

static cell AMX_NATIVE_CALL n_SetPlayerArmour(AMX *amx, cell *params)
{
	CHECK_PARAMS(2);

	if (!pNetGame->GetPlayerPool()->GetSlotState((BYTE)params[1])) return 0;
	float fArmour = amx_ctof(params[2]);

	RakNet::BitStream bsArmour;
	bsArmour.Write(fArmour);

	pNetGame->GetRakServer()->RPC(RPC_ScrSetPlayerArmour , &bsArmour, HIGH_PRIORITY, 
		RELIABLE, 0, pNetGame->GetRakServer()->GetPlayerIDFromIndex(params[1]), false, false);

	return 1;
}

//-----------------------------------------------------
// native SetPlayerMarkerForPlayer(playerid, showplayerid, color)
// Sets a players radar blip color for another player
static cell AMX_NATIVE_CALL n_SetPlayerMarkerForPlayer(AMX *amx, cell *params)
{
	CHECK_PARAMS(3);

	if (pNetGame->GetPlayerPool()->GetAt((BYTE)params[1]) &&
		pNetGame->GetPlayerPool()->GetAt((BYTE)params[2]))
	{
		RakNet::BitStream bsMarker;
		bsMarker.Write((BYTE)params[2]);
		bsMarker.Write((DWORD)params[3]);

		
		pNetGame->GetRakServer()->RPC(RPC_ScrSetPlayerColor , &bsMarker, HIGH_PRIORITY, 
			RELIABLE, 0, pNetGame->GetRakServer()->GetPlayerIDFromIndex(params[1]), false, false);

		return 1;
	}
	else
	{
		return 0;
	}
}

//-----------------------------------------------------
// native SetPlayerMapIcon(playerid, iconid, Float:x, Float:y, Float:z, icontype, color)
static cell AMX_NATIVE_CALL n_SetPlayerMapIcon(AMX *amx, cell *params)
{
	CHECK_PARAMS(7); // Playerid,
	if ((BYTE)params[2] >= 32) return 0;
	
	//CPlayer* pPlayer = pNetGame->GetPlayerPool()->GetAt((BYTE)params[1]);

	RakNet::BitStream bsIcon;
	//float fPos[3];
	
	CPlayer* pPlayer = pNetGame->GetPlayerPool()->GetAt((BYTE)params[1]);
	if(!pPlayer) return 0;

	//fPos[0] = amx_ctof(params[3]);
	//fPos[1] = amx_ctof(params[4]);
	//fPos[2] = amx_ctof(params[5]);
	
	bsIcon.Write((BYTE)params[2]);
	bsIcon.Write(amx_ctof(params[3])); //fPos[0]);
	bsIcon.Write(amx_ctof(params[4])); //fPos[1]);
	bsIcon.Write(amx_ctof(params[5])); //fPos[2]);
	bsIcon.Write((BYTE)params[6]);
	bsIcon.Write((BYTE)params[7]);

	RakServerInterface* pRak = pNetGame->GetRakServer();
	pRak->RPC(RPC_ScrSetMapIcon , &bsIcon, HIGH_PRIORITY, 
		RELIABLE, 0, pRak->GetPlayerIDFromIndex(params[1]), false, false);

	return 1;
}

//-----------------------------------------------------
// native RemovePlayerMapIcon(playerid, iconid)
static cell AMX_NATIVE_CALL n_RemovePlayerMapIcon(AMX *amx, cell *params)
{
	CHECK_PARAMS(2); // Playerid, 
	
	//CPlayer* pPlayer = pNetGame->GetPlayerPool()->GetAt((BYTE)params[1]);
	if(!pNetGame->GetPlayerPool()->GetAt((BYTE)params[1])) return 0;
	// Not technically needed but adds checking incase they're not in the server (not actually sure if it'll matter)
	
	RakNet::BitStream bsIcon;
	bsIcon.Write((BYTE)params[2]);

	RakServerInterface* pRak = pNetGame->GetRakServer();
	pRak->RPC(RPC_ScrDisableMapIcon , &bsIcon, HIGH_PRIORITY, 
		RELIABLE, 0, pRak->GetPlayerIDFromIndex(params[1]), false, false);

	return 1;
}

//----------------------------------------------------------------------------------
// native GetPlayerKeys(playerid);

static cell AMX_NATIVE_CALL n_GetPlayerKeys(AMX *amx, cell *params)
{
	CHECK_PARAMS(4);

	CPlayer * pPlayer = pNetGame->GetPlayerPool()->GetAt((BYTE)params[1]);
	if (pPlayer) {
		WORD wKeys, udAnalog, lrAnalog;
		int iPlayerState = pPlayer->GetState();
		switch (iPlayerState)
		{
		case PLAYER_STATE_ONFOOT:
			wKeys = pPlayer->GetOnFootSyncData()->wKeys;
			udAnalog = (short)pPlayer->GetOnFootSyncData()->udAnalog;
			lrAnalog = (short)pPlayer->GetOnFootSyncData()->lrAnalog;
			break;

		case PLAYER_STATE_DRIVER:
			wKeys = pPlayer->GetInCarSyncData()->wKeys;
			udAnalog = (short)pPlayer->GetInCarSyncData()->udAnalog;
			lrAnalog = (short)pPlayer->GetInCarSyncData()->lrAnalog;
			break;

		case PLAYER_STATE_PASSENGER:
			wKeys = pPlayer->GetPassengerSyncData()->wKeys;
			udAnalog = (short)pPlayer->GetPassengerSyncData()->udAnalog;
			lrAnalog = (short)pPlayer->GetPassengerSyncData()->lrAnalog;
			break;

		case PLAYER_STATE_SPECTATING:
			wKeys = pPlayer->GetSpectatorSyncData()->wKeys;
			udAnalog = (short)pPlayer->GetSpectatorSyncData()->udAnalog;
			lrAnalog = (short)pPlayer->GetSpectatorSyncData()->lrAnalog;
			break;

		default:
			return 0;
		}
		cell* cptr;
		amx_GetAddr(amx, params[2], &cptr);
		*cptr = (cell)wKeys;
		amx_GetAddr(amx, params[3], &cptr);
		*cptr = (cell)udAnalog;
		amx_GetAddr(amx, params[4], &cptr);
		*cptr = (cell)lrAnalog;
		return 1;
	}
	return 0;
}

//----------------------------------------------------------------------------------

// native EnableTirePopping(enable)
static cell AMX_NATIVE_CALL n_EnableTirePopping(AMX *amx, cell *params)
{	
	pNetGame->m_bTirePopping = (bool)params[1];
	//char szPopping[128];
	//sprintf(szPopping, "%s", (params[1] == 1) ? "True" : "False");
	//pConsole->SetStringVariable("tirepopping", szPopping);
	// Removed - 
	return 1;
}

//----------------------------------------------------------------------------------

// native SetWeather(weather)
static cell AMX_NATIVE_CALL n_SetWeather(AMX *amx, cell *params)
{
	CHECK_PARAMS(1);

	
	pNetGame->SetWeather((BYTE)params[1]);

	//pNetGame->SendClientMessageToAll(0xFFFFFFFF, "Weather changed");
	// Removed - annoying for a release

	return 1;
}

//----------------------------------------------------------------------------------

// native SetPlayerWeather(playerid, weather)
static cell AMX_NATIVE_CALL n_SetPlayerWeather(AMX *amx, cell *params)
{
	CHECK_PARAMS(2);

	if (!pNetGame->GetPlayerPool()->GetSlotState((BYTE)params[1])) return 0;
	RakNet::BitStream bsWeather;
	bsWeather.Write((BYTE)params[2]);
	RakServerInterface* pRak = pNetGame->GetRakServer();
	pRak->RPC(RPC_Weather , &bsWeather, HIGH_PRIORITY, RELIABLE, 0, pRak->GetPlayerIDFromIndex(params[1]), false, false);
	//pRak->RPC(RPC_ScrDisableMapIcon", &bsIcon, HIGH_PRIORITY, RELIABLE, 0, pRak->GetPlayerIDFromIndex(params[1]), false, false);

	return 1;
}

//----------------------------------------------------------------------------------

static cell AMX_NATIVE_CALL n_asin(AMX *amx, cell *params)
{
	CHECK_PARAMS(1);
	float fResult = (float)(asin(amx_ctof(params[1])) * 180 / PI);
	return amx_ftoc(fResult);
}

static cell AMX_NATIVE_CALL n_acos(AMX *amx, cell *params)
{
	CHECK_PARAMS(1);
	float fResult = (float)(acos(amx_ctof(params[1])) * 180 / PI);
	return amx_ftoc(fResult);
}

static cell AMX_NATIVE_CALL n_atan(AMX *amx, cell *params)
{
	CHECK_PARAMS(1);
	float fResult = (float)(atan(amx_ctof(params[1])) * 180 / PI);
	return amx_ftoc(fResult);
}

static cell AMX_NATIVE_CALL n_atan2(AMX *amx, cell *params)
{
	CHECK_PARAMS(2);
	float fResult = (float)(atan2(amx_ctof(params[1]), amx_ctof(params[2])) * 180 / PI);
	return amx_ftoc(fResult);
}

//----------------------------------------------------
// native SetGravity(gravity)

static cell AMX_NATIVE_CALL n_SetGravity(AMX *amx, cell *params)
{
	CHECK_PARAMS(1);

	float fGravity = amx_ctof(params[1]);

	pNetGame->SetGravity(fGravity);

	return 1;
}

//----------------------------------------------------------------------------------
// native SetVehicleHealth(vehicleid, Float:health)

static cell AMX_NATIVE_CALL n_SetVehicleHealth(AMX *amx, cell *params)
{
	CHECK_PARAMS(2);

	CVehicle* pVehicle = pNetGame->GetVehiclePool()->GetAt((VEHICLEID)params[1]);
	if (pVehicle)
	{
		pVehicle->SetHealth(amx_ctof(params[2]));
		return 1;
	}

	return 0;
}

//----------------------------------------------------------------------------------
// native GetVehicleHealth(vehicleid, &Float:health)

static cell AMX_NATIVE_CALL n_GetVehicleHealth(AMX *amx, cell *params)
{
	CHECK_PARAMS(2);

	CVehicle* Vehicle = pNetGame->GetVehiclePool()->GetAt((VEHICLEID)params[1]);

	if (Vehicle)
	{

		cell* cptr;
		amx_GetAddr(amx, params[2], &cptr);
		*cptr = amx_ftoc(Vehicle->m_fHealth);

		return 1;
	} else {
		return 0;
	}
}

//----------------------------------------------------------------------------------
// native ApplyAnimation(playerid, animlib[], animname[], Float:fS, opt1, opt2, opt3, opt4, opt5)

static cell AMX_NATIVE_CALL n_ApplyAnimation(AMX *amx, cell *params)
{
	CHECK_PARAMS(9);
	RakNet::BitStream bsSend;

	char *szAnimLib;
	char *szAnimName;
	BYTE byteAnimLibLen;
	BYTE byteAnimNameLen;
	float fS;
	bool opt1,opt2,opt3,opt4;
	int opt5;
	
	CPlayerPool *pPlayerPool = pNetGame->GetPlayerPool();
	if(!pPlayerPool || !pPlayerPool->GetSlotState(params[1])) return 1;

	amx_StrParam(amx, params[2], szAnimLib);
	amx_StrParam(amx, params[3], szAnimName);

	byteAnimLibLen = strlen(szAnimLib);
	byteAnimNameLen = strlen(szAnimName);

	fS = amx_ctof(params[4]);
	opt1 = (bool)params[5];
	opt2 = (bool)params[6];
	opt3 = (bool)params[7];
	opt4 = (bool)params[8];
	opt5 = (int)params[9];

	bsSend.Write((BYTE)params[1]);
	bsSend.Write(byteAnimLibLen);
	bsSend.Write(szAnimLib,byteAnimLibLen);
	bsSend.Write(byteAnimNameLen);
	bsSend.Write(szAnimName,byteAnimNameLen);
	bsSend.Write(fS);
	bsSend.Write(opt1);
	bsSend.Write(opt2);
	bsSend.Write(opt3);
	bsSend.Write(opt4);
	bsSend.Write(opt5);

	pNetGame->BroadcastDistanceRPC(RPC_ScrApplyAnimation,&bsSend,UNRELIABLE,(BYTE)params[1],200.0f);
	
	return 1;
}
//----------------------------------------------------------------------------------
// native ClearAnimations(playerid)

static cell AMX_NATIVE_CALL n_ClearAnimations(AMX *amx, cell *params)
{
	CHECK_PARAMS(1);
	RakNet::BitStream bsSend;
	
	CPlayerPool *pPlayerPool = pNetGame->GetPlayerPool();
	if(!pPlayerPool || !pPlayerPool->GetSlotState(params[1])) return 1;

	bsSend.Write((BYTE)params[1]);
	pNetGame->BroadcastDistanceRPC(RPC_ScrClearAnimations,&bsSend,UNRELIABLE,(BYTE)params[1],200.0f);
	
	return 1;
}


//----------------------------------------------------------------------------------
// native AllowPlayerTeleport(playerid, allow)

static cell AMX_NATIVE_CALL n_AllowPlayerTeleport(AMX *amx, cell *params)
{
	CHECK_PARAMS(2);
	
	CPlayer* pPlayer = pNetGame->GetPlayerPool()->GetAt((BYTE)params[1]);
	if(!pPlayer) return 0;
	if (params[2])
	{
		pPlayer->m_bCanTeleport = true;
	}
	else
	{
		pPlayer->m_bCanTeleport = false;
	}
	return 1;
}

//----------------------------------------------------------------------------------

// native AllowAdminTeleport(allow)

static cell AMX_NATIVE_CALL n_AllowAdminTeleport(AMX *amx, cell *params)
{
	CHECK_PARAMS(1);
	
	if (params[1])
	{
		pNetGame->m_bAdminTeleport = true;
	}
	else
	{
		pNetGame->m_bAdminTeleport = false;
	}
	return 1;
}

//----------------------------------------------------

static cell AMX_NATIVE_CALL n_AttachTrailerToVehicle(AMX *amx, cell *params)
{
	CHECK_PARAMS(2);
	CVehiclePool* pVehiclePool = pNetGame->GetVehiclePool();

	if ( pVehiclePool->GetAt((VEHICLEID)params[1]) && pVehiclePool->GetAt((VEHICLEID)params[2]) )
	{
		RakNet::BitStream bsParams;
		bsParams.Write((VEHICLEID)params[1]);
		bsParams.Write((VEHICLEID)params[2]);
		pNetGame->GetRakServer()->RPC(RPC_ScrAttachTrailerToVehicle , &bsParams, HIGH_PRIORITY, RELIABLE, 0, UNASSIGNED_PLAYER_ID, true, false);
	}
	return 1;
}

//----------------------------------------------------

static cell AMX_NATIVE_CALL n_DetachTrailerFromVehicle(AMX *amx, cell *params)
{
	CHECK_PARAMS(1);
	CVehiclePool* pVehiclePool = pNetGame->GetVehiclePool();

	if ( pVehiclePool->GetAt((VEHICLEID)params[1]) && pVehiclePool->GetAt((VEHICLEID)params[2]) )
	{
		RakNet::BitStream bsParams;
		bsParams.Write((VEHICLEID)params[1]);
		pNetGame->GetRakServer()->RPC(RPC_ScrDetachTrailerFromVehicle , &bsParams, HIGH_PRIORITY, RELIABLE, 0, UNASSIGNED_PLAYER_ID, true, false);
	}
	return 1;
}

//----------------------------------------------------

static cell AMX_NATIVE_CALL n_IsTrailerAttachedToVehicle(AMX *amx, cell *params)
{
	CHECK_PARAMS(1);
	CVehiclePool* pVehiclePool = pNetGame->GetVehiclePool();
	if ( pVehiclePool->GetAt((VEHICLEID)params[1])->m_TrailerID != 0 )
	{
		return 1;
	}
	return 0;
}

//----------------------------------------------------

static cell AMX_NATIVE_CALL n_GetVehicleTrailer(AMX *amx, cell *params)
{
	CHECK_PARAMS(1);
	if (!pNetGame->GetVehiclePool()->GetSlotState((VEHICLEID)params[1])) return 0;
	return (cell)pNetGame->GetVehiclePool()->GetAt((VEHICLEID)params[1])->m_TrailerID;
}

cell* get_amxaddr(AMX *amx, cell amx_addr);
// native CallRemoteFunction(functionname[], paramlist[], parameters...)
static cell AMX_NATIVE_CALL n_CallRemoteFunction(AMX *amx, cell *params)
{
	if (params[0] < 2 * sizeof (cell))
	{
		logprintf("SCRIPT: Bad parameter count (%d < 2): ", params[0]);
		return 0;
	}
	else if (params[0] > 258 * sizeof (cell))
	{
		logprintf("SCRIPT: Bad parameter count (%d > 258): ", params[0]);
		return 0;
	}
	int iLength;
	char* szFuncName; //[32];
	char* szParamList; //[16];
	AMX *amxFile;
	bool bFound = false;
	
	amx_StrParam(amx, params[1], szFuncName);
	amx_StrParam(amx, params[2], szParamList);
	if (szParamList == NULL) iLength = 0;
	else iLength = strlen(szParamList);
	
	int idx, i, j;
	cell ret = 0;
	
	for (i = -1; i < MAX_FILTER_SCRIPTS; i++)
	{
		//printf("hi 3: %d\n", i);
		if (i == -1)
		{
			CGameMode* pGameMode = pNetGame->GetGameMode();
			if (pGameMode != NULL && pGameMode->IsInitialised())
			{
				amxFile = pGameMode->GetGameModePointer();
			}
			else
			{
				amxFile = NULL;
			}
		}
		else
		{
			CFilterScripts * pFilterScripts = pNetGame->GetFilterScripts();
			if (pFilterScripts != NULL)
			{
				amxFile = pFilterScripts->GetFilterScript(i);
			}
			else
			{
				amxFile = NULL;
			}
		}
		if (amxFile != NULL)
		{
			if (!amx_FindPublic(amxFile, szFuncName, &idx))
			{
				cell amx_addr[256]; // = NULL, *phys_addr[16];
				j = iLength;
				int iOff = 3, numstr;
				for (numstr = 0; numstr < 16; numstr++)
				{
					amx_addr[numstr] = NULL;
				}
				numstr = 0;
				while (j)
				{
					j--;
					if (*(szParamList + j) == 'a')
					{
						cell *paddr; //, *amx_addr;
						int numcells = *get_amxaddr(amx, params[j + iOff + 1]);
						if (amx_Allot(amxFile, numcells, &amx_addr[numstr], &paddr) == AMX_ERR_NONE)
						{
							memcpy(paddr, get_amxaddr(amx, params[j + iOff]), numcells * sizeof (cell));
							amx_Push(amxFile, amx_addr[numstr]);
							numstr++;
						} /* if */
						//iOff++;
					}
					else if (*(szParamList + j) == 's')
					{
						char* szParamText;
						
						//amx_StrParam(amx, *get_amxaddr(amx, params[j + iOff]), szParamText);
						//printf("source 1");
						amx_StrParam(amx, params[j + iOff], szParamText);
						//printf("source 2");
						if (szParamText != NULL && strlen(szParamText) > 0)
						{
							//printf("source 3");
							//printf("%s", szParamText);
							amx_PushString(amxFile, &amx_addr[numstr], NULL, szParamText, 0, 0);
							numstr++;
						}
						else
						{
							//printf("source 4");
							//amx_Push(amxFile, *get_amxaddr(amx, params[j + iOff]));
							*szParamText = 1;
							*(szParamText + 1) = 0;
							amx_PushString(amxFile, &amx_addr[numstr], NULL, szParamText, 0, 0);
						}
					}
					else
					{
						amx_Push(amxFile, *get_amxaddr(amx, params[j + iOff]));
					}
				}
				amx_Exec(amxFile, &ret, idx);
				while (numstr)
				{
					numstr--;
					amx_Release(amxFile, amx_addr[numstr]);
				}
			}
		}
	}
	return (int)ret;
}
// native CallLocalFunction(functionname[], paramlist[], parameters...)
static cell AMX_NATIVE_CALL n_CallLocalFunction(AMX *amx, cell *params)
{
	if (params[0] < 2 * sizeof (cell))
	{
		logprintf("SCRIPT: Bad parameter count (%d < 2): ", params[0]);
		return 0;
	}
	else if (params[0] > 258 * sizeof (cell))
	{
		logprintf("SCRIPT: Bad parameter count (%d > 258): ", params[0]);
		return 0;
	}
	int iLength;
	char* szFuncName; //[32];
	char* szParamList; //[16];
	bool bFound = false;
	
	amx_StrParam(amx, params[1], szFuncName);
	amx_StrParam(amx, params[2], szParamList);
	if (szParamList == NULL) iLength = 0;
	else iLength = strlen(szParamList);
	
	int idx, j;
	cell ret = 0;
	
	if (!amx_FindPublic(amx, szFuncName, &idx))
	{
		cell amx_addr[256];
		j = iLength;
		int numstr, iOff = 3; // Count, func, map
		for (numstr = 0; numstr < 16; numstr++)
		{
			amx_addr[numstr] = NULL;
		}
		numstr = 0;
		while (j)
		{
			j--;
			if (*(szParamList + j) == 'a')
			{
				cell *paddr;
				int numcells = *get_amxaddr(amx, params[j + iOff + 1]);
				if (amx_Allot(amx, numcells, &amx_addr[numstr], &paddr) == AMX_ERR_NONE)
				{
					memcpy(paddr, get_amxaddr(amx, params[j + iOff]), numcells * sizeof (cell));
					amx_Push(amx, amx_addr[numstr]);
					numstr++;
				}
			}
			else if (*(szParamList + j) == 's')
			{
				char* szParamText;
				
				amx_StrParam(amx, params[j + iOff], szParamText);
				if (szParamText != NULL && strlen(szParamText) > 0)
				{
					amx_PushString(amx, &amx_addr[numstr], NULL, szParamText, 0, 0);
					numstr++;
				}
				else
				{
					*szParamText = 1;
					*(szParamText + 1) = 0;
					amx_PushString(amx, &amx_addr[numstr], NULL, szParamText, 0, 0);
				}
			}
			else
			{
				amx_Push(amx, *get_amxaddr(amx, params[j + iOff]));
			}
		}
		amx_Exec(amx, &ret, idx);
		while (numstr)
		{
			numstr--;
			amx_Release(amx, amx_addr[numstr]);
		}
	}
	return (int)ret;
}

// native SetDeathDropAmount(amount)
static cell AMX_NATIVE_CALL n_SetDeathDropAmount(AMX *amx, cell *params)
{
	CHECK_PARAMS(1);
	pNetGame->m_iDeathDropMoney = params[1];
	return 1;
}

// native GetGravity()

static cell AMX_NATIVE_CALL n_GetGravity(AMX *amx, cell *params)
{
	return amx_ftoc(pNetGame->m_fGravity);
}

// ============================
// Start of server-wide object code
// ============================

static cell AMX_NATIVE_CALL n_CreateObject(AMX *amx, cell *params)
{
	CHECK_PARAMS(7);
	CObjectPool*	pObjectPool = pNetGame->GetObjectPool();
	VECTOR vecPos, vecRot;

	vecPos.X = amx_ctof(params[2]);
	vecPos.Y = amx_ctof(params[3]);
	vecPos.Z = amx_ctof(params[4]);

	vecRot.X = amx_ctof(params[5]);
	vecRot.Y = amx_ctof(params[6]);
	vecRot.Z = amx_ctof(params[7]);
	
	BYTE byteObjectID = pObjectPool->New((int)params[1], &vecPos, &vecRot);

	if (byteObjectID != 0xFF)
	{
		CObject* pObject = pNetGame->GetObjectPool()->GetAt(byteObjectID);
		CPlayerPool* pPlayerPool = pNetGame->GetPlayerPool();
		for(int x = 0; x < MAX_PLAYERS;x++) 
		{	
			if (pPlayerPool->GetSlotState(x))
			{
				pObject->SpawnForPlayer(x); // Done 100 times, may as well speed up with pointers.
			}
		}
	}
	return byteObjectID;
}

static cell AMX_NATIVE_CALL n_SetObjectPos(AMX *amx, cell *params)
{
	CHECK_PARAMS(4);

	VECTOR vecPos;
	vecPos.X = amx_ctof(params[2]);
	vecPos.Y = amx_ctof(params[3]);
	vecPos.Z = amx_ctof(params[4]);

	RakNet::BitStream bsParams;
	bsParams.Write((BYTE)params[1]); // byteObjectID
	bsParams.Write(vecPos.X);	// X
	bsParams.Write(vecPos.Y);	// Y
	bsParams.Write(vecPos.Z);	// Z

	RakServerInterface* pRak = pNetGame->GetRakServer();
	pRak->RPC(RPC_ScrSetObjectPos , &bsParams, HIGH_PRIORITY, RELIABLE, 0, UNASSIGNED_PLAYER_ID, true, false);
	CObjectPool *pObjectPool = pNetGame->GetObjectPool();
	CObject*	pObject = pObjectPool->GetAt((BYTE)params[1]);
	pObject->m_matWorld.pos.X = vecPos.X;
	pObject->m_matWorld.pos.Y = vecPos.Y;
	pObject->m_matWorld.pos.Z = vecPos.Z;
	return 1;
}

static cell AMX_NATIVE_CALL n_GetObjectPos(AMX *amx, cell *params)
{
	CHECK_PARAMS(4);
	CObjectPool*	pObjectPool = pNetGame->GetObjectPool();
	CObject*		pObject		= pObjectPool->GetAt((BYTE)params[1]);

	if (pObject)
	{
		cell* cptr;
		amx_GetAddr(amx, params[2], &cptr);
		*cptr = amx_ftoc(pObject->m_matWorld.pos.X);
		amx_GetAddr(amx, params[3], &cptr);
		*cptr = amx_ftoc(pObject->m_matWorld.pos.Y);
		amx_GetAddr(amx, params[4], &cptr);
		*cptr = amx_ftoc(pObject->m_matWorld.pos.Z);

		return 1;
	}

	return 0;
}

static cell AMX_NATIVE_CALL n_SetObjectRot(AMX *amx, cell *params)
{
	CHECK_PARAMS(4);

	VECTOR vecRot;
	vecRot.X = amx_ctof(params[2]);
	vecRot.Y = amx_ctof(params[3]);
	vecRot.Z = amx_ctof(params[4]);

	RakNet::BitStream bsParams;
	bsParams.Write((BYTE)params[1]); // byteObjectID
	bsParams.Write(vecRot.X);	// X
	bsParams.Write(vecRot.Y);	// Y
	bsParams.Write(vecRot.Z);	// Z

	RakServerInterface* pRak = pNetGame->GetRakServer();
	pRak->RPC(RPC_ScrSetObjectRotation , &bsParams, HIGH_PRIORITY, RELIABLE, 0, UNASSIGNED_PLAYER_ID, true, false);

	CObjectPool *pObjectPool = pNetGame->GetObjectPool();
	CObject*	pObject = pObjectPool->GetAt((BYTE)params[1]);
	pObject->m_matWorld.up.X = vecRot.X;
	pObject->m_matWorld.up.Y = vecRot.Y;
	pObject->m_matWorld.up.Z = vecRot.Z;

	return 1;
}

static cell AMX_NATIVE_CALL n_GetObjectRot(AMX *amx, cell *params)
{
	CHECK_PARAMS(4);
	CObjectPool*	pObjectPool = pNetGame->GetObjectPool();
	CObject*		pObject		= pObjectPool->GetAt((BYTE)params[1]);

	if (pObject)
	{
		cell* cptr;
		amx_GetAddr(amx, params[2], &cptr);
		*cptr = amx_ftoc(pObject->m_matWorld.up.X);
		amx_GetAddr(amx, params[3], &cptr);
		*cptr = amx_ftoc(pObject->m_matWorld.up.Y);
		amx_GetAddr(amx, params[4], &cptr);
		*cptr = amx_ftoc(pObject->m_matWorld.up.Z);
		return 1;
	}
	return 0;
}

static cell AMX_NATIVE_CALL n_IsValidObject(AMX *amx, cell *params)
{
	CHECK_PARAMS(1);
	CObjectPool*	pObjectPool = pNetGame->GetObjectPool();

	if (pObjectPool->GetAt((BYTE)params[1]))
	{
		return 1;
	}

	return 0;
}

static cell AMX_NATIVE_CALL n_DestroyObject(AMX *amx, cell *params)
{
	CHECK_PARAMS(1);
	CObjectPool*	pObjectPool = pNetGame->GetObjectPool();

	if (pObjectPool->GetAt((BYTE)params[1]))
	{
		pObjectPool->Delete((BYTE)params[1]);
		RakNet::BitStream bsParams;

		bsParams.Write((BYTE)params[1]);

		pNetGame->GetRakServer()->RPC(RPC_ScrDestroyObject , &bsParams, HIGH_PRIORITY, RELIABLE, 0, UNASSIGNED_PLAYER_ID, true, false);
	}

	return 1;
}

static cell AMX_NATIVE_CALL n_MoveObject(AMX *amx, cell *params)
{
	CHECK_PARAMS(5);
	CObjectPool*	pObjectPool = pNetGame->GetObjectPool();
	BYTE byteObject = (BYTE)params[1];
	CObject* pObject = pObjectPool->GetAt(byteObject);
	float ret = 0.0f;

	if (pObject)
	{
		RakNet::BitStream bsParams;

		float x = amx_ctof(params[2]);
		float y = amx_ctof(params[3]);
		float z = amx_ctof(params[4]);
		float s = amx_ctof(params[5]);
		bsParams.Write(byteObject);
		bsParams.Write(pObject->m_matWorld.pos.X);
		bsParams.Write(pObject->m_matWorld.pos.Y);
		bsParams.Write(pObject->m_matWorld.pos.Z);
		// It may have been moved before, make sure all players are up to date
		bsParams.Write(x);
		bsParams.Write(y);
		bsParams.Write(z);
		bsParams.Write(s);

		pNetGame->GetRakServer()->RPC(RPC_ScrMoveObject , &bsParams, HIGH_PRIORITY, RELIABLE, 0, UNASSIGNED_PLAYER_ID, true, false);
		//return *amx_ftoc(pObject->MoveTo(x, y, z, s));
		ret = pObject->MoveTo(x, y, z, s);
	}
	return (int)(ret * 1000.0f);
	//return *amx_ftoc(0.0f);
}

static cell AMX_NATIVE_CALL n_StopObject(AMX *amx, cell *params)
{
	CHECK_PARAMS(1);
	CObjectPool*	pObjectPool = pNetGame->GetObjectPool();
	BYTE byteObject = (BYTE)params[1];
	CObject* pObject = pObjectPool->GetAt(byteObject);
	
	if (pObject)
	{
		RakNet::BitStream bsParams;

		pObject->Stop();
		bsParams.Write(byteObject);
		bsParams.Write(pObject->m_matWorld.pos.X);
		bsParams.Write(pObject->m_matWorld.pos.Y);
		bsParams.Write(pObject->m_matWorld.pos.Z);
		// Make sure it stops for the player where the server thinks it is

		pNetGame->GetRakServer()->RPC(RPC_ScrStopObject , &bsParams, HIGH_PRIORITY, RELIABLE, 0, UNASSIGNED_PLAYER_ID, true, false);
		//return *amx_ftoc(pObject->MoveTo(x, y, z, s));
		return 1;
	}
	return 0;
}

// =======================
// Start of player object code
// =======================

static cell AMX_NATIVE_CALL n_CreatePlayerObject(AMX *amx, cell *params)
{
	CHECK_PARAMS(8);
	if (!pNetGame->GetPlayerPool()->GetSlotState((BYTE)params[1])) return 0;
	CObjectPool*	pObjectPool = pNetGame->GetObjectPool();
	VECTOR vecPos, vecRot;

	vecPos.X = amx_ctof(params[3]);
	vecPos.Y = amx_ctof(params[4]);
	vecPos.Z = amx_ctof(params[5]);

	vecRot.X = amx_ctof(params[6]);
	vecRot.Y = amx_ctof(params[7]);
	vecRot.Z = amx_ctof(params[8]);
	
	BYTE byteObjectID = pObjectPool->New((int)params[1], (int)params[2], &vecPos, &vecRot);

	if (byteObjectID != 0xFF)
	{
		pNetGame->GetObjectPool()->GetAtIndividual((BYTE)params[1], byteObjectID)->SpawnForPlayer((BYTE)params[1]);
	}
	return byteObjectID;
}

static cell AMX_NATIVE_CALL n_SetPlayerObjectPos(AMX *amx, cell *params)
{
	CHECK_PARAMS(5);

	if (!pNetGame->GetPlayerPool()->GetSlotState((BYTE)params[1]) ||
		!pNetGame->GetObjectPool()->GetPlayerSlotState((BYTE)params[1], (BYTE)params[2])) return 0;
	VECTOR vecPos;
	vecPos.X = amx_ctof(params[3]);
	vecPos.Y = amx_ctof(params[4]);
	vecPos.Z = amx_ctof(params[5]);

	RakNet::BitStream bsParams;
	bsParams.Write((BYTE)params[2]); // byteObjectID
	bsParams.Write(vecPos.X);	// X
	bsParams.Write(vecPos.Y);	// Y
	bsParams.Write(vecPos.Z);	// Z

	RakServerInterface* pRak = pNetGame->GetRakServer();
	pRak->RPC(RPC_ScrSetObjectPos , &bsParams, HIGH_PRIORITY, RELIABLE, 0, pRak->GetPlayerIDFromIndex(params[1]), false, false);
	
	CObject*	pObject = pNetGame->GetObjectPool()->GetAtIndividual((BYTE)params[1], (BYTE)params[2]);
	pObject->m_matWorld.pos.X = vecPos.X;
	pObject->m_matWorld.pos.Y = vecPos.Y;
	pObject->m_matWorld.pos.Z = vecPos.Z;
	
	return 1;
}

static cell AMX_NATIVE_CALL n_GetPlayerObjectPos(AMX *amx, cell *params)
{
	CHECK_PARAMS(5);
	if (!pNetGame->GetPlayerPool()->GetSlotState((BYTE)params[1])) return 0;
	CObject* pObject = pNetGame->GetObjectPool()->GetAtIndividual((BYTE)params[1], (BYTE)params[2]);

	if (pObject)
	{
		cell* cptr;
		amx_GetAddr(amx, params[3], &cptr);
		*cptr = amx_ftoc(pObject->m_matWorld.pos.X);
		amx_GetAddr(amx, params[4], &cptr);
		*cptr = amx_ftoc(pObject->m_matWorld.pos.Y);
		amx_GetAddr(amx, params[5], &cptr);
		*cptr = amx_ftoc(pObject->m_matWorld.pos.Z);

		return 1;
	}

	return 0;
}

static cell AMX_NATIVE_CALL n_SetPlayerObjectRot(AMX *amx, cell *params)
{
	CHECK_PARAMS(5);

	if (!pNetGame->GetPlayerPool()->GetSlotState((BYTE)params[1]) ||
		!pNetGame->GetObjectPool()->GetPlayerSlotState((BYTE)params[1], (BYTE)params[2])) return 0;
	VECTOR vecRot;
	vecRot.X = amx_ctof(params[3]);
	vecRot.Y = amx_ctof(params[4]);
	vecRot.Z = amx_ctof(params[5]);

	RakNet::BitStream bsParams;
	bsParams.Write((BYTE)params[2]); // byteObjectID
	bsParams.Write(vecRot.X);	// X
	bsParams.Write(vecRot.Y);	// Y
	bsParams.Write(vecRot.Z);	// Z

	RakServerInterface* pRak = pNetGame->GetRakServer();
	pRak->RPC(RPC_ScrSetObjectRotation , &bsParams, HIGH_PRIORITY, RELIABLE, 0, pRak->GetPlayerIDFromIndex(params[1]), true, false);
	//printf("rotation sent");

	CObject*	pObject = pNetGame->GetObjectPool()->GetAtIndividual((BYTE)params[1], (BYTE)params[2]);
	pObject->m_matWorld.up.X = vecRot.X;
	pObject->m_matWorld.up.Y = vecRot.Y;
	pObject->m_matWorld.up.Z = vecRot.Z;

	return 1;
}

static cell AMX_NATIVE_CALL n_GetPlayerObjectRot(AMX *amx, cell *params)
{
	CHECK_PARAMS(5);
	if (!pNetGame->GetPlayerPool()->GetSlotState((BYTE)params[1])) return 0;
	CObject* pObject = pNetGame->GetObjectPool()->GetAtIndividual((BYTE)params[1], (BYTE)params[2]);

	if (pObject)
	{
		cell* cptr;
		amx_GetAddr(amx, params[3], &cptr);
		*cptr = amx_ftoc(pObject->m_matWorld.up.X);
		amx_GetAddr(amx, params[4], &cptr);
		*cptr = amx_ftoc(pObject->m_matWorld.up.Y);
		amx_GetAddr(amx, params[5], &cptr);
		*cptr = amx_ftoc(pObject->m_matWorld.up.Z);
		return 1;
	}
	return 0;
}

static cell AMX_NATIVE_CALL n_IsValidPlayerObject(AMX *amx, cell *params)
{

	CHECK_PARAMS(2);
	if (!pNetGame->GetPlayerPool()->GetSlotState((BYTE)params[1])) return 0;

	if (pNetGame->GetObjectPool()->GetAtIndividual((BYTE)params[1], (BYTE)params[2]))
	{
		return 1;
	}

	return 0;
}

static cell AMX_NATIVE_CALL n_DestroyPlayerObject(AMX *amx, cell *params)
{
	CHECK_PARAMS(2);
	if (!pNetGame->GetPlayerPool()->GetSlotState((BYTE)params[1])) return 0;
	CObjectPool*	pObjectPool = pNetGame->GetObjectPool();

	if (pObjectPool->GetAtIndividual((BYTE)params[1], (BYTE)params[2]) && pObjectPool->DeleteForPlayer((BYTE)params[1], (BYTE)params[2]))
	{
		RakNet::BitStream bsParams;

		bsParams.Write((BYTE)params[2]);

		RakServerInterface* pRak = pNetGame->GetRakServer();
		pRak->RPC(RPC_ScrDestroyObject , &bsParams, HIGH_PRIORITY, RELIABLE, 0, pRak->GetPlayerIDFromIndex(params[1]), false, false);
	}

	return 1;
}

static cell AMX_NATIVE_CALL n_MovePlayerObject(AMX *amx, cell *params)
{
	CHECK_PARAMS(6);
	if (!pNetGame->GetPlayerPool()->GetSlotState((BYTE)params[1])) return 0;
	CObjectPool*	pObjectPool = pNetGame->GetObjectPool();
	BYTE bytePlayer = (BYTE)params[1];
	BYTE byteObject = (BYTE)params[2];
	CObject* pObject = pObjectPool->GetAtIndividual(bytePlayer, byteObject);
	float ret = 0.0f;
	
	if (pObject)
	{
		RakNet::BitStream bsParams;

		float x = amx_ctof(params[3]);
		float y = amx_ctof(params[4]);
		float z = amx_ctof(params[5]);
		float s = amx_ctof(params[6]);
		bsParams.Write(byteObject);
		bsParams.Write(pObject->m_matWorld.pos.X);
		bsParams.Write(pObject->m_matWorld.pos.Y);
		bsParams.Write(pObject->m_matWorld.pos.Z);
		// It may have been moved before, make sure all players are up to date
		bsParams.Write(x);
		bsParams.Write(y);
		bsParams.Write(z);
		bsParams.Write(s);

		RakServerInterface* pRak = pNetGame->GetRakServer();
		pRak->RPC(RPC_ScrMoveObject , &bsParams, HIGH_PRIORITY, RELIABLE, 0, pRak->GetPlayerIDFromIndex(bytePlayer), false, false);
		//return *amx_ftoc(pObject->MoveTo(x, y, z, s));
		ret = pObject->MoveTo(x, y, z, s);
	}
	return (int)(ret * 1000.0f);
}

static cell AMX_NATIVE_CALL n_StopPlayerObject(AMX *amx, cell *params)
{
	CHECK_PARAMS(2);
	if (!pNetGame->GetPlayerPool()->GetSlotState((BYTE)params[1])) return 0;
	CObjectPool*	pObjectPool = pNetGame->GetObjectPool();
	BYTE bytePlayer = (BYTE)params[1];
	BYTE byteObject = (BYTE)params[2];
	CObject* pObject = pObjectPool->GetAtIndividual(bytePlayer, byteObject);

	if (pObject)
	{
		RakNet::BitStream bsParams;

		pObject->Stop();
		bsParams.Write(byteObject);
		bsParams.Write(pObject->m_matWorld.pos.X);
		bsParams.Write(pObject->m_matWorld.pos.Y);
		bsParams.Write(pObject->m_matWorld.pos.Z);
		// Make sure it stops for the player where the server thinks it is

		RakServerInterface* pRak = pNetGame->GetRakServer();
		pRak->RPC(RPC_ScrStopObject , &bsParams, HIGH_PRIORITY, RELIABLE, 0, pRak->GetPlayerIDFromIndex(bytePlayer), false, false);
		//return *amx_ftoc(pObject->MoveTo(x, y, z, s));
		return 1;
	}
	return 0;
}

// Menus

// native Menu:CreateMenu(title[], columns, Float:X, Float:Y, Float:column1width, Float:column2width = 0.0);
static cell AMX_NATIVE_CALL n_CreateMenu(AMX *amx, cell *params)
{
	CHECK_PARAMS(6);
	CMenuPool* pMenuPool = pNetGame->GetMenuPool();
	if (!pMenuPool) return -1;
	char* szMenuTitle;
	amx_StrParam(amx, params[1], szMenuTitle);
	BYTE menuid = pMenuPool->New(szMenuTitle, amx_ctof(params[3]), amx_ctof(params[4]), params[2], amx_ctof(params[5]), amx_ctof(params[6]));
	if (menuid == 0xFF) return -1;
	return menuid;
}

// native DestroyMenu(Menu:menuid);
static cell AMX_NATIVE_CALL n_DestroyMenu(AMX *amx, cell *params)
{
	CHECK_PARAMS(1);
	CMenuPool* pMenuPool = pNetGame->GetMenuPool();
	if (!pMenuPool) return 0;
	if (pMenuPool->Delete(params[1])) return 1;
	return 0;
}

// native AddMenuItem(Menu:menuid, column, item[]);
static cell AMX_NATIVE_CALL n_AddMenuItem(AMX *amx, cell *params)
{
	CHECK_PARAMS(3);
	CMenuPool* pMenuPool = pNetGame->GetMenuPool();
	if (!pMenuPool) return 0;
	char* szItemText;
	amx_StrParam(amx, params[3], szItemText);
	BYTE ret = pMenuPool->GetAt((BYTE)params[1])->AddMenuItem(params[2], szItemText);
	if (ret == 0xFF) return -1;
	return ret;
}

// native SetMenuColumnHeader(Menu:menuid, column, header[]);
static cell AMX_NATIVE_CALL n_SetMenuColumnHeader(AMX *amx, cell *params)
{
	CHECK_PARAMS(3);
	CMenuPool* pMenuPool = pNetGame->GetMenuPool();
	if (!pMenuPool) return 0;
	char* szItemText;
	amx_StrParam(amx, params[3], szItemText);
	pMenuPool->GetAt((BYTE)params[1])->SetColumnTitle(params[2], szItemText);
	return 1;
}

// native ShowMenuForPlayer(Menu:menuid, playerid);
static cell AMX_NATIVE_CALL n_ShowMenuForPlayer(AMX *amx, cell *params)
{
	CHECK_PARAMS(2);
	if (!pNetGame->GetPlayerPool()->GetSlotState((BYTE)params[2])) return 0;
	CMenuPool* pMenuPool = pNetGame->GetMenuPool();
	if (!pMenuPool) return 0;
	pMenuPool->GetAt((BYTE)params[1])->ShowForPlayer((BYTE)params[2]);
	pMenuPool->SetPlayerMenu((BYTE)params[2], (BYTE)params[1]);
	return 1;
}

// native HideMenuForPlayer(Menu:menuid, playerid);
static cell AMX_NATIVE_CALL n_HideMenuForPlayer(AMX *amx, cell *params)
{
	CHECK_PARAMS(2);
	if (!pNetGame->GetPlayerPool()->GetSlotState((BYTE)params[2])) return 0;
	CMenuPool* pMenuPool = pNetGame->GetMenuPool();
	if (!pMenuPool) return 0;
	pMenuPool->GetAt((BYTE)params[1])->HideForPlayer((BYTE)params[2]);
	pMenuPool->SetPlayerMenu((BYTE)params[2], 255);
	return 1;
}

// native IsValidMenu(Menu:menuid);
static cell AMX_NATIVE_CALL n_IsValidMenu(AMX *amx, cell *params)
{
	CHECK_PARAMS(1);
	CMenuPool* pMenuPool = pNetGame->GetMenuPool();
	if (pMenuPool && pMenuPool->GetSlotState(params[1])) return 1;
	return 0;
}

// native DisableMenu(Menu:menuid);
static cell AMX_NATIVE_CALL n_DisableMenu(AMX *amx, cell *params)
{
	CHECK_PARAMS(1);
	CMenuPool* pMenuPool = pNetGame->GetMenuPool();
	if (!pMenuPool) return 0;
	pMenuPool->GetAt((BYTE)params[1])->DisableInteraction();
	return 1;
}

// native DisableMenuRow(Menu:menuid, row);
static cell AMX_NATIVE_CALL n_DisableMenuRow(AMX *amx, cell *params)
{
	CHECK_PARAMS(2);
	CMenuPool* pMenuPool = pNetGame->GetMenuPool();
	if (!pMenuPool) return 0;
	pMenuPool->GetAt((BYTE)params[1])->DisableRow((BYTE)params[2]);
	return 1;
}

// native Menu:GetPlayerMenu(playerid);
static cell AMX_NATIVE_CALL n_GetPlayerMenu(AMX *amx, cell *params)
{
	CHECK_PARAMS(1);
	CMenuPool* pMenuPool = pNetGame->GetMenuPool();
	if (!pMenuPool) return 255;
	return pMenuPool->GetPlayerMenu((BYTE)params[1]);
}

static cell AMX_NATIVE_CALL n_CreateExplosion(AMX *amx, cell *params)
{
	CHECK_PARAMS(5);

	RakNet::BitStream bsParams;

	bsParams.Write(params[1]);
	bsParams.Write(params[2]);
	bsParams.Write(params[3]);
	bsParams.Write(params[4]);
	bsParams.Write(params[5]);

	pNetGame->GetRakServer()->RPC(RPC_ScrCreateExplosion , &bsParams, HIGH_PRIORITY, RELIABLE, 0, UNASSIGNED_PLAYER_ID, true, false);

	return 1;
}

static cell AMX_NATIVE_CALL n_SetDisabledWeapons(AMX *amx, cell *params)
{
	long long* lpWeapons = &pNetGame->m_longSynchedWeapons;
	*lpWeapons = DEFAULT_WEAPONS;
	int numweaps = params[0] / sizeof (cell);
	while (numweaps)
	{
		int val = *get_amxaddr(amx, params[numweaps]);
		if (val < 47)
		{
			*lpWeapons &= (long long)~(long long)(((long long)1) << val);
		}
		numweaps--;
	}
	return 1;
}

// native EnableZoneNames(enable);
static cell AMX_NATIVE_CALL n_EnableZoneNames(AMX *amx, cell *params)
{
	CHECK_PARAMS(1);
	pNetGame->m_bZoneNames = (bool)params[1];
	return 1;
}

// native AttachObjectToPlayer( objectid, playerid, Float:OffsetX, Float:OffsetY, Float:OffsetZ, Float:rX, Float:rY, Float:rZ )
static cell AMX_NATIVE_CALL n_AttachObjectToPlayer( AMX *amx, cell *params )
{
	CHECK_PARAMS(8);

	if ( pNetGame->GetObjectPool()->GetAt( (BYTE)params[1] ) && 
		 pNetGame->GetPlayerPool()->GetAt( (BYTE)params[2] ) )
	{
		RakNet::BitStream bsParams;

		bsParams.Write((BYTE)params[1]);
		bsParams.Write((BYTE)params[2]);

		VECTOR vecOffsets, vecRotations;
		vecOffsets.X = amx_ctof(params[3]);
		vecOffsets.Y = amx_ctof(params[4]);
		vecOffsets.Z = amx_ctof(params[5]);

		vecRotations.X = amx_ctof(params[6]);
		vecRotations.Y = amx_ctof(params[7]);
		vecRotations.Z = amx_ctof(params[8]);

		bsParams.Write(vecOffsets.X);
		bsParams.Write(vecOffsets.Y);
		bsParams.Write(vecOffsets.Z);

		bsParams.Write(vecRotations.X);
		bsParams.Write(vecRotations.Y);
		bsParams.Write(vecRotations.Z);

		pNetGame->GetRakServer()->RPC( RPC_ScrAttachObjectToPlayer, &bsParams, HIGH_PRIORITY, RELIABLE, 0, UNASSIGNED_PLAYER_ID, true, false);
	}

	return 0;
}

//----------------------------------------------------------------------------------

//native AttachPlayerObjectToPlayer(objectplayer, objectid, attachplayer, Float:OffsetX, Float:OffsetY, Float:OffsetZ, Float:rX, Float:rY, Float:rZ);
static cell AMX_NATIVE_CALL n_AttachPlayerObjectToPlayer(AMX *amx, cell *params)
{
	CHECK_PARAMS(9);

	if (pNetGame->GetPlayerPool()->GetAt((BYTE)params[1]) &&
		pNetGame->GetObjectPool()->GetAtIndividual((BYTE)params[1], (BYTE)params[2]) && 
		pNetGame->GetPlayerPool()->GetAt((BYTE)params[3]))
	{
		RakNet::BitStream bsParams;

		bsParams.Write((BYTE)params[2]);
		bsParams.Write((BYTE)params[3]);

		VECTOR vecOffsets, vecRotations;
		vecOffsets.X = amx_ctof(params[4]);
		vecOffsets.Y = amx_ctof(params[5]);
		vecOffsets.Z = amx_ctof(params[6]);

		vecRotations.X = amx_ctof(params[7]);
		vecRotations.Y = amx_ctof(params[8]);
		vecRotations.Z = amx_ctof(params[9]);

		bsParams.Write(vecOffsets.X);
		bsParams.Write(vecOffsets.Y);
		bsParams.Write(vecOffsets.Z);

		bsParams.Write(vecRotations.X);
		bsParams.Write(vecRotations.Y);
		bsParams.Write(vecRotations.Z);

		RakServerInterface* pRak = pNetGame->GetRakServer();
		pRak->RPC(RPC_ScrAttachObjectToPlayer, &bsParams, HIGH_PRIORITY, RELIABLE, 0, pRak->GetPlayerIDFromIndex((BYTE)params[1]), false, false);
	}

	return 0;
}

//----------------------------------------------------------------------------------

static cell AMX_NATIVE_CALL n_SetPlayerWantedLevel(AMX *amx, cell *params)
{
	CHECK_PARAMS(2);
	CPlayer* pPlayer = pNetGame->GetPlayerPool()->GetAt((BYTE)params[1]);
	if (pPlayer)
	{
		pPlayer->SetWantedLevel((BYTE)params[2]);
		RakNet::BitStream bsParams;
		bsParams.Write((BYTE)params[2]);
		RakServerInterface* pRak = pNetGame->GetRakServer();
		pRak->RPC(RPC_ScrSetPlayerWantedLevel, &bsParams, HIGH_PRIORITY, RELIABLE, 0, pRak->GetPlayerIDFromIndex((BYTE)params[1]), false, false);
		return 1;
	}
	return 0;
}

static cell AMX_NATIVE_CALL n_GetPlayerWantedLevel(AMX *amx, cell *params)
{
	CHECK_PARAMS(1);
	CPlayer* pPlayer = pNetGame->GetPlayerPool()->GetAt((BYTE)params[1]);
	if (pPlayer) return pPlayer->GetWantedLevel();
	return 0;
}

//----------------------------------------------------------------------------------

static cell AMX_NATIVE_CALL n_TextDrawCreate(AMX *amx, cell *params)
{
	CHECK_PARAMS(3);
	CTextDrawPool* pTextDraw = pNetGame->GetTextDrawPool();
	if (pTextDraw)
	{
		char* szText;
		amx_StrParam(amx, params[3], szText);
		return pTextDraw->New(amx_ctof(params[1]), amx_ctof(params[2]), szText);
	}
	return 0xFFFF;
}

static cell AMX_NATIVE_CALL n_TextDrawSetString(AMX *amx, cell *params)
{
	CHECK_PARAMS(2);
	CTextDrawPool* pTextDraw = pNetGame->GetTextDrawPool();
	if (pTextDraw && pTextDraw->GetSlotState(params[1]))
	{
		char* szText;
		amx_StrParam(amx, params[2], szText);
		pTextDraw->SetTextString(params[1], szText);
		return 1;
	}
	return 0;
}

static cell AMX_NATIVE_CALL n_TextDrawLetterSize(AMX *amx, cell *params)
{
	CHECK_PARAMS(3);
	CTextDrawPool* pTextDraw = pNetGame->GetTextDrawPool();
	if (pTextDraw && pTextDraw->GetSlotState(params[1]))
	{
		pTextDraw->SetLetterSize(params[1], amx_ctof(params[2]), amx_ctof(params[3]));
		return 1;
	}
	return 0;
}

static cell AMX_NATIVE_CALL n_TextDrawTextSize(AMX *amx, cell *params)
{
	CHECK_PARAMS(3);
	CTextDrawPool* pTextDraw = pNetGame->GetTextDrawPool();
	if (pTextDraw && pTextDraw->GetSlotState(params[1]))
	{
		pTextDraw->SetTextSize(params[1], amx_ctof(params[2]), amx_ctof(params[3]));
		return 1;
	}
	return 0;
}

static cell AMX_NATIVE_CALL n_TextDrawAlignment(AMX *amx, cell *params)
{
	CHECK_PARAMS(2);
	CTextDrawPool* pTextDraw = pNetGame->GetTextDrawPool();
	if (pTextDraw && pTextDraw->GetSlotState(params[1]))
	{
		pTextDraw->SetAlignment(params[1], params[2]);
		return 1;
	}
	return 0;
}

static cell AMX_NATIVE_CALL n_TextDrawColor(AMX *amx, cell *params)
{
	CHECK_PARAMS(2);
	CTextDrawPool* pTextDraw = pNetGame->GetTextDrawPool();
	if (pTextDraw && pTextDraw->GetSlotState(params[1]))
	{
		pTextDraw->SetColor(params[1], params[2]);
		return 1;
	}
	return 0;
}

static cell AMX_NATIVE_CALL n_TextDrawUseBox(AMX *amx, cell *params)
{
	CHECK_PARAMS(2);
	CTextDrawPool* pTextDraw = pNetGame->GetTextDrawPool();
	if (pTextDraw && pTextDraw->GetSlotState(params[1]))
	{
		pTextDraw->SetUseBox(params[1], params[2]);
		return 1;
	}
	return 0;
}

static cell AMX_NATIVE_CALL n_TextDrawBoxColor(AMX *amx, cell *params)
{
	CHECK_PARAMS(2);
	CTextDrawPool* pTextDraw = pNetGame->GetTextDrawPool();
	if (pTextDraw && pTextDraw->GetSlotState(params[1]))
	{
		pTextDraw->SetBoxColor(params[1], params[2]);
		return 1;
	}
	return 0;
}

static cell AMX_NATIVE_CALL n_TextDrawSetShadow(AMX *amx, cell *params)
{
	CHECK_PARAMS(2);
	CTextDrawPool* pTextDraw = pNetGame->GetTextDrawPool();
	if (pTextDraw && pTextDraw->GetSlotState(params[1]))
	{
		pTextDraw->SetShadow(params[1], params[2]);
		return 1;
	}
	return 0;
}

static cell AMX_NATIVE_CALL n_TextDrawSetOutline(AMX *amx, cell *params)
{
	CHECK_PARAMS(2);
	CTextDrawPool* pTextDraw = pNetGame->GetTextDrawPool();
	if (pTextDraw && pTextDraw->GetSlotState(params[1]))
	{
		pTextDraw->SetOutline(params[1], params[2]);
		return 1;
	}
	return 0;
}

static cell AMX_NATIVE_CALL n_TextDrawBackgroundColor(AMX *amx, cell *params)
{
	CHECK_PARAMS(2);
	CTextDrawPool* pTextDraw = pNetGame->GetTextDrawPool();
	if (pTextDraw && pTextDraw->GetSlotState(params[1]))
	{
		pTextDraw->SetBackgroundColor(params[1], params[2]);
		return 1;
	}
	return 0;
}

static cell AMX_NATIVE_CALL n_TextDrawFont(AMX *amx, cell *params)
{
	CHECK_PARAMS(2);
	CTextDrawPool* pTextDraw = pNetGame->GetTextDrawPool();
	if (pTextDraw && pTextDraw->GetSlotState(params[1]))
	{
		pTextDraw->SetFont(params[1], params[2]);
		return 1;
	}
	return 0;
}

static cell AMX_NATIVE_CALL n_TextDrawSetProportional(AMX *amx, cell *params)
{
	CHECK_PARAMS(2);
	CTextDrawPool* pTextDraw = pNetGame->GetTextDrawPool();
	if (pTextDraw && pTextDraw->GetSlotState(params[1]))
	{
		pTextDraw->SetProportional(params[1], params[2]);
		return 1;
	}
	return 0;
}

static cell AMX_NATIVE_CALL n_TextDrawShowForPlayer(AMX *amx, cell *params)
{
	CHECK_PARAMS(2);
	if (!pNetGame->GetPlayerPool()->GetSlotState(params[1])) return 0;
	CTextDrawPool* pTextDraw = pNetGame->GetTextDrawPool();
	if (pTextDraw && pTextDraw->GetSlotState(params[2]))
	{
		pTextDraw->ShowForPlayer(params[1], params[2]);
		return 1;
	}
	return 0;
}

static cell AMX_NATIVE_CALL n_TextDrawHideForPlayer(AMX *amx, cell *params)
{
	CHECK_PARAMS(2);
	if (!pNetGame->GetPlayerPool()->GetSlotState(params[1])) return 0;
	CTextDrawPool* pTextDraw = pNetGame->GetTextDrawPool();
	if (pTextDraw->GetSlotState(params[2]))
	{
		pTextDraw->HideForPlayer(params[1], params[2]);
		return 1;
	}
	return 0;
}

static cell AMX_NATIVE_CALL n_TextDrawShowForAll(AMX *amx, cell *params)
{
	CHECK_PARAMS(1);
	CTextDrawPool* pTextDraw = pNetGame->GetTextDrawPool();
	if (pTextDraw && pTextDraw->GetSlotState(params[1]))
	{
		pTextDraw->ShowForAll(params[1]);
		return 1;
	}
	return 0;
}

static cell AMX_NATIVE_CALL n_TextDrawHideForAll(AMX *amx, cell *params)
{
	CHECK_PARAMS(1);
	CTextDrawPool* pTextDraw = pNetGame->GetTextDrawPool();
	if (pTextDraw->GetSlotState(params[1]))
	{
		pTextDraw->HideForAll(params[1]);
		return 1;
	}
	return 0;
}

static cell AMX_NATIVE_CALL n_TextDrawDestroy(AMX *amx, cell *params)
{
	CHECK_PARAMS(1);
	if (!pNetGame->GetTextDrawPool()->GetSlotState(params[1])) return 0;
	pNetGame->GetTextDrawPool()->Delete(params[1]);
	return 1;
}

//----------------------------------------------------------------------------------

static cell AMX_NATIVE_CALL n_GangZoneCreate(AMX *amx, cell *params)
{
	CHECK_PARAMS(4);
	CGangZonePool *pGangZonePool = pNetGame->GetGangZonePool();
	if (!pGangZonePool) return -1;
	WORD ret = pGangZonePool->New(amx_ctof(params[1]), amx_ctof(params[2]), amx_ctof(params[3]), amx_ctof(params[4]));
	if (ret == 0xFFFF) return -1;
	return ret;
}

static cell AMX_NATIVE_CALL n_GangZoneDestroy(AMX *amx, cell *params)
{
	CHECK_PARAMS(1);
	CGangZonePool *pGangZonePool = pNetGame->GetGangZonePool();
	if (!pGangZonePool || !pGangZonePool->GetSlotState(params[1])) return 0;
	pGangZonePool->Delete(params[1]);
	return 1;
}

static cell AMX_NATIVE_CALL n_GangZoneShowForPlayer(AMX *amx, cell *params)
{
	CHECK_PARAMS(3);
	if (!pNetGame->GetPlayerPool()->GetSlotState(params[1])) return 0;
	CGangZonePool *pGangZonePool = pNetGame->GetGangZonePool();
	if (!pGangZonePool || !pGangZonePool->GetSlotState(params[2])) return 0;
	pGangZonePool->ShowForPlayer(params[1], params[2], params[3]);
	return 1;
}

static cell AMX_NATIVE_CALL n_GangZoneShowForAll(AMX *amx, cell *params)
{
	CHECK_PARAMS(2);
	CGangZonePool *pGangZonePool = pNetGame->GetGangZonePool();
	if (!pGangZonePool || !pGangZonePool->GetSlotState(params[1])) return 0;
	pGangZonePool->ShowForAll(params[1], params[2]);
	return 1;
}

static cell AMX_NATIVE_CALL n_GangZoneHideForPlayer(AMX *amx, cell *params)
{
	CHECK_PARAMS(2);
	if (!pNetGame->GetPlayerPool()->GetSlotState(params[1])) return 0;
	CGangZonePool *pGangZonePool = pNetGame->GetGangZonePool();
	if (!pGangZonePool || !pGangZonePool->GetSlotState(params[2])) return 0;
	pGangZonePool->HideForPlayer(params[1], params[2]);
	return 1;
}

static cell AMX_NATIVE_CALL n_GangZoneHideForAll(AMX *amx, cell *params)
{
	CHECK_PARAMS(1);
	CGangZonePool *pGangZonePool = pNetGame->GetGangZonePool();
	if (!pGangZonePool || !pGangZonePool->GetSlotState(params[1])) return 0;
	pGangZonePool->HideForAll(params[1]);
	return 1;
}

static cell AMX_NATIVE_CALL n_GangZoneFlashForPlayer(AMX *amx, cell *params)
{
	CHECK_PARAMS(3);
	if (!pNetGame->GetPlayerPool()->GetSlotState(params[1])) return 0;
	CGangZonePool *pGangZonePool = pNetGame->GetGangZonePool();
	if (!pGangZonePool || !pGangZonePool->GetSlotState(params[2])) return 0;
	pGangZonePool->FlashForPlayer(params[1], params[2], params[3]);
	return 1;
}

static cell AMX_NATIVE_CALL n_GangZoneFlashForAll(AMX *amx, cell *params)
{
	CHECK_PARAMS(2);
	CGangZonePool *pGangZonePool = pNetGame->GetGangZonePool();
	if (!pGangZonePool || !pGangZonePool->GetSlotState(params[1])) return 0;
	pGangZonePool->FlashForAll(params[1], params[2]);
	return 1;
}

static cell AMX_NATIVE_CALL n_GangZoneStopFlashForPlayer(AMX *amx, cell *params)
{
	CHECK_PARAMS(2);
	if (!pNetGame->GetPlayerPool()->GetSlotState(params[1])) return 0;
	CGangZonePool *pGangZonePool = pNetGame->GetGangZonePool();
	if (!pGangZonePool || !pGangZonePool->GetSlotState(params[2])) return 0;
	pGangZonePool->StopFlashForPlayer(params[1], params[2]);
	return 1;
}

static cell AMX_NATIVE_CALL n_GangZoneStopFlashForAll(AMX *amx, cell *params)
{
	CHECK_PARAMS(1);
	CGangZonePool *pGangZonePool = pNetGame->GetGangZonePool();
	if (!pGangZonePool || !pGangZonePool->GetSlotState(params[1])) return 0;
	pGangZonePool->StopFlashForAll(params[1]);
	return 1;
}

//----------------------------------------------------------------------------------
// native GetServerVarAsString(const varname[], buffer[], len)

static cell AMX_NATIVE_CALL n_GetServerVarAsString(AMX *amx, cell *params)
{
	CHECK_PARAMS(3);
	char *szParam;
	amx_StrParam(amx,params[1],szParam);
	return set_amxstring(amx,params[2],pConsole->GetStringVariable(szParam),params[3]);
}

// native GetServerVarAsInt(const varname[])
static cell AMX_NATIVE_CALL n_GetServerVarAsInt(AMX *amx, cell *params)
{
	CHECK_PARAMS(1);
	char *szParam;
	amx_StrParam(amx,params[1],szParam);
	return pConsole->GetIntVariable(szParam);
}

// native GetServerVarAsBool(const varname[])
static cell AMX_NATIVE_CALL n_GetServerVarAsBool(AMX *amx, cell *params)
{
	CHECK_PARAMS(1);
	char *szParam;
	amx_StrParam(amx,params[1],szParam);
	return (int)pConsole->GetBoolVariable(szParam);
}

//----------------------------------------------------------------------------------

// native EnableStuntBonusForAll(enable)
static cell AMX_NATIVE_CALL n_EnableStuntBonusForAll(AMX *amx, cell *params)
{
	CHECK_PARAMS(1);
	if (params[1] != 1) params[1] = 0;

	pNetGame->m_bStuntBonus = (bool)params[1];
	RakNet::BitStream bsParams;
	bsParams.Write((bool)params[1]);

	pNetGame->GetRakServer()->RPC(RPC_ScrEnableStuntBonus , &bsParams, HIGH_PRIORITY, RELIABLE, 0, UNASSIGNED_PLAYER_ID, true, false);
	return 1;
}

// native EnableStuntBonusForPlayer(playerid, enable)
static cell AMX_NATIVE_CALL n_EnableStuntBonusForPlayer(AMX *amx, cell *params)
{
	CHECK_PARAMS(2);
	if (!pNetGame->GetPlayerPool()->GetSlotState(params[1])) return 0;
	if (params[2] != 1) params[2] = 0;

	RakNet::BitStream bsParams;
	bsParams.Write((bool)params[2]);

	RakServerInterface* pRak = pNetGame->GetRakServer();
	pRak->RPC(RPC_ScrEnableStuntBonus , &bsParams, HIGH_PRIORITY, RELIABLE, 0, pRak->GetPlayerIDFromIndex((BYTE)params[1]), false, false);
	return 1;
}

//----------------------------------------------------------------------------------

// native DisableInteriorEnterExits()
static cell AMX_NATIVE_CALL n_DisableInteriorEnterExits(AMX *amx, cell *params)
{
	pNetGame->m_bDisableEnterExits = true;

	return 1;
}

// native SetNameTagDrawDistance(Float:distance)
static cell AMX_NATIVE_CALL n_SetNameTagDrawDistance(AMX *amx, cell *params)
{
	pNetGame->m_fNameTagDrawDistance = amx_ctof(params[1]);

	return 1;
}

//----------------------------------------------------------------------------------

// native CreatePlayerPickup(pickupid,playerid,model,type,Float:PosX,Float:PosY,Float:PosZ)
static cell AMX_NATIVE_CALL n_CreatePlayerPickup(AMX *amx, cell *params)
{
	int iPickupId = params[1];
	if(!pNetGame->GetPlayerPool()) return 0;
	if(!pNetGame->GetPlayerPool()->GetSlotState(params[2])) return 0;

	PICKUP Pickup;
    Pickup.iModel = params[3];
	Pickup.iType = params[4];
	Pickup.fX = amx_ctof(params[5]);
	Pickup.fY = amx_ctof(params[6]);
	Pickup.fZ = amx_ctof(params[7]);

	RakNet::BitStream bsPickup;
	bsPickup.Write(iPickupId);
	bsPickup.Write((PCHAR)&Pickup,sizeof(PICKUP));
	pNetGame->GetRakServer()->RPC(RPC_Pickup, &bsPickup, HIGH_PRIORITY, RELIABLE, 0,
		pNetGame->GetRakServer()->GetPlayerIDFromIndex(params[2]), false, false);

	return 1;
}

//----------------------------------------------------------------------------------

// native DestroyPlayerPickup(pickupid,playerid)
static cell AMX_NATIVE_CALL n_DestroyPlayerPickup(AMX *amx, cell *params)
{
	int iPickupId = params[1];
	if(!pNetGame->GetPlayerPool()) return 0;
	if(!pNetGame->GetPlayerPool()->GetSlotState(params[2])) return 0;

	RakNet::BitStream bsPickup;
	bsPickup.Write(iPickupId);
	pNetGame->GetRakServer()->RPC(RPC_DestroyPickup, &bsPickup, HIGH_PRIORITY, RELIABLE, 0,
		pNetGame->GetRakServer()->GetPlayerIDFromIndex(params[2]), false, false);

	return 1;
}

//----------------------------------------------------------------------------------

// native IsPlayerInRangeOfPoint(playerid,Float:fRange,Float:PosX,Float:PosY,Float:PosZ)
static cell AMX_NATIVE_CALL n_IsPlayerInRangeOfPoint(AMX *amx, cell *params)
{
	CPlayerPool *pPlayerPool = pNetGame->GetPlayerPool();

	if(!pPlayerPool) return 0;
	if(!pPlayerPool->GetSlotState(params[1])) return 0;

	CPlayer *pPlayer = pPlayerPool->GetAt(params[1]);

	VECTOR vecTestPoint;
	VECTOR *vecThisPlayer;

	float fRange = amx_ctof(params[2]);
	vecTestPoint.X = amx_ctof(params[3]);
	vecTestPoint.Y = amx_ctof(params[4]);
	vecTestPoint.Z = amx_ctof(params[5]);

    fRange = fRange * fRange; // we'll use the squared distance, not the square root.    
	
	vecThisPlayer = &pPlayer->m_vecPos;
	
	float fSX = (vecThisPlayer->X - vecTestPoint.X) * (vecThisPlayer->X - vecTestPoint.X);
	float fSY = (vecThisPlayer->Y - vecTestPoint.Y) * (vecThisPlayer->Y - vecTestPoint.Y);
	float fSZ = (vecThisPlayer->Z - vecTestPoint.Z) * (vecThisPlayer->Z - vecTestPoint.Z);

	if((float)(fSX + fSY + fSZ) < fRange) return 1;

	return 0;
}

//----------------------------------------------------------------------------------

AMX_NATIVE_INFO custom_Natives[] =
{
	// Util
	{ "print",					n_print },
	{ "printf",					n_printf },
	{ "format",					n_format },
	{ "SetTimer",				n_SetTimer },
	{ "KillTimer",				n_KillTimer },
	{ "GetTickCount",			n_GetTickCount },
	{ "GetMaxPlayers",			n_GetMaxPlayers },
	{ "SetTimerEx",				n_SetTimerEx },
	//{ "SetMaxPlayers",			n_SetMaxPlayers },
	{ "LimitGlobalChatRadius",	n_LimitGlobalChatRadius },
	{ "SetWeather",				n_SetWeather },
	{ "SetPlayerWeather",		n_SetPlayerWeather },
	{ "CallRemoteFunction",		n_CallRemoteFunction },
	{ "CallLocalFunction",		n_CallLocalFunction },
	{ "asin",					n_asin },
	{ "acos",					n_acos },
	{ "atan2",					n_atan2 },
	{ "atan",					n_atan },
	
	// Game
	{ "GameModeExit",			n_GameModeExit },
	{ "SetGameModeText",		n_SetGameModeText },
	{ "SetTeamCount",			n_SetTeamCount },
	{ "AddPlayerClass",			n_AddPlayerClass },
	{ "AddPlayerClassEx",		n_AddPlayerClassEx },
	{ "AddStaticVehicle",		n_AddStaticVehicle },
	{ "AddStaticVehicleEx",		n_AddStaticVehicleEx },
	{ "AddStaticPickup",		n_AddStaticPickup },
	{ "CreatePickup",			n_CreatePickup },
	{ "DestroyPickup",			n_DestroyPickup },
	{ "SetPlayerWorldBounds",	n_SetPlayerWorldBounds },
	{ "ShowNameTags",			n_ShowNameTags },
	{ "ShowPlayerMarkers",		n_ShowPlayerMarkers },
	{ "SetWorldTime",			n_SetWorldTime },
	{ "GetWeaponName",			n_GetWeaponName },
	{ "EnableTirePopping",		n_EnableTirePopping },
	{ "AllowInteriorWeapons",	n_AllowInteriorWeapons },
	{ "SetGravity",				n_SetGravity },
	{ "GetGravity",				n_GetGravity },
	{ "AllowAdminTeleport",		n_AllowAdminTeleport },
	{ "SetDeathDropAmount",		n_SetDeathDropAmount },
	{ "CreateExplosion",        n_CreateExplosion },
	{ "SetDisabledWeapons",		n_SetDisabledWeapons },
	{ "UsePlayerPedAnims",		n_UsePlayerPedAnims },
	{ "DisableInteriorEnterExits", n_DisableInteriorEnterExits },
	{ "SetNameTagDrawDistance", n_SetNameTagDrawDistance },
	
	// Zones
	{ "EnableZoneNames",		n_EnableZoneNames },
	{ "GangZoneCreate",				n_GangZoneCreate },
	{ "GangZoneDestroy",			n_GangZoneDestroy },
	{ "GangZoneShowForPlayer",		n_GangZoneShowForPlayer },
	{ "GangZoneShowForAll",			n_GangZoneShowForAll },
	{ "GangZoneHideForPlayer",		n_GangZoneHideForPlayer },
	{ "GangZoneHideForAll",			n_GangZoneHideForAll },
	{ "GangZoneFlashForPlayer",		n_GangZoneFlashForPlayer },
	{ "GangZoneFlashForAll",		n_GangZoneFlashForAll },
	{ "GangZoneStopFlashForPlayer",	n_GangZoneStopFlashForPlayer },
	{ "GangZoneStopFlashForAll",	n_GangZoneStopFlashForAll },

	// Admin
	{ "IsPlayerAdmin",			n_IsPlayerAdmin },
	{ "Kick",					n_Kick },
	{ "Ban",					n_Ban },
	{ "BanEx",					n_BanEx },
	{ "SendRconCommand",		n_SendRconCommand },
	{ "GetServerVarAsString",	n_GetServerVarAsString },
	{ "GetServerVarAsInt",		n_GetServerVarAsInt },
	{ "GetServerVarAsBool",		n_GetServerVarAsBool },


	// Player
	{ "SetSpawnInfo",			n_SetSpawnInfo },
	{ "SpawnPlayer",			n_SpawnPlayer },
	{ "SetPlayerTeam",			n_SetPlayerTeam },
	{ "GetPlayerTeam",			n_GetPlayerTeam },
	{ "SetPlayerName",			n_SetPlayerName },
	{ "SetPlayerSkin",			n_SetPlayerSkin },
	{ "GetPlayerSkin",			n_GetPlayerSkin },
	{ "GetPlayerPos",			n_GetPlayerPos },
	{ "SetPlayerPos",			n_SetPlayerPos },
	{ "SetPlayerPosFindZ",		n_SetPlayerPosFindZ },
	{ "GetPlayerHealth",		n_GetPlayerHealth },
	{ "SetPlayerHealth",		n_SetPlayerHealth },
	{ "SetPlayerColor",			n_SetPlayerColor },
	{ "GetPlayerColor",			n_GetPlayerColor },
	{ "GetPlayerVehicleID",		n_GetPlayerVehicleID },
	{ "PutPlayerInVehicle",		n_PutPlayerInVehicle },
	{ "RemovePlayerFromVehicle",n_RemovePlayerFromVehicle },
	{ "IsPlayerInVehicle",		n_IsPlayerInVehicle },
	{ "IsPlayerInAnyVehicle",	n_IsPlayerInAnyVehicle },
	{ "GetPlayerName",			n_GetPlayerName },
	{ "SetPlayerColor",			n_SetPlayerColor },
	{ "GetPlayerColor",			n_GetPlayerColor },
	{ "GetPlayerVehicleID",		n_GetPlayerVehicleID },
	{ "SetPlayerCheckpoint",	n_SetPlayerCheckpoint },
	{ "DisablePlayerCheckpoint",n_DisablePlayerCheckpoint },
	{ "IsPlayerInCheckpoint",	n_IsPlayerInCheckpoint },
	{ "SetPlayerRaceCheckpoint",	n_SetPlayerRaceCheckpoint },
	{ "DisablePlayerRaceCheckpoint",n_DisablePlayerRaceCheckpoint },
	{ "IsPlayerInRaceCheckpoint",	n_IsPlayerInRaceCheckpoint },
	{ "SetPlayerInterior",		n_SetPlayerInterior },
	{ "GetPlayerInterior",		n_GetPlayerInterior },
	{ "SetPlayerCameraLookAt",	n_SetPlayerCameraLookAt },
	{ "SetPlayerCameraPos",		n_SetPlayerCameraPos },
	{ "SetCameraBehindPlayer",	n_SetCameraBehindPlayer },
	{ "TogglePlayerControllable",	n_TogglePlayerControllable },
	{ "PlayerPlaySound",		n_PlayerPlaySound },
	{ "SetPlayerScore",			n_SetPlayerScore },
	{ "GetPlayerScore",			n_GetPlayerScore },
	{ "SetPlayerFacingAngle",	n_SetPlayerFacingAngle },
	{ "GetPlayerFacingAngle",	n_GetPlayerFacingAngle },
	{ "GivePlayerMoney",		n_GivePlayerMoney },
	{ "GetPlayerMoney",			n_GetPlayerMoney },
	{ "ResetPlayerMoney",		n_ResetPlayerMoney },
	{ "IsPlayerConnected",		n_IsPlayerConnected },
	{ "GetPlayerState",			n_GetPlayerState },	
	{ "ResetPlayerWeapons",		n_ResetPlayerWeapons },
	{ "GivePlayerWeapon",		n_GivePlayerWeapon },
	{ "GetPlayerIp",			n_GetPlayerIp },
	{ "GetPlayerPing",			n_GetPlayerPing },
	{ "GetPlayerWeapon",		n_GetPlayerWeapon },
	{ "SetPlayerArmour",		n_SetPlayerArmour },
	{ "GetPlayerArmour",		n_GetPlayerArmour },
	{ "SetPlayerMapIcon",		n_SetPlayerMapIcon },
	{ "RemovePlayerMapIcon",	n_RemovePlayerMapIcon },
	{ "GetPlayerKeys",			n_GetPlayerKeys },
	{ "SetPlayerMarkerForPlayer",		n_SetPlayerMarkerForPlayer }, // Changed function name
	{ "GetPlayerAmmo",			n_GetPlayerAmmo },
	{ "SetPlayerAmmo",			n_SetPlayerAmmo },
	{ "GetPlayerWeaponData",	n_GetPlayerWeaponData },
	{ "AllowPlayerTeleport",	n_AllowPlayerTeleport },
	{ "ForceClassSelection",	n_ForceClassSelection },
	{ "SetPlayerWantedLevel",	n_SetPlayerWantedLevel },
	{ "GetPlayerWantedLevel",	n_GetPlayerWantedLevel },
	
	{ "SetPlayerVirtualWorld",		n_SetPlayerVirtualWorld },
	{ "GetPlayerVirtualWorld",		n_GetPlayerVirtualWorld },
	{ "ShowPlayerNameTagForPlayer",	n_ShowPlayerNameTagForPlayer},
	
	{ "EnableStuntBonusForAll",		n_EnableStuntBonusForAll },
	{ "EnableStuntBonusForPlayer",	n_EnableStuntBonusForPlayer },

	{ "TogglePlayerSpectating",	n_TogglePlayerSpectating },
	{ "PlayerSpectateVehicle",	n_PlayerSpectateVehicle },
	{ "PlayerSpectatePlayer",	n_PlayerSpectatePlayer },
	{ "ApplyAnimation",			n_ApplyAnimation },
	{ "ClearAnimations",		n_ClearAnimations },
	{ "SetPlayerSpecialAction", n_SetPlayerSpecialAction },
	{ "GetPlayerSpecialAction", n_GetPlayerSpecialAction },

	{ "CreatePlayerPickup",		n_CreatePlayerPickup },
	{ "DestroyPlayerPickup",	n_DestroyPlayerPickup },
	{ "IsPlayerInRangeOfPoint", n_IsPlayerInRangeOfPoint },

	// Vehicle
	{ "CreateVehicle",			n_CreateVehicle },
	{ "DestroyVehicle",			n_DestroyVehicle },
	{ "GetVehiclePos",			n_GetVehiclePos },
	{ "SetVehiclePos",			n_SetVehiclePos },
	{ "GetVehicleZAngle",		n_GetVehicleZAngle },
	{ "SetVehicleZAngle",		n_SetVehicleZAngle },
	{ "SetVehicleParamsForPlayer",	n_SetVehicleParamsForPlayer },
	{ "SetVehicleToRespawn",	n_SetVehicleToRespawn },
	{ "AddVehicleComponent",	n_AddVehicleComponent },
	{ "RemoveVehicleComponent",	n_RemoveVehicleComponent },
	{ "ChangeVehicleColor",		n_ChangeVehicleColor },
	{ "ChangeVehiclePaintjob",	n_ChangeVehiclePaintjob },
	{ "LinkVehicleToInterior",	n_LinkVehicleToInterior },
	{ "SetVehicleHealth",		n_SetVehicleHealth },
	{ "GetVehicleHealth",		n_GetVehicleHealth },
	{ "AttachTrailerToVehicle", n_AttachTrailerToVehicle },
	{ "DetachTrailerFromVehicle", n_DetachTrailerFromVehicle },
	{ "IsTrailerAttachedToVehicle",		n_IsTrailerAttachedToVehicle },
	{ "GetVehicleTrailer",		n_GetVehicleTrailer },
	{ "SetVehicleNumberPlate",	n_SetVehicleNumberPlate },
	{ "GetVehicleModel",		n_GetVehicleModel },

	{ "SetVehicleVirtualWorld",		n_SetVehicleVirtualWorld },
	{ "GetVehicleVirtualWorld",		n_GetVehicleVirtualWorld },

	// Messaging
	{ "SendClientMessage",		n_SendClientMessage },
	{ "SendClientMessageToAll",	n_SendClientMessageToAll },
	{ "SendDeathMessage",		n_SendDeathMessage },
	{ "GameTextForAll",			n_GameTextForAll },
	{ "GameTextForPlayer",		n_GameTextForPlayer },
	{ "SendPlayerMessageToPlayer",	n_SendPlayerMessageToPlayer },
	{ "SendPlayerMessageToAll",		n_SendPlayerMessageToAll },
	
	{ "TextDrawCreate",				n_TextDrawCreate },
	{ "TextDrawSetString",				n_TextDrawSetString },
	{ "TextDrawLetterSize",			n_TextDrawLetterSize },
	{ "TextDrawTextSize",			n_TextDrawTextSize },
	{ "TextDrawAlignment",			n_TextDrawAlignment },
	{ "TextDrawColor",				n_TextDrawColor },
	{ "TextDrawUseBox",				n_TextDrawUseBox },
	{ "TextDrawBoxColor",			n_TextDrawBoxColor },
	{ "TextDrawSetShadow",			n_TextDrawSetShadow },
	{ "TextDrawSetOutline",			n_TextDrawSetOutline },
	{ "TextDrawBackgroundColor",	n_TextDrawBackgroundColor },
	{ "TextDrawFont",				n_TextDrawFont },
	{ "TextDrawSetProportional",	n_TextDrawSetProportional },
	{ "TextDrawShowForPlayer",		n_TextDrawShowForPlayer },
	{ "TextDrawShowForAll",			n_TextDrawShowForAll },
	{ "TextDrawHideForPlayer",		n_TextDrawHideForPlayer },
	{ "TextDrawHideForAll",			n_TextDrawHideForAll },
	{ "TextDrawDestroy",			n_TextDrawDestroy },
	
	// Objects
	{ "CreateObject",			n_CreateObject },
	{ "SetObjectPos",			n_SetObjectPos },
	{ "SetObjectRot",			n_SetObjectRot },
	{ "GetObjectPos",			n_GetObjectPos },
	{ "GetObjectRot",			n_GetObjectRot },
	{ "IsValidObject",			n_IsValidObject },
	{ "DestroyObject",			n_DestroyObject },
	{ "MoveObject",				n_MoveObject },
	{ "StopObject",				n_StopObject },
	
	{ "CreatePlayerObject",			n_CreatePlayerObject },
	{ "SetPlayerObjectPos",			n_SetPlayerObjectPos },
	{ "SetPlayerObjectRot",			n_SetPlayerObjectRot },
	{ "GetPlayerObjectPos",			n_GetPlayerObjectPos },
	{ "GetPlayerObjectRot",			n_GetPlayerObjectRot },
	{ "IsValidPlayerObject",		n_IsValidPlayerObject },
	{ "DestroyPlayerObject",		n_DestroyPlayerObject },
	{ "MovePlayerObject",			n_MovePlayerObject },
	{ "StopPlayerObject",			n_StopPlayerObject },

	{ "AttachObjectToPlayer",		n_AttachObjectToPlayer },
	{ "AttachPlayerObjectToPlayer",	n_AttachPlayerObjectToPlayer },
	
	// Menus
	{ "CreateMenu",				n_CreateMenu },
	{ "DestroyMenu",			n_DestroyMenu },
	{ "AddMenuItem",			n_AddMenuItem },
	{ "SetMenuColumnHeader",	n_SetMenuColumnHeader },
	{ "ShowMenuForPlayer",		n_ShowMenuForPlayer },
	{ "HideMenuForPlayer",		n_HideMenuForPlayer },
	{ "IsValidMenu",			n_IsValidMenu },
	{ "DisableMenu",			n_DisableMenu },
	{ "DisableMenuRow",			n_DisableMenuRow },
	{ "GetPlayerMenu",			n_GetPlayerMenu },
	
	{ "SetPlayerTime",			n_SetPlayerTime },
	{ "TogglePlayerClock",		n_TogglePlayerClock },
	{ "GetPlayerTime",			n_GetPlayerTime },

	{ NULL, NULL }
};

//----------------------------------------------------------------------------------

int amx_CustomInit(AMX *amx)
{
  return amx_Register(amx, custom_Natives, -1);
}

//----------------------------------------------------------------------------------
