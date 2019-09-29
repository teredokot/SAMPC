#pragma once

#include "main.h"

class CAntiCheat
{
private:
	typedef struct _PlayerACInfo
	{
		bool bIsEngineSent;
		bool bClientJoinProcessed;
		bool bIsEngineLoaded;
		time_t timeEngineSent;
		time_t timeEngineLoaded;
		time_t timeLastAuthReqSent;
		time_t timeLastAuthKeyRecv;
		bool bIsFirstAuth;
		bool bNewAuthKey;
		UINT nAuthKey;
		bool bAuthFailed;
		char nServerIndex;
	} PlayerACInfo;

	static PlayerACInfo ms_playerACInfo[MAX_PLAYERS];
	static time_t ms_timeLastServerCheck;
	static bool ms_bLastServerCheckSuccess;	// previous value
	static bool ms_bServerCheckSuccess;		// current value

    static void VerifyWithAuthServer(UINT nPlayer);
	
	static bool CheckAuthServerOnline();

    static void AuthLoadEngine(UINT nPlayer);
	static void AuthRequest(UINT nPlayer);
	static void AuthResponse(UINT nPlayer, UINT nAuthKey, char nServerIndex);
	static void AuthEngineLoaded(UINT nPlayer);

	static void RPCAuthResponse(RPCParameters *rpcParams);
	static void RPCAuthEngineLoaded(RPCParameters *rpcParams);

#ifdef _WIN32
	static unsigned int __stdcall ProcessAuthThread(LPVOID pArgs);
#else
	static void* ProcessAuthThread(void* pArgs);
#endif

	static void ProcessAuth();

public:
	
	static bool Initialize(CNetGame *pNetGame);
	static void Shutdown(CNetGame *pNetGame);

	static void InitForPlayer(UINT nPlayer);
	static void Process();

	static bool TestAuthServerConnection();

};
