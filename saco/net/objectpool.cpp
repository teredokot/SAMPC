/*

SA:MP Multiplayer Modification
Copyright 2004-2005 SA:MP Team

*/

#include "../main.h"
#include "../game/util.h"

extern CGame	*pGame;

CObjectPool::CObjectPool()
{
	for(BYTE byteObjectID = 0; byteObjectID < MAX_OBJECTS; byteObjectID++)
	{
		m_bObjectSlotState[byteObjectID]	= FALSE;
		m_pObjects[byteObjectID]			= NULL;
	}
};

CObjectPool::~CObjectPool()
{
	for(int i = 0; i < MAX_OBJECTS; i++)
	{
		Delete(i);
	}
}

BOOL CObjectPool::Delete(BYTE byteObjectID)
{
	if(!GetSlotState(byteObjectID) || !m_pObjects[byteObjectID])
	{
		return FALSE; // Vehicle already deleted or not used.
	}

	m_bObjectSlotState[byteObjectID] = FALSE;
	delete m_pObjects[byteObjectID];
	m_pObjects[byteObjectID] = NULL;

	return TRUE;
}

BOOL CObjectPool::New(byte byteObjectID, int iModel, VECTOR vecPos, VECTOR vecRot)
{
	if (m_pObjects[byteObjectID] != NULL)
	{
		Delete(byteObjectID);
	}

	m_pObjects[byteObjectID] = pGame->NewObject(iModel, vecPos.X, vecPos.Y, vecPos.Z, vecRot);

	if (m_pObjects[byteObjectID])
	{
		m_bObjectSlotState[byteObjectID] = TRUE;

		return TRUE;
	}

	return FALSE; // Will only be called if m_pObjects[byteObjectID] is null
}

//----------------------------------------------------

int CObjectPool::FindIDFromGtaPtr(ENTITY_TYPE * pGtaObject)
{
	int x=1;

	while(x!=MAX_OBJECTS) {
		if(pGtaObject == m_pObjects[x]->m_pEntity) return x;
		x++;
	}

	return (-1);
}

void CObjectPool::Process()
{
	static unsigned long s_ulongLastCall = 0;
	if (!s_ulongLastCall) s_ulongLastCall = GetTickCount();
	unsigned long ulongTick = GetTickCount();
	float fElapsedTime = ((float)(ulongTick - s_ulongLastCall)) / 1000.0f;
	// Get elapsed time in seconds
	for (BYTE i = 0; i < MAX_OBJECTS; i++)
	{
		if (m_bObjectSlotState[i]) m_pObjects[i]->Process(fElapsedTime);
	}
	s_ulongLastCall = ulongTick;
}