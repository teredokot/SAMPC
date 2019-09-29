/*

	SA:MP Multiplayer Modification
	Copyright 2004-2005 SA:MP Team

    Version: $Id: pickuppool.cpp,v 1.5 2006/05/07 15:35:32 kyeman Exp $

*/

#include "main.h"
extern CNetGame *pNetGame;

//----------------------------------------------------

int CPickupPool::New(int iModel, int iType, float fX, float fY, float fZ, BYTE staticp)
{
	if (m_iPickupCount >= MAX_PICKUPS) return -1;

	for (int i = 0; i < MAX_PICKUPS; i++)
	{
		if (!m_bActive[i])
		{
			m_Pickups[i].iModel = iModel;
			m_Pickups[i].iType = iType;
			m_Pickups[i].fX = fX;
			m_Pickups[i].fY = fY;	
			m_Pickups[i].fZ = fZ;
			if (staticp)
			{
				// Static, can't be destroyed
				m_bActive[i] = -1;
			}
			else
			{
				// Dynamic, can be destroyed
				m_bActive[i] = 1;
			}
			m_iPickupCount++;
			
			// Broadcast to existing players:
			
			RakNet::BitStream bsPickup;
			bsPickup.Write(i);
			bsPickup.Write((PCHAR)&m_Pickups[i], sizeof (PICKUP));
			pNetGame->GetRakServer()->RPC(RPC_Pickup, &bsPickup, HIGH_PRIORITY, RELIABLE, 0, UNASSIGNED_PLAYER_ID, true, false);
			return i;
		}
	}
	return -1;
}

int CPickupPool::Destroy(int iPickup)
{
	if (iPickup >= 0 && iPickup < MAX_PICKUPS && m_bActive[iPickup] == 1)
	{
		m_bActive[iPickup] = 0;
		m_iPickupCount--;
		RakNet::BitStream bsPickup;
		bsPickup.Write(iPickup);
		pNetGame->GetRakServer()->RPC(RPC_DestroyPickup, &bsPickup, HIGH_PRIORITY, RELIABLE, 0, UNASSIGNED_PLAYER_ID, true, false);
		return 1;
		
	}
	return 0;
}

//----------------------------------------------------

void CPickupPool::InitForPlayer(BYTE bytePlayerID)
{	
	RakNet::BitStream *pbsPickup;

	int x=0;

	pbsPickup = new RakNet::BitStream();

	while(x != MAX_PICKUPS)
	{
		if (m_bActive[x])
		{
			pbsPickup->Write(x);
			pbsPickup->Write((PCHAR)&m_Pickups[x], sizeof (PICKUP));
		
			pNetGame->GetRakServer()->RPC(RPC_Pickup,pbsPickup,HIGH_PRIORITY,RELIABLE,
				0,pNetGame->GetRakServer()->GetPlayerIDFromIndex(bytePlayerID),false,false);

			pbsPickup->Reset();
		}

		x++;
	}

	delete pbsPickup;
}

//----------------------------------------------------