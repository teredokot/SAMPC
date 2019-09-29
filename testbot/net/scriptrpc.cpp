/*

	SA:MP Multiplayer Modification
	Copyright 2004-2005 SA:MP Team

	file:
		scriptrpc.cpp
	desc:
		Scripting RPCs.

	Version: $Id: scriptrpc.cpp,v 1.36 2006/05/08 14:31:50 kyeman Exp $
*/

#include "../main.h"

using namespace RakNet;
extern CNetGame*	pNetGame;
extern ONFOOT_SYNC_DATA ofSync;
extern bool	bSpawned;

//----------------------------------------------------

void ScrSetSpawnInfo(RPCParameters *rpcParams)
{
	PCHAR Data = reinterpret_cast<PCHAR>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;
	PlayerID sender = rpcParams->sender;
	PLAYER_SPAWN_INFO SpawnInfo;
	RakNet::BitStream bsData(Data,(iBitLength/8)+1,false);
	CPlayerPool *pPlayerPool = pNetGame->GetPlayerPool();
	bsData.Read((PCHAR)&SpawnInfo, sizeof(PLAYER_SPAWN_INFO));
}

//----------------------------------------------------

void ScrSetPlayerTeam(RPCParameters *rpcParams)
{
}

//----------------------------------------------------

void ScrSetPlayerName(RPCParameters *rpcParams)
{
}

void ScrSetPlayerSkin(RPCParameters *rpcParams)
{
}

//----------------------------------------------------

void ScrSetPlayerPos(RPCParameters *rpcParams)
{
	/*
	PCHAR Data = reinterpret_cast<PCHAR>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;
	PlayerID sender = rpcParams->sender;
	RakNet::BitStream bsData(Data,(iBitLength/8)+1,false);
	
	bsData.Read(ofSync.vecPos.X);
	bsData.Read(ofSync.vecPos.Y);
	bsData.Read(ofSync.vecPos.Z);*/
}

//----------------------------------------------------

void ScrSetPlayerPosFindZ(RPCParameters *rpcParams)
{
}

//----------------------------------------------------

void ScrSetPlayerHealth(RPCParameters *rpcParams)
{
}

//----------------------------------------------------

void ScrPutPlayerInVehicle(RPCParameters *rpcParams)
{
}

//----------------------------------------------------

void ScrRemovePlayerFromVehicle(RPCParameters *rpcParams)
{
}

//----------------------------------------------------

void ScrSetPlayerColor(RPCParameters *rpcParams)
{
}

//----------------------------------------------------

void ScrDisplayGameText(RPCParameters *rpcParams)
{
	PCHAR Data = reinterpret_cast<PCHAR>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;
	PlayerID sender = rpcParams->sender;

	RakNet::BitStream bsData(Data,(iBitLength/8)+1,false);
	char szMessage[512];
	int iType;
	int iTime;
	int iLength;

	bsData.Read(iType);
	bsData.Read(iTime);
	bsData.Read(iLength);

	if(iLength > 512) return;

	bsData.Read(szMessage,iLength);
	szMessage[iLength] = '\0';

	OutputDebugString(szMessage);
}

//----------------------------------------------------

void ScrSetInterior(RPCParameters *rpcParams)
{
}

//----------------------------------------------------

void ScrSetCameraPos(RPCParameters *rpcParams)
{
}

//----------------------------------------------------

void ScrSetCameraLookAt(RPCParameters *rpcParams)
{
}

//----------------------------------------------------

void ScrSetVehiclePos(RPCParameters *rpcParams)
{
}

//----------------------------------------------------

void ScrSetVehicleZAngle(RPCParameters *rpcParams)
{
}

//----------------------------------------------------

void ScrVehicleParams(RPCParameters *rpcParams)
{

}

//----------------------------------------------------

void ScrLinkVehicle(RPCParameters *rpcParams)
{
}

//----------------------------------------------------

void ScrSetCameraBehindPlayer(RPCParameters *rpcParams)
{
}

//----------------------------------------------------

void ScrTogglePlayerControllable(RPCParameters *rpcParams)
{
}

//----------------------------------------------------

void ScrPlaySound(RPCParameters *rpcParams)
{
}

//----------------------------------------------------

void ScrSetWorldBounds(RPCParameters *rpcParams)
{
}

//----------------------------------------------------

void ScrHaveSomeMoney(RPCParameters *rpcParams)
{
}

//----------------------------------------------------

void ScrSetPlayerFacingAngle(RPCParameters *rpcParams)
{
}

//----------------------------------------------------

void ScrResetMoney(RPCParameters *rpcParams)
{
}

//----------------------------------------------------

void ScrResetPlayerWeapons(RPCParameters *rpcParams)
{
}

//----------------------------------------------------

void ScrGivePlayerWeapon(RPCParameters *rpcParams)
{
}

//----------------------------------------------------

void ScrRespawnVehicle(RPCParameters *rpcParams)
{
}

//----------------------------------------------------

void ScrDeathMessage(RPCParameters *rpcParams)
{
}

//----------------------------------------------------

void ScrSetMapIcon(RPCParameters *rpcParams)
{
}

void ScrDisableMapIcon(RPCParameters *rpcParams)
{
}

void ScrSetPlayerArmour(RPCParameters *rpcParams)
{
}

void ScrSetWeaponAmmo(RPCParameters *rpcParams)
{
}

//----------------------------------------------------

void ScrSetGravity(RPCParameters *rpcParams)
{
}

//----------------------------------------------------

void ScrSetVehicleHealth(RPCParameters *rpcParams)
{
}

//----------------------------------------------------

void ScrAttachTrailerToVehicle(RPCParameters *rpcParams)
{
}

//----------------------------------------------------

void ScrDetachTrailerFromVehicle(RPCParameters *rpcParams)
{
}

//----------------------------------------------------

void ScrCreateObject(RPCParameters *rpcParams)
{
}

//----------------------------------------------------

void ScrSetObjectPos(RPCParameters *rpcParams)
{
}

//----------------------------------------------------

void ScrSetObjectRotation(RPCParameters *rpcParams)
{
}

//----------------------------------------------------

void ScrDestroyObject(RPCParameters *rpcParams)
{
}

//----------------------------------------------------

void ScrSetPlayerVirtualWorld(RPCParameters *rpcParams)
{
}

//----------------------------------------------------

void ScrSetVehicleVirtualWorld(RPCParameters *rpcParams)
{
}

void ScrCreateExplosion(RPCParameters *rpcParams)
{
}

void ScrShowNameTag(RPCParameters *rpcParams)
{
}

void ScrMoveObject(RPCParameters *rpcParams)
{
}

void ScrStopObject(RPCParameters *rpcParams)
{
}

void ScrNumberPlate(RPCParameters *rpcParams)
{
}

//----------------------------------------------------

void ScrTogglePlayerSpectating(RPCParameters *rpcParams)
{
}

void ScrSetPlayerSpectating(RPCParameters *rpcParams)
{
}

#define SPECTATE_TYPE_NORMAL	1
#define SPECTATE_TYPE_FIXED		2
#define SPECTATE_TYPE_SIDE		3

void ScrPlayerSpectatePlayer(RPCParameters *rpcParams)
{
}

void ScrPlayerSpectateVehicle(RPCParameters *rpcParams)
{
}

void ScrRemoveComponent(RPCParameters *rpcParams)
{
}

void ScrForceSpawnSelection(RPCParameters *rpcParams)
{
}

void ScrAttachObjectToPlayer(RPCParameters *rpcParams)
{
}

void ScrInitMenu(RPCParameters *rpcParams)
{
}

void ScrShowMenu(RPCParameters *rpcParams)
{
}

void ScrHideMenu(RPCParameters *rpcParams)
{
}

void ScrSetPlayerWantedLevel(RPCParameters *rpcParams)
{
}

void ScrShowTextDraw(RPCParameters *rpcParams)
{
}

void ScrHideTextDraw(RPCParameters *rpcParams)
{
}

void ScrAddGangZone(RPCParameters *rpcParams)
{
}

void ScrRemoveGangZone(RPCParameters *rpcParams)
{
}

void ScrFlashGangZone(RPCParameters *rpcParams)
{
}

void ScrStopFlashGangZone(RPCParameters *rpcParams)
{
}

//----------------------------------------------------

void ScrApplyAnimation(RPCParameters *rpcParams)
{
}

//----------------------------------------------------

void ScrClearAnimations(RPCParameters *rpcParams)
{
}

//----------------------------------------------------

void ScrSetSpecialAction(RPCParameters *rpcParams)
{
}

//----------------------------------------------------

void RegisterScriptRPCs(RakClientInterface* pRakClient)
{
	REGISTER_STATIC_RPC(pRakClient, ScrSetSpawnInfo);
	REGISTER_STATIC_RPC(pRakClient, ScrSetPlayerTeam);
	REGISTER_STATIC_RPC(pRakClient, ScrSetPlayerSkin);
	REGISTER_STATIC_RPC(pRakClient, ScrSetPlayerName);
	REGISTER_STATIC_RPC(pRakClient, ScrSetPlayerPos);
	REGISTER_STATIC_RPC(pRakClient, ScrSetPlayerPosFindZ);
	REGISTER_STATIC_RPC(pRakClient, ScrSetPlayerHealth);
	REGISTER_STATIC_RPC(pRakClient, ScrPutPlayerInVehicle);
	REGISTER_STATIC_RPC(pRakClient, ScrRemovePlayerFromVehicle);
	REGISTER_STATIC_RPC(pRakClient, ScrSetPlayerColor);
	REGISTER_STATIC_RPC(pRakClient, ScrDisplayGameText);
	REGISTER_STATIC_RPC(pRakClient, ScrSetInterior);
	REGISTER_STATIC_RPC(pRakClient, ScrSetCameraPos);
	REGISTER_STATIC_RPC(pRakClient, ScrSetCameraLookAt);
	REGISTER_STATIC_RPC(pRakClient, ScrSetVehiclePos);
	REGISTER_STATIC_RPC(pRakClient, ScrSetVehicleZAngle);
	REGISTER_STATIC_RPC(pRakClient, ScrVehicleParams);
	REGISTER_STATIC_RPC(pRakClient, ScrSetCameraBehindPlayer);
	REGISTER_STATIC_RPC(pRakClient, ScrTogglePlayerControllable);
	REGISTER_STATIC_RPC(pRakClient, ScrPlaySound);
	REGISTER_STATIC_RPC(pRakClient, ScrSetWorldBounds);
	REGISTER_STATIC_RPC(pRakClient, ScrHaveSomeMoney);
	REGISTER_STATIC_RPC(pRakClient, ScrSetPlayerFacingAngle);
	REGISTER_STATIC_RPC(pRakClient, ScrResetMoney);
	REGISTER_STATIC_RPC(pRakClient, ScrResetPlayerWeapons);
	REGISTER_STATIC_RPC(pRakClient, ScrGivePlayerWeapon);
	REGISTER_STATIC_RPC(pRakClient, ScrRespawnVehicle);
	REGISTER_STATIC_RPC(pRakClient, ScrLinkVehicle);
	REGISTER_STATIC_RPC(pRakClient, ScrSetPlayerArmour);
	REGISTER_STATIC_RPC(pRakClient, ScrDeathMessage);
	//REGISTER_STATIC_RPC(pRakClient, ScrSetPlayerMarker);
	REGISTER_STATIC_RPC(pRakClient, ScrSetMapIcon);
	REGISTER_STATIC_RPC(pRakClient, ScrDisableMapIcon);
	REGISTER_STATIC_RPC(pRakClient, ScrSetWeaponAmmo);
	REGISTER_STATIC_RPC(pRakClient, ScrSetGravity);
	REGISTER_STATIC_RPC(pRakClient, ScrSetVehicleHealth);
	REGISTER_STATIC_RPC(pRakClient, ScrAttachTrailerToVehicle);
	REGISTER_STATIC_RPC(pRakClient, ScrDetachTrailerFromVehicle);
	REGISTER_STATIC_RPC(pRakClient, ScrCreateObject);
	REGISTER_STATIC_RPC(pRakClient, ScrSetObjectPos);
	REGISTER_STATIC_RPC(pRakClient, ScrSetObjectRotation);
	REGISTER_STATIC_RPC(pRakClient, ScrDestroyObject);
	REGISTER_STATIC_RPC(pRakClient, ScrSetPlayerVirtualWorld);
	REGISTER_STATIC_RPC(pRakClient, ScrSetVehicleVirtualWorld);
	REGISTER_STATIC_RPC(pRakClient, ScrCreateExplosion);
	REGISTER_STATIC_RPC(pRakClient, ScrShowNameTag);
	REGISTER_STATIC_RPC(pRakClient, ScrMoveObject);
	REGISTER_STATIC_RPC(pRakClient, ScrStopObject);
	REGISTER_STATIC_RPC(pRakClient, ScrNumberPlate);
	REGISTER_STATIC_RPC(pRakClient, ScrTogglePlayerSpectating);
	REGISTER_STATIC_RPC(pRakClient, ScrSetPlayerSpectating);
	REGISTER_STATIC_RPC(pRakClient, ScrPlayerSpectatePlayer);
	REGISTER_STATIC_RPC(pRakClient, ScrPlayerSpectateVehicle);
	REGISTER_STATIC_RPC(pRakClient, ScrRemoveComponent);
	REGISTER_STATIC_RPC(pRakClient, ScrForceSpawnSelection);
	REGISTER_STATIC_RPC(pRakClient, ScrAttachObjectToPlayer);
	REGISTER_STATIC_RPC(pRakClient, ScrInitMenu);
	REGISTER_STATIC_RPC(pRakClient, ScrShowMenu);
	REGISTER_STATIC_RPC(pRakClient, ScrHideMenu);
	REGISTER_STATIC_RPC(pRakClient, ScrSetPlayerWantedLevel);
	REGISTER_STATIC_RPC(pRakClient, ScrShowTextDraw);
	REGISTER_STATIC_RPC(pRakClient, ScrHideTextDraw);
	REGISTER_STATIC_RPC(pRakClient, ScrAddGangZone);
	REGISTER_STATIC_RPC(pRakClient, ScrRemoveGangZone);
	REGISTER_STATIC_RPC(pRakClient, ScrFlashGangZone);
	REGISTER_STATIC_RPC(pRakClient, ScrStopFlashGangZone);
	REGISTER_STATIC_RPC(pRakClient, ScrApplyAnimation);
	REGISTER_STATIC_RPC(pRakClient, ScrClearAnimations);
	REGISTER_STATIC_RPC(pRakClient, ScrSetSpecialAction);
}

//----------------------------------------------------

void UnRegisterScriptRPCs(RakClientInterface* pRakClient)
{
	UNREGISTER_STATIC_RPC(pRakClient, ScrSetSpawnInfo); // 32
	UNREGISTER_STATIC_RPC(pRakClient, ScrSetPlayerTeam);
	UNREGISTER_STATIC_RPC(pRakClient, ScrSetPlayerName);
	UNREGISTER_STATIC_RPC(pRakClient, ScrSetPlayerSkin);
	UNREGISTER_STATIC_RPC(pRakClient, ScrSetPlayerPos);
	UNREGISTER_STATIC_RPC(pRakClient, ScrSetPlayerPosFindZ);
	UNREGISTER_STATIC_RPC(pRakClient, ScrSetPlayerHealth);
	UNREGISTER_STATIC_RPC(pRakClient, ScrPutPlayerInVehicle);
	UNREGISTER_STATIC_RPC(pRakClient, ScrRemovePlayerFromVehicle);
	UNREGISTER_STATIC_RPC(pRakClient, ScrSetPlayerColor);
	UNREGISTER_STATIC_RPC(pRakClient, ScrDisplayGameText);
	UNREGISTER_STATIC_RPC(pRakClient, ScrSetInterior);
	UNREGISTER_STATIC_RPC(pRakClient, ScrSetCameraPos);
	UNREGISTER_STATIC_RPC(pRakClient, ScrSetCameraLookAt);
	UNREGISTER_STATIC_RPC(pRakClient, ScrSetVehiclePos);
	UNREGISTER_STATIC_RPC(pRakClient, ScrSetVehicleZAngle);
	UNREGISTER_STATIC_RPC(pRakClient, ScrVehicleParams);
	UNREGISTER_STATIC_RPC(pRakClient, ScrSetCameraBehindPlayer);
	UNREGISTER_STATIC_RPC(pRakClient, ScrTogglePlayerControllable); // 50
	UNREGISTER_STATIC_RPC(pRakClient, ScrPlaySound);
	UNREGISTER_STATIC_RPC(pRakClient, ScrSetWorldBounds);
	UNREGISTER_STATIC_RPC(pRakClient, ScrHaveSomeMoney);
	UNREGISTER_STATIC_RPC(pRakClient, ScrSetPlayerFacingAngle);
	UNREGISTER_STATIC_RPC(pRakClient, ScrResetMoney);
	UNREGISTER_STATIC_RPC(pRakClient, ScrResetPlayerWeapons);
	UNREGISTER_STATIC_RPC(pRakClient, ScrGivePlayerWeapon);
	UNREGISTER_STATIC_RPC(pRakClient, ScrRespawnVehicle); // 58
	UNREGISTER_STATIC_RPC(pRakClient, ScrLinkVehicle);
	UNREGISTER_STATIC_RPC(pRakClient, ScrDeathMessage);
	//UNREGISTER_STATIC_RPC(pRakClient, ScrSetPlayerMarker);
	UNREGISTER_STATIC_RPC(pRakClient, ScrSetMapIcon);
	UNREGISTER_STATIC_RPC(pRakClient, ScrDisableMapIcon);
	UNREGISTER_STATIC_RPC(pRakClient, ScrSetWeaponAmmo);
	UNREGISTER_STATIC_RPC(pRakClient, ScrSetGravity);
	UNREGISTER_STATIC_RPC(pRakClient, ScrSetVehicleHealth);
	UNREGISTER_STATIC_RPC(pRakClient, ScrAttachTrailerToVehicle);
	UNREGISTER_STATIC_RPC(pRakClient, ScrDetachTrailerFromVehicle);
	UNREGISTER_STATIC_RPC(pRakClient, ScrCreateObject);
	UNREGISTER_STATIC_RPC(pRakClient, ScrSetObjectPos);
	UNREGISTER_STATIC_RPC(pRakClient, ScrSetObjectRotation);
	UNREGISTER_STATIC_RPC(pRakClient, ScrDestroyObject); // Possible problem source
	UNREGISTER_STATIC_RPC(pRakClient, ScrSetPlayerVirtualWorld);
	UNREGISTER_STATIC_RPC(pRakClient, ScrSetVehicleVirtualWorld);
	UNREGISTER_STATIC_RPC(pRakClient, ScrCreateExplosion);
	UNREGISTER_STATIC_RPC(pRakClient, ScrShowNameTag);
	UNREGISTER_STATIC_RPC(pRakClient, ScrMoveObject);
	UNREGISTER_STATIC_RPC(pRakClient, ScrStopObject);
	UNREGISTER_STATIC_RPC(pRakClient, ScrNumberPlate);
	UNREGISTER_STATIC_RPC(pRakClient, ScrTogglePlayerSpectating);
	UNREGISTER_STATIC_RPC(pRakClient, ScrSetPlayerSpectating);
	UNREGISTER_STATIC_RPC(pRakClient, ScrPlayerSpectatePlayer);
	UNREGISTER_STATIC_RPC(pRakClient, ScrPlayerSpectateVehicle);
	UNREGISTER_STATIC_RPC(pRakClient, ScrRemoveComponent);
	UNREGISTER_STATIC_RPC(pRakClient, ScrForceSpawnSelection);
	UNREGISTER_STATIC_RPC(pRakClient, ScrAttachObjectToPlayer);
	UNREGISTER_STATIC_RPC(pRakClient, ScrInitMenu);
	UNREGISTER_STATIC_RPC(pRakClient, ScrShowMenu);
	UNREGISTER_STATIC_RPC(pRakClient, ScrHideMenu);
	UNREGISTER_STATIC_RPC(pRakClient, ScrSetPlayerWantedLevel);
	UNREGISTER_STATIC_RPC(pRakClient, ScrShowTextDraw);
	UNREGISTER_STATIC_RPC(pRakClient, ScrHideTextDraw);
	UNREGISTER_STATIC_RPC(pRakClient, ScrAddGangZone);
	UNREGISTER_STATIC_RPC(pRakClient, ScrRemoveGangZone);
	UNREGISTER_STATIC_RPC(pRakClient, ScrFlashGangZone);
	UNREGISTER_STATIC_RPC(pRakClient, ScrStopFlashGangZone);
	UNREGISTER_STATIC_RPC(pRakClient, ScrApplyAnimation);
	UNREGISTER_STATIC_RPC(pRakClient, ScrClearAnimations);
	UNREGISTER_STATIC_RPC(pRakClient, ScrSetSpecialAction);
}

//----------------------------------------------------
