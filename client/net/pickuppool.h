/*

	SA:MP Multiplayer Modification
	Copyright 2004-2005 SA:MP Team

	file:
		pickuppool.h
	desc:
		Umm, Pickups?

*/
/*

	SA:MP Multiplayer Modification
	Copyright 2004-2005 SA:MP Team

    Version: $Id: pickuppool.h,v 1.2 2006/03/20 17:59:34 kyeman Exp $

*/

#pragma once

#define MAX_PICKUPS 400

typedef struct _PICKUP
{
	int iModel;
	int iType;
	float fX;
	float fY;
	float fZ;
} PICKUP;

typedef struct _DROPPED_WEAPON
{
	bool bDroppedWeapon;
	BYTE bytePlayerID;
} DROPPED_WEAPON;

//----------------------------------------------------

class CPickupPool
{
private:

	PICKUP  m_Pickups[MAX_PICKUPS];
	int		m_iPickupCount;
	DWORD	m_dwHnd[MAX_PICKUPS];
	DWORD   m_iTimer[MAX_PICKUPS];
	DROPPED_WEAPON	m_droppedWeapon[MAX_PICKUPS];

public:
	
	CPickupPool() {
		memset(&m_Pickups[0],0,sizeof(PICKUP) * MAX_PICKUPS);
		m_iPickupCount = 0;
		for (int i = 0; i < MAX_PICKUPS; i++)
		{
			m_dwHnd[i] = NULL;
			m_iTimer[i] = NULL;
		}
	};

	~CPickupPool();

	void New(int iModel, float fX, float fY, float fZ, DWORD dwAmmo, BYTE byteDeadPlayer);
	void New(PICKUP* pPickup, int iPickup);
	void InitForPlayer(BYTE bytePlayerID);
	void Destroy(int iPickup);
	void DestroyDropped(BYTE byteFromPlayer);
	void PickedUp(int iPickup);
	void Process();
};

//----------------------------------------------------