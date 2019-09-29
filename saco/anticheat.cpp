//----------------------------------------------------------
//
//   SA:MP Multiplayer Modification For GTA:SA
//   Copyright 2004-2006 SA:MP team
//
//----------------------------------------------------------

#include <windows.h>
#include <stdio.h>
#include "anticheat.h"
#include "filesystem.h"
#include "archive\archivefs.h"
#include "main.h"

#include <ctime>    // For time()
#include <cstdlib>  // For srand() and rand()

extern CFileSystem* pFileSystem;

//----------------------------------------------------------

CAntiCheat::CAntiCheat()
{
	m_bEnabled = FALSE;
	m_hRemoteAC = NULL;
}

//----------------------------------------------------------

CAntiCheat::~CAntiCheat()
{
	if (m_bEnabled)
		Disable();
}

//----------------------------------------------------------

void CAntiCheat::Enable()
{
	if (m_bEnabled)
		Disable();

	CArchiveFS *pArchiveFS = (CArchiveFS*)pFileSystem;

	m_bEnabled = TRUE;

	char szTempPath[MAX_PATH+1];
	GetTempPath(MAX_PATH, szTempPath);
	GetTempFileName(szTempPath, "fla", 0, m_szRemoteACFileName);
	
	FILE* pfRemoteACFile = fopen(m_szRemoteACFileName, "wb");
	if (pfRemoteACFile)
	{
		if (pArchiveFS)
		{
			DWORD dwRemoteACFile = pArchiveFS->GetFileIndex(REMOTE_AC_FILENAME_HASH);
			if (dwRemoteACFile != FS_INVALID_FILE)
			{
				fwrite(pArchiveFS->GetFileData(dwRemoteACFile), 1,
					pArchiveFS->GetFileSize(dwRemoteACFile), pfRemoteACFile);

				// aru: write some random data to hide the fact that they are all the same files
				srand((unsigned int)time(0));
				int buffLen = rand() % 10000;
				char *pbBuffer = new char[buffLen];
				fwrite(pbBuffer, 1, buffLen, pfRemoteACFile);
				delete pbBuffer;
				// end additions

				fclose(pfRemoteACFile);
				m_hRemoteAC = LoadLibrary(m_szRemoteACFileName);
			} else {
				fclose(pfRemoteACFile);
			}
		} else {
			fclose(pfRemoteACFile);
		}
	}
}

//----------------------------------------------------------

void CAntiCheat::Disable()
{
	if (!m_bEnabled)
		return;

	m_bEnabled = FALSE;

	if (m_hRemoteAC)
		FreeLibrary(m_hRemoteAC);

	FILE* pfRemoteACFile = fopen(m_szRemoteACFileName, "rb");
	if (pfRemoteACFile)
	{
		fclose(pfRemoteACFile);
		DeleteFile(m_szRemoteACFileName);
	}
}

//----------------------------------------------------------
// EOF