/*

SA:MP Multiplayer Modification
Copyright 2004-2005 SA:MP Team

Version: $Id: vehiclepool.h,v 1.8 2006/04/12 19:26:45 mike Exp $

*/

#ifndef SAMPSRV_OBJECTPOOL_H
#define SAMPSRV_OBJECTPOOL_H

//----------------------------------------------------

class CObjectPool
{
private:

	bool m_bObjectSlotState[MAX_OBJECTS];
	CObject *m_pObjects[MAX_OBJECTS];
	
	bool m_bPlayerObjectSlotState[MAX_PLAYERS][MAX_OBJECTS];
	bool m_bPlayersObject[MAX_OBJECTS];
	CObject *m_pPlayerObjects[MAX_PLAYERS][MAX_OBJECTS];
public:
	CObjectPool();
	~CObjectPool();

	BYTE New(int iModel, VECTOR * vecPos, VECTOR * vecRot);
	BYTE New(int iPlayer, int iModel, VECTOR* vecPos, VECTOR* vecRot);
	
	bool Delete(BYTE byteObjectID);	
	bool DeleteForPlayer(BYTE bytePlayerID, BYTE byteObjectID);
	
	void Process(float fElapsedTime);

	// Retrieve an object by id
	CObject* GetAt(BYTE byteObjectID)
	{
		if(byteObjectID > MAX_OBJECTS) { return NULL; }
		return m_pObjects[byteObjectID];
	};
	
	CObject* GetAtIndividual(BYTE bytePlayerID, BYTE byteObjectID)
	{
		if (byteObjectID > MAX_OBJECTS || bytePlayerID > MAX_PLAYERS) { return NULL; }
		return m_pPlayerObjects[bytePlayerID][byteObjectID];
	};


	// Find out if the slot is inuse.
	bool GetSlotState(BYTE byteObjectID)
	{
		if(byteObjectID > MAX_OBJECTS) { return false; }
		return m_bObjectSlotState[byteObjectID];
	};
	
	// Find out if the slot is inuse by an individual (and not global).
	bool GetPlayerSlotState(BYTE bytePlayerID, BYTE byteObjectID)
	{
		if (byteObjectID > MAX_OBJECTS || bytePlayerID > MAX_PLAYERS) { return false; }
		//if (m_pObjects[byteObjectID]) return TRUE; // Can't use global slots
		if (!m_bPlayersObject[byteObjectID]) { return false; } // Can use empty ones
		return m_bPlayerObjectSlotState[bytePlayerID][byteObjectID];
	};

	void InitForPlayer(BYTE bytePlayerID);
};

//----------------------------------------------------

#endif

