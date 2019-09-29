//----------------------------------------------------------
//
// SA:MP Multiplayer Modification For GTA:SA
// Copyright 2004-2005 SA:MP team
//
// Version: $Id: aimstuff.h,v 1.5 2006/05/07 15:38:35 kyeman Exp $
//
//----------------------------------------------------------
#pragma once

typedef struct _CAMERA_AIM
{
	float f1x,f1y,f1z;
	float pos1x,pos1y,pos1z;
	float pos2x,pos2y,pos2z;
	float f2x,f2y,f2z;

} CAMERA_AIM;

void __stdcall GameAimSyncInit();
void __stdcall GameStoreLocalPlayerAim();
void __stdcall GameSetLocalPlayerAim();
CAMERA_AIM * __stdcall GameGetInternalAim();
void __stdcall GameStoreRemotePlayerAim(int iPlayer, CAMERA_AIM * caAim);
void __stdcall GameSetRemotePlayerAim(int iPlayer);
CAMERA_AIM * __stdcall GameGetRemotePlayerAim(int iPlayer);

void __stdcall GameSetPlayerCameraMode(BYTE byteMode, BYTE bytePlayerID);
BYTE __stdcall GameGetPlayerCameraMode(BYTE bytePlayerID);
BYTE __stdcall GameGetLocalPlayerCameraMode();

void __stdcall GameStoreLocalPlayerCameraExtZoom();
void __stdcall GameSetLocalPlayerCameraExtZoom();
void __stdcall GameSetPlayerCameraExtZoom(BYTE bytePlayerID, float fZoom);
void __stdcall GameSetRemotePlayerCameraExtZoom(BYTE bytePlayerID);
float __stdcall GameGetLocalPlayerCameraExtZoom();

//----------------------------------------------------------