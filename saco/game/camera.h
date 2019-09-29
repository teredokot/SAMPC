//----------------------------------------------------------
//
// SA:MP Multiplayer Modification For GTA:SA
// Copyright 2004-2005 SA:MP team
//
// Version: $Id: camera.h,v 1.4 2006/03/20 17:44:20 kyeman Exp $
//
//----------------------------------------------------------

#pragma once

#include "game.h" // Main runtime interface base structs.

//-----------------------------------------------------------

class CCamera
{
public:
	MATRIX4X4 *m_matPos;
public:
	void SetBehindPlayer();
	void SetPosition(float fX, float fY, float fZ, float fRotationX, float fRotationY, float fRotationZ);	// tested
	void LookAtPoint(float fX, float fY, float fZ, int iType);
	void Restore();
	void Fade(int iInOut);
	void GetMatrix(PMATRIX4X4 Matrix);
	
	CCamera(){ m_matPos = (MATRIX4X4 *)ADDR_CAMERA; };
	~CCamera(){};
};

//-----------------------------------------------------------
