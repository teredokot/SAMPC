//----------------------------------------------------
//
//	Utility and runtime procedures header
//	(c) 2005 Kye Bitossi
//
//  Version: $Id: runutil.h,v 1.2 2006/03/20 17:59:34 kyeman Exp $
//
//----------------------------------------------------

#define VALIDATE_FIGHTING(style, moves) \
	(((style) >= 4 && (style) <= 7) || (style) == 15 || (style) == 16) && \
	((moves) >= 0 && (moves) <= 5)

namespace Utils
{
	int GetTypeByComponentId(short sModelId);
};

bool IsVehicleModelIdValid(int iId);
const char* GetVehicleName(int iModelId);
bool IsPedSkinIdValid(int iId);
const float* GetVehicleModelInfoData(int modelid, int infotype);
const char* GetWeaponName(int iWeaponID);

void Util_UrlUnencode(char *enc);
char Util_toupper(char c);
char *Util_stristr(const char *String, const char *Pattern);
void Util_strupr(char *string);
int Util_wildcmp(char *wild, char *string);
int Util_strnicmp(const char *s1, const char *s2, size_t n);
char *Util_strrev(char *str);
char * Util_itoa(int v, char *s, int r);
void Util_Base64Encode( char *cpInput, char *cpOutput );
bool ContainsInvalidNickChars(char* szString);
void ReplaceBadChars(char * szString);
int CanFileBeOpenedForReading(char* filename);
//----------------------------------------------------