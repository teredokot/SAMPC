/*

	SA:MP Multiplayer Modification
	Copyright 2004-2005 SA:MP Team

	file:
		main.h
	desc:
		Main header file for the whole project.
		Should be included by all *.cpp files.

*/

#ifndef SAMPSRV_MAIN_H
#define SAMPSRV_MAIN_H

#pragma warning(disable:4786)
#pragma warning(disable:4996)
#pragma warning(disable:4244)

// -------
// DEFINES
// -------

//#define RAKRCON

#define MAX_PLAYER_NAME			24
#ifdef SCRIPTING_ONLY
#define MAX_PLAYERS				8		// Only 8 players for scripting builds
#else
#define MAX_PLAYERS				200
#endif
#define MAX_VEHICLES			700
#define MAX_FILTER_SCRIPTS		16
#define MAX_OBJECTS				255
#define MAX_MENUS				128
#define MAX_TEXT_DRAWS			1024
#define MAX_GANG_ZONES			1024
#define MAX_CMD_INPUT			128		// This limitation is also found on the client. it applies to chat and commands.

#define DEFAULT_MAX_PLAYERS		32
#define DEFAULT_LISTEN_PORT		8192
#define DEFAULT_RCON_PORT		8193
#define DEFAULT_RCON_MAXUSERS	8

#define EVENT_TYPE_PAINTJOB			1
#define EVENT_TYPE_CARCOMPONENT		2
#define EVENT_TYPE_CARCOLOR			3

#define PI 3.14159265

#define ARRAY_SIZE(a)	( sizeof((a)) / sizeof(*(a)) )
#define SAFE_DELETE(p)	{ if (p) { delete (p); (p) = NULL; } }
#define SAFE_RELEASE(p)	{ if (p) { (p)->Release(); (p) = NULL; } }

// ------------
// VERSION INFO
// ------------

#define SAMP_VERSION "0.2X"
#define NETGAME_VERSION 8866

// ------------
// OS SPECIFICS
// ------------

#ifdef WIN32
	#define WIN32_LEAN_AND_MEAN
	#define SLEEP(x) { Sleep(x); }

	#include <windows.h>
	#include <tchar.h>
	#include <mmsystem.h>
	#include <malloc.h>
	#include <shellapi.h>
	#include <time.h>
#else
	#define SLEEP(x) { usleep(x * 1000); }
	#define MAX_PATH 260

	#include <dlfcn.h>
	#include <unistd.h>
	#include <sys/time.h>
	#include <sys/times.h>
	#include <signal.h>
	#include <sys/types.h>
	#include <sys/sysinfo.h>
	#include <dirent.h>

	typedef int SOCKET;

	#ifndef stricmp
		#define stricmp strcasecmp
	#endif
#endif


// --------
// SETTINGS DEF
typedef struct _SERVER_SETTINGS {
	int  iMaxPlayers;
	int  iPort;
	char szBindIp[64];
} SERVER_SETTINGS;

// --------
// INCLUDES
// --------

// Regular crap
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdarg.h>

// Std
#include <map>
#include <string>
#include <vector>

// Raknet
#include "../raknet/RakServer.h"
#include "../raknet/RakNetworkFactory.h"
#include "../raknet/BitStream.h"
#include "../raknet/PacketEnumerations.h"
#include "../raknet/SAMPRPC.h"

// amx
#include "amx/amx.h"

// SA:MP

typedef unsigned short VEHICLEID;

#include "system.h"
#include "console.h"
#include "scrtimers.h"
#include "gamemodes.h"
#include "filterscripts.h"
#include "netrpc.h"
#include "player.h"
#include "playerpool.h"
#include "vehicle.h"
#include "vehiclepool.h"
#include "pickuppool.h"
#include "object.h"
#include "objectpool.h"
#include "menu.h"
#include "menupool.h"
#include "textdrawpool.h"
#include "gangzonepool.h"
#include "netgame.h"
#include "plugins.h"
#include "rcon.h"

// ---------
// EXTERNALS
// ---------

extern CConsole* pConsole;
extern CNetGame* pNetGame;
extern CPlugins* pPlugins;

#ifdef RAKRCON
extern CRcon *pRcon;
#endif

extern BYTE byteRconUser;
extern BOOL bRconSocketReply;

// -------------------
// FUNCTION PROTOTYPES
// -------------------

void logprintf(char* format, ...);
void flogprintf(char* format, ...);
void LoadLogFile();

#ifdef LINUX
void SignalHandler(int sig);
long GetTickCount();
char* strlwr(char* str);
#endif

#endif // SAMPSRV_MAIN_H
