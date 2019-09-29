//----------------------------------------------------------
//
// SA:MP Multiplayer Modification For GTA:SA
// Copyright 2007 SA:MP team
//
// A basic bot that can connect and spawn
//
//----------------------------------------------------------

#include "main.h"

GAME_SETTINGS	tSettings;
CNetGame		*pNetGame=0;

void InitSettings();

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdLine, int nCmdShow)
{
	OutputDebugString("----- Bot Starting -----");

	InitSettings();
	
	pNetGame = new CNetGame(tSettings.szConnectHost,atoi(tSettings.szConnectPort),
				tSettings.szNickName,tSettings.szConnectPass);
	
	while(1) {
		pNetGame->Process();
		Sleep(200);

		if( pNetGame->GetGameState() == GAMESTATE_CONNECTED &&
			!pNetGame->GetRakClient()->IsConnected() ) {
				break;
		}
	}

	OutputDebugString("----- BOT: End -----");
	return 0;
}


//----------------------------------------------------

void SetStringFromCommandLine(char *szCmdLine, char *szString)
{
	while(*szCmdLine == ' ') szCmdLine++;
	while(*szCmdLine &&
		  *szCmdLine != ' ' &&
		  *szCmdLine != '-' &&
		  *szCmdLine != '/') 
	{
		*szString = *szCmdLine;
		szString++; szCmdLine++;
	}
	*szString = '\0';
}

//----------------------------------------------------

void InitSettings()
{
	PCHAR szCmdLine = GetCommandLineA();

	OutputDebugString(szCmdLine);
	OutputDebugString("\n");

	memset(&tSettings,0,sizeof(GAME_SETTINGS));

	while(*szCmdLine) {

		if(*szCmdLine == '-' || *szCmdLine == '/') {
			szCmdLine++;
			switch(*szCmdLine) {
				case 'z':
					szCmdLine++;
					SetStringFromCommandLine(szCmdLine,tSettings.szConnectPass);
					break;
				case 'h':
					szCmdLine++;
					SetStringFromCommandLine(szCmdLine,tSettings.szConnectHost);
					break;
				case 'p':
					szCmdLine++;
					SetStringFromCommandLine(szCmdLine,tSettings.szConnectPort);
					break;
				case 'n':
					szCmdLine++;
					SetStringFromCommandLine(szCmdLine,tSettings.szNickName);
					break;
			}
		}

		szCmdLine++;
	}
}

//----------------------------------------------------

