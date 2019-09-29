//----------------------------------------------------------
//
// SA:MP Multiplayer Modification For GTA:SA
// Copyright 2004-2005 SA:MP team
//
// Version: $Id: netgame.cpp,v 1.60 2006/05/21 11:28:29 kyeman Exp $
//
//----------------------------------------------------------

#include "../main.h"

#define NETGAME_VERSION 5511

ONFOOT_SYNC_DATA ofSync;
bool	bSpawned = false;

DWORD dwLastPedSync;
DWORD dwLastSay;

//----------------------------------------------------

BYTE GetPacketID(Packet *p)
{
	if (p==0) return 255;

	if ((unsigned char)p->data[0] == ID_TIMESTAMP) {
		assert(p->length > sizeof(unsigned char) + sizeof(unsigned long));
		return (unsigned char) p->data[sizeof(unsigned char) + sizeof(unsigned long)];
	}
	else {
		return (unsigned char) p->data[0];
	}
}

//----------------------------------------------------

CNetGame::CNetGame(PCHAR szHostOrIp, int iPort, 
				   PCHAR szPlayerName, PCHAR szPass)
{
	strcpy(m_szHostName, "San Andreas Multiplayer");
	strncpy(m_szHostOrIp, szHostOrIp, sizeof(m_szHostOrIp));
	m_iPort = iPort;

	// Setup player pool
	m_pPlayerPool = new CPlayerPool();
	m_pPlayerPool->SetLocalPlayerName(szPlayerName);
	m_pVehiclePool = new CVehiclePool();

	m_pRakClient = RakNetworkFactory::GetRakClientInterface();

	RegisterRPCs(m_pRakClient);
	RegisterScriptRPCs(m_pRakClient);	// Register server-side scripting RPCs.
	m_pRakClient->SetPassword(szPass);
	m_pRakClient->Connect(szHostOrIp,iPort,0,0,50);
	
	m_iGameState = GAMESTATE_CONNECTING;

	char s[256];
	sprintf(s,"Bot(%s): connecting to %s:%d...",szPlayerName,szHostOrIp,iPort);
	OutputDebugString(s);

	m_iSpawnsAvailable = 0;
	m_byteWorldTime = 12;
	m_byteWorldMinute = 0;
	m_byteWeather	= 10;
	m_fGravity = (float)0.008000000;
	m_iDeathDropMoney = 0;
	m_bLanMode = FALSE;
	m_byteHoldTime = 1;
	m_bUseCJWalk = FALSE;

	int i;
	for (i = 0; i < 32; i++) m_dwMapIcon[i] = NULL;

	m_byteFriendlyFire = 1;
	m_bZoneNames = FALSE;
	m_bInstagib = FALSE;

	memset(&ofSync,0,sizeof(ONFOOT_SYNC_DATA));
	dwLastPedSync = GetTickCount();
	dwLastSay = GetTickCount();
}

//----------------------------------------------------

CNetGame::~CNetGame()
{
	m_pRakClient->Disconnect(0);
	UnRegisterRPCs(m_pRakClient);
	UnRegisterScriptRPCs(m_pRakClient);	// Unregister server-side scripting RPCs.
	RakNetworkFactory::DestroyRakClientInterface(m_pRakClient);
	SAFE_DELETE(m_pPlayerPool);
	SAFE_DELETE(m_pVehiclePool);
}

//----------------------------------------------------

void CNetGame::ShutdownForGameModeRestart()
{
	m_byteWorldTime = 12;
	m_byteWorldMinute = 0;
	m_byteWeather	= 10;
	m_byteHoldTime = 1;
	m_bUseCJWalk = FALSE;
	m_fGravity = (float)0.008000000;
	m_iDeathDropMoney = 0;

	m_iGameState = GAMESTATE_RESTARTING;

	// Disable the ingame players and reset the vehicle pool.
	m_pPlayerPool->DeactivateAll();
	
	// Process the pool one last time
	m_pPlayerPool->Process();

	ResetVehiclePool();
	m_bZoneNames = FALSE;
}

//----------------------------------------------------


void CNetGame::Process()
{	
	UpdateNetwork();

	if( (GetTickCount() - dwLastPedSync) > 200 ) {
		RakNet::BitStream bsPlayerSync;
		bsPlayerSync.Write((BYTE)ID_PLAYER_SYNC);
		bsPlayerSync.Write((PCHAR)&ofSync,sizeof(ONFOOT_SYNC_DATA));
		GetRakClient()->Send(&bsPlayerSync,HIGH_PRIORITY,UNRELIABLE,0);
		dwLastPedSync = GetTickCount();
	}

	if( (GetTickCount() - dwLastSay) > 60000 ) {
		char * szText = "Kick me coz I'm lame!";
		BYTE byteTextLen = strlen(szText);

		RakNet::BitStream bsSend;
		bsSend.Write(byteTextLen);
		bsSend.Write(szText,byteTextLen);

		GetRakClient()->RPC(RPC_Chat,&bsSend,HIGH_PRIORITY,RELIABLE,0,false);
		dwLastSay = GetTickCount();
	}
}

//----------------------------------------------------
// UPDATE NETWORK
//----------------------------------------------------

void CNetGame::UpdateNetwork()
{
	Packet* pkt=NULL;
	unsigned char packetIdentifier;

	while((pkt = m_pRakClient->Receive()))
	{
		packetIdentifier = GetPacketID(pkt);

		switch(packetIdentifier)
		{
		case ID_RSA_PUBLIC_KEY_MISMATCH:
			Packet_RSAPublicKeyMismatch(pkt);
			break;
		case ID_CONNECTION_BANNED:
			Packet_ConnectionBanned(pkt);
			break;
		case ID_NO_FREE_INCOMING_CONNECTIONS:
			Packet_NoFreeIncomingConnections(pkt);
			break;
		case ID_DISCONNECTION_NOTIFICATION:
			Packet_DisconnectionNotification(pkt);
			break;
		case ID_CONNECTION_LOST:
			Packet_ConnectionLost(pkt);
			break;
		case ID_INVALID_PASSWORD:
			Packet_InvalidPassword(pkt);
			break;
		case ID_MODIFIED_PACKET:
			Packet_ModifiedPacket(pkt);
			break;
		case ID_CONNECTION_ATTEMPT_FAILED: 
			Packet_ConnectAttemptFailed(pkt); 
			break; 
		case ID_CONNECTION_REQUEST_ACCEPTED:
			Packet_ConnectionSucceeded(pkt);
			break;
		case ID_PLAYER_SYNC:
			Packet_PlayerSync(pkt);
			break;
		case ID_VEHICLE_SYNC:
			Packet_VehicleSync(pkt);
			break;
		case ID_PASSENGER_SYNC:
			Packet_PassengerSync(pkt);
			break;
		case ID_AIM_SYNC:
			Packet_AimSync(pkt);
			break;
		case ID_TRAILER_SYNC:
			Packet_TrailerSync(pkt);
			break;
		}

		m_pRakClient->DeallocatePacket(pkt);		
	}

}

//----------------------------------------------------
// PACKET HANDLERS INTERNAL
//----------------------------------------------------

void CNetGame::Packet_PlayerSync(Packet *p)
{
	RakNet::BitStream bsPlayerSync((PCHAR)p->data, p->length, false);
	ONFOOT_SYNC_DATA ofSync;
	BYTE bytePacketID=0;
	BYTE bytePlayerID=0;
	
	bool bHasLR,bHasUD;
	bool bMoveSpeedX,bMoveSpeedY,bMoveSpeedZ;
	bool bHasVehicleSurfingInfo;

	if(GetGameState() != GAMESTATE_CONNECTED) return;

	memset(&ofSync,0,sizeof(ONFOOT_SYNC_DATA));

	bsPlayerSync.Read(bytePacketID);
	bsPlayerSync.Read(bytePlayerID);

	// LEFT/RIGHT KEYS
	bsPlayerSync.Read(bHasLR);
	if(bHasLR) bsPlayerSync.Read(ofSync.lrAnalog);
	
	// UP/DOWN KEYS
	bsPlayerSync.Read(bHasUD);
	if(bHasUD) bsPlayerSync.Read(ofSync.udAnalog);

	// GENERAL KEYS
	bsPlayerSync.Read(ofSync.wKeys);

	// VECTOR POS
	bsPlayerSync.Read((char*)&ofSync.vecPos,sizeof(VECTOR));

	// ROTATION
	bsPlayerSync.Read(ofSync.fRotation);

	// HEALTH/ARMOUR (COMPRESSED INTO 1 BYTE)
	BYTE byteHealthArmour;
	BYTE byteArmTemp=0,byteHlTemp=0;

	bsPlayerSync.Read(byteHealthArmour);
	byteArmTemp = (byteHealthArmour & 0x0F);
	byteHlTemp = (byteHealthArmour >> 4);

	if(byteArmTemp == 0xF) ofSync.byteArmour = 100;
	else if(byteArmTemp == 0) ofSync.byteArmour = 0;
	else ofSync.byteArmour = byteArmTemp * 7;

	if(byteHlTemp == 0xF) ofSync.byteHealth = 100;
	else if(byteHlTemp == 0) ofSync.byteHealth = 0;
	else ofSync.byteHealth = byteHlTemp * 7;

	// CURRENT WEAPON
	bsPlayerSync.Read(ofSync.byteCurrentWeapon);

	// Special Action
	bsPlayerSync.Read(ofSync.byteSpecialAction);
	
	// READ MOVESPEED VECTORS
	bsPlayerSync.Read(bMoveSpeedX);
	if(bMoveSpeedX) bsPlayerSync.Read(ofSync.vecMoveSpeed.X);

	bsPlayerSync.Read(bMoveSpeedY);
	if(bMoveSpeedY) bsPlayerSync.Read(ofSync.vecMoveSpeed.Y);

	bsPlayerSync.Read(bMoveSpeedZ);
	if(bMoveSpeedZ) bsPlayerSync.Read(ofSync.vecMoveSpeed.Z);

	bsPlayerSync.Read(bHasVehicleSurfingInfo);
	if(bHasVehicleSurfingInfo) {
		bsPlayerSync.Read(ofSync.SurfVehicleId);
		bsPlayerSync.Read(ofSync.vecSurfOffsets.X);
		bsPlayerSync.Read(ofSync.vecSurfOffsets.Y);
		bsPlayerSync.Read(ofSync.vecSurfOffsets.Z);
	}

}

//----------------------------------------------------

void CNetGame::Packet_AimSync(Packet *p)
{
	RakNet::BitStream bsAimSync((PCHAR)p->data, p->length, false);
	AIM_SYNC_DATA aimSync;
	BYTE bytePacketID=0;
	BYTE bytePlayerID=0;

	if(GetGameState() != GAMESTATE_CONNECTED) return;

	bsAimSync.Read(bytePacketID);
	bsAimSync.Read(bytePlayerID);
	bsAimSync.Read((PCHAR)&aimSync,sizeof(AIM_SYNC_DATA));
}

//----------------------------------------------------

void CNetGame::Packet_VehicleSync(Packet *p)
{
	RakNet::BitStream bsSync((PCHAR)p->data, p->length, false);
	BYTE		bytePacketID=0;
	BYTE		bytePlayerID=0;
	INCAR_SYNC_DATA icSync;

	bool bSiren,bLandingGear;
	bool bHydra,bTrain,bTrailer;
	bool bTire;

	if(GetGameState() != GAMESTATE_CONNECTED) return;

	memset(&icSync,0,sizeof(INCAR_SYNC_DATA));

	bsSync.Read(bytePacketID);
	bsSync.Read(bytePlayerID);
	bsSync.Read(icSync.VehicleID);

	//bsSync.Read((PCHAR)&icSync,sizeof(INCAR_SYNC_DATA));

	// KEYS
	bsSync.Read(icSync.lrAnalog);
	bsSync.Read(icSync.udAnalog);
	bsSync.Read(icSync.wKeys);

	// ROLL / DIRECTION / POSITION / MOVE SPEED
	bsSync.Read((char*)&icSync.cvecRoll,sizeof(C_VECTOR1));
	bsSync.Read((char*)&icSync.cvecDirection,sizeof(C_VECTOR1));
	bsSync.Read((char*)&icSync.vecPos,sizeof(VECTOR));
	bsSync.Read((char*)&icSync.vecMoveSpeed,sizeof(VECTOR));

	// VEHICLE HEALTH
	WORD wTempVehicleHealth;
	bsSync.Read(wTempVehicleHealth);
	icSync.fCarHealth = (float)wTempVehicleHealth;

	// HEALTH/ARMOUR (COMPRESSED INTO 1 BYTE)
	BYTE byteHealthArmour;
	BYTE byteArmTemp=0,byteHlTemp=0;

	bsSync.Read(byteHealthArmour);
	byteArmTemp = (byteHealthArmour & 0x0F);
	byteHlTemp = (byteHealthArmour >> 4);

	if(byteArmTemp == 0xF) icSync.bytePlayerArmour = 100;
	else if(byteArmTemp == 0) icSync.bytePlayerArmour = 0;
	else icSync.bytePlayerArmour = byteArmTemp * 7;

	if(byteHlTemp == 0xF) icSync.bytePlayerHealth = 100;
	else if(byteHlTemp == 0) icSync.bytePlayerHealth = 0;
	else icSync.bytePlayerHealth = byteHlTemp * 7;

	// CURRENT WEAPON
	bsSync.Read(icSync.byteCurrentWeapon);
	
	// SIREN
	bsSync.Read(bSiren);
	if(bSiren) icSync.byteSirenOn = 1;

	// LANDING GEAR
	bsSync.Read(bLandingGear);
	if(bLandingGear) icSync.byteLandingGearState = 1;

	if (m_bTirePopping) {
		bsSync.Read(bTire);
		if (bTire) icSync.byteTires[0] = 1;
		bsSync.Read(bTire);
		if (bTire) icSync.byteTires[1] = 1;
		bsSync.Read(bTire);
		if (bTire) icSync.byteTires[2] = 1;
		bsSync.Read(bTire);
		if (bTire) icSync.byteTires[3] = 1;
	}

	// HYDRA SPECIAL
	bsSync.Read(bHydra);
	if(bHydra) bsSync.Read(icSync.dwHydraThrustAngle);

	// TRAIN SPECIAL
	bsSync.Read(bTrain);
	if(bTrain) bsSync.Read(icSync.fTrainSpeed);

	// TRAILER ID
	bsSync.Read(bTrailer);
	if(bTrailer) bsSync.Read(icSync.TrailerID);
}

//----------------------------------------------------

void CNetGame::Packet_PassengerSync(Packet *p)
{
	RakNet::BitStream bsPassengerSync((PCHAR)p->data, p->length, false);
	BYTE		bytePacketID=0;
	BYTE		bytePlayerID=0;
	PASSENGER_SYNC_DATA psSync;

	if(GetGameState() != GAMESTATE_CONNECTED) return;

	bsPassengerSync.Read(bytePacketID);
	bsPassengerSync.Read(bytePlayerID);
	bsPassengerSync.Read((PCHAR)&psSync,sizeof(PASSENGER_SYNC_DATA));
}

//----------------------------------------------------

void CNetGame::Packet_TrailerSync(Packet *p)
{
	RakNet::BitStream bsSpectatorSync((PCHAR)p->data, p->length, false);
	if(GetGameState() != GAMESTATE_CONNECTED) return;

	BYTE bytePacketID=0;
	BYTE bytePlayerID=0;
	TRAILER_SYNC_DATA trSync;
	
	bsSpectatorSync.Read(bytePacketID);
	bsSpectatorSync.Read(bytePlayerID);
	bsSpectatorSync.Read((PCHAR)&trSync, sizeof(TRAILER_SYNC_DATA));
}

//----------------------------------------------------

void CNetGame::Packet_RSAPublicKeyMismatch(Packet* packet)
{
	OutputDebugString("BOT: Failed to initialize encryption.");
	exit(1);
}

//----------------------------------------------------

void CNetGame::Packet_ConnectionBanned(Packet* packet)
{
	OutputDebugString("BOT: You're banned from the server.");
	exit(1);
}

//----------------------------------------------------

void CNetGame::Packet_ConnectionRequestAccepted(Packet* packet)
{
	OutputDebugString("BOT: Server has accepted the connection.");
}

//----------------------------------------------------

void CNetGame::Packet_NoFreeIncomingConnections(Packet* packet)
{
	OutputDebugString("BOT: The server is full.");
	exit(1);
}

//----------------------------------------------------

void CNetGame::Packet_DisconnectionNotification(Packet* packet)
{
	OutputDebugString("BOT: Disconnected.");
	m_pRakClient->Disconnect(0);
	exit(1);
}

//----------------------------------------------------

void CNetGame::Packet_ConnectionLost(Packet* packet)
{
	OutputDebugString("BOT: Lost connection to the server.");
	m_pRakClient->Disconnect(0);
	exit(1);
}

//----------------------------------------------------

void CNetGame::Packet_InvalidPassword(Packet* packet)
{
	OutputDebugString("BOT: Wrong server password.");
	m_pRakClient->Disconnect(0);
	exit(1);
}

//----------------------------------------------------

void CNetGame::Packet_ModifiedPacket(Packet* packet)
{
}

//----------------------------------------------------
// RST

void CNetGame::Packet_ConnectAttemptFailed(Packet* packet)
{
	OutputDebugString("BOT: Connection attempt failed.");
	exit(1);
}

//----------------------------------------------------
// Connection Success

void CNetGame::Packet_ConnectionSucceeded(Packet *p)
{
	m_iGameState = GAMESTATE_AWAIT_JOIN;

	int iVersion = NETGAME_VERSION;
	BYTE byteMod = 1;
	BYTE byteNameLen = (BYTE)strlen(m_pPlayerPool->GetLocalPlayerName());
	
	RakNet::BitStream bsSend;
	bsSend.Write(iVersion);
	bsSend.Write(byteMod);
	bsSend.Write(byteNameLen);
	bsSend.Write(m_pPlayerPool->GetLocalPlayerName(),byteNameLen);
	m_pRakClient->RPC(RPC_ClientJoin,&bsSend,HIGH_PRIORITY,RELIABLE,0,FALSE);

	OutputDebugString("BOT: Connection Succeeded, send ClientJoin RPC");
}

//----------------------------------------------------

void CNetGame::UpdatePlayerScoresAndPings()
{
	static DWORD dwLastUpdateTick = 0;

	if ((GetTickCount() - dwLastUpdateTick) > 3000) {
		dwLastUpdateTick = GetTickCount();
		RakNet::BitStream bsParams;
		m_pRakClient->RPC(RPC_UpdateScoresPingsIPs, &bsParams, HIGH_PRIORITY, RELIABLE, 0, FALSE);
	}
}

//----------------------------------------------------

void CNetGame::ResetVehiclePool()
{
	if(m_pVehiclePool) {
		delete m_pVehiclePool;
	}
	m_pVehiclePool = new CVehiclePool();
}

//----------------------------------------------------

void CNetGame::ResetPlayerPool()
{
	if(m_pPlayerPool) {
		delete m_pPlayerPool;
	}
	m_pPlayerPool = new CPlayerPool();
}

//----------------------------------------------------
// EOF
