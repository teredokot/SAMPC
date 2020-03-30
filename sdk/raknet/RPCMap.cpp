/// \file
///
/// This file is part of RakNet Copyright 2003 Kevin Jenkins.
///
/// Usage of RakNet is subject to the appropriate license agreement.
/// Creative Commons Licensees are subject to the
/// license found at
/// http://creativecommons.org/licenses/by-nc/2.5/
/// Single application licensees are subject to the license found at
/// http://www.rakkarsoft.com/SingleApplicationLicense.html
/// Custom license users are subject to the terms therein.
/// GPL license users are subject to the GNU General Public
/// License as published by the Free
/// Software Foundation; either version 2 of the License, or (at your
/// option) any later version.

#include "RPCMap.h"
#include "RakAssert.h"

RPCMap::RPCMap()
{
	for (short i = 0; i < MAX_RPC_ID_AVAILABLE; i++)
		rpcSet[i] = NULL;
}

RPCMap::~RPCMap()
{
	Clear();
}

void RPCMap::Clear(void)
{
	short i;
	for (i=0; i < MAX_RPC_ID_AVAILABLE; i++)
	{
		if (rpcSet[i] != NULL)
		{
			delete rpcSet[i];
			rpcSet[i] = NULL;
		}
	}
}

RPCNode* RPCMap::GetNodeFromID(short uniqueIdentifier)
{
	return rpcSet[uniqueIdentifier];
}

// Called from the user thread for the local system
void RPCMap::AddIdentifierWithFunction(short uniqueIdentifier, void *functionPointer, bool isPointerToMember)
{
	if(rpcSet[uniqueIdentifier] != NULL)
	{
		// Trying to insert an identifier at any free slot and that identifier already exists
		// The user should not insert nodes that already exist in the list
		RakAssert(0);
		return;
	}

	RPCNode* node;
	try {
		node = new RPCNode;
	} catch (...) {
		return;
	}

	node->functionPointer = functionPointer;
	node->isPointerToMember = isPointerToMember;

	rpcSet[uniqueIdentifier] = node;	
}

void RPCMap::RemoveNode(short uniqueIdentifier)
{
	if (rpcSet[uniqueIdentifier] != NULL)
	{
		delete rpcSet[uniqueIdentifier];
		rpcSet[uniqueIdentifier] = NULL;
	}
}
