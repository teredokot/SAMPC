#include "anticheat.h"
#include "httpclient.h"
#include <stdio.h>

#define AC_AUTH_URL "auth.sa-mp.com/sv/"
#define AC_AUTH_N_URL "auth%d.sa-mp.com/sv/"
#define AC_AUTH_MAX 5
#define AC_AUTH_QUERY_FORMAT "a=%s&k=%d"
#define AC_AUTH_TEST_FORMAT "p=1"

#ifdef _DEBUG
#define TIME_ENGINE_SENT_WAIT 10
#define TIME_AUTHKEY_REPLY    30
#define TIME_BETWEEN_AUTHS    30
#define TIME_SERVER_CHECK     15
#else
#define TIME_ENGINE_SENT_WAIT 60
#define TIME_AUTHKEY_REPLY    30
#define TIME_BETWEEN_AUTHS    120
#define TIME_SERVER_CHECK     120
#endif

extern bool bQuitApp;

CAntiCheat::PlayerACInfo CAntiCheat::ms_playerACInfo[MAX_PLAYERS];

time_t CAntiCheat::ms_timeLastServerCheck = 0;
bool   CAntiCheat::ms_bLastServerCheckSuccess = true;
bool   CAntiCheat::ms_bServerCheckSuccess = false;

bool CAntiCheat::Initialize(CNetGame *pNetGame)
{
	memset(ms_playerACInfo, 0, sizeof(ms_playerACInfo));
	
	// Create a thread to process AuthRequests

#ifdef _WIN32
	DWORD dwThreadId = 0;
	HANDLE threadHandle;
	threadHandle = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ProcessAuthThread, NULL, 0, &dwThreadId);
	if (threadHandle == 0 || threadHandle == INVALID_HANDLE_VALUE)
		return false;
	CloseHandle(threadHandle);
#else
	pthread_t threadHandle;
	pthread_attr_t attr;
	pthread_attr_init( &attr );
	pthread_attr_setdetachstate( &attr, PTHREAD_CREATE_DETACHED );

	int error = pthread_create( &threadHandle, &attr, &ProcessAuthThread, NULL );
	if (error)
		return false;
#endif

	// Register some RPCs
	pNetGame->GetRakServer()->RegisterAsRemoteProcedureCall(RPC_ACAuthResponse, CAntiCheat::RPCAuthResponse);
	pNetGame->GetRakServer()->RegisterAsRemoteProcedureCall(RPC_ACAuthEngineLoaded, CAntiCheat::RPCAuthEngineLoaded);

	return true;
}

void CAntiCheat::Shutdown(CNetGame *pNetGame)
{
	pNetGame->GetRakServer()->UnregisterAsRemoteProcedureCall(RPC_ACAuthResponse);
	pNetGame->GetRakServer()->UnregisterAsRemoteProcedureCall(RPC_ACAuthEngineLoaded);
}

void CAntiCheat::VerifyWithAuthServer(UINT nPlayer)
{
	CHAR szQuery[64];
	CHAR szAuthURL[64];
	bool bSuccess = false;
	CHttpClient client;

	// Determine PlayerID from Index
	PlayerID playerID = pNetGame->GetRakServer()->GetPlayerIDFromIndex(nPlayer);

	// Build HTTP request and send it

	sprintf(szQuery, AC_AUTH_QUERY_FORMAT, playerID.ToString(false), ms_playerACInfo[nPlayer].nAuthKey);

	// Try the player's auth server...
	if (ms_playerACInfo[nPlayer].nServerIndex == 1)
		strcpy(szAuthURL, AC_AUTH_URL);
	else
		sprintf(szAuthURL, AC_AUTH_N_URL, (UINT)(ms_playerACInfo[nPlayer].nServerIndex));

	if (client.ProcessURL(HTTP_POST, szAuthURL, szQuery, "") == HTTP_SUCCESS)
		bSuccess = true;

    // Try another server if the player's auth server didn't respond
	if (!bSuccess)
	{
		for(int i=1; i<=AC_AUTH_MAX; i++)
		{
			if (i != ms_playerACInfo[nPlayer].nServerIndex)
			{
				if (i == 1)
					strcpy(szAuthURL, AC_AUTH_URL);
				else
					sprintf(szAuthURL, AC_AUTH_N_URL, i);

				if (client.ProcessURL(HTTP_POST, szAuthURL, szQuery, "") == HTTP_SUCCESS)
					bSuccess = true;
			}
		}
	}

	if (bSuccess)
	{
		// Connected successfully and received response   
		INT rc = client.GetResponseCode();

		if (rc == 200)	// OK
		{
			ms_playerACInfo[nPlayer].bAuthFailed = false;
		}
		else
		{
			// Any other response code
			ms_playerACInfo[nPlayer].bAuthFailed = true;
		}
	}
	else
	{
		// Failed to connect to ANY auth server
		// We'll allow the player to continue since its the Auth Server's fault (or is it?!)

		ms_playerACInfo[nPlayer].bAuthFailed = false;
	}

}

bool CAntiCheat::CheckAuthServerOnline()
{
	CHAR szAuthURL[64];
	CHttpClient client;

	for(int i=1; i<=AC_AUTH_MAX; i++)
	{
		if (i == 1)
			strcpy(szAuthURL, AC_AUTH_URL);
		else
			sprintf(szAuthURL, AC_AUTH_N_URL, i);

#ifdef _DEBUG
		logprintf("[sac] Trying to receive: %s...", szAuthURL);
#endif

		if (client.ProcessURL(HTTP_POST, szAuthURL, AC_AUTH_TEST_FORMAT, "") == HTTP_SUCCESS)
		{
			if (client.GetResponseCode() == 200)
			{
#ifdef _DEBUG
				logprintf("[sac] Success.");
#endif
				return true;
			}
		}
	}

	return false;
}

void CAntiCheat::AuthLoadEngine(UINT nPlayer)
{
	// Determine PlayerID from Index
	PlayerID playerID = pNetGame->GetRakServer()->GetPlayerIDFromIndex(nPlayer);

	// Update player data
	ms_playerACInfo[nPlayer].bIsEngineSent = true;
	ms_playerACInfo[nPlayer].timeEngineSent = time(NULL);

	// Send a request for loading engine
	pNetGame->GetRakServer()->RPC(RPC_ACServerProtected,NULL,HIGH_PRIORITY,RELIABLE,0,playerID,false,false);
}

void CAntiCheat::AuthRequest(UINT nPlayer)
{
	// Determine PlayerID from Index
	PlayerID playerID = pNetGame->GetRakServer()->GetPlayerIDFromIndex(nPlayer);

	// Update player data
	ms_playerACInfo[nPlayer].bNewAuthKey = false;
	ms_playerACInfo[nPlayer].timeLastAuthReqSent = time(NULL);
	ms_playerACInfo[nPlayer].nAuthKey = 0;

	// Send a request for authentication
	pNetGame->GetRakServer()->RPC(RPC_ACAuthRequest,NULL,HIGH_PRIORITY,RELIABLE,0,playerID,false,false);	
}

void CAntiCheat::AuthResponse(UINT nPlayer, UINT nAuthKey, char nServerIndex)
{
	// Update player data
	ms_playerACInfo[nPlayer].timeLastAuthKeyRecv = time(NULL);
	ms_playerACInfo[nPlayer].nAuthKey = nAuthKey;
	ms_playerACInfo[nPlayer].nServerIndex = nServerIndex;
	ms_playerACInfo[nPlayer].bNewAuthKey = true;
}

void CAntiCheat::AuthEngineLoaded(UINT nPlayer)
{
	// Update player data
	ms_playerACInfo[nPlayer].bIsEngineLoaded = true;
	ms_playerACInfo[nPlayer].timeEngineLoaded = time(NULL);

	// Request our first auth
	AuthRequest(nPlayer);
}

void CAntiCheat::RPCAuthResponse(RPCParameters *rpcParams)
{
	// This is the static method that will be called by RakNet's RPC processor
	// Kept in here rather than netrpc for cleanliness.

	UINT nPlayer = pNetGame->GetRakServer()->GetIndexFromPlayerID(rpcParams->sender);

	UINT nAuthKey;
	char nServerIndex;

	RakNet::BitStream bsParams(rpcParams->input, (rpcParams->numberOfBitsOfData/8)+1, false);
	bsParams.Read(nAuthKey);
	bsParams.Read(nServerIndex);

	if (pNetGame->GetPlayerPool()->GetSlotState(nPlayer))
	{
		CAntiCheat::AuthResponse(nPlayer, nAuthKey, nServerIndex);
	}

}

void CAntiCheat::RPCAuthEngineLoaded(RPCParameters *rpcParams)
{
	// This is the static method that will be called by RakNet's RPC processor
	// Kept in here rather than netrpc for cleanliness.

	UINT nPlayer = pNetGame->GetRakServer()->GetIndexFromPlayerID(rpcParams->sender);
	
	if (pNetGame->GetPlayerPool()->GetSlotState(nPlayer))
	{
		CAntiCheat::AuthEngineLoaded(nPlayer);
	}

}

void CAntiCheat::InitForPlayer(UINT nPlayer)
{
	memset(&ms_playerACInfo[nPlayer], 0, sizeof(CAntiCheat::PlayerACInfo));

	ms_playerACInfo[nPlayer].bIsFirstAuth = true;

	if (!ms_bServerCheckSuccess)
	{
		// Uh oh! There is no auth server access at the moment. 
		// Let the user pass through without authentication.

		logprintf("[sac] Auth Server Offline. Allowing player %d without auth.", nPlayer);

		ms_playerACInfo[nPlayer].bClientJoinProcessed = true;
		pNetGame->ProcessClientJoin((BYTE)nPlayer);

	}

}

void CAntiCheat::Process()
{
	// Have we even connected to an auth server yet?
	if (ms_timeLastServerCheck == 0)
		return;

	// Inform the log about auth server status changes
	if (ms_bLastServerCheckSuccess && !ms_bServerCheckSuccess)
		logprintf("[sac] Auth servers are currently offline.");
	else if (!ms_bLastServerCheckSuccess && ms_bServerCheckSuccess)
		logprintf("[sac] Auth servers are back online.");

	// Update the last status
	ms_bLastServerCheckSuccess = ms_bServerCheckSuccess;

	// If the servers are currently offline, don't proceed!
	if (!ms_bServerCheckSuccess)
		return;

	bool bKickUser = false;
	time_t timeNow;

	timeNow = time(NULL);

	// Loop for all players
	for(UINT i=0; i<MAX_PLAYERS; i++)
	{
		// Make sure player slot is active
		if (pNetGame->GetPlayerPool()->GetSlotState(i))
		{
			CAntiCheat::PlayerACInfo *pACInfo = &ms_playerACInfo[i];
			
			// Check for AC engine being sent
			if (pACInfo->bIsEngineSent)
			{
				if (!pACInfo->bIsEngineLoaded)
				{
					// Engine is not loaded yet, check for timeouts (i.e. if they bypass the loading thing)
					if ((timeNow - pACInfo->timeEngineSent) > TIME_ENGINE_SENT_WAIT)
					{
						// Exceeded our time limit, mark as a bad player.
						bKickUser = true;
					}
				}
				else
				{
					// Engine has been loaded.
					
					if (pACInfo->bAuthFailed)
					{
						// Last auth was a failure
						bKickUser = true;
					}
					else if (!pACInfo->bNewAuthKey &&											// Don't have new key to process
							(pACInfo->timeLastAuthKeyRecv < pACInfo->timeLastAuthReqSent) &&	// We haven't received our reply yet
							(timeNow - pACInfo->timeLastAuthReqSent) > TIME_AUTHKEY_REPLY)		// We expired the time limit
							
					{
						// Exceeded our time limit for auth, mark as bad
						bKickUser = true;
					}
					else
					{
						// Should be a full success at this point.
						
						if (pACInfo->bIsFirstAuth && (pACInfo->timeLastAuthKeyRecv >= pACInfo->timeLastAuthReqSent))
						{
							// If this is our first auth, lets do some processing...
							pACInfo->bIsFirstAuth = false;

							if (!pACInfo->bClientJoinProcessed)
							{
								pACInfo->bClientJoinProcessed = true;
								pNetGame->ProcessClientJoin((BYTE)i);
							}
						}

						if ((timeNow - pACInfo->timeLastAuthReqSent) > TIME_BETWEEN_AUTHS)
						{
							// Request new auth if we are over our time limit
							AuthRequest(i);
						}
					}
				}
			}
			else
			{
				// No auth engine was sent... boo!
				// Lets make them load it up

				AuthLoadEngine(i);
			}

			if (bKickUser)
			{
				logprintf("[sac] Player %d failed SAC Auth, kicked.", i);
				pNetGame->KickPlayer(i);
				bKickUser = false;
			}
		}
	}

}

#ifdef _WIN32
unsigned int __stdcall CAntiCheat::ProcessAuthThread(LPVOID pArgs)
#else
void* CAntiCheat::ProcessAuthThread(void* pArgs)
#endif
{
	while(!bQuitApp)
	{
		ProcessAuth();
		SLEEP(1000);
	}
	return 0;
}

void CAntiCheat::ProcessAuth()
{
	if (!pNetGame)
		return;

	if (!pNetGame->GetPlayerPool())
		return;

	// Verify the server is alive every few minutes
	if ((time(NULL) - ms_timeLastServerCheck) > TIME_SERVER_CHECK)
	{
		bool bServerAlive = CheckAuthServerOnline();
		if (ms_bServerCheckSuccess && !bServerAlive)
		{
			//OutputDebugString("Auth Server died.");
		}
		else if (!ms_bServerCheckSuccess && bServerAlive)
		{
			//OutputDebugString("Auth Server alive.");
			
			// Reset all the players auth data, so they get authed again
			for (UINT i=0; i<MAX_PLAYERS; i++)
			{
				// So that we can avoid the check for timeouts
				ms_playerACInfo[i].timeLastAuthReqSent = 0;
				ms_playerACInfo[i].timeLastAuthKeyRecv = 0;
				ms_playerACInfo[i].bNewAuthKey = false;
			}

		}
		ms_bServerCheckSuccess = bServerAlive;

		ms_timeLastServerCheck = time(NULL);
	}

	if (!ms_bServerCheckSuccess)
		return;

	// Loop for all players
	for(UINT i=0; i<MAX_PLAYERS; i++)
	{
		// Make sure player slot is active
		if (pNetGame->GetPlayerPool()->GetSlotState(i))
		{
			CAntiCheat::PlayerACInfo *pACInfo = &ms_playerACInfo[i];
			
			if (!pACInfo->bAuthFailed && pACInfo->bNewAuthKey)
			{
				pACInfo->bNewAuthKey = false;
				VerifyWithAuthServer(i);
			}
		}
	}

}
