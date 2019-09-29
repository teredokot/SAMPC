/*

	SA:MP Multiplayer Modification
	Copyright 2004-2005 SA:MP Team

    Version: $Id: main.cpp,v 1.26 2006/05/08 17:35:55 kyeman Exp $

*/

#include "main.h"

CNetGame		*pNetGame	= NULL;
CConsole		*pConsole	= NULL;
CPlugins		*pPlugins	= NULL;

SERVER_SETTINGS gServerSettings;

#ifdef RAKRCON
CRcon		*pRcon		= NULL;
#endif

BYTE		byteRconUser= INVALID_ID;
SOCKET		sockRconReply=INVALID_SOCKET;
void*		dataRconReply=NULL;
DWORD		dwdlRconReply=0;

FILE		*pLogFile;
bool		bQuitApp = false;
BOOL		bGameModeFinished=FALSE;

unsigned int _uiRndSrvChallenge;

#ifdef WIN32
extern LONG WINAPI exc_handler(_EXCEPTION_POINTERS* exc_inf);
#endif

//----------------------------------------------------

void InitSettingsFromCommandLine(char * szCmdLine);

//----------------------------------------------------

#ifdef WIN32
HANDLE hConsoleExecuteEvent;
DWORD WINAPI ConsoleInputThread(void* pParam)
{
	char buf[512];
	while (true)
	{
		DWORD dwRead;
		ReadConsole(GetStdHandle(STD_INPUT_HANDLE), buf, 255, &dwRead, NULL);
		if (dwRead > 2)
		{
			buf[dwRead-2] = 0;
			WaitForSingleObject(hConsoleExecuteEvent, INFINITE);
			flogprintf("Console input: %s", buf);
			pConsole->Execute(buf);
			SetEvent(hConsoleExecuteEvent);
		}
	}
}

//----------------------------------------------------

BOOL WINAPI CtrlHandler(DWORD type)
{
    switch (type)
    {
        case CTRL_C_EVENT:
        case CTRL_BREAK_EVENT:
        case CTRL_CLOSE_EVENT:
        case CTRL_LOGOFF_EVENT:
        case CTRL_SHUTDOWN_EVENT:
			bQuitApp = true;
            return TRUE;
    }
    return FALSE;
}

#endif

//----------------------------------------------------

void ServerPasswordChanged()
{
	if (pNetGame)
	{
		char* szPass = pConsole->GetStringVariable("password");
		if ((szPass) && (szPass[0] != 0) && (szPass[0] != '0'))
		{
			logprintf("Setting server password to: \"%s\"", szPass);
			pNetGame->GetRakServer()->SetPassword(szPass);
		} else {
			logprintf("Server password has been removed.");
			pNetGame->GetRakServer()->SetPassword(NULL);
		}
	}
}

//----------------------------------------------------

void ServerMaxPlayersChanged()
{
	int maxplayers = pConsole->GetIntVariable("maxplayers");
	if (maxplayers < 0)
		pConsole->SetIntVariable("maxplayers", 0);
	if (maxplayers > MAX_PLAYERS)
		pConsole->SetIntVariable("maxplayers", MAX_PLAYERS);

	if (pNetGame)
	{
		if (pConsole->GetIntVariable("maxplayers") > MAX_PLAYERS) {
			pConsole->SetIntVariable("maxplayers", MAX_PLAYERS);
		}
		pNetGame->GetRakServer()->SetAllowedPlayers((WORD)pConsole->GetIntVariable("maxplayers"));
	}
}

//----------------------------------------------------

void ServerInstagibChanged()
{
	if (pNetGame) {
		pNetGame->UpdateInstagib();
	}
}

//----------------------------------------------------

void LoadLogFile()
{
	int reload = 0;
	if (pLogFile)
	{
		fclose(pLogFile);
		reload = 1;
	}
	pLogFile = fopen("server_log.txt", "a");
	if (pLogFile)
	{
		logprintf("");
		logprintf("----------");
		if (reload) logprintf("Reloaded log file: \"server_log.txt\".");
		else logprintf("Loaded log file: \"server_log.txt\".");
		logprintf("----------");
	} else {
		logprintf("Failed to load log file: \"server_log.txt\".");
	}
}

#ifdef LINUX

void SignalHandler(int sig)
{
	switch (sig)
	{
	case SIGUSR1:
		{
		LoadLogFile();
		break;
		}
	case SIGUSR2:
		{
		if (pNetGame)
		{
			pNetGame->LoadBanList();
		}
		break;
		}
	case SIGINT:
	case SIGTERM:
		{
			bQuitApp = true;
			break;
		}
	}
}

//----------------------------------------------------
#include <time.h>

long GetTickCount()
{
	tms tm;
	return (times(&tm) * 10);
}

// strlwr is not included with the GNU C lib it seems.
char* strlwr(char* str)
{
	for (size_t i=0; i<strlen(str); i++)
	{
		if ((str[i] >= 'A') && (str[i] <= 'Z'))
		{
			str[i] -= 32;
		}
	}
	return str;
}

#endif	// #ifdef LINUX


//----------------------------------------------------

int main (int argc, char** argv)
{
#ifdef LINUX
	struct sigaction sv;
	sigemptyset(&sv.sa_mask);
	sv.sa_flags = 0;
	sv.sa_handler = SignalHandler;
	sigaction(SIGTERM, &sv, NULL);
	sigaction(SIGQUIT, &sv, NULL);
	sigaction(SIGINT, &sv, NULL);
//	sigaction(SIGHUP, &sv, NULL);
	sigaction(SIGUSR1, &sv, NULL);
	sigaction(SIGUSR2, &sv, NULL);
	bool bOutputEnable = false;
#endif

	bool bEnableAnnounce = true;
	int iMaxPlayers	= DEFAULT_MAX_PLAYERS;
	int iListenPort	= DEFAULT_LISTEN_PORT;
	int iRconPort	= DEFAULT_RCON_PORT;
	int iRconMaxAdmins	= DEFAULT_RCON_MAXUSERS;
	bool bLanModeEnable = false;
	bool bEnableTimestamp = true;
	bool bEnableInstagib = false;
	bool bGameMod = false;
	bool bEnableAC = false;
	bool bAllowQuery = true;

	// Open the log file
	LoadLogFile();

	// Create the command line string and process if needed.
	char szCmdLine[1024];
	memset(szCmdLine,0,1024);

	int cmdcnt=1;
	if(argc > 1) {
		while(cmdcnt != argc) {
			strcat(szCmdLine, argv[cmdcnt]);
			strcat(szCmdLine, " ");
			cmdcnt++;
		}
		InitSettingsFromCommandLine(szCmdLine);
	}

	// Write welcome message.
	logprintf("");
	logprintf("SA-MP Dedicated Server");
	logprintf("----------------------");
	logprintf("v" SAMP_VERSION ", (C)2005-2009 SA-MP Team\n");

#ifdef _DEBUG
	logprintf("Debug Build Info:\n   NET_VERSION=%d\n   BUILD_DATE=%s\n   BUILD_TIME=%s\n",
				NETGAME_VERSION, __DATE__, __TIME__);
#endif

	// Create a challenge number for the clients to be able to connect
	srand(time(NULL));
	_uiRndSrvChallenge = (unsigned int)rand();
    
	// Create the Console
	pConsole = new CConsole();

	pConsole->AddVariable("announce",CON_VARTYPE_BOOL, 0, &bEnableAnnounce);
	
	if(gServerSettings.iMaxPlayers) {
		iMaxPlayers = gServerSettings.iMaxPlayers;
		pConsole->AddVariable("maxplayers", CON_VARTYPE_INT, 0, &iMaxPlayers);
		pConsole->ModifyVariableFlags("maxplayers", CON_VARFLAG_READONLY);
	} else {
		pConsole->AddVariable("maxplayers", CON_VARTYPE_INT, 0, &iMaxPlayers, ServerMaxPlayersChanged);
	}

	if(gServerSettings.iPort) {
        iListenPort = gServerSettings.iPort;
		pConsole->AddVariable("port", CON_VARTYPE_INT, 0, &iListenPort);
		pConsole->ModifyVariableFlags("port", CON_VARFLAG_READONLY);
	} else {
		pConsole->AddVariable("port", CON_VARTYPE_INT, 0, &iListenPort);
	}

	if(strlen(gServerSettings.szBindIp)) {
		pConsole->AddStringVariable("bind", 0, gServerSettings.szBindIp);
		pConsole->ModifyVariableFlags("bind", CON_VARFLAG_READONLY);
	} else {
		pConsole->AddStringVariable("bind", 0, NULL);
	}

	pConsole->AddVariable("lanmode",CON_VARTYPE_BOOL,0, &bLanModeEnable);
	pConsole->AddVariable("query",CON_VARTYPE_BOOL, 0, &bAllowQuery);

#ifdef RAKRCON
	pConsole->AddVariable("rcon_port", CON_VARTYPE_INT, 0, &iRconPort);
	pConsole->AddVariable("rcon_maxadmins", CON_VARTYPE_INT, 0, &iRconMaxAdmins);
	pConsole->AddStringVariable("rcon_bind", 0, NULL);
#endif

#ifdef LINUX
	pConsole->AddVariable("output",CON_VARTYPE_BOOL,0,&bOutputEnable);
#endif

	pConsole->AddVariable("timestamp",CON_VARTYPE_BOOL,0,&bEnableTimestamp);
	pConsole->AddStringVariable("password", 0, NULL, ServerPasswordChanged);
	pConsole->AddStringVariable("hostname", 0, "SA:MP Server");
	pConsole->AddStringVariable("mapname", CON_VARFLAG_RULE, "San Andreas");
	pConsole->AddStringVariable("weburl", CON_VARFLAG_RULE, "www.sa-mp.com");
	pConsole->AddStringVariable("rcon_password", 0, "changeme");
	pConsole->AddStringVariable("gravity", CON_VARFLAG_RULE, "0.008");
	pConsole->AddStringVariable("weather", CON_VARFLAG_RULE, "10");
	//pConsole->AddStringVariable("tirepopping", CON_VARFLAG_RULE, "0");
	pConsole->AddStringVariable("gamemodetext", 0, "Unknown");
	pConsole->AddStringVariable("filterscripts", 0, "");
	pConsole->AddStringVariable("plugins", 0, "");
	pConsole->AddStringVariable("nosign", 0, "");
	pConsole->AddVariable("anticheat",CON_VARTYPE_BOOL, /* CON_VARFLAG_RULE */ 0, &bEnableAC);
	pConsole->AddVariable("instagib", CON_VARTYPE_BOOL, CON_VARFLAG_RULE, &bEnableInstagib, ServerInstagibChanged);
	pConsole->AddVariable("myriad", CON_VARTYPE_BOOL, 0, &bGameMod);

	// Add 16 gamemode variables.
	int x=0;
	char t[64];
	while(x!=16) {
		sprintf(t,"gamemode%u",x);
		pConsole->AddStringVariable(t,0,"");
		x++;
	}
	
	// Exec the server config!
	pConsole->Execute("exec server");

	if ( !strcmp( pConsole->GetStringVariable("rcon_password"), "changeme" ) )
	{
		logprintf("Error: Your password must be changed from the default password, please change it.");
		return 0;
	}

	// Change some var flags to read-only (can only be accessed from server.cfg).
	pConsole->ModifyVariableFlags("maxplayers", CON_VARFLAG_READONLY);
	pConsole->ModifyVariableFlags("bind", CON_VARFLAG_READONLY);
	pConsole->ModifyVariableFlags("port", CON_VARFLAG_READONLY);
	pConsole->ModifyVariableFlags("rcon_bind", CON_VARFLAG_READONLY);
	pConsole->ModifyVariableFlags("rcon_port", CON_VARFLAG_READONLY);
	pConsole->ModifyVariableFlags("filterscripts", CON_VARFLAG_READONLY);
	pConsole->ModifyVariableFlags("plugins", CON_VARFLAG_READONLY);
	pConsole->ModifyVariableFlags("anticheat", CON_VARFLAG_READONLY /* | CON_VARFLAG_RULE */);
	pConsole->ModifyVariableFlags("nosign", CON_VARFLAG_READONLY);

	// Add the version as a rule
	pConsole->AddStringVariable("version", CON_VARFLAG_RULE | CON_VARFLAG_READONLY, SAMP_VERSION);

	// If we're running in Windows, allow console input and catch exit messages.
	// TODO: Same thing for Linux.
#ifdef WIN32
	SetConsoleCtrlHandler(CtrlHandler, TRUE);
	hConsoleExecuteEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	DWORD dwThreadId;
	HANDLE hThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ConsoleInputThread, NULL, 0, &dwThreadId);
	
	// Setup the exception handler on windows
	SetUnhandledExceptionFilter(exc_handler);
#endif

	// Load up the plugins
	pPlugins = new CPlugins();
	pPlugins->LoadPlugins("plugins");

	// Create the NetGame.
	pNetGame = new CNetGame();
	pNetGame->Init(true);

	// Start the rcon server
	PCHAR szBindAddress = pConsole->GetStringVariable("rcon_bind");
	if (!szBindAddress || szBindAddress[0] == 0)
		szBindAddress = pConsole->GetStringVariable("bind");
	if (!szBindAddress || szBindAddress[0] == 0)
		szBindAddress = NULL;

#ifdef RAKRCON
	pRcon = new CRcon(pConsole->GetStringVariable("rcon_password"), 
						pConsole->GetIntVariable("rcon_port"), 
						pConsole->GetIntVariable("rcon_maxadmins"),
						szBindAddress);
	pRcon->Process();
#endif

	// While the app is running...
	while (!bQuitApp)
	{
		pNetGame->Process();

#ifdef RAKRCON
		pRcon->Process();
#endif

		if(TRUE == bGameModeFinished) {
			pNetGame->ShutdownForGameModeRestart();
			bGameModeFinished = FALSE;
		}

		#ifdef WIN32
			SetEvent(hConsoleExecuteEvent);
			WaitForSingleObject(hConsoleExecuteEvent, INFINITE);
		#endif

		SLEEP(10);		
	}

#ifdef RAKRCON
	delete pRcon;
#endif

	delete pNetGame;

	delete pPlugins;

	// If WIN32: Kill the input thread.
	#ifdef WIN32
		TerminateThread(hThread, 0);
		CloseHandle(hConsoleExecuteEvent);
	#endif

	delete pConsole;

	fclose(pLogFile);

	return 0;
}

//----------------------------------------------------

void RconSocketReply(char* szMessage); // query.cpp

void logprintf(char* format, ...)
{
	va_list ap;
	va_start(ap, format);

	char buffer[512];
	vsprintf(buffer, format, ap);

	va_end(ap);

#ifdef WIN32
	puts(buffer);
#else
	if (pConsole && pConsole->GetBoolVariable("output"))
	{
		puts(buffer);
	}
#endif
	if (pLogFile) {
		if (pConsole) {
			if (pConsole->GetBoolVariable("timestamp"))
			{
				const struct tm *tm;
				time_t now;
				now = time(NULL);
				tm = localtime(&now);
				char *s;
				s = new char[256];
				strftime(s, 256, "[%H:%M:%S]", tm);
				fprintf(pLogFile, "%s %s\n", s, buffer);
				delete [] s;
			}
			else fprintf(pLogFile, "%s\n", buffer);
		}
		else fprintf(pLogFile, "%s\n", buffer);
		fflush(pLogFile);
	}

	if (byteRconUser != INVALID_ID)
	{
		pNetGame->SendClientMessage(pNetGame->GetRakServer()->GetPlayerIDFromIndex(byteRconUser), 0xFFFFFFFF, buffer);
	} else if (bRconSocketReply) {
		RconSocketReply(buffer);
	}
}

//----------------------------------------------------

// Print to log file only.
void flogprintf(char* format, ...)
{
	if (!pLogFile) return;
	va_list ap;
	va_start(ap, format);
	char buffer[512];
	vsprintf(buffer, format, ap);
	fprintf(pLogFile, "%s\n", buffer);
	fflush(pLogFile);
	va_end(ap);
}

//----------------------------------------------------

void SetStringFromCommandLine(char *szCmdLine, char *szString);

void InitSettingsFromCommandLine(char * szCmdLine)
{
	//printf("CmdLine: %s",szCmdLine);

	memset(&gServerSettings,0,sizeof(SERVER_SETTINGS));

	char tmp[256];

	while(*szCmdLine) {

		if(*szCmdLine == '-' || *szCmdLine == '/') {
			szCmdLine++;
			switch(*szCmdLine) {
				case 'm':
					szCmdLine++;
					SetStringFromCommandLine(szCmdLine,tmp);
                    gServerSettings.iMaxPlayers = atoi(tmp);
					break;
				case 'p':
					szCmdLine++;
					SetStringFromCommandLine(szCmdLine,tmp);
                    gServerSettings.iPort = atoi(tmp);
					break;
				case 'b':
					szCmdLine++;
					SetStringFromCommandLine(szCmdLine,gServerSettings.szBindIp);
					break;
			}
		}

		szCmdLine++;
	}
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
