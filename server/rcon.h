/*

SA:MP Multiplayer Modification
Copyright 2004-2005 SA:MP Team

*/

#ifndef _RCON_H_INCLUDED
#define _RCON_H_INCLUDED

#ifdef RAKRCON

class CRcon
{
public:

	typedef struct PLAYER_DATA
	{
		char szName[MAX_PLAYER_NAME];
		char szIP[16];
		float fX;
		float fY;
		float fZ;
		float fHealth;
		float fArmour;
		int iScore;
		int iPing;
	} PLAYER_DATA;

	CRcon(PCHAR szPassword, WORD wPort, WORD wMaxAdminCount, PCHAR szBindAddress = NULL);
	~CRcon(void);

	RakServerInterface *GetRakServer() { return m_pRakServer; };

	void StartRconServer(PCHAR szBindAddress, WORD wPort, WORD wMaxAdminCount);
	void Process(void);
	void Packet_NewIncomingConnection(Packet* pPacket);
	void Packet_DisconnectionNotification(Packet* pPacket);
	void Packet_ConnectionLost(Packet* pPacket);
	void Packet_ConnectionBanned(Packet* pPacket);
	void SendEventString(char *szString);
	void RegisterRPCs(RakServerInterface *pRakServer);
	void UnregisterRPCs(RakServerInterface *pRakServer);

	PLAYER_DATA GetPlayerInformation( BYTE byteplayerID );

	//PLAYER_DATA m_tPlayerData[100];

private:

	int m_iCurrentAdminCount;
	RakServerInterface *m_pRakServer;
};

#endif

#endif

