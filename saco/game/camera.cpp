//----------------------------------------------------------
//
// SA:MP Multiplayer Modification For GTA:SA
// Copyright 2004-2005 SA:MP team
//
// Version: $Id: camera.cpp,v 1.4 2006/03/20 17:44:20 kyeman Exp $
//
//----------------------------------------------------------

#include "game.h"
#include "util.h"

//-----------------------------------------------------------

void CCamera::SetBehindPlayer()
{
	ScriptCommand(&set_camera_behind_player);
	ScriptCommand(&restore_camera_jumpcut);
	//ScriptCommand(&restore_camera);
}

//-----------------------------------------------------------

void CCamera::SetPosition(float fX, float fY, float fZ, float fRotationX, float fRotationY, float fRotationZ)
{
	ScriptCommand(&set_camera_position,fX,fY,fZ,fRotationX,fRotationY,fRotationZ);
}

//-----------------------------------------------------------

void CCamera::LookAtPoint(float fX, float fY, float fZ, int iType)
{
	ScriptCommand(&point_camera,fX,fY,fZ,iType);
}

//-----------------------------------------------------------

void CCamera::Restore()
{
	ScriptCommand(&restore_camera_jumpcut);
}

//-----------------------------------------------------------

void CCamera::Fade(int iInOut)
{
	ScriptCommand(&fade,500,iInOut);
}

//-----------------------------------------------------------

void CCamera::GetMatrix(PMATRIX4X4 Matrix)
{
	Matrix->right.X = m_matPos->right.X;
	Matrix->right.Y = m_matPos->right.Y;
	Matrix->right.Z = m_matPos->right.Z;

	Matrix->up.X = m_matPos->up.X;
	Matrix->up.Y = m_matPos->up.Y;
	Matrix->up.Z = m_matPos->up.Z;

	Matrix->at.X = m_matPos->at.X;
	Matrix->at.Y = m_matPos->at.Y;
	Matrix->at.Z = m_matPos->at.Z;

	Matrix->pos.X = m_matPos->pos.X;
	Matrix->pos.Y = m_matPos->pos.Y;
	Matrix->pos.Z = m_matPos->pos.Z;
}