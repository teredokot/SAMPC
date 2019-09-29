//----------------------------------------------------------
//
// SA:MP Multiplayer Modification For GTA:SA
// Copyright 2004-2005 SA:MP team
//
// Version: $Id: keystuff.h,v 1.4 2006/03/20 17:44:20 kyeman Exp $
//
//----------------------------------------------------------

typedef struct _GTA_CONTROLSET
{
	WORD wKeys1[24];
	WORD wKeys2[24];
	BYTE bytePadding1[212];
} GTA_CONTROLSET;

//-----------------------------------------------------------

void GameKeyStatesInit();
void GameStoreLocalPlayerKeys();
void GameSetLocalPlayerKeys();
void GameStoreRemotePlayerKeys(int iPlayer, GTA_CONTROLSET *pGcsKeyStates);
void GameSetRemotePlayerKeys(int iPlayer);
GTA_CONTROLSET *GameGetInternalKeys();
GTA_CONTROLSET *GameGetLocalPlayerKeys();
GTA_CONTROLSET *GameGetPlayerKeys(int iPlayer);
void GameResetPlayerKeys(int iPlayer);
void GameResetInternalKeys();

//-----------------------------------------------------------
