#pragma once

/*

SA:MP Multiplayer Modification
Copyright 2004-2005 SA:MP Team

Version: $Id: gangzonepool.h,v 1.0 2007/05/25 19:26:45 Y_Less Exp $

*/

//----------------------------------------------------

class CGangZonePool
{
private:
	float			m_fGangZone[MAX_GANG_ZONES][4];
	bool			m_bSlotState[MAX_GANG_ZONES];
public:
	CGangZonePool();
	~CGangZonePool() {};
	WORD New(float fMinX, float fMinY, float fMaxX, float fMaxY);
	void Delete(WORD wZone);
	void ShowForPlayer(BYTE bytePlayer, WORD wZone, DWORD dwColor);
	void ShowForAll(WORD wZone, DWORD dwColor);
	void HideForPlayer(BYTE bytePlayer, WORD wZone);
	void HideForAll(WORD wZone);
	void FlashForPlayer(BYTE bytePlayer, WORD wZone, DWORD dwColor);
	void FlashForAll(WORD wZone, DWORD dwColor);
	void StopFlashForPlayer(BYTE bytePlayer, WORD wZone);
	void StopFlashForAll(WORD wZone);
	bool GetSlotState(int iZone)
	{
		return (iZone >= 0 && iZone < MAX_GANG_ZONES) ? m_bSlotState[iZone] : false;
	}
};

//----------------------------------------------------
