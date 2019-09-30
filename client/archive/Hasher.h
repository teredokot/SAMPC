//----------------------------------------------------------
//
//   SA:MP Multiplayer Modification For GTA:SA
//   Copyright 2004-2006 SA:MP team
//
//----------------------------------------------------------

#pragma once

#include <windows.h>
#include "CryptoContext.h"

class CHasher
{
private:
	static DWORD ms_dwHashAlgorithm;
	
	HCRYPTHASH m_hCryptHash;
	CCryptoContext* m_pContext;

public:
	CHasher(CCryptoContext* pContext);
	~CHasher(void);

	void AddData(DWORD dwDataLength, BYTE* pbData);
	HCRYPTHASH GetContainer();

};
