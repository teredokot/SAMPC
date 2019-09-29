/*

	SA:MP Multiplayer Modification
	Copyright 2004-2005 SA:MP Team

    Version: $Id: playerpool.cpp,v 1.19 2006/05/07 15:35:32 kyeman Exp $

*/

#include "main.h"
extern CNetGame *pNetGame;

//----------------------------------------------------

CPlayerPool::CPlayerPool()
{
	// loop through and initialize all net players to null and slot states to false
	for(BYTE bytePlayerID = 0; bytePlayerID < MAX_PLAYERS; bytePlayerID++) {
		m_bPlayerSlotState[bytePlayerID] = FALSE;
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

BOOL CPlayerPool::New(BYTE bytePlayerID, PCHAR szPlayerName)
{
	if(bytePlayerID > MAX_PLAYERS) return FALSE;
	if(strlen(szPlayerName) > MAX_PLAYER_NAME) return FALSE;

	m_pPlayers[bytePlayerID] = new CPlayer();

	if(m_pPlayers[bytePlayerID])
	{
		strcpy(m_szPlayerName[bytePlayerID],szPlayerName);
		m_pPlayers[bytePlayerID]->SetID(bytePlayerID);
		m_bPlayerSlotState[bytePlayerID] = TRUE;
		m_iPlayerScore[bytePlayerID] = 0;
		m_iPlayerMoney[bytePlayerID] = 0;
		m_bIsAnAdmin[bytePlayerID] = FALSE;
		m_byteVirtualWorld[bytePlayerID] = 0;

		// Notify all the other players of a newcommer with
		// a 'ServerJoin' join RPC 
		RakNet::BitStream bsSend;
		bsSend.Write(bytePlayerID);
		BYTE namelen = strlen(szPlayerName);
		bsSend.Write(namelen);
		bsSend.Write(szPlayerName,namelen);

		pNetGame->GetRakServer()->RPC(RPC_ServerJoin ,&bsSend,HIGH_PRIORITY,RELIABLE,0,
			pNetGame->GetRakServer()->GetPlayerIDFromIndex(bytePlayerID),true,false);

		RakServerInterface* pRak = pNetGame->GetRakServer();
		PlayerID Player = pRak->GetPlayerIDFromIndex(bytePlayerID);
		in_addr in;
		in.s_addr = Player.binaryAddress;

		logprintf("[join] %s has joined the server (%u:%s)",szPlayerName,bytePlayerID, inet_ntoa(in));

#ifdef RAKRCON
		bsSend.Reset();
		bsSend.Write(bytePlayerID);
		bsSend.Write(szPlayerName,MAX_PLAYER_NAME);

		pRcon->GetRakServer()->RPC( RPC_ServerJoin, &bsSend, HIGH_PRIORITY, RELIABLE, 0,
			UNASSIGNED_PLAYER_ID, true, false );
#endif

		pNetGame->GetFilterScripts()->OnPlayerConnect(bytePlayerID);
		CGameMode *pGameMode = pNetGame->GetGameMode();
		if(pGameMode) {
			pGameMode->OnPlayerConnect(bytePlayerID);
		}
		
		m_iPlayerCount++;

		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

//----------------------------------------------------

BOOL CPlayerPool::Delete(BYTE bytePlayerID, BYTE byteReason)
{
	if(!GetSlotState(bytePlayerID) || !m_pPlayers[bytePlayerID])
	{
		return FALSE; // Player already deleted or not used.
	}

	pNetGame->GetFilterScripts()->OnPlayerDisconnect(bytePlayerID, byteReason);
	CGameMode *pGameMode = pNetGame->GetGameMode();
	if(pGameMode) {
		pGameMode->OnPlayerDisconnect(bytePlayerID, byteReason);
	}

	m_bPlayerSlotState[bytePlayerID] = FALSE;
	delete m_pPlayers[bytePlayerID];
	m_pPlayers[bytePlayerID] = NULL;
	m_bIsAnAdmin[bytePlayerID] = FALSE;
	
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

#ifdef RAKRCON
	pRcon->GetRakServer()->RPC( RPC_ServerQuit, &bsSend, HIGH_PRIORITY, RELIABLE, 0,
		UNASSIGNED_PLAYER_ID, true, false);
#endif

	m_iPlayerCount--;

	return TRUE;
}

//----------------------------------------------------

BOOL CPlayerPool::Process(float fElapsedTime)
{
	// Process all CPlayers
	for(BYTE bytePlayerID = 0; bytePlayerID < MAX_PLAYERS; bytePlayerID++)
	{
		if(TRUE == m_bPlayerSlotState[bytePlayerID])
		{
			m_pPlayers[bytePlayerID]->Process(fElapsedTime);
		}
	}
	return TRUE;
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
		if((GetSlotState(lp) == TRUE) && (lp != bytePlayerID)) {
			BYTE namelen = strlen(GetPlayerName(lp));

			bsExistingClient.Write(lp);
			bsExistingClient.Write(namelen);
			bsExistingClient.Write(GetPlayerName(lp),namelen);

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
		if((GetSlotState(x) == TRUE) && (x != bytePlayerID)) {
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

BOOL CPlayerPool::IsNickInUse(PCHAR szNick)
{
	int x=0;
	while(x!=MAX_PLAYERS) {
		if(GetSlotState((BYTE)x)) {
			if(!stricmp(GetPlayerName((BYTE)x),szNick)) {
				return TRUE;
			}
		}
		x++;
	}
	return FALSE;
}

//----------------------------------------------------

void CPlayerPool::DeactivateAll()
{
	CGameMode* pGameMode = pNetGame->GetGameMode();
	CFilterScripts* pFilterScripts = pNetGame->GetFilterScripts();
	for(BYTE bytePlayerID = 0; bytePlayerID < MAX_PLAYERS; bytePlayerID++) {
		if(TRUE == m_bPlayerSlotState[bytePlayerID]) {
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
