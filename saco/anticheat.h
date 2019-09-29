//----------------------------------------------------------
//
//   SA:MP Multiplayer Modification For GTA:SA
//   Copyright 2004-2006 SA:MP team
//
// Version: $Id: anticheat.h,v 1.1 2006/04/02 17:48:57 spookie Exp $
//
//----------------------------------------------------------

#pragma once

//----------------------------------------------------------

#define REMOTE_AC_FILENAME "LOADSCV.txd"
#define REMOTE_AC_FILENAME_HASH 0x7a504fb9

//----------------------------------------------------------

class CAntiCheat
{
private:
	BOOL m_bEnabled;
	HMODULE m_hRemoteAC;
	char m_szRemoteACFileName[MAX_PATH+1];
public:
	CAntiCheat();
	~CAntiCheat();

	inline BOOL IsEnabled() { return m_bEnabled; }

	void Enable();
	void Disable();
};

//----------------------------------------------------------
// EOF