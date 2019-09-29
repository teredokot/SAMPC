//----------------------------------------------------------
//
// SA:MP Multiplayer Modification For GTA:SA
// Copyright 2004-2005 SA:MP team
//
// Version: $Id: debug.h,v 1.3 2006/03/20 17:44:20 kyeman Exp $
//
//----------------------------------------------------------

#include <windows.h>

void GameDebugEntity(DWORD dwEnt1, DWORD dwEnt2, int type);
BOOL IsGameDebugScreenOn();
void GameDebugScreensOff();
void GameDebugSetSyncActors(DWORD dwID1, DWORD dwID2);
void GameDebugDrawDebugScreens();
void GameDebugDrawSpawnInfo();
void GameDebugGetEntityType( BYTE bType, char * cType );
void GameDebugInitTextScreen();
void GameDebugAddMessage(char *format, ...);
void GameDrawDebugTextInfo();

//-----------------------------------------------------------
