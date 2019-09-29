/*

	SA:MP Multiplayer Modification
	Copyright 2004-2005 SA:MP Team

	file:
		scriptrpc.h
	desc:
		Scripting RPCs header file.

	Version: $Id: scriptrpc.h,v 1.2 2006/03/20 17:44:20 kyeman Exp $

*/

#pragma once
#ifndef SAMPCLI_SCRIPTRPC_H
#define SAMPCLI_SCRIPTRPC_H

//----------------------------------------------------

void RegisterScriptRPCs(RakClientInterface* pRakClient);
void UnRegisterScriptRPCs(RakClientInterface* pRakClient);

//----------------------------------------------------

#endif