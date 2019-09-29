//----------------------------------------------------------
//
// SA:MP Multiplayer Modification For GTA:SA
// Copyright 2004-2005 SA:MP team
//
// Version: $Id: entity.h,v 1.8 2006/04/16 11:37:59 kyeman Exp $
//
//----------------------------------------------------------

#pragma once

#include "game.h"

//----------------------------------------------------------

class CEntity
{
public:
	CEntity() {};
	virtual ~CEntity() {};
	virtual void  Add();
	virtual void  Remove();

	BOOL  IsAdded();

	void  GetMatrix(PMATRIX4X4 Matrix);
	void  SetMatrix(MATRIX4X4 Matrix);
	void  GetMoveSpeedVector(PVECTOR Vector);
	void  SetMoveSpeedVector(VECTOR Vector);
	void  GetTurnSpeedVector(PVECTOR Vector);
	void  SetTurnSpeedVector(VECTOR Vector);
	UINT  GetModelIndex();
	void  SetModelIndex(UINT uiModel);
	void  TeleportTo(float x, float y, float z);
	float GetDistanceFromLocalPlayerPed();
	float GetDistanceFromPoint(float X, float Y, float Z);
	void  ApplyMoveSpeed();
	BOOL  IsStationary();
	
	BOOL  EnforceWorldBoundries(float fPX, float fZX, float fPY, float fNY);
	BOOL  HasExceededWorldBoundries(float fPX, float fZX, float fPY, float fNY);

	ENTITY_TYPE *m_pEntity;
	DWORD		m_dwGTAId;
};

//----------------------------------------------------------