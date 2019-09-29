/*

	SA:MP Multiplayer Modification
	Copyright 2004-2005 SA:MP Team

	file:
		console.h
	desc:
		Console command and variable system header file.

    Version: $Id: console.h,v 1.6 2006/03/20 17:59:34 kyeman Exp $

*/

#ifndef SAMPSRV_CONSOLE_H
#define SAMPSRV_CONSOLE_H

enum CON_VARTYPE {CON_VARTYPE_FLOAT, CON_VARTYPE_INT, CON_VARTYPE_BOOL, CON_VARTYPE_STRING};

#define CON_VARFLAG_DEBUG		1
#define CON_VARFLAG_READONLY	2
#define CON_VARFLAG_RULE		4	// Gets sent with a RULES query responce

typedef void (*VARCHANGEFUNC)();

struct ConsoleVariable_s
{
	CON_VARTYPE VarType;
	DWORD VarFlags;
	void* VarPtr;
	VARCHANGEFUNC VarChangeFunc;
};

typedef std::map<std::string, ConsoleVariable_s*> StringConvarMap;

class CConsole
{
public:
	StringConvarMap ConsoleVariables;

	CConsole();
	~CConsole();

	ConsoleVariable_s* FindVariable(char* pVarName);
	void PrintVariableList();

	void AddVariable(char* pVarName, CON_VARTYPE VarType, DWORD VarFlags, void* VarPtr,
		VARCHANGEFUNC VarChangeFunc = NULL);
	char* AddStringVariable(char* pVarName, DWORD VarFlags, char* pInitStr,
		VARCHANGEFUNC VarChangeFunc = NULL);
	void RemoveVariable(char* pVarName);
	void ModifyVariableFlags(char* pVarName, DWORD VarFlags);

	char* GetStringVariable(char* pVarName);
	void SetStringVariable(char* pVarName, char* pString);

	float GetFloatVariable(char* pVarName);
	void SetFloatVariable(char* pVarName, float fFloat);

	int GetIntVariable(char* pVarName);
	void SetIntVariable(char* pVarName, int iInt);

	bool GetBoolVariable(char* pVarName);
	void SetBoolVariable(char* pVarName, bool bBool);

	void SendRules(SOCKET s, char* data, const sockaddr_in* to, int tolen);

	void Execute(char* pExecLine);
};

#endif