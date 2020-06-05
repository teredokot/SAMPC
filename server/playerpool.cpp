/*

	SA:MP Multiplayer Modification
	Copyright 2004-2005 SA:MP Team

    Version: $Id: playerpool.cpp,v 1.19 2006/05/07 15:35:32 kyeman Exp $

*/

#include "main.h"

//----------------------------------------------------

CPlayerPool::CPlayerPool()
{
	// loop through and initialize all net players to null and slot states to false
	for(BYTE bytePlayerID = 0; bytePlayerID < MAX_PLAYERS; bytePlayerID++) {
		m_bPlayerSlotState[bytePlayerID] = false;
		m_pPlayers[bytePlayerID] = NULL;
	}
	m_iPlayerCount = 0;
}

//----------------------------------------------------

CPlayerPool::~CPlayerPool()
{	
	for(BYTE bytePlayerID = 0; bytePlayerID < MAX_PLAYERS; bytePlayerID++) {
		Delete(bytePlayerID,0);
	}
	m_iPlayerCount = 0;
}

//----------------------------------------------------

bool CPlayerPool::New(BYTE bytePlayerID, PCHAR szPlayerName, char* szVersion)
{
	if(bytePlayerID > MAX_PLAYERS) return false;
	if(strlen(szPlayerName) > MAX_PLAYER_NAME) return false;

	m_pPlayers[bytePlayerID] = new CPlayer();

	if(m_pPlayers[bytePlayerID])
	{
		strcpy(m_szPlayerName[bytePlayerID],szPlayerName);
		strcpy(m_pPlayers[bytePlayerID]->m_szClientVersion, szVersion);

		m_pPlayers[bytePlayerID]->SetID(bytePlayerID);
		m_bPlayerSlotState[bytePlayerID] = true;
		m_iPlayerScore[bytePlayerID] = 0;
		m_iPlayerMoney[bytePlayerID] = 0;
		m_bIsAnAdmin[bytePlayerID] = false;
		m_byteVirtualWorld[bytePlayerID] = 0;

		// Notify all the other players of a newcommer with
		// a 'ServerJoin' join RPC 
		RakNet::BitStream bsSend;
		bsSend.Write(bytePlayerID);
		size_t uiNameLen = strlen(szPlayerName);
		bsSend.Write(uiNameLen);
		bsSend.Write(szPlayerName, uiNameLen);

		pNetGame->GetRakServer()->RPC(RPC_ServerJoin ,&bsSend,HIGH_PRIORITY,RELIABLE,0,
			pNetGame->GetRakServer()->GetPlayerIDFromIndex(bytePlayerID),true,false);

		RakServerInterface* pRak = pNetGame->GetRakServer();
		PlayerID Player = pRak->GetPlayerIDFromIndex(bytePlayerID);
		in_addr in;
		in.s_addr = Player.binaryAddress;

		int iTime = pConsole->GetIntVariable("playertimeout");
		pNetGame->GetRakServer()->SetTimeoutTime(iTime, Player);

		logprintf("[join] %s has joined the server (%u:%s)",szPlayerName,bytePlayerID, inet_ntoa(in));

/*#ifdef RAKRCON
		bsSend.Reset();
		bsSend.Write(bytePlayerID);
		bsSend.Write(szPlayerName,MAX_PLAYER_NAME);

		pRcon->GetRakServer()->RPC( RPC_ServerJoin, &bsSend, HIGH_PRIORITY, RELIABLE, 0,
			UNASSIGNED_PLAYER_ID, true, false );
#endif*/

		pNetGame->GetFilterScripts()->OnPlayerConnect(bytePlayerID);
		CGameMode *pGameMode = pNetGame->GetGameMode();
		if(pGameMode) {
			pGameMode->OnPlayerConnect(bytePlayerID);
		}
		
		m_iPlayerCount++;

		for (unsigned int i = 0; i < MAX_PLAYERS; i++)
		{
			if (GetSlotState((BYTE)i))
				m_uiLastPlayerId = i;
		}
		return true;
	}
	else
	{
		return false;
	}
}

//----------------------------------------------------

bool CPlayerPool::Delete(BYTE bytePlayerID, BYTE byteReason)
{
	if(!GetSlotState(bytePlayerID) || !m_pPlayers[bytePlayerID])
	{
		return false; // Player already deleted or not used.
	}

	pNetGame->GetFilterScripts()->OnPlayerDisconnect(bytePlayerID, byteReason);
	CGameMode *pGameMode = pNetGame->GetGameMode();
	if(pGameMode) {
		pGameMode->OnPlayerDisconnect(bytePlayerID, byteReason);
	}

	m_bPlayerSlotState[bytePlayerID] = false;
	delete m_pPlayers[bytePlayerID];
	m_pPlayers[bytePlayerID] = NULL;
	m_bIsAnAdmin[bytePlayerID] = false;
	
	// Notify all the other players that this client is quiting.
	RakNet::BitStream bsSend;
	bsSend.Write(bytePlayerID);
	bsSend.Write(byteReason);
	pNetGame->GetRakServer()->RPC(RPC_ServerQuit ,&bsSend,HIGH_PRIORITY,RELIABLE,0,
		pNetGame->GetRakServer()->GetPlayerIDFromIndex(bytePlayerID),true,false);
		
	CObjectPool* pObjectPool = pNetGame->GetObjectPool();
	for (BYTE i = 0; i < MAX_OBJECTS; i++)
	{
		// Remove all personal objects (checking done by the function)
		pObjectPool->DeleteForPlayer(bytePlayerID, i);
	}

	logprintf("[part] %s has left the server (%u:%u)",m_szPlayerName[bytePlayerID],bytePlayerID,byteReason);

/*#ifdef RAKRCON
	pRcon->GetRakServer()->RPC( RPC_ServerQuit, &bsSend, HIGH_PRIORITY, RELIABLE, 0,
		UNASSIGNED_PLAYER_ID, true, false);
#endif*/

	m_iPlayerCount--;

	for (unsigned int i = 0; i < MAX_PLAYERS; i++)
	{
		if (GetSlotState((BYTE)i))
			m_uiLastPlayerId = i;
	}
	return true;
}

//----------------------------------------------------

bool CPlayerPool::Process(float fElapsedTime)
{
	// Process all CPlayers
	for(BYTE bytePlayerID = 0; bytePlayerID < MAX_PLAYERS; bytePlayerID++)
	{
		if(true == m_bPlayerSlotState[bytePlayerID])
		{
			m_pPlayers[bytePlayerID]->Process(fElapsedTime);
		}
	}
	return true;
}

//----------------------------------------------------

void CPlayerPool::InitPlayersForPlayer(BYTE bytePlayerID)
{
	BYTE lp=0;
	RakNet::BitStream bsExistingClient;
	RakNet::BitStream bsPlayerVW;

	RakServerInterface* pRak = pNetGame->GetRakServer();
	PlayerID Player = pRak->GetPlayerIDFromIndex(bytePlayerID);

	CPlayerPool* pPlayerPool = pNetGame->GetPlayerPool();
	bool send = false;

	while(lp!=MAX_PLAYERS) {
		if((GetSlotState(lp) == true) && (lp != bytePlayerID)) {
			size_t uiNameLen = strlen(GetPlayerName(lp));

			bsExistingClient.Write(lp);
			bsExistingClient.Write(uiNameLen);
			bsExistingClient.Write(GetPlayerName(lp), uiNameLen);

			pNetGame->GetRakServer()->RPC(RPC_ServerJoin,&bsExistingClient,HIGH_PRIORITY,RELIABLE,0,Player,false,false);
			bsExistingClient.Reset();
			
			// Send all the VW data in one lump
			bsPlayerVW.Write(lp);
			bsPlayerVW.Write(GetPlayerVirtualWorld(lp));
			send = true;
		}
		lp++;
	}
	if (send)
	{
		pRak->RPC(RPC_ScrSetPlayerVirtualWorld , &bsPlayerVW, HIGH_PRIORITY, RELIABLE, 0, Player, false, false);
	}
}

//----------------------------------------------------

void CPlayerPool::InitSpawnsForPlayer(BYTE bytePlayerID)
{
	BYTE x=0;
	CPlayer *pSpawnPlayer;

	while(x!=MAX_PLAYERS) {
		if((GetSlotState(x) == true) && (x != bytePlayerID)) {
			pSpawnPlayer = GetAt(x);
			if(pSpawnPlayer->IsActive()) {
				pSpawnPlayer->SpawnForPlayer(bytePlayerID);
			}
		}
		x++;
	}
}

//----------------------------------------------------
// Return constant describing the type of kill.

BYTE CPlayerPool::GetKillType(BYTE byteWhoKilled, BYTE byteWhoDied)
{

	if( byteWhoKilled != INVALID_PLAYER_ID &&
		byteWhoKilled < MAX_PLAYERS &&
		byteWhoDied < MAX_PLAYERS ) {

		if(m_bPlayerSlotState[byteWhoKilled] && m_bPlayerSlotState[byteWhoDied]) {
			if(GetAt(byteWhoKilled)->GetTeam() == NO_TEAM || GetAt(byteWhoDied)->GetTeam() == NO_TEAM) {
				return VALID_KILL;
			}
			if(GetAt(byteWhoKilled)->GetTeam() != GetAt(byteWhoDied)->GetTeam()) {
				return VALID_KILL;
			}
			else {
				return TEAM_KILL;
			}
		}
		return SELF_KILL;
	}

	if(byteWhoKilled == INVALID_PLAYER_ID && byteWhoDied < MAX_PLAYERS)
	{
		return SELF_KILL;
	}
	
	return SELF_KILL;						
}

//----------------------------------------------------

float CPlayerPool::GetDistanceFromPlayerToPlayer(BYTE bytePlayer1, BYTE bytePlayer2)
{
	VECTOR	*vecFromPlayer;
	VECTOR	*vecThisPlayer;
	float	fSX,fSY,fSZ;

	CPlayer * pPlayer1 = GetAt(bytePlayer1);
	CPlayer * pPlayer2 = GetAt(bytePlayer2);

	vecFromPlayer = &pPlayer1->m_vecPos;
	vecThisPlayer = &pPlayer2->m_vecPos;
	
	fSX = (vecThisPlayer->X - vecFromPlayer->X) * (vecThisPlayer->X - vecFromPlayer->X);
	fSY = (vecThisPlayer->Y - vecFromPlayer->Y) * (vecThisPlayer->Y - vecFromPlayer->Y);
	fSZ = (vecThisPlayer->Z - vecFromPlayer->Z) * (vecThisPlayer->Z - vecFromPlayer->Z);

	return (float)sqrt(fSX + fSY + fSZ);	
}

//----------------------------------------------------

float CPlayerPool::GetDistanceSquaredFromPlayerToPlayer(BYTE bytePlayer1, BYTE bytePlayer2)
{
	VECTOR	*vecFromPlayer;
	VECTOR	*vecThisPlayer;
	float	fSX,fSY,fSZ;

	CPlayer * pPlayer1 = GetAt(bytePlayer1);
	CPlayer * pPlayer2 = GetAt(bytePlayer2);

	vecFromPlayer = &pPlayer1->m_vecPos;
	vecThisPlayer = &pPlayer2->m_vecPos;
	
	fSX = (vecThisPlayer->X - vecFromPlayer->X) * (vecThisPlayer->X - vecFromPlayer->X);
	fSY = (vecThisPlayer->Y - vecFromPlayer->Y) * (vecThisPlayer->Y - vecFromPlayer->Y);
	fSZ = (vecThisPlayer->Z - vecFromPlayer->Z) * (vecThisPlayer->Z - vecFromPlayer->Z);

	return (float)(fSX + fSY + fSZ);
}

//----------------------------------------------------

bool CPlayerPool::IsNickInUse(PCHAR szNick)
{
	int x=0;
	while(x!=MAX_PLAYERS) {
		if(GetSlotState((BYTE)x)) {
			//if(!stricmp(GetPlayerName((BYTE)x),szNick)) {
			if (!strcmp(GetPlayerName((BYTE)x), szNick)) {
				return true;
			}
		}
		x++;
	}
	return false;
}

//----------------------------------------------------

void CPlayerPool::DeactivateAll()
{
	CGameMode* pGameMode = pNetGame->GetGameMode();
	CFilterScripts* pFilterScripts = pNetGame->GetFilterScripts();
	for(BYTE bytePlayerID = 0; bytePlayerID < MAX_PLAYERS; bytePlayerID++) {
		if(true == m_bPlayerSlotState[bytePlayerID]) {
			m_pPlayers[bytePlayerID]->Deactivate();
			pGameMode->OnPlayerDisconnect(bytePlayerID, 1);
			pFilterScripts->OnPlayerDisconnect(bytePlayerID, 1);
		}
		m_byteVirtualWorld[bytePlayerID] = 0;
	}
}

//----------------------------------------------------

void CPlayerPool::SetPlayerVirtualWorld(BYTE bytePlayerID, BYTE byteVirtualWorld)
{
	if (bytePlayerID >= MAX_PLAYERS) return;
	
	m_byteVirtualWorld[bytePlayerID] = byteVirtualWorld;
	// Tell existing players it's changed
	RakNet::BitStream bsData;
	bsData.Write(bytePlayerID); // player id
	bsData.Write(byteVirtualWorld); // vw id
	RakServerInterface *pRak = pNetGame->GetRakServer();
	pRak->RPC(RPC_ScrSetPlayerVirtualWorld , &bsData, HIGH_PRIORITY, RELIABLE, 0, UNASSIGNED_PLAYER_ID, true, false);
}
	
//----------------------------------------------------
