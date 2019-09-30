//----------------------------------------------------------
//
// SA:MP Multiplayer Modification For GTA:SA
// Copyright 2004-2005 SA:MP team
//
// Version: $Id: cmdwindow.h,v 1.9 2006/05/08 13:28:46 kyeman Exp $
//
//----------------------------------------------------------

//#pragma twice <- that's a joke btw.

#pragma once

#define MAX_CMD_INPUT   128
#define MAX_CMDS		128
#define MAX_CMD_STRLEN  32
#define CMD_CHARACTER   '/'
#define MAX_RECALLS		10

typedef void (__cdecl *CMDPROC)(PCHAR);

//----------------------------------------------------

class CCmdWindow
{
private:

	IDirect3DDevice9 *m_pD3DDevice;
	CDXUTDialog		*m_pGameUI;
	CDXUTEditBox	*m_pEditControl;

public:

	char		m_szInputBuffer[MAX_CMD_INPUT + 1];
	char		m_szRecallBuffer[MAX_RECALLS][MAX_CMD_INPUT + 1];
	char		m_szCurBuffer[MAX_CMD_INPUT+1];
    int			m_iCurrentRecallAt;
	int			m_iTotalRecalls;

	BOOL		m_bEnabled;
	
	// Command procedure stuff.
	CMDPROC		m_pDefaultCmd;	 // used when no command specifier was
								 // used (ie. a normal chat message)
	CMDPROC		m_pCmds[MAX_CMDS];
	CHAR        m_szCmdNames[MAX_CMDS][MAX_CMD_STRLEN+1];
	int			m_iCmdCount;

	void Draw();

	void Enable();
	void Disable();

	BOOL isEnabled() { return m_bEnabled; };

	int MsgProc(UINT uMsg, DWORD wParam, DWORD lParam);

	void ProcessInput();
	void RecallUp();
	void RecallDown();
	
	CMDPROC GetCmdHandler(PCHAR szCmdName);
	void AddDefaultCmdProc(CMDPROC cmdDefault);
	void AddCmdProc(PCHAR szCmdName, CMDPROC cmdHandler);

	void AddToRecallBuffer(char *szCmdInput);
	void SendToServer(char* szServerCommand);

	void ResetDialogControls(CDXUTDialog *pGameUI);
	CCmdWindow(IDirect3DDevice9 *pD3DDevice);
	~CCmdWindow();
};

//----------------------------------------------------
// EOF