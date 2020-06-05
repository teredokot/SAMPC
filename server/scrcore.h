
#ifndef SAMPSRV_SCRCORE_H
#define SAMPSRV_SCRCORE_H

int aux_LoadProgram(AMX* amx, char* filename);
int aux_LoadProgramFromMemory(AMX* amx, char* filedata);
int aux_FreeProgram(AMX* amx);
char* aux_StrError(int errnum);
void AMXPrintError(CGameMode* pGameMode, AMX* amx, int error);
cell* get_amxaddr(AMX* amx, cell amx_addr);
int set_amxstring(AMX* amx, cell amx_addr, const char* source, int max);
char* format_amxstring(AMX* amx, cell* params, int parm, int& len);
void PrintMissingNatives(AMX* amx, const char* szScriptName);
char* GetScriptName(AMX* amx);

extern "C" int amx_CoreInit(AMX * amx);
extern "C" int amx_CoreCleanup(AMX * amx);
extern "C" int amx_FloatInit(AMX * amx);
extern "C" int amx_FloatCleanup(AMX * amx);
extern "C" int amx_StringInit(AMX * amx);
extern "C" int amx_StringCleanup(AMX * amx);
extern "C" int amx_FileInit(AMX * amx);
extern "C" int amx_FileCleanup(AMX * amx);
extern "C" int amx_TimeInit(AMX * amx);
extern "C" int amx_TimeCleanup(AMX * amx);

int amx_CustomInit(AMX* amx);
int amx_sampDbInit(AMX* amx);
int amx_sampDbCleanup(AMX* amx);

#endif // SAMPSRV_SCRCORE_H
