//----------------------------------------------------------
//
// SA:MP Multiplayer Modification For GTA:SA
// Copyright 2004-2006 SA:MP team
//
//----------------------------------------------------------

#pragma once

// Ensure OutputDebugString() is disabled on release builds

#ifndef _DEBUG

#undef OutputDebugString
#define OutputDebugString(a) NULL

#undef OutputDebugStringW
#define OutputDebugStringW(a) NULL

#endif
