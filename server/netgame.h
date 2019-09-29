/*

	SA:MP Multiplayer Modification
	Copyright 2004-2005 SA:MP Team

    Version: $Id: netgame.h,v 1.26 2006/04/15 18:58:21 spookie Exp $

*/

#ifndef SAMPSRV_NETGAME_H
#define SAMPSRV_NETGAME_H

//----------------------------------------------------

#define IS_FIRING(x) (x & 0x200) // for checking the keystate firing bit

//----------------------------------------------------
#define MAX_SPAWNS 500

#define UPDATE_TYPE_NONE	0
#define UPDATE_TYPE_FULL	1
#define UPDATE_TYPE_MINIMAL 2

#define GAMESTATE_STOPPED	 0
#define GAMESTATE_RUNNING	 1
#define GAMESTATE_RESTARTING 2

#include "main.h"
#include "player.h"
#include "playerpool.h"
#include "vehicle.h"
#include "vehiclepool.h"
#include "../raknet/PacketEnumerations.h"
#include "netrpc.h"

#define INVALID_ID			0xFF

// 0b0000 0000 0000 0000 0111 1110 0111 1111 1111 1111 1100 0111 1111 1111 1111 1111
//                       47   43   39   35   31   27   23   19   15   11   7    3

#define DEFAULT_WEAPONS 0x00007E7FFFC7FFFFLL

//----------------------------------------------------

class CNetGame
{
private:
	RakServerInterface			*m_pRak;

	CPlayerPool					*m_pPlayerPool;
	CVehiclePool				*m_pVehiclePool;
	CPickupPool					*m_pPickupPool;
	CObjectPool					*m_pObjectPool;
	CGameMode					*m_pGameMode;
	CFilterScripts				*m_pFilterScripts;
	CMenuPool					*m_pMenuPool;
	CTextDrawPool				*m_pTextPool;
	CGangZonePool				*m_pGangZonePool;

    int							m_iCurrentGameModeIndex;
	int							m_iCurrentGameModeRepeat;
	BOOL						m_bFirstGameModeLoaded;

	BOOL						m_bLanMode;
	BOOL						m_bACEnabled;

	void UpdateNetwork();
	CScriptTimers* m_pScriptTimers;
	
public:

	CScriptTimers* GetTimers() { return m_pScriptTimers; };

	bool m_bShowPlayerMarkers;
	bool m_bShowNameTags;
	bool m_bTirePopping;
	BOOL IsACEnabled() { return m_bACEnabled; }
	BYTE m_byteWorldTime;
	bool m_bAllowWeapons; // Allow weapons in interiors
	bool m_bStuntBonus; // Insane stunt bonusses enabled?
	BYTE m_byteWeather;
	int	 m_iGameState;
	float m_fGravity;
	int  m_iDeathDropMoney;
	BOOL m_bAdminTeleport;
	bool m_bZoneNames;
	BYTE m_byteMod;
	bool m_bLimitGlobalChatRadius; // limit global player chat to other players within a certain radius
	bool m_bUseCJWalk;
	float m_fGlobalChatRadius; // limit global chat radius
	float m_fNameTagDrawDistance; // The distance which players will start rendering nametags
	bool m_bDisableEnterExits; // Interior enter/exits disabled?
	
	long long m_longSynchedWeapons;
	
	#ifndef WIN32
		double m_dElapsedTime;
	#endif

	CNetGame();
	~CNetGame();

	void Init(BOOL bFirst);
	void ShutdownForGameModeRestart();
	void ReInitWhenRestarting();
	BOOL SetNextScriptFile(char *szFile);
	
	int GetGameState() { return m_iGameState; };

	CPlayerPool * GetPlayerPool() { return m_pPlayerPool; };
	CVehiclePool * GetVehiclePool() { return m_pVehiclePool; };
	CPickupPool * GetPickupPool() { return m_pPickupPool; };
	CObjectPool	* GetObjectPool() { return m_pObjectPool; };
	RakServerInterface * GetRakServer() { return m_pRak; };
	CGameMode * GetGameMode() { return m_pGameMode; };
	CFilterScripts * GetFilterScripts() { return m_pFilterScripts; };
	CMenuPool * GetMenuPool() { return m_pMenuPool; };
	CTextDrawPool * GetTextDrawPool() { return m_pTextPool; };
	CGangZonePool * GetGangZonePool() { return m_pGangZonePool; };

	void ProcessClientJoin(BYTE bytePlayerID);

	void SendClientMessage(PlayerID pidPlayer, DWORD dwColor, char* szMessage, ...);
	void SendClientMessageToAll(DWORD dwColor, char* szMessage, ...);
	void InitGameForPlayer(BYTE bytePlayerID);
	void MasterServerAnnounce(float fElapsedTime);
	char *GetNextScriptFile();
	void LoadAllFilterscripts();
	
	void Process();

	int GetBroadcastSendRateFromPlayerDistance(float fDistance);

	void BroadcastData( RakNet::BitStream *bitStream, PacketPriority priority,
						PacketReliability reliability,
						char orderingStream,
						BYTE byteExcludedPlayer,
						BOOL bBroadcastLocalRangeOnly = FALSE,
						BOOL bAimSync = FALSE );

	void BroadcastDistanceRPC( char *szUniqueID, 
							   RakNet::BitStream *bitStream,
							   PacketReliability reliability,
							   BYTE byteExcludedPlayer,
							   float fUseDistance );

	void AdjustAimSync(RakNet::BitStream *bitStream, BYTE byteTargetPlayerID, RakNet::BitStream *adjbitStream);

	// Packet Handlers
	void Packet_AimSync(Packet *p);
	void Packet_PlayerSync(Packet *p);
	void Packet_VehicleSync(Packet *p);
	void Packet_PassengerSync(Packet *p);
	void Packet_SpectatorSync(Packet *p);
	void Packet_NewIncomingConnection(Packet* packet);
	void Packet_DisconnectionNotification(Packet* packet);
	void Packet_ConnectionLost(Packet* packet);
	void Packet_ModifiedPacket(Packet* packet);
	void Packet_RemotePortRefused(Packet* packet);
	void Packet_InGameRcon(Packet* packet);
	void Packet_StatsUpdate(Packet *p);
	void Packet_WeaponsUpdate(Packet *p);
	void Packet_TrailerSync(Packet *p);


	void KickPlayer(BYTE byteKickPlayer);
	void AddBan(char * nick, char * ip_mask, char * reason);
	void RemoveBan(char * ip_mask);
	void LoadBanList();
		
	BOOL IsLanMode() { return m_bLanMode; };

	// CLASS SYSTEM
	int					m_iSpawnsAvailable;
	PLAYER_SPAWN_INFO	m_AvailableSpawns[MAX_SPAWNS];
	int AddSpawn(PLAYER_SPAWN_INFO *pSpawnInfo);

	void SetWorldTime(BYTE byteHour);
	void SetWeather(BYTE byteWeather);
	void SetGravity(float fGravity);
	void UpdateInstagib();
	const PCHAR GetWeaponName(int iWeaponID);
};

//----------------------------------------------------

#define WEAPON_BRASSKNUCKLE				1
#define WEAPON_GOLFCLUB					2
#define WEAPON_NITESTICK				3
#define WEAPON_KNIFE					4
#define WEAPON_BAT						5
#define WEAPON_SHOVEL					6
#define WEAPON_POOLSTICK				7
#define WEAPON_KATANA					8
#define WEAPON_CHAINSAW					9
#define WEAPON_DILDO					10
#define WEAPON_DILDO2					11
#define WEAPON_VIBRATOR					12
#define WEAPON_VIBRATOR2				13
#define WEAPON_FLOWER					14
#define WEAPON_CANE						15
#define WEAPON_GRENADE					16
#define WEAPON_TEARGAS					17
#define WEAPON_MOLTOV					18
#define WEAPON_COLT45					22
#define WEAPON_SILENCED					23
#define WEAPON_DEAGLE					24
#define WEAPON_SHOTGUN					25
#define WEAPON_SAWEDOFF					26
#define WEAPON_SHOTGSPA					27
#define WEAPON_UZI						28
#define WEAPON_MP5						29
#define WEAPON_AK47						30
#define WEAPON_M4						31
#define WEAPON_TEC9						32
#define WEAPON_RIFLE					33
#define WEAPON_SNIPER					34
#define WEAPON_ROCKETLAUNCHER			35
#define WEAPON_HEATSEEKER				36
#define WEAPON_FLAMETHROWER				37
#define WEAPON_MINIGUN					38
#define WEAPON_SATCHEL					39
#define WEAPON_BOMB						40
#define WEAPON_SPRAYCAN					41
#define WEAPON_FIREEXTINGUISHER			42
#define WEAPON_CAMERA					43
#define WEAPON_PARACHUTE				46
#define WEAPON_VEHICLE					49
#define WEAPON_DROWN					53
#define WEAPON_COLLISION				54

//----------------------------------------------------

#endif

