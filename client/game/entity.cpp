//----------------------------------------------------------
//
// SA:MP Multiplayer Modification For GTA:SA
// Copyright 2004-2005 SA:MP team
//
// Version: $Id: entity.cpp,v 1.19 2006/05/07 17:32:29 kyeman Exp $
//
//----------------------------------------------------------


#include <windows.h>
#include <math.h>
#include <stdio.h>

#include "../main.h"
#include "util.h"
#include "entity.h"

extern CGame *pGame;
extern CNetGame	*pNetGame;
//-----------------------------------------------------------

void CEntity::GetMatrix(PMATRIX4X4 Matrix)
{
	if (!m_pEntity || !m_pEntity->mat) return;

	Matrix->right.X = m_pEntity->mat->right.X;
	Matrix->right.Y = m_pEntity->mat->right.Y;
	Matrix->right.Z = m_pEntity->mat->right.Z;

	Matrix->up.X = m_pEntity->mat->up.X;
	Matrix->up.Y = m_pEntity->mat->up.Y;
	Matrix->up.Z = m_pEntity->mat->up.Z;

	Matrix->at.X = m_pEntity->mat->at.X;
	Matrix->at.Y = m_pEntity->mat->at.Y;
	Matrix->at.Z = m_pEntity->mat->at.Z;

	Matrix->pos.X = m_pEntity->mat->pos.X;
	Matrix->pos.Y = m_pEntity->mat->pos.Y;
	Matrix->pos.Z = m_pEntity->mat->pos.Z;
}

//-----------------------------------------------------------

void CEntity::SetMatrix(MATRIX4X4 Matrix)
{
	if (!m_pEntity || !m_pEntity->mat) return;

	m_pEntity->mat->right.X = Matrix.right.X;
	m_pEntity->mat->right.Y = Matrix.right.Y;
	m_pEntity->mat->right.Z = Matrix.right.Z;

	m_pEntity->mat->up.X = Matrix.up.X;
	m_pEntity->mat->up.Y = Matrix.up.Y;
	m_pEntity->mat->up.Z = Matrix.up.Z;

	m_pEntity->mat->at.X = Matrix.at.X;
	m_pEntity->mat->at.Y = Matrix.at.Y;
	m_pEntity->mat->at.Z = Matrix.at.Z;

	m_pEntity->mat->pos.X = Matrix.pos.X;
	m_pEntity->mat->pos.Y = Matrix.pos.Y;
	m_pEntity->mat->pos.Z = Matrix.pos.Z;
}

//-----------------------------------------------------------

void CEntity::GetMoveSpeedVector(PVECTOR Vector)
{
	Vector->X = m_pEntity->vecMoveSpeed.X;
	Vector->Y = m_pEntity->vecMoveSpeed.Y;
	Vector->Z = m_pEntity->vecMoveSpeed.Z;
}

//-----------------------------------------------------------

void CEntity::SetMoveSpeedVector(VECTOR Vector)
{
	m_pEntity->vecMoveSpeed.X = Vector.X;
	m_pEntity->vecMoveSpeed.Y = Vector.Y;
	m_pEntity->vecMoveSpeed.Z = Vector.Z;
}

//-----------------------------------------------------------

void CEntity::GetTurnSpeedVector(PVECTOR Vector)
{
	Vector->X = m_pEntity->vecTurnSpeed.X;
	Vector->Y = m_pEntity->vecTurnSpeed.Y;
	Vector->Z = m_pEntity->vecTurnSpeed.Z;
}

//-----------------------------------------------------------

void CEntity::SetTurnSpeedVector(VECTOR Vector)
{
	m_pEntity->vecTurnSpeed.X = Vector.X;
	m_pEntity->vecTurnSpeed.Y = Vector.Y;
	m_pEntity->vecTurnSpeed.Z = Vector.Z;
}
//-----------------------------------------------------------

void CEntity::ApplyMoveSpeed()
{
	DWORD dwEnt = (DWORD)m_pEntity;
	if(!dwEnt) return;

	_asm mov ecx, dwEnt
	_asm mov edx, 0x542DD0
	_asm call edx
}

//-----------------------------------------------------------

BOOL CEntity::IsStationary()
{
    if( m_pEntity->vecMoveSpeed.X == 0.0f &&
		m_pEntity->vecMoveSpeed.Y == 0.0f &&
		m_pEntity->vecMoveSpeed.Z == 0.0f )
	{
		return TRUE;
	}
    return FALSE;
}

//-----------------------------------------------------------

void CEntity::SetModelIndex(UINT uiModel)
{
	if(!m_pEntity) return;

	if(!pGame->IsModelLoaded(uiModel)) {
		pGame->RequestModel(uiModel);
		pGame->LoadRequestedModels();
		while(!pGame->IsModelLoaded(uiModel)) Sleep(1);
	}

	DWORD dwThisEntity = (DWORD)m_pEntity;

	_asm {
		mov		esi, dwThisEntity
		mov		edi, uiModel
		mov     edx, [esi]
		mov     ecx, esi
		call    dword ptr [edx+32] ; destroy RW
		mov     eax, [esi]
		mov		edx, edi
		push    edi
		mov     ecx, esi
		mov     word ptr [esi+34], dx
		call    dword ptr [eax+20] ; SetModelIndex
	}

	pGame->RemoveModel(uiModel);
}

//-----------------------------------------------------------

UINT CEntity::GetModelIndex()
{
	return m_pEntity->nModelIndex;	
}

//-----------------------------------------------------------

void CEntity::TeleportTo(float x, float y, float z)
{
	DWORD dwThisEntity = (DWORD)m_pEntity;

	if(dwThisEntity) {
		if( GetModelIndex() != TRAIN_PASSENGER_LOCO &&
			GetModelIndex() != TRAIN_FREIGHT_LOCO &&
			GetModelIndex() != TRAIN_TRAM) {
			_asm mov ecx, dwThisEntity
			_asm mov edx, [ecx] ; vtbl
			_asm push 0
			_asm push z
			_asm push y
			_asm push x
			_asm call dword ptr [edx+56] ; method 14
		} else {
			ScriptCommand(&put_train_at,m_dwGTAId,x,y,z);
		}
	}
}

//-----------------------------------------------------------

float CEntity::GetDistanceFromLocalPlayerPed()
{
	MATRIX4X4	matFromPlayer;
	MATRIX4X4	matThis;
	float		fSX,fSY,fSZ;

	CPlayerPed *pLocalPlayerPed = pGame->FindPlayerPed();
	CLocalPlayer *pLocalPlayer=NULL;

	if(!pLocalPlayerPed) return 10000.0f;
	if(!m_pEntity) return 10000.0f;
	
	GetMatrix(&matThis);

	if(pNetGame) {
		pLocalPlayer = pNetGame->GetPlayerPool()->GetLocalPlayer();
		if(pLocalPlayer && (pLocalPlayer->IsSpectating() || pLocalPlayer->IsInRCMode())) {
			pGame->GetCamera()->GetMatrix(&matFromPlayer);
		} else {
			pLocalPlayerPed->GetMatrix(&matFromPlayer);
		}
	} else {
		pLocalPlayerPed->GetMatrix(&matFromPlayer);
	}
	
	fSX = (matThis.pos.X - matFromPlayer.pos.X) * (matThis.pos.X - matFromPlayer.pos.X);
	fSY = (matThis.pos.Y - matFromPlayer.pos.Y) * (matThis.pos.Y - matFromPlayer.pos.Y);
	fSZ = (matThis.pos.Z - matFromPlayer.pos.Z) * (matThis.pos.Z - matFromPlayer.pos.Z);
	
	return (float)sqrt(fSX + fSY + fSZ);
}

//-----------------------------------------------------------

float CEntity::GetDistanceFromPoint(float X, float Y, float Z)
{
	MATRIX4X4	matThis;
	float		fSX,fSY,fSZ;

	GetMatrix(&matThis);
	fSX = (matThis.pos.X - X) * (matThis.pos.X - X);
	fSY = (matThis.pos.Y - Y) * (matThis.pos.Y - Y);
	fSZ = (matThis.pos.Z - Z) * (matThis.pos.Z - Z);
	
	return (float)sqrt(fSX + fSY + fSZ);
}

//-----------------------------------------------------------

void CEntity::Add()
{
	// Check for CPlaceable messup
	if(!m_pEntity || m_pEntity->vtable == 0x863C40) 
	{
#ifdef _DEBUG
		OutputDebugString("CEntity::Add - m_pEntity == NULL or CPlaceable");
#endif
		return;
	}

	if(!m_pEntity->dwUnkModelRel) {
		// Make sure the move/turn speed is reset

		VECTOR vec = {0.0f,0.0f,0.0f};

		SetMoveSpeedVector(vec);
		SetTurnSpeedVector(vec);

		WorldAddEntity((PDWORD)m_pEntity);

		MATRIX4X4 mat;
		GetMatrix(&mat);
		TeleportTo(mat.pos.X,mat.pos.Y,mat.pos.Z);

#ifdef _DEBUG
		if (!IsAdded())
		{
			OutputDebugString("CEntity::Add failed...");
		}
#endif
	}
}

//-----------------------------------------------------------

BOOL CEntity::IsAdded()
{
	// Check for CPlaceable messup
	if(m_pEntity) {
		if (m_pEntity->vtable == 0x863C40) 
			return FALSE;

		if(m_pEntity->dwUnkModelRel)
			return TRUE;
	}
	return FALSE;
}

//-----------------------------------------------------------

void CEntity::Remove()
{
	// Check for CPlaceable messup
	if(!m_pEntity || m_pEntity->vtable == 0x863C40) 
	{
#ifdef _DEBUG
		OutputDebugString("CEntity::Remove - m_pEntity == NULL or CPlaceable");
#endif
		return;
	}

	if(m_pEntity->dwUnkModelRel) {
		WorldRemoveEntity((PDWORD)m_pEntity);

#ifdef _DEBUG
		if (IsAdded())
		{
			OutputDebugString("CEntity::Remove failed...");
		}
#endif
	}
}

//-----------------------------------------------------------

BOOL CEntity::EnforceWorldBoundries(float fPX, float fZX, float fPY, float fNY)
{
	MATRIX4X4 matWorld;
	VECTOR vecMoveSpeed;

	if(!m_pEntity) return FALSE;

	GetMatrix(&matWorld);
	GetMoveSpeedVector(&vecMoveSpeed);

	if(matWorld.pos.X > fPX)
	{
		if(vecMoveSpeed.X != 0.0f) {
			vecMoveSpeed.X = -0.2f;
			vecMoveSpeed.Z = 0.1f;
		}
		SetMoveSpeedVector(vecMoveSpeed);
		matWorld.pos.Z += 0.04f;
		SetMatrix(matWorld);
		return TRUE;
	}
	else if(matWorld.pos.X < fZX)
	{
		if(vecMoveSpeed.X != 0.0f) {
			vecMoveSpeed.X = 0.2f;
			vecMoveSpeed.Z = 0.1f;
		}
		SetMoveSpeedVector(vecMoveSpeed);
		matWorld.pos.Z += 0.04f;
		SetMatrix(matWorld);
		return TRUE;
	}
	else if(matWorld.pos.Y > fPY)
	{
		if(vecMoveSpeed.Y != 0.0f) {
			vecMoveSpeed.Y = -0.2f;
			vecMoveSpeed.Z = 0.1f;
		}

		SetMoveSpeedVector(vecMoveSpeed);
		matWorld.pos.Z += 0.04f;
		SetMatrix(matWorld);
		return TRUE;
	}
	else if(matWorld.pos.Y < fNY)
	{
		if(vecMoveSpeed.Y != 0.0f) {
			vecMoveSpeed.Y = 0.2f;
			vecMoveSpeed.Z = 0.1f;
		}

		SetMoveSpeedVector(vecMoveSpeed);
		matWorld.pos.Z += 0.04f;
		SetMatrix(matWorld);
		return TRUE;
	}

	return FALSE;
}

//-----------------------------------------------------------

BOOL CEntity::HasExceededWorldBoundries(float fPX, float fZX, float fPY, float fNY)
{
	MATRIX4X4 matWorld;

	if(!m_pEntity) return FALSE;

	GetMatrix(&matWorld);

	if(matWorld.pos.X > fPX) {
		return TRUE;
	}
	else if(matWorld.pos.X < fZX) {
		return TRUE;
	}
	else if(matWorld.pos.Y > fPY) {
		return TRUE;
	}
	else if(matWorld.pos.Y < fNY) {
		return TRUE;
	}
	return FALSE;
}