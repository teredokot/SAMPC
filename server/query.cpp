/*

	SA:MP Multiplayer Modification
	Copyright 2004-2005 SA:MP Team

    Version: $Id: query.cpp,v 1.16 2006/05/08 13:28:46 kyeman Exp $

*/

#include "main.h"

//----------------------------------------------------

BOOL	bRconSocketReply	= FALSE;

SOCKET	cur_sock			= INVALID_SOCKET;
char*	cur_data			= NULL;
int		cur_datalen			= 0;
sockaddr_in to;

extern CNetGame* pNetGame;
extern RakServerInterface* pRak;

//----------------------------------------------------

void RconSocketReply(char* szMessage);

//----------------------------------------------------
// int CheckQueryFlood()
// returns 1 if this query could flood
// returns 0 otherwise

DWORD dwLastQueryTick=0;
unsigned int lastBinAddr=0;

int CheckQueryFlood(unsigned int binaryAddress)
{
	if(!dwLastQueryTick) {
		dwLastQueryTick = (DWORD)GetTickCount();
		lastBinAddr = binaryAddress;
		return 0;
	}
	if(lastBinAddr == binaryAddress) {
		return 0;
	}
	if((GetTickCount() - dwLastQueryTick) < 25) {
		return 1;
	}
	dwLastQueryTick = GetTickCount();
	lastBinAddr = binaryAddress;
	return 0;
}

//----------------------------------------------------

int ProcessQueryPacket(unsigned int binaryAddress, unsigned short port, char* data, int length, SOCKET s)
{
	char* szPassword=NULL;
	WORD wStrLen=0;
	in_addr in;
	in.s_addr = binaryAddress;

	if ((length > 4) && (*(unsigned long*)(data) == 0x504D4153)) // 0x504D4153 = "SAMP" as hex.
	{
		if(!pNetGame || (pNetGame->GetGameState() != GAMESTATE_RUNNING)) return 0;

		if (length >= 11)
		{
			to.sin_family = AF_INET;
			to.sin_port = htons(port);
			to.sin_addr.s_addr = binaryAddress;

			switch (data[10])
			{
				case 'p':	// ping
				{
					if (length == 15)
					{
						sendto(s, data, 15, 0, (sockaddr*)&to, sizeof(to));
					}
					break;
				}
				case 'i':	// info
				{
					// We do not process these queries 'query' is 0
					if(!pConsole->GetBoolVariable("query")) return 1;
					if(CheckQueryFlood(binaryAddress)) return 1;

					char* szHostname = pConsole->GetStringVariable("hostname");
					DWORD dwHostnameLen = strlen(szHostname);
					if (dwHostnameLen > 50) dwHostnameLen = 50;

					char* szGameMode = pConsole->GetStringVariable("gamemodetext");
					DWORD dwGameModeLen = strlen(szGameMode);
					if (dwGameModeLen > 30) dwGameModeLen = 30;

					char* szMapName = pConsole->GetStringVariable("mapname");
					DWORD dwMapNameLen = strlen(szMapName);
					if (dwMapNameLen > 30) dwMapNameLen = 30;

					WORD wPlayerCount = 0;
					CPlayerPool* pPlayerPool = pNetGame->GetPlayerPool();
					if (pPlayerPool)
						for (int i=0; i<MAX_PLAYERS; i++)
							if (pPlayerPool->GetSlotState(i))
								wPlayerCount++;

					WORD wMaxPlayers = pConsole->GetIntVariable("maxplayers");

					BYTE byteIsPassworded = pConsole->GetStringVariable("password")[0] != 0;

					DWORD datalen = 28;	// Previous data = 11b
										// IsPassworded = 1b
										// Player count = 2b
										// Max player count = 2b
										// String-length bytes = 12b (3 * sizeof(DWORD))
					datalen += dwHostnameLen;
					datalen += dwGameModeLen;
					datalen += dwMapNameLen;


					char* newdata = (char*)malloc(datalen);
					char* keep_ptr = newdata;
					// Previous Data
					memcpy(newdata, data, 11);
					newdata += 11;

					// IsPassworded
					memcpy(newdata, &byteIsPassworded, sizeof(BYTE));
					newdata += sizeof(BYTE);

					// Player Count
					memcpy(newdata, &wPlayerCount, sizeof(WORD));
					newdata += sizeof(WORD);

					// Max Players
					memcpy(newdata, &wMaxPlayers, sizeof(WORD));
					newdata += sizeof(WORD);

					// Hostname
					memcpy(newdata, &dwHostnameLen, sizeof(DWORD));
					newdata += sizeof(DWORD);
					memcpy(newdata, szHostname, dwHostnameLen);
					newdata += dwHostnameLen;

					// Game Mode
					memcpy(newdata, &dwGameModeLen, sizeof(DWORD));
					newdata += sizeof(DWORD);
					memcpy(newdata, szGameMode, dwGameModeLen);
					newdata += dwGameModeLen;

					// Map Name
					memcpy(newdata, &dwMapNameLen, sizeof(DWORD));
					newdata += sizeof(DWORD);
					memcpy(newdata, szMapName, dwMapNameLen);
					newdata += dwMapNameLen;

					sendto(s, keep_ptr, datalen, 0, (sockaddr*)&to, sizeof(to));
					free(keep_ptr);
					break;
				}
				case 'c':	// players
				{
					// We do not process these queries 'query' is 0
					if(!pConsole->GetBoolVariable("query")) return 1;
					if(CheckQueryFlood(binaryAddress)) return 1;

					WORD wPlayerCount = 0;
					CPlayerPool* pPlayerPool = pNetGame->GetPlayerPool();
					if (pPlayerPool) {
						for (int i=0; i<MAX_PLAYERS; i++)
							if (pPlayerPool->GetSlotState(i))
								wPlayerCount++;
					}

					char* newdata = (char*)malloc(13 + (wPlayerCount * (MAX_PLAYER_NAME + 5))); // 5 = 1b String len, and 4b Score
					char* keep_ptr = newdata;
					// Previous Data
					memcpy(newdata, data, 11);
					newdata += 11;

					// Player Count
					memcpy(newdata, &wPlayerCount, sizeof(WORD));
					newdata += sizeof(WORD);

					if (pPlayerPool) {
						char* szName;
						BYTE byteNameLen;
						DWORD dwScore;
						
						for (int r=0; r<MAX_PLAYERS; r++)
						{
							if (pPlayerPool->GetSlotState(r))
							{
								szName = pPlayerPool->GetPlayerName(r);
								byteNameLen = (BYTE)strlen(szName);
								memcpy(newdata, &byteNameLen, sizeof(BYTE));
								newdata += sizeof(BYTE);
								memcpy(newdata, szName, byteNameLen);
								newdata += byteNameLen;
								dwScore = pPlayerPool->GetPlayerScore(r);
								memcpy(newdata, &dwScore, sizeof(DWORD));
								newdata += sizeof(DWORD);							

							}
						}
					}

					sendto(s, keep_ptr, (int)(newdata - keep_ptr), 0, (sockaddr*)&to, sizeof(to));
					free(keep_ptr);
					break;
				}
				case 'd':	// detailed player list id.namelength.name.score.ping
				{
						// We do not process these queries 'query' is 0
						if(!pConsole->GetBoolVariable("query")) return 1;
						if(CheckQueryFlood(binaryAddress)) return 1;

						WORD wPlayerCount = 0;
						CPlayerPool* pPlayerPool = pNetGame->GetPlayerPool();
						if (pPlayerPool) {
							for (int i=0; i<MAX_PLAYERS; i++)
								if (pPlayerPool->GetSlotState(i))
									wPlayerCount++;
						}
						char* newdata = (char*)malloc(13 + (wPlayerCount * (MAX_PLAYER_NAME + 10))); // 9 = 1b String len, 4b Score, 4b Ping, 1b Playerid
						char* keep_ptr = newdata;
						// Previous Data
						memcpy(newdata, data, 11);
						newdata += 11;

						// Player Count
						memcpy(newdata, &wPlayerCount, sizeof(WORD));
						newdata += sizeof(WORD);

						if (pPlayerPool) {
							char* szName;
							BYTE byteNameLen;
							DWORD dwScore, dwPing;

							for (int r=0; r<MAX_PLAYERS; r++)
							{
								if (pPlayerPool->GetSlotState(r))
								{
									memcpy(newdata, &r, sizeof(BYTE));
									newdata += sizeof(BYTE);
									szName = pPlayerPool->GetPlayerName(r);
									byteNameLen = (BYTE)strlen(szName);
									memcpy(newdata, &byteNameLen, sizeof(BYTE));
									newdata += sizeof(BYTE);
									memcpy(newdata, szName, byteNameLen);
									newdata += byteNameLen;
									dwScore = pPlayerPool->GetPlayerScore(r);
									memcpy(newdata, &dwScore, sizeof(DWORD));
									newdata += sizeof(DWORD);
									dwPing = pRak->GetLastPing( pRak->GetPlayerIDFromIndex( r ) );
									memcpy(newdata, &dwPing, sizeof(DWORD));
									newdata += sizeof(DWORD);
								}
							}
						}
						sendto(s, keep_ptr, (int)(newdata - keep_ptr), 0, (sockaddr*)&to, sizeof(to));
						free(keep_ptr);
						break;
				}

				case 'r':	// rules
				{
					// We do not process these queries 'query' is 0
					if(!pConsole->GetBoolVariable("query")) return 1;
					if(CheckQueryFlood(binaryAddress)) return 1;

					pConsole->SendRules(s, data, (sockaddr_in*)&to, sizeof(to));
					break;
				}
				case 'x':	// rcon
				{
					if(pRak && pRak->IsBanned(inet_ntoa(in))) return 1;

					// We do not process these queries 'query' is 0
					if(!pConsole->GetBoolVariable("query")) return 1;
					if(CheckQueryFlood(binaryAddress)) return 1;

					cur_sock = s;
					cur_data = data;
					cur_datalen = 11; // How much of the message we send back.
					int tmp_datalen = cur_datalen;

					data += cur_datalen;

					// Check there's enough data for another WORD
					tmp_datalen += sizeof(WORD);
					if (length < tmp_datalen)
						goto cleanup;	// Malformed packet! Haxy bastard!
					// Read the string length for the password
					wStrLen = *(WORD*)data;
					data += sizeof(WORD);
					// Check there's enough data for the password string
					tmp_datalen += wStrLen;
					if (length < tmp_datalen)
						goto cleanup;	// Malformed packet! Haxy bastard!
					// Read the password string
					szPassword = (char*)malloc(wStrLen + 1);
					memcpy(szPassword, data, wStrLen);
					szPassword[wStrLen] = 0;
					data += wStrLen;

					if (strcmp(szPassword, pConsole->GetStringVariable("rcon_password")) == 0)
					{
						// Check there's enough data for another WORD
						tmp_datalen += sizeof(WORD);
						if (length < tmp_datalen)
						{
							free(szPassword);
							goto cleanup;	// Malformed packet! Haxy bastard!
						}
						// Read the string length for the command
						wStrLen = *(WORD*)data;
						data += sizeof(WORD);
						tmp_datalen += wStrLen;
						if (length < tmp_datalen)
						{
							free(szPassword);
							goto cleanup;	// Malformed packet! Haxy bastard!
						}

						// Read the command string
						char* szCommand = (char*)malloc(wStrLen + 1);
						memcpy(szCommand, data, wStrLen);
						szCommand[wStrLen] = 0;

						if (pConsole)
						{
							bRconSocketReply = TRUE;
							// Execute the command
							pConsole->Execute(szCommand);
							bRconSocketReply = FALSE;
						}

						free(szCommand);
					} else {
						in_addr in;
						in.s_addr = binaryAddress;
						logprintf("BAD RCON ATTEMPT BY: %s", inet_ntoa(in));

						bRconSocketReply = TRUE;
						RconSocketReply("Invalid RCON password.");
						bRconSocketReply = FALSE;
					}
					free(szPassword);

				cleanup:
					cur_datalen = 0;
					cur_data = NULL;
					cur_sock = INVALID_SOCKET;
					break;
				}
			}
		}
		return 1;
	} else {
		return 0;
	}
}

//----------------------------------------------------

void RconSocketReply(char* szMessage)
{
	// IMPORTANT!
	// Don't use logprintf from here... You'll cause an infinite loop.
	if (bRconSocketReply)
	{
		char* newdata = (char*)malloc(cur_datalen + strlen(szMessage) + sizeof(WORD));
		char* keep_ptr = newdata;
		memcpy(newdata, cur_data, cur_datalen);
		newdata += cur_datalen;
		*(WORD*)newdata = (WORD)strlen(szMessage);
		newdata += sizeof(WORD);
		memcpy(newdata, szMessage, strlen(szMessage));
		newdata += strlen(szMessage);
		sendto(cur_sock, keep_ptr, (int)(newdata - keep_ptr), 0, (sockaddr*)&to, sizeof(to));
		free(keep_ptr);
	}
}

//----------------------------------------------------
// EOF
