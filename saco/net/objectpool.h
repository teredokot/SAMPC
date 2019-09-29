/*

SA:MP Multiplayer Modification
Copyright 2004-2005 SA:MP Team

*/

#pragma once

class CObjectPool
{
private:
	BOOL		m_bObjectSlotState[MAX_OBJECTS];
	CObject		*m_pObjects[MAX_OBJECTS];


public:
	CObjectPool();
	~CObjectPool();

	BOOL New(byte byteObjectID, int iModel, VECTOR vecPos, VECTOR vecRot);
	BOOL Delete(BYTE byteObjectID);

	// Find out if the slot is inuse.
	BOOL GetSlotState(BYTE byteObjectID) {
		if(byteObjectID > MAX_OBJECTS) { return FALSE; }
		return m_bObjectSlotState[byteObjectID];
	};

	// Retrieve a vehicle
	CObject* GetAt(BYTE byteObjectID) {
		if(byteObjectID> MAX_OBJECTS || !m_bObjectSlotState[byteObjectID]) { return NULL; }
		return m_pObjects[byteObjectID];
	};

	int FindIDFromGtaPtr(ENTITY_TYPE * pGtaObject);
	
	void Process();
};