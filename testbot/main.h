#pragma once

#define _CRT_SECURE_NO_DEPRECATE

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ARRAY_SIZE(a)	( sizeof((a)) / sizeof(*(a)) )
#define SAFE_DELETE(p)	{ if (p) { delete (p); (p) = NULL; } }
#define SAFE_RELEASE(p)	{ if (p) { (p)->Release(); (p) = NULL; } }

#define MAX_PLAYER_NAME		24
#define MAX_SETTINGS_STRING 256

#define GTASA_VERSION_UNKNOWN	0
#define GTASA_VERSION_USA10		1
#define GTASA_VERSION_EU10		2

#define IDC_GUIBUTTON1		5
#define IDC_GUIEDIT1		6

#define LOCKING_DISTANCE 200.0f
#define CSCANNER_DISTANCE 300.0f
#define PSCANNER_DISTANCE 600.0f

typedef struct _GAME_SETTINGS {
	CHAR szConnectPass[MAX_SETTINGS_STRING+1];
	CHAR szConnectHost[MAX_SETTINGS_STRING+1];
	CHAR szConnectPort[MAX_SETTINGS_STRING+1];
	CHAR szNickName[MAX_SETTINGS_STRING+1];
} GAME_SETTINGS;

#include "../raknet/RakClientInterface.h"
#include "../raknet/RakNetworkFactory.h"
#include "../raknet/BitStream.h"
#include "../raknet/PacketEnumerations.h"
#include "../raknet/SAMPRPC.h"

#include "../saco/game/common.h"

#define SPECTATE_TYPE_NONE				0
#define SPECTATE_TYPE_PLAYER			1
#define SPECTATE_TYPE_VEHICLE			2

#define SPECIAL_ACTION_NONE				0
#define SPECIAL_ACTION_USEJETPACK		2
#define SPECIAL_ACTION_DANCE1			5
#define SPECIAL_ACTION_DANCE2			6
#define SPECIAL_ACTION_DANCE3			7
#define SPECIAL_ACTION_DANCE4			8
#define SPECIAL_ACTION_HANDSUP			10
#define SPECIAL_ACTION_SITTING			12

#pragma pack(1)
typedef struct _PLAYER_SPAWN_INFO
{
	BYTE byteTeam;
	int iSkin;
	VECTOR vecPos;
	float fRotation;
	int iSpawnWeapons[3];
	int iSpawnWeaponsAmmo[3];
} PLAYER_SPAWN_INFO;

#pragma pack(1)
typedef struct _ONFOOT_SYNC_DATA
{
	WORD lrAnalog;
	WORD udAnalog;
	WORD wKeys;
	VECTOR vecPos;
	float fRotation;
	BYTE byteHealth;
	BYTE byteArmour;
	BYTE byteCurrentWeapon;
	BYTE byteSpecialAction;	
	VECTOR vecMoveSpeed;
	VECTOR vecSurfOffsets;
	VEHICLEID SurfVehicleId;
} ONFOOT_SYNC_DATA;

enum eWeaponState
{
	WS_NO_BULLETS = 0,
	WS_LAST_BULLET = 1,
	WS_MORE_BULLETS = 2,
	WS_RELOADING = 3,
};

#pragma pack(1)
typedef struct _AIM_SYNC_DATA
{
	BYTE byteCamMode;
	BYTE byteCamExtZoom : 6;	// 0-63 normalized
	BYTE byteWeaponState : 2;	// see eWeaponState
	C_VECTOR1 cvecAimf1;
	C_VECTOR1 cvecAimf2;
	VECTOR vecAimPos;
	float fAimZ;
} AIM_SYNC_DATA;

#pragma pack(1)
typedef struct _INCAR_SYNC_DATA
{
	VEHICLEID VehicleID;
	WORD lrAnalog;
	WORD udAnalog;
	WORD wKeys;
	C_VECTOR1 cvecRoll;
	C_VECTOR1 cvecDirection;
	VECTOR vecPos;
	VECTOR vecMoveSpeed;
	float fCarHealth;
	BYTE bytePlayerHealth;
	BYTE bytePlayerArmour;
	BYTE byteCurrentWeapon;
	BYTE byteSirenOn;
	BYTE byteLandingGearState;
	BYTE byteTires[4];
	VEHICLEID TrailerID;
	DWORD dwHydraThrustAngle;
	FLOAT fTrainSpeed;
} INCAR_SYNC_DATA;

#pragma pack(1)
typedef struct _PASSENGER_SYNC_DATA
{
	VEHICLEID VehicleID;
	BYTE byteSeatFlags : 7;
	BYTE byteDriveBy : 1;
	BYTE byteCurrentWeapon;
	BYTE bytePlayerHealth;
	BYTE bytePlayerArmour;
	WORD lrAnalog;
	WORD udAnalog;
	WORD wKeys;
	VECTOR vecPos;
} PASSENGER_SYNC_DATA;

#pragma pack(1)
typedef struct _SPECTATOR_SYNC_DATA
{
	WORD lrAnalog;
	WORD udAnalog;
	WORD wKeys;
	VECTOR vecPos;
} SPECTATOR_SYNC_DATA;

#pragma pack(1)
typedef struct _TRAILER_SYNC_DATA
{
	C_VECTOR1 cvecRoll;
	C_VECTOR1 cvecDirection;
	VECTOR vecPos;
	VECTOR vecMoveSpeed;
} TRAILER_SYNC_DATA;

#pragma pack(1)
typedef struct _CAR_MOD_INFO
{
	BYTE byteCarMod0;
	BYTE byteCarMod1;
	BYTE byteCarMod2;
	BYTE byteCarMod3;
	BYTE byteCarMod4;
	BYTE byteCarMod5;
	BYTE byteCarMod6;
	BYTE byteCarMod7;
	BYTE byteCarMod8;
	BYTE byteCarMod9;
	BYTE byteCarMod10;
	BYTE byteCarMod11;
	BYTE byteCarMod12;
	BYTE byteCarMod13;
	BYTE bytePaintJob;
	int iColor0;
	int iColor1;
} CAR_MOD_INFO;

#include "net/netrpc.h"
#include "net/playerpool.h"
#include "net/vehiclepool.h"
#include "net/netgame.h"
#include "net/scriptrpc.h"
