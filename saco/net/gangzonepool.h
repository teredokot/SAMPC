#pragma once

/*

SA:MP Multiplayer Modification
Copyright 2004-2005 SA:MP Team

Version: $Id: gangzonepool.h,v 1.0 2007/05/25 19:26:45 Y_Less Exp $

*/

//----------------------------------------------------

typedef struct _GANG_ZONE
{
	float	fPos[4];
	DWORD	dwColor;
	DWORD	dwAltColor;
} GANG_ZONE;

class CGangZonePool
{
private:
	GANG_ZONE		*m_pGangZone[MAX_GANG_ZONES];
	BOOL			m_bSlotState[MAX_GANG_ZONES];
public:
	CGangZonePool();
	~CGangZonePool();
	void New(WORD wZone, float fMinX, float fMinY, float fMaxX, float fMaxY, DWORD dwColor);
	void Flash(WORD wZone, DWORD dwColor);
	void StopFlash(WORD wZone);
	void Delete(WORD wZone);
	void Draw();
};

//----------------------------------------------------
