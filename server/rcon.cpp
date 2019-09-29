/*

SA:MP Multiplayer Modification
Copyright 2004-2005 SA:MP Team

*/
#include "main.h"
#ifdef RAKRCON
#include "rcon.h"
#include "rconrpc.h"

extern CNetGame *pNetGame;

CRcon::CRcon(PCHAR szPassword, WORD wPort, WORD wMaxAdminCount, PCHAR szBindAddress)
{
	m_iCurrentAdminCount = 0;
	m_pRakServer = RakNetworkFactory::GetRakServerInterface();
	RegisterRPCs(m_pRakServer);
	m_pRakServer->SetPassword(szPassword);
	StartRconServer(szBindAddress, wPort, wMaxAdminCount);
}

CRcon::~CRcon(void)
{
	if (m_pRakServer)
	{
		m_pRakServer->Disconnect(100);
		UnregisterRPCs(m_pRakServer);
		RakNetworkFactory::DestroyRakServerInterface(m_pRakServer);
	}
}

void CRcon::StartRconServer(PCHAR szBindAddress, WORD wPort, WORD wMaxAdminCount)
{
	if (!m_pRakServer->Start(wMaxAdminCount, 0, 5, wPort, szBindAddress))
	{
		if (szBindAddress) 
			logprintf("[RCON] Unable to start RCON server on %s:%u.", szBindAddress, wPort);
		else
			logprintf("[RCON] Unable to start RCON server on port %u.", wPort);
	} 
	else
	{
		if (szBindAddress) 
			logprintf("[RCON] Server started on %s:%u.", szBindAddress, wPort);
		else
			logprintf("[RCON] Server started on port %u.", wPort);
	}
}

void CRcon::Process(void)
{
	if (!m_pRakServer) return;

	Packet *pPacket;
	while (pPacket = m_pRakServer->Receive())
	{
		switch (pPacket->data[0])
		{
			case ID_NEW_INCOMING_CONNECTION:
				Packet_NewIncomingConnection(pPacket);
				break;
			case ID_DISCONNECTION_NOTIFICATION:
				Packet_DisconnectionNotification(pPacket);
				break;
			case ID_CONNECTION_LOST:
				Packet_ConnectionLost(pPacket);
				break;
		}
		m_pRakServer->DeallocatePacket(pPacket);  
	}
}

void CRcon::Packet_NewIncomingConnection(Packet* pPacket)
{
	in_addr in;
	in.s_addr = pPacket->playerId.binaryAddress;
	logprintf("[RCON] Admin [%s] has connected.", inet_ntoa(in));

	RakNet::BitStream bsSend;

	char* szHostname = pConsole->GetStringVariable( "hostname" );
	BYTE byteHostnameLength = strlen(szHostname);

	CPlayerPool* pPlayerPool = pNetGame->GetPlayerPool();

	bsSend.Write( byteHostnameLength );
	bsSend.Write( szHostname, byteHostnameLength );

	m_pRakServer->RPC( RPC_RconConnect,&bsSend,HIGH_PRIORITY,RELIABLE,0,UNASSIGNED_PLAYER_ID,true,false);
	bsSend.Reset();

	for( int i = 0; i < MAX_PLAYERS; i++ )
	{
		if( pPlayerPool->GetSlotState(i) == TRUE ) 
		{
			bsSend.Write((BYTE)i);
			bsSend.Write(pPlayerPool->GetPlayerName(i),MAX_PLAYER_NAME);

			pRcon->GetRakServer()->RPC( RPC_ServerJoin, &bsSend, HIGH_PRIORITY, RELIABLE, 0,
				UNASSIGNED_PLAYER_ID, true, false );

			bsSend.Reset();	
			
			PLAYER_DATA playerData = GetPlayerInformation( i );
			bsSend.Write( (BYTE)i );
			bsSend.Write( (PCHAR)&playerData, sizeof(PLAYER_DATA) );
			pRcon->GetRakServer()->RPC( RPC_RconPlayerInfo, &bsSend, HIGH_PRIORITY, RELIABLE, 0,
				UNASSIGNED_PLAYER_ID, true, false );

			bsSend.Reset();	
		}
	}
}

CRcon::PLAYER_DATA CRcon::GetPlayerInformation(BYTE byteplayerID)
{
	PLAYER_DATA data;

	memset( &data, 0, sizeof(PLAYER_DATA) );

	CPlayer* pPlayer = pNetGame->GetPlayerPool()->GetAt(byteplayerID);
	PlayerID Player = pNetGame->GetRakServer()->GetPlayerIDFromIndex(byteplayerID);

	in_addr in;
	in.s_addr = Player.binaryAddress;
	
	strcpy(data.szName, pNetGame->GetPlayerPool()->GetPlayerName(byteplayerID));
	strcpy(data.szIP, inet_ntoa(in));

	data.fX = pPlayer->m_vecPos.X;
	data.fY = pPlayer->m_vecPos.Y;
	data.fZ = pPlayer->m_vecPos.Z;

	data.fHealth = pPlayer->m_fHealth;
	data.fArmour = pPlayer->m_fArmour;

	data.iScore = pNetGame->GetPlayerPool()->GetPlayerScore(byteplayerID);
	data.iPing = pNetGame->GetRakServer()->GetAveragePing( Player );

	return data;
}

void CRcon::Packet_DisconnectionNotification(Packet* pPacket)
{
	in_addr in;
	in.s_addr = pPacket->playerId.binaryAddress;
	logprintf("[RCON] Admin [%s] has disconnected.", inet_ntoa(in));
}

void CRcon::Packet_ConnectionLost(Packet* pPacket)
{
	in_addr in;
	in.s_addr = pPacket->playerId.binaryAddress;
	logprintf("[RCON] Admin [%s] has lost connection.", inet_ntoa(in));
}

void CRcon::SendEventString(char *szString)
{
	BYTE byteLen = (BYTE)strlen(szString);
	RakNet::BitStream bsRconSend;
	bsRconSend.Write(byteLen);
	bsRconSend.Write(szString, byteLen);
	pRcon->GetRakServer()->RPC(RPC_RconEvent, &bsRconSend, HIGH_PRIORITY, RELIABLE, 0, UNASSIGNED_PLAYER_ID, true, false);
}

void CRcon::RegisterRPCs(RakServerInterface *pRakServer)
{
	REGISTER_STATIC_RPC(pRakServer, RconConnect);
	REGISTER_STATIC_RPC(pRakServer, RconCommand);
}

void CRcon::UnregisterRPCs(RakServerInterface *pRakServer)
{
	UNREGISTER_STATIC_RPC(pRakServer, RconConnect);
	UNREGISTER_STATIC_RPC(pRakServer, RconCommand);
}

#endif
