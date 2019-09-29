//----------------------------------------------------------
//
// SA:MP Multiplayer Modification For GTA:SA
// Copyright 2004-2005 SA:MP team
//
// Version: $Id: debug.cpp,v 1.27 2006/05/08 20:33:58 kyeman Exp $
//
//----------------------------------------------------------

#include <windows.h>
#include <stdio.h>

#include "../main.h"
#include "debug.h"
#include "util.h"
#include "keystuff.h"
#include "aimstuff.h"

extern CChatWindow   *pChatWindow;
extern CCmdWindow	 *pCmdWindow;
extern CNetGame		 *pNetGame;
extern CGame		 *pGame;
extern CFontRender	 *pDefaultFont;

int		iGameDebugType=0;
DWORD	dwDebugEntity1=0;
DWORD	dwDebugEntity2=0;

int debug_draw_top=0;

// screen buffer for debug text
#define NUM_SCREEN_LINES 35
PCHAR screen_buf[NUM_SCREEN_LINES];

extern CAMERA_AIM * pcaInternalAim;
extern BYTE	* pbyteCameraMode;

extern INCAR_SYNC_DATA DebugSync;
extern BOOL bDebugUpdate;

extern char *aScanListMemory;

extern float fFarClip;

#ifdef _DEBUG

//----------------------------------------------------------

void GameDebugDrawNextLine(PCHAR szText, DWORD dwColor)
{	
	RECT rect;
	rect.top		= debug_draw_top;
	rect.left		= 30;
	//rect.bottom		= debug_draw_top+16;
	//rect.right		= 600;

	pDefaultFont->RenderText(szText,rect,dwColor);
	debug_draw_top+=16;
}

//----------------------------------------------------------

DWORD * GetNextTaskFromTask(DWORD *task)
{
	DWORD *ret_task=NULL;

	if(!task || *task < 0x800000 || *task > 0x900000) return NULL;
	
	_asm mov edx, task
	_asm mov ebx, [edx]
	_asm mov edx, [ebx+8]
	_asm mov ecx, task
	_asm call edx
	_asm mov ret_task, eax

	return ret_task;

}

//----------------------------------------------------------

int GetTaskTypeFromTask(DWORD *task)
{
	int i = 0;

	if(!task || *task < 0x800000 || *task > 0x900000) return 0;

	_asm mov edx, task
	_asm mov ebx, [edx]
	_asm mov edx, [ebx+16]
	_asm mov ecx, task
	_asm call edx
	_asm mov i, eax

	return i;
}

//----------------------------------------------------------

struct tTaskInfo
{
	unsigned int id;
	const char *name;
};

tTaskInfo taskInfosById[] = 
{
	{ 0x191, "AutoAnimation" },
	{ 0x1AA, "GHands" },
	{ 0x3F8, "Fight" },
	{ 0x3F9, "UseGun" },
	{ 0x3F0, "TakeDamage" },
	{ 0x3E8, "KillActorMaybe" },
	{ 0x3FE, "DriveByFreeAim" },
	{ 0x0CB, "ActorOnFoot" },
	{ 0x0CC, "StayPut" },
	{ 0x0D4, "WastedDie" },
	{ 0x0D9, "Wasted" },
	{ 0x0DA, "FallShotDown" },
	{ 0x0DB, "Tired" },
	{ 0x0DF, "SitDown" },
	{ 0x0DD, "SitIdle" },
	{ 0x0E7, "SunBathe" },
	{ 0x0E9, "PedAttrRel1" },
	{ 0x0EB, "InterestingRandom" },
	{ 0x10D, "TriggerLookAt" },
	{ 0x640, "PhoneInOut" },
	{ 0x644, "Goggles" },
	{ 0x10C, "Swim" },
	{ 0x11C, "FallParachute" },
	{ 0x103, "PedAttrRel2" },
	{ 0x104, "PedAttrRel3" },
	{ 0x2C9, "EnterCarDriver" },
	{ 0x2C8, "EnterCarPassenger" },
	{ 0x2C0, "LeaveCar" },
	{ 0x2C5, "DriveCar" },
	{ 0x2C6, "DriveToPoint" },
	{ 0x2B8, "EnterAnyCarDriver" },
	{ 0x2BC, "EnterAnyCarPassenger" },
	{ 0x1F8, "JumpActorForDriveBy" },
	{ 0x06D, "WaterTurretSomething" },
	{ 0x06E, "TreatAccident" },
	{ 0x38C, "FleePoint" },
	{ 0x38D, "FleeEntity" },
	{ 0x38E, "SmartFleePoint" },
	{ 0x38F, "SmartFleeEntity" },
	{ 0x4B1, "GangsRel" },
	{ 0x4B5, "HassleVehicle" },
	{ 0x101, "StareAtPed" },
	{ 0x4BA, "SignalAtPed" },
	{ 0x4BB, "PassObject" },
	{ 0x51D, "Prozzy" },
	{ 0x38B, "FollowActor" },
	{ 0x386, "AchvHeading" },
	{ 0x387, "GoToPoint" },
	{ 0x395, "AvoidOthPed" },
	{ 0x3AB, "AvoidEntity" },
	{ 0x393, "GoToAttractor" },
	{ 0x398, "RotateToActor" },
	{ 0x3A4, "OpenDoor" },
	{ 0x3A7, "InvDisturb" },
	{ 0x389, "ExecutePath" },
	{ 0x57C, "BeInShop" },
	{ 0x580, "SitInChair" },
	{ 0x0D0, "TakeDamageFall" },
	{ 0x0F0, "Fall" },
	{ 0x0D3, "Jump" },
	{ 0x517, "Jetpack" },
	{ 0x4B3, "GreetPartner" },
	{ 0x4B4, "CommandTask" },
	{ 0x4B9, "BeInCouple" },
	{ 0x000, "OnFoot" },
	{ 0x12E, "ShowFinger" },
	{ 0x131, "InitialState" },
	{ 0x132, "ShowFingerNoPed" },
	{ 0x258, "InvDeadPed" },
	{ 0x0C9, "WalkThruDoor" },
	{ 0x2CF, "DriveCarSpecial" },
	{ 0x0CA, "InitialStateInternal" },
	{ 0x2D9, "PedResp" },
	{ 0x2CA, "BailFromCar" },
	{ 0x259, "ThreatResp" },
	{ 0x19F, "Crouch" },
	{ 0x0F4, "GoToPointWithinRadius" },
	{ 0x3A6, "WalkAnimation" },			// This takes an angle
};

tTaskInfo taskInfosByVTbl[] = 
{
	{ 0x86D528, "ScriptAnimation" },
	{ 0x86D570, "PedAnimation" },
	{ 0x86DB70, "HitFromBack" },
	{ 0x86DBA4, "HitFromLeft" },
	{ 0x86DBD8, "HitFromRight" },
	{ 0x86DC10, "HitByGunFromFront" },
	{ 0x86DC4C, "HitByGunFromRear" },
	{ 0x86DC88, "HitByGunFromLeft" },
	{ 0x86DCC4, "HitByGunFromRight" },
	{ 0x86E5EC, "HitFromFront" },
	{ 0x86F168, "HitFromBehind" },
	{ 0x86F1A0, "HitWall" },
	{ 0x86F45C, "Absiel" },
	{ 0x86FDD4, "WalkInSeq" },		// Same ID as GoToPoint
	{ 0x85A0D0, "Cover" },
	{ 0x85A100, "ScratchHead" },
	{ 0x85A134, "UseATM" },
	{ 0x85A164, "LookAbout" },
	{ 0x85A29C, "HandsUp" },
	{ 0x86C78C, "Chat" },

};

const char* GetTaskNameFromTask(DWORD *task)
{
	if (task == NULL)
		return "Null";

	unsigned int vtbl = task[0];
	for(int i=0; i<sizeof(taskInfosByVTbl)/sizeof(tTaskInfo); i++)
	{
		if (taskInfosByVTbl[i].id == vtbl)
			return taskInfosByVTbl[i].name;
	}

	unsigned int type = (unsigned)GetTaskTypeFromTask(task);
	for(int i=0; i<sizeof(taskInfosById)/sizeof(tTaskInfo); i++)
	{
		if (taskInfosById[i].id == type)
			return taskInfosById[i].name;
	}

	return "Unknown";
}

//----------------------------------------------------------

void PrintInfoForTask(char *name, DWORD *task)
{
	CHAR line_buffer[512];

	sprintf( line_buffer, "%s: S: 0x%08X  VT: 0x%08X  T: %s (0x%x)", 
		name,task,task?*task:0,GetTaskNameFromTask(task),GetTaskTypeFromTask(task));
	GameDebugDrawNextLine(line_buffer,D3DCOLOR_ARGB(255,255,255,255));

	while(task = GetNextTaskFromTask(task)) {
		sprintf( line_buffer, "%s: S: 0x%08X  VT: 0x%08X  T: %s (0x%x)", 
			name,task,task?*task:0,GetTaskNameFromTask(task),GetTaskTypeFromTask(task));
		GameDebugDrawNextLine(line_buffer,D3DCOLOR_ARGB(255,255,255,255));
	}
}

//----------------------------------------------------------

void GameDebugDrawTaskInfo()
{
	PED_TYPE *actor=0;
	PED_TYPE *player=0;

	player = GamePool_FindPlayerPed();

	if(player && player->pTarget) {
		dwDebugEntity1 = (DWORD)player->pTarget;
	}

	if(dwDebugEntity1 != 0) {
		actor = (PED_TYPE *)dwDebugEntity1;
	} else {
		actor = player;
	}	

	if (!actor || !actor->Tasks) {
		dwDebugEntity1 = 0;
		return;
	}

	CHAR line_buffer[512];

	// init position for drawing
	debug_draw_top = 50;

	sprintf(line_buffer,"PlayerPed %u (struct: 0x%X, vtbl: 0x%X)",dwDebugEntity1,actor,actor->entity.vtable);
	GameDebugDrawNextLine(line_buffer,D3DCOLOR_ARGB(255,90,90,255));

	GameDebugDrawNextLine("Displaying Ped Intelligence...",D3DCOLOR_ARGB(255,90,90,255));
	GameDebugDrawNextLine("",D3DCOLOR_ARGB(255,255,255,255));

	sprintf(line_buffer, "Ped: struct: 0x%08X,  vtbl: 0x%08X", 
		actor->Tasks->pdwPed, actor->Tasks->pdwPed?*(actor->Tasks->pdwPed):0);
	GameDebugDrawNextLine(line_buffer,D3DCOLOR_ARGB(255,255,255,255));

	GameDebugDrawNextLine("",D3DCOLOR_ARGB(255,255,255,255));
	GameDebugDrawNextLine("Basic Tasks",D3DCOLOR_ARGB(255,255,90,90));
	PrintInfoForTask("Damage",actor->Tasks->pdwDamage);
	PrintInfoForTask("Fall",actor->Tasks->pdwFallEnterExit);
	PrintInfoForTask("SwimWasted",actor->Tasks->pdwSwimWasted);
	PrintInfoForTask("Jump",actor->Tasks->pdwJumpJetPack);
	PrintInfoForTask("Action",actor->Tasks->pdwAction);
	GameDebugDrawNextLine("",D3DCOLOR_ARGB(255,255,255,255));
	GameDebugDrawNextLine("Extended Tasks",D3DCOLOR_ARGB(255,255,90,90));
	PrintInfoForTask("Crouch",actor->Tasks->pdwCrouching);
	PrintInfoForTask("Fight",actor->Tasks->pdwFighting);
	PrintInfoForTask("Unk1",actor->Tasks->pdwExtUnk1);
	PrintInfoForTask("Unk2",actor->Tasks->pdwExtUnk2);
	PrintInfoForTask("Unk3",actor->Tasks->pdwExtUnk3);
	PrintInfoForTask("Unk4",actor->Tasks->pdwExtUnk4);
}

void GameDebugDrawActorInfo()
{

	CHAR line_buffer[512];
	PED_TYPE *actor=0;
	CHAR s[256];

	// init position for drawing
	debug_draw_top = 50;

	if(dwDebugEntity1 != 0) {
		actor = GamePool_Ped_GetAt((dwDebugEntity1 << 8) + 1);
	}
	else {
		actor = GamePool_FindPlayerPed();
	}

	if(actor)
	{		

		sprintf(line_buffer,"PlayerPed %u (entity offset: 0x%X)",dwDebugEntity1,actor);
		GameDebugDrawNextLine(line_buffer,D3DCOLOR_ARGB(255,90,90,255));

		sprintf(line_buffer,"vtbl: 0x%X",actor->entity.vtable);
		GameDebugDrawNextLine(line_buffer,D3DCOLOR_ARGB(150,255,255,255));

		sprintf(line_buffer,"0xB6F178=%f 0xB7CB5C=%f 0x863AC0=%f 0x858FE8=%f",
			*(float *)0xB6F178,*(float *)0xB7CB5C,*(float *)0x863AC0,*(float *)0x858FE8);
        GameDebugDrawNextLine(line_buffer,D3DCOLOR_ARGB(150,255,255,255));

		sprintf(line_buffer,"right: %f %f %f = %f",
			actor->entity.mat->right.X,actor->entity.mat->right.Y,actor->entity.mat->right.Z,
			(actor->entity.mat->right.X * actor->entity.mat->right.X) +
			(actor->entity.mat->right.Y * actor->entity.mat->right.Y) + 
			(actor->entity.mat->right.Z * actor->entity.mat->right.Z));
		GameDebugDrawNextLine(line_buffer,D3DCOLOR_ARGB(150,255,255,255));
		
		sprintf(line_buffer,"up: %f %f %f = %f",
			actor->entity.mat->up.X,actor->entity.mat->up.Y,actor->entity.mat->up.Z,
			(actor->entity.mat->up.X * actor->entity.mat->up.X) +
			(actor->entity.mat->up.Y * actor->entity.mat->up.Y) + 
			(actor->entity.mat->up.Z * actor->entity.mat->up.Z));
		GameDebugDrawNextLine(line_buffer,D3DCOLOR_ARGB(150,255,255,255));

		sprintf(line_buffer,"at: %f %f %f",
			actor->entity.mat->at.X,actor->entity.mat->at.Y,actor->entity.mat->at.Z);
		GameDebugDrawNextLine(line_buffer,D3DCOLOR_ARGB(150,255,255,255));
		
		sprintf(line_buffer,"pos: %f %f %f",
			actor->entity.mat->pos.X,actor->entity.mat->pos.Y,actor->entity.mat->pos.Z);
		GameDebugDrawNextLine(line_buffer,D3DCOLOR_ARGB(150,255,255,255));
			
		sprintf(line_buffer,"Move: %f %f %f",
			actor->entity.vecMoveSpeed.X,actor->entity.vecMoveSpeed.Y,actor->entity.vecMoveSpeed.Z);
		GameDebugDrawNextLine(line_buffer,D3DCOLOR_ARGB(150,255,255,255));

		sprintf(line_buffer,"Turn: %f %f %f",
			actor->entity.vecTurnSpeed.X,actor->entity.vecTurnSpeed.Y,actor->entity.vecTurnSpeed.Z);
		GameDebugDrawNextLine(line_buffer,D3DCOLOR_ARGB(150,255,255,255));
		
		sprintf(line_buffer,"Control Flags: 0x%X",actor->entity.nControlFlags);
		GameDebugDrawNextLine(line_buffer,D3DCOLOR_ARGB(150,255,255,255));

		sprintf(line_buffer,"Model Index: %u",actor->entity.nModelIndex);
		GameDebugDrawNextLine(line_buffer,D3DCOLOR_ARGB(150,255,255,255));

		sprintf(line_buffer,"RW: 0x%X",actor->entity.pdwRenderWare);
		GameDebugDrawNextLine(line_buffer,D3DCOLOR_ARGB(150,255,255,255));
		
		_itoa(actor->dwStateFlags,s,2);
		sprintf(line_buffer,"StateFlags: %s",s);
		GameDebugDrawNextLine(line_buffer,D3DCOLOR_ARGB(150,255,255,255));
		
		_itoa(actor->entity.byteImmunities,s,2);
		sprintf(line_buffer,"Immunities: %s",s);
		GameDebugDrawNextLine(line_buffer,D3DCOLOR_ARGB(150,255,255,255));

		sprintf(line_buffer,"Vehicle: 0x%X Intelligence: 0x%X",actor->pVehicle,actor->Tasks);
		GameDebugDrawNextLine(line_buffer,D3DCOLOR_ARGB(150,255,255,255));

		sprintf(line_buffer,"MovRot1: %f MovRot2: %f ToRot: %f ActRot: %f CamAdj: %f",
			actor->fMoveRot1,actor->fMoveRot2,actor->fRotation1,actor->fRotation2,actor->fRotCamAdjust);
        GameDebugDrawNextLine(line_buffer,D3DCOLOR_ARGB(150,255,255,255));

		sprintf(line_buffer,"Health: %f",actor->fHealth);
		GameDebugDrawNextLine(line_buffer,D3DCOLOR_ARGB(150,255,255,255));

		sprintf(line_buffer,"Action: %u",actor->dwAction);
		GameDebugDrawNextLine(line_buffer,D3DCOLOR_ARGB(150,255,255,255));

		sprintf(line_buffer,"PedType: %u",actor->dwPedType);
		GameDebugDrawNextLine(line_buffer,D3DCOLOR_ARGB(150,255,255,255));

		sprintf(line_buffer,"PedStatPrim: %d PedStat: %d",
			Game_PedStatPrim(actor->entity.nModelIndex),
			Game_PedStat(actor->entity.nModelIndex));

		GameDebugDrawNextLine(line_buffer,D3DCOLOR_ARGB(150,255,255,255));

		/*
		sprintf(line_buffer,"Crouch: 0x%X Fighting: 0x%X PedTask: 0x%X",
			actor->Tasks->pdwCrouching,
			actor->Tasks->pdwFighting,
			actor->Tasks->pdwPed);
		GameDebugDrawNextLine(line_buffer,D3DCOLOR_ARGB(150,255,255,255));*/

		DWORD dwPlayerInfo = actor->dwPlayerInfoOffset;
		float fAimZ;
		DWORD dwAnimSet;

		_asm mov eax, dwPlayerInfo
		_asm mov ebx, [eax+84]
		_asm mov fAimZ, ebx

		_asm mov eax, actor
		_asm mov ebx, [eax+1236]
		_asm mov dwAnimSet, ebx

		if(actor->Tasks->pdwFighting) {
			sprintf(line_buffer,"FightVtbl: 0x%X   FightStruct: 0x%X   PlayerInfoAim: %f",
				*actor->Tasks->pdwFighting,actor->Tasks->pdwFighting,fAimZ);
			GameDebugDrawNextLine(line_buffer,D3DCOLOR_ARGB(150,255,255,255));
		}

		WEAPON_SLOT_TYPE * Weapon = &actor->WeaponSlots[actor->byteCurWeaponSlot];
		sprintf(line_buffer,"Weap: %u Sta: %u Clip: %u Ammo: %u",
			Weapon->dwType, Weapon->dwState, Weapon->dwAmmoInClip, Weapon->dwAmmo );
		GameDebugDrawNextLine(line_buffer,D3DCOLOR_ARGB(150,255,255,255));

		sprintf(line_buffer,"Target: %u Jetpack: %u AnimSet: %u",actor->pTarget,pGame->FindPlayerPed()->IsInJetpackMode(),dwAnimSet);
		GameDebugDrawNextLine(line_buffer,D3DCOLOR_ARGB(150,255,255,255));

		sprintf(line_buffer,"Cam1: %f %f %f = %f",
			pcaInternalAim->f1x,pcaInternalAim->f1y,pcaInternalAim->f1z,
			(pcaInternalAim->f1x * pcaInternalAim->f1x) +
			(pcaInternalAim->f1y * pcaInternalAim->f1y) + 
			(pcaInternalAim->f1z * pcaInternalAim->f1z)
			);
		GameDebugDrawNextLine(line_buffer,D3DCOLOR_ARGB(150,255,255,255));
	
		sprintf(line_buffer,"Cam2: %f %f %f = %f",
			pcaInternalAim->f2x,pcaInternalAim->f2y,pcaInternalAim->f2z,
			(pcaInternalAim->f2x * pcaInternalAim->f2x) +
			(pcaInternalAim->f2y * pcaInternalAim->f2y) + 
			(pcaInternalAim->f2z * pcaInternalAim->f2z)
			);
		GameDebugDrawNextLine(line_buffer,D3DCOLOR_ARGB(150,255,255,255));

		sprintf(line_buffer,"Campos: %f %f %f PosFromPlayer: %f %f %f",
			pcaInternalAim->pos1x,
			pcaInternalAim->pos1y,
			pcaInternalAim->pos1z,
			(pcaInternalAim->pos1x - actor->entity.mat->pos.X),
			(pcaInternalAim->pos1y - actor->entity.mat->pos.Y),
			(pcaInternalAim->pos1z - actor->entity.mat->pos.Z));			

		GameDebugDrawNextLine(line_buffer,D3DCOLOR_ARGB(150,255,255,255));

		float camExZoom = *(float*)0xB6F250;

		sprintf(line_buffer,"Cam Extended Zoom Values: %f", camExZoom);
			
		GameDebugDrawNextLine(line_buffer,D3DCOLOR_ARGB(150,255,255,255));

		//DWORD cam = *(PDWORD)0xB6F028;
		WORD camMode2 = *(WORD*)0xB6F858;
		DWORD camMode3 = *(DWORD*)0xB6F1C4;

		sprintf(line_buffer,"CamMode: %u FarClip: %f HudScaleX: %f HudScaleY: %f",*pbyteCameraMode,fFarClip,*(float *)0x859520,*(float *)0x859524);
		GameDebugDrawNextLine(line_buffer,D3DCOLOR_ARGB(150,255,255,255));

		sprintf(line_buffer,"dwWeaponUsed: %u pdwDamageEnt: 0x%X",actor->dwWeaponUsed,actor->pdwDamageEntity);
		GameDebugDrawNextLine(line_buffer,D3DCOLOR_ARGB(150,255,255,255));

		DWORD dwRet;
		ScriptCommand(&get_active_interior,&dwRet);
		sprintf(line_buffer,"Current Player Interior: %u",dwRet);
		GameDebugDrawNextLine(line_buffer,D3DCOLOR_ARGB(150,255,255,255));

		WORD lr,ud;
		pGame->FindPlayerPed()->GetKeys(&lr,&ud);
		sprintf(line_buffer,"Analog1LR: %d Analog1UD: %d",lr,ud);
		GameDebugDrawNextLine(line_buffer,D3DCOLOR_ARGB(150,255,255,255));
	}
}

//----------------------------------------------------------

void GameDebugDrawVehicleInfo()
{
	CHAR line_buffer[512];
	VEHICLE_TYPE *vehicle=0;
	
	// init position for drawing
	debug_draw_top = 50;

	if(iGameDebugType != 3)
	{
		CVehicle *pVehicle = pNetGame->GetVehiclePool()->GetAt((BYTE)dwDebugEntity1);
		if(pVehicle) {
			vehicle = pVehicle->m_pVehicle;
		}
	}
	else
	{
		// get player's vehicle
		vehicle = (VEHICLE_TYPE *)GamePool_FindPlayerPed()->pVehicle;
	} 

	if(vehicle)
	{
		sprintf(line_buffer,"Vehicle %u",dwDebugEntity1);
		GameDebugDrawNextLine(line_buffer,D3DCOLOR_ARGB(255,255,90,90));
		
		sprintf(line_buffer,"addr: 0x%X",vehicle);
		GameDebugDrawNextLine(line_buffer,D3DCOLOR_ARGB(150,255,255,255));

		sprintf(line_buffer,"vtbl: 0x%X",vehicle->entity.vtable);
		GameDebugDrawNextLine(line_buffer,D3DCOLOR_ARGB(150,255,255,255));

		sprintf(line_buffer,"Roll: %f %f %f",
			vehicle->entity.mat->right.X,vehicle->entity.mat->right.Y,vehicle->entity.mat->right.Z);
		GameDebugDrawNextLine(line_buffer,D3DCOLOR_ARGB(150,255,255,255));
		
		sprintf(line_buffer,"Direction: %f %f %f",
			vehicle->entity.mat->up.X,vehicle->entity.mat->up.Y,vehicle->entity.mat->up.Z);
		GameDebugDrawNextLine(line_buffer,D3DCOLOR_ARGB(150,255,255,255));

		sprintf(line_buffer,"Scale: %f %f %f",
			vehicle->entity.mat->at.X,vehicle->entity.mat->at.Y,vehicle->entity.mat->at.Z);
		GameDebugDrawNextLine(line_buffer,D3DCOLOR_ARGB(150,255,255,255));
		
		sprintf(line_buffer,"Position: %f %f %f",
			vehicle->entity.mat->pos.X,vehicle->entity.mat->pos.Y,vehicle->entity.mat->pos.Z);
		GameDebugDrawNextLine(line_buffer,D3DCOLOR_ARGB(150,255,255,255));
		
		float fZRot1 = atan2(-vehicle->entity.mat->up.X, vehicle->entity.mat->up.Y) * 180.0f/3.1415926536f;
		
		sprintf(line_buffer,"Z Rot: %f degrees", fZRot1);
		GameDebugDrawNextLine(line_buffer,D3DCOLOR_ARGB(150,255,255,255));

		sprintf(line_buffer,"Move: %f %f %f",
			vehicle->entity.vecMoveSpeed.X,vehicle->entity.vecMoveSpeed.Y,vehicle->entity.vecMoveSpeed.Z);
		GameDebugDrawNextLine(line_buffer,D3DCOLOR_ARGB(150,255,255,255));

		sprintf(line_buffer,"Turn: %f %f %f",
			vehicle->entity.vecTurnSpeed.X,vehicle->entity.vecTurnSpeed.Y,vehicle->entity.vecTurnSpeed.Z);
		GameDebugDrawNextLine(line_buffer,D3DCOLOR_ARGB(150,255,255,255));
		
		sprintf(line_buffer,"Control Flags: 0x%X",vehicle->entity.nControlFlags);
		GameDebugDrawNextLine(line_buffer,D3DCOLOR_ARGB(150,255,255,255));

		sprintf(line_buffer,"Model Index: %u",vehicle->entity.nModelIndex);
		GameDebugDrawNextLine(line_buffer,D3DCOLOR_ARGB(150,255,255,255));
		
		sprintf(line_buffer,"Renderware: 0x%X",vehicle->entity.pdwRenderWare);
		GameDebugDrawNextLine(line_buffer,D3DCOLOR_ARGB(150,255,255,255));

		sprintf(line_buffer,"dwUnkModelRel: 0x%X",vehicle->entity.dwUnkModelRel);
		GameDebugDrawNextLine(line_buffer,D3DCOLOR_ARGB(150,255,255,255));

		char s[256];

		_itoa(vehicle->entity.byteImmunities,s,2);
		sprintf(line_buffer,"Immunities: %s",s);
		GameDebugDrawNextLine(line_buffer,D3DCOLOR_ARGB(150,255,255,255));
		
		sprintf(line_buffer,"Driver: 0x%X",vehicle->pDriver);
		GameDebugDrawNextLine(line_buffer,D3DCOLOR_ARGB(150,255,255,255));
		
		sprintf(line_buffer,"Health: %f",vehicle->fHealth);
		GameDebugDrawNextLine(line_buffer,D3DCOLOR_ARGB(150,255,255,255));
			
		sprintf(line_buffer,"mat: 0x%X",vehicle->entity.mat);
		GameDebugDrawNextLine(line_buffer,D3DCOLOR_ARGB(150,255,255,255));

		sprintf(line_buffer,"RadioPed: 0x%X 0x%X",*(PDWORD)0xB6B98C,*(PDWORD)0xB6B99C);
		GameDebugDrawNextLine(line_buffer,D3DCOLOR_ARGB(150,255,255,255));

		sprintf(line_buffer,"Flags: 0x%X",vehicle->byteFlags);
		GameDebugDrawNextLine(line_buffer,D3DCOLOR_ARGB(150,255,255,255));

		sprintf(line_buffer,"Steer1: %f",vehicle->fSteerAngle1);
		GameDebugDrawNextLine(line_buffer,D3DCOLOR_ARGB(150,255,255,255));
		
		sprintf(line_buffer,"Steer2: %f",vehicle->fSteerAngle2);
		GameDebugDrawNextLine(line_buffer,D3DCOLOR_ARGB(150,255,255,255));
				
		sprintf(line_buffer,"Accel: %f",vehicle->fAcceleratorPedal);
		GameDebugDrawNextLine(line_buffer,D3DCOLOR_ARGB(150,255,255,255));

		sprintf(line_buffer,"Brake: %f",vehicle->fBrakePedal);
		GameDebugDrawNextLine(line_buffer,D3DCOLOR_ARGB(150,255,255,255));
		
		sprintf(line_buffer,"Hydra: %u",vehicle->dwHydraThrusters);
		GameDebugDrawNextLine(line_buffer,D3DCOLOR_ARGB(150,255,255,255));

		sprintf(line_buffer,"Color1: %u",vehicle->byteColor1);
		GameDebugDrawNextLine(line_buffer,D3DCOLOR_ARGB(150,255,255,255));
		
		sprintf(line_buffer,"Color2: %u",vehicle->byteColor2);
		GameDebugDrawNextLine(line_buffer,D3DCOLOR_ARGB(150,255,255,255));

		sprintf(line_buffer,"Siren On: %u, Horn: %d %d %d %d, Landing Gear: %f", vehicle->bSirenOn, vehicle->byteHorn, 
			vehicle->iHornLevel, vehicle->byteHorn2, vehicle->iSirenLevel, vehicle->fPlaneLandingGear);

		GameDebugDrawNextLine(line_buffer,D3DCOLOR_ARGB(150,255,255,255));

		BYTE byte164;
		BYTE byte165;
		BYTE byte166;
		BYTE byte167;
		BYTE byte168;

		_asm mov eax, vehicle
		_asm lea edx, [eax+312]

		_asm mov al, [edx+164]
		_asm mov byte164, al
		_asm mov al, [edx+165]
		_asm mov byte165, al
		_asm mov al, [edx+166]
		_asm mov byte166, al
		_asm mov al, [edx+167]
		_asm mov byte167, al
		_asm mov al, [edx+168]
		_asm mov byte168, al

			sprintf(line_buffer,"164:%u 165:%u 166:%u 167:%u 168:%u",byte164,byte165,byte166,byte167,byte168);
		GameDebugDrawNextLine(line_buffer,D3DCOLOR_ARGB(150,255,255,255));
	}	
}

//----------------------------------------------------------

void GameDebugInitTextScreen()
{
	int x=0;
	while(x != NUM_SCREEN_LINES)
	{
		screen_buf[x] = (PCHAR)malloc(256);
		memset(screen_buf[x],0,256);
		x++;
	}
}

//----------------------------------------------------------

void GameDebugAddMessage(char *szFormat, ...)
{
	char tmp_buf[512];
	int x=0;

	// free the last message
	free(screen_buf[0]);
	// shift the rest down
	while(x!=(NUM_SCREEN_LINES - 1)) {
		screen_buf[x] = screen_buf[x + 1];
		x++;
	}

	va_list args;
	va_start(args, szFormat);
	vsprintf(tmp_buf, szFormat, args);
	va_end(args);

	// move in the new line
	screen_buf[NUM_SCREEN_LINES - 1] = (PCHAR)malloc(256);
	strcpy(screen_buf[NUM_SCREEN_LINES - 1],tmp_buf);	
}

//----------------------------------------------------------

void GameDrawDebugTextInfo()
{
	int x=0;

	while(x!=NUM_SCREEN_LINES)
	{
		GameDebugDrawNextLine(screen_buf[x],D3DCOLOR_ARGB(255,255,255,255));
		x++;
	}
}

//----------------------------------------------------------

void GameDrawMemoryInfo()
{

	CHAR line_buffer[512];
	BYTE memory_data[512];
	int x=0,y=0;
	
	// init position for drawing
	debug_draw_top = 20;

	sprintf(line_buffer,"Displaying 512 Bytes From Address 0x%X + %u",dwDebugEntity1,dwDebugEntity2);
	GameDebugDrawNextLine(line_buffer,D3DCOLOR_ARGB(255,255,10,10));

	if(IsBadReadPtr((PVOID)(dwDebugEntity1+dwDebugEntity2),512)) return;
	memcpy(memory_data,(PVOID)(dwDebugEntity1+dwDebugEntity2),512);

	while(x!=32)
	{
		sprintf(line_buffer,"%.3u: %.2X %.2X %.2X %.2X %.2X %.2X %.2X %.2X %.2X %.2X %.2X %.2X %.2X %.2X %.2X %.2X",
			((x*16)+dwDebugEntity2),
			memory_data[y],
			memory_data[y+1],
			memory_data[y+2],
			memory_data[y+3],
			memory_data[y+4],
			memory_data[y+5],
			memory_data[y+6],
			memory_data[y+7],
			memory_data[y+8],
			memory_data[y+9],
			memory_data[y+10],
			memory_data[y+11],
			memory_data[y+12],
			memory_data[y+13],
			memory_data[y+14],
			memory_data[y+15]	
			);

		y+=16;

		GameDebugDrawNextLine(line_buffer,D3DCOLOR_ARGB(150,255,255,255));

		x++;
	}
}

//----------------------------------------------------------

void GameDrawMemoryInfoAscii()
{

	CHAR line_buffer[512];
	BYTE memory_data[512];
	int x=0,y=0;
	
	// init position for drawing
	debug_draw_top = 20;

	sprintf(line_buffer,"Displaying 512 Bytes From Address 0x%X + %u",dwDebugEntity1,dwDebugEntity2);
	GameDebugDrawNextLine(line_buffer,D3DCOLOR_ARGB(255,255,10,10));

	if(IsBadReadPtr((PVOID)(dwDebugEntity1+dwDebugEntity2),512)) return;
	memcpy(memory_data,(PVOID)(dwDebugEntity1+dwDebugEntity2),512);

	while(x!=32)
	{
		sprintf(line_buffer,"%.3u: %c %c %c %c %c %c %c %c %c %c %c %c %c %c %c %c",
			((x*16)+dwDebugEntity2),
			memory_data[y],
			memory_data[y+1],
			memory_data[y+2],
			memory_data[y+3],
			memory_data[y+4],
			memory_data[y+5],
			memory_data[y+6],
			memory_data[y+7],
			memory_data[y+8],
			memory_data[y+9],
			memory_data[y+10],
			memory_data[y+11],
			memory_data[y+12],
			memory_data[y+13],
			memory_data[y+14],
			memory_data[y+15]	
			);

		y+=16;

		GameDebugDrawNextLine(line_buffer,D3DCOLOR_ARGB(150,255,255,255));

		x++;
	}
}

//--------------------------------------------------------------

int sync_inited=0;
CVehicle * pv[2];
CPlayerPed * pActor;
CPlayerPed * pPlayer;
WORD w1,w2,w3;
BOOL bOffFrame=FALSE;

void GameDoVehicleSyncTest()
{
	/*
	if(!sync_inited) {

		pv[0] = pNetGame->GetVehiclePool()->GetAt(1);
		pv[1] = pNetGame->GetVehiclePool()->GetAt(2);
		pActor = pGame->NewPlayer(3,190,1655.105f,-1044.467f,23.61058f,0.0f);

		pActor->PutDirectlyInVehicle(pv[1]->m_dwGTAId);
		pGame->FindPlayerPed()->PutDirectlyInVehicle(pv[0]->m_dwGTAId);

		//ScriptCommand(&create_actor,24,0,1655.105f,-1044.467f,23.61058f,&iActor);
		//ScriptCommand(&put_actor_in_car,iActor,pv[1]->m_dwGTAId);
		sync_inited++;
		return;
	}

	if(bDebugUpdate) {

		MATRIX4X4 matV;

		pv[1]->GetMatrix(&matV);

		matV.right.X = DebugSync.vecRoll.X;
		matV.right.Y = DebugSync.vecRoll.Y;
		matV.right.Z = DebugSync.vecRoll.Z;
		matV.up.X = DebugSync.vecDirection.X;
		matV.up.Y = DebugSync.vecDirection.Y;
		matV.up.Z = DebugSync.vecDirection.Z;
		matV.pos.X = DebugSync.vecPos.X;
		matV.pos.Y = DebugSync.vecPos.Y;
		matV.pos.Z = DebugSync.vecPos.Z;

		matV.pos.X -= 5.0f;
		matV.pos.Y -= 5.0f;

		pv[1]->m_pVehicle->byteFlags = pv[0]->m_pVehicle->byteFlags;

		pv[1]->SetMatrixAndSpeedForUpdate(matV,DebugSync.vecMoveSpeed);
		pActor->SetKeys(DebugSync.wKeys,DebugSync.lrAnalog,DebugSync.udAnalog);

		bDebugUpdate = FALSE;
	}

	/*
	pv[1]->m_pVehicle->steer_angle1=pv[0]->m_pVehicle->steer_angle1;
	pv[1]->m_pVehicle->accelerator_pedal=pv[0]->m_pVehicle->accelerator_pedal;
	pv[1]->m_pVehicle->brake_pedal=pv[0]->m_pVehicle->brake_pedal;
	pv[1]->m_pVehicle->flags=pv[0]->m_pVehicle->flags;*/

}

#endif // _DEBUG

//----------------------------------------------------------
// Switches on the debug screen for debugging raw vehicles/actors

void GameDebugEntity(DWORD dwEnt1, DWORD dwEnt2, int type)
{
	iGameDebugType=type;
	dwDebugEntity1=dwEnt1;
	dwDebugEntity2=dwEnt2;
}

//----------------------------------------------------------
// Switches off any driver debug screen or sync sequence

void GameDebugScreensOff()
{
	iGameDebugType=0;
}

//----------------------------------------------------------

BOOL bSelVehicleInit=FALSE;
CVehicle *pVehicle=NULL;
int iSelection = 400;
GTA_CONTROLSET *pControls;
CCamera *pCam;
char sz2[256];

void GameBuildRecreateVehicle()
{
	if(pVehicle) delete pVehicle;

	CHAR blank[2] = "";
	pVehicle = pGame->NewVehicle(iSelection,5.0f,5.0f,500.0f,0.0f,(PCHAR)blank);
	pVehicle->Add();
}

void GameBuildSelectVehicle()
{
	if(!bSelVehicleInit) {
		pControls = GameGetInternalKeys();
		pCam = pGame->GetCamera();
		pCam->SetPosition(-4.0f,-4.0f,502.0f,0.0f,0.0f,0.0f);
		pCam->LookAtPoint(5.0f,5.0f,500.0f,1);
		pGame->FindPlayerPed()->TogglePlayerControllable(0);
		pGame->DisplayGameText("Vehicle Select",4000,6);
		GameBuildRecreateVehicle();
		bSelVehicleInit = TRUE;
	}

	pGame->DisplayHud(FALSE);

	if(pVehicle && pVehicle->m_pEntity) {
		VECTOR vecTurn = { 0.0f, 0.0f, 0.03f };
		VECTOR vecMove = { 0.0f, 0.0f, 0.0f };
		pVehicle->SetTurnSpeedVector(vecTurn);
		pVehicle->SetMoveSpeedVector(vecMove);

		MATRIX4X4 mat;
		pVehicle->GetMatrix(&mat);
		mat.pos.X = 5.0f;
		mat.pos.Y = 5.0f;
		mat.pos.Z = 500.0f;
		pVehicle->SetMatrix(mat);
	}

	if(pControls->wKeys1[14] && !pControls->wKeys2[14]) {
		iSelection--;
		if(iSelection==538) iSelection-=2; // trains
		if(iSelection==399) iSelection=611;
		GameBuildRecreateVehicle();
		return;
	}
	
	if(pControls->wKeys1[16] && !pControls->wKeys2[16]) {
		iSelection++;
		if(iSelection==537) iSelection+=2;  // trains
		if(iSelection==612) iSelection=400;
		GameBuildRecreateVehicle();
		return;
	}
		
	if(pControls->wKeys1[15] && !pControls->wKeys2[15]) {

		delete pVehicle;
		pVehicle = NULL;

		pCam->SetBehindPlayer();
		GameDebugEntity(0,0,0);

		// place this vehicle near the player.
		CPlayerPed *pPlayer = pGame->FindPlayerPed();

		if(pPlayer) 
		{
			MATRIX4X4 matPlayer;
			pPlayer->GetMatrix(&matPlayer);
			CHAR blank[9] = "";
			sprintf(blank, "TYPE_%d", iSelection);
			CVehicle *pTestVehicle = pGame->NewVehicle(iSelection,
				(matPlayer.pos.X - 5.0f), (matPlayer.pos.Y - 5.0f),
				matPlayer.pos.Z+1.0f,0.0f,(PCHAR)blank);

			pTestVehicle->Add();
			pPlayer->PutDirectlyInVehicle(pTestVehicle->m_dwGTAId,0);
			if(iSelection == 464) {
				pPlayer->Remove();
			}
		}

		pCam->Restore();
		pCam->SetBehindPlayer();	
		pGame->FindPlayerPed()->TogglePlayerControllable(1);
		pGame->DisplayHud(TRUE);
		bSelVehicleInit=FALSE;

		return;
	}
}

//----------------------------------------------------------

void GameDebugDrawDebugScreens()
{
	if(!iGameDebugType) return;

	//if(pCmdWindow->isEnabled()) return;

#ifdef _DEBUG

	if(iGameDebugType==1)
	{
		GameDebugDrawActorInfo();
		return;
	}
	
	if(iGameDebugType==2 || iGameDebugType==3)
	{
		GameDebugDrawVehicleInfo();
		return;
	}

	if(iGameDebugType==5)
	{
		GameDrawMemoryInfo();
		return;
	}

	if(iGameDebugType==15)
	{
		GameDrawMemoryInfoAscii();
		return;
	}

	if(iGameDebugType==6)
	{
		GameDrawDebugTextInfo();
		return;
	}

	if(iGameDebugType==7)
	{
		GameDoVehicleSyncTest();
		return;
	}

	if (iGameDebugType==8)
	{
		GameDebugDrawTaskInfo();
		return;
	}

#endif //_DEBUG

	if(iGameDebugType==10)
	{
		GameBuildSelectVehicle();
		return;
	}
}

//----------------------------------------------------------