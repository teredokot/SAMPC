//----------------------------------------------------
//
//	Utility and runtime procedures header
//	(c) 2005 Kye Bitossi
//
//  Version: $Id: runutil.h,v 1.2 2006/03/20 17:59:34 kyeman Exp $
//
//----------------------------------------------------


void Util_UrlUnencode(char *enc);
char Util_toupper(char c);
char *Util_stristr(const char *String, const char *Pattern);
void Util_strupr(char *string);
int Util_wildcmp(char *wild, char *string);
int Util_strnicmp(const char *s1, const char *s2, size_t n);
char *Util_strrev(char *str);
char * Util_itoa(int v, char *s, int r);
void Util_Base64Encode( char *cpInput, char *cpOutput );
void ReplaceBadChars(char * szString);

//----------------------------------------------------