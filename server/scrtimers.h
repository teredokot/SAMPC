/*

	SA:MP Multiplayer Modification
	Copyright 2004-2006 SA:MP Team

	file:
		scrtimers.h
	desc:
		Gamemode script timers header file.

    Version: $Id: scrtimers.h,v 1.3 2006/03/25 01:47:13 spookie Exp $

*/

#ifndef SAMPSRV_TIMERS_H
#define SAMPSRV_TIMERS_H

struct ScriptTimer_s
{
	char szScriptFunc[255];
	int iTotalTime;
	int iRemainingTime;
	BOOL bRepeating;
	//BOOL bFilterscript;
	BOOL bKilled;
	AMX* pAMX;
	int iParamCount;
	//cell cellParams[16];
	//char szParamMap[16];
	void* cellParams;
	//void* szParamMap;
};

typedef std::map<DWORD, ScriptTimer_s*> DwordTimerMap;

//----------------------------------------------------------------------------------

class CScriptTimers
{
private:
	DwordTimerMap m_Timers;
	DWORD m_dwTimerCount;
public:
	CScriptTimers();
	~CScriptTimers();
	
	void DeleteForMode(AMX* pEndedAMX);
	DWORD New(char* szScriptFunc, int iInterval, BOOL bRepeating, AMX* pAMX);
	DWORD NewEx(char* szScriptFunc, int iInterval, BOOL bRepeating, cell *params, AMX* pAMX);
	void Delete(DWORD dwTimerId);
	void Kill(DWORD dwTimerId);
	void Process(int iElapsedTime);
	void FreeMem(ScriptTimer_s* Timer);
};


#endif


//----------------------------------------------------------------------------------
// EOF
