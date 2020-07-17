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
	CAMERA_TYPE* m_pCamera;
	MATRIX4X4 *m_matPos;
public:
	void InterpolateCameraPos(VECTOR* from, VECTOR* to, FLOAT time, BYTE mode);
	void InterpolateCameraLookAt(VECTOR* from, VECTOR* to, FLOAT time, BYTE mode);
	void SetBehindPlayer();
	void SetPosition(float fX, float fY, float fZ, float fRotationX, float fRotationY, float fRotationZ);	// tested
	void LookAtPoint(float fX, float fY, float fZ, int iType);
	void Restore();
	void Fade(int iInOut);
	void GetMatrix(PMATRIX4X4 Matrix);
	void ToggleWidescreen(bool bOn);

	CCamera()
	{
		m_matPos = (MATRIX4X4 *)ADDR_CAMERA;
		m_pCamera = (CAMERA_TYPE*)0xB6F028;
	};
	~CCamera(){};
};

//-----------------------------------------------------------
