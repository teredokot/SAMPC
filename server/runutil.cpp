//----------------------------------------------------
//
//	Utility runtime procedures
//	(c) 2005 Kye Bitossi
//
//  Version: $Id: runutil.cpp,v 1.4 2006/03/20 17:59:34 kyeman Exp $
// 
//----------------------------------------------------

#include "runutil.h"

#include <stdio.h>
#include <string.h>

//----------------------------------------------------

int Utils::GetTypeByComponentId(short sModelId)
{
	switch (sModelId)
	{
	case 1000: case 1001: case 1002: case 1003: case 1014: case 1015:
	case 1016: case 1023: case 1049: case 1050: case 1058: case 1060:
	case 1138: case 1139: case 1146: case 1147: case 1158: case 1162:
	case 1163: case 1164:
		return 0; // CARMODTYPE_SPOILER
	case 1004: case 1005: case 1011: case 1012:
		return 1; // CARMODTYPE_HOOD
	case 1006: case 1032: case 1033: case 1035: case 1038: case 1053:
	case 1054: case 1055: case 1061: case 1067: case 1068: case 1088:
	case 1091: case 1103: case 1128: case 1130: case 1131:
		return 2; // CARMODTYPE_ROOF
	case 1007: case 1017: case 1026: case 1027: case 1030: case 1031:
	case 1036: case 1039: case 1040: case 1041: case 1042: case 1047:
	case 1048: case 1051: case 1052: case 1056: case 1057: case 1062:
	case 1063: case 1069: case 1070: case 1071: case 1072: case 1090:
	case 1093: case 1094: case 1095: case 1099: case 1101: case 1102:
	case 1106: case 1107: case 1108: case 1118: case 1119: case 1120:
	case 1121: case 1122: case 1124: case 1133: case 1134: case 1137:
		return 3; // CARMODTYPE_SIDESKIRT
	case 1013: case 1024:
		return 4; // CARMODTYPE_LAMPS
	case 1008: case 1009: case 1010:
		return 5; // CARMODTYPE_NITRO
	case 1018: case 1019: case 1020: case 1021: case 1022: case 1028:
	case 1029: case 1034: case 1037: case 1043: case 1044: case 1045:
	case 1046: case 1059: case 1064: case 1065: case 1066: case 1089:
	case 1092: case 1104: case 1105: case 1113: case 1114: case 1126:
	case 1127: case 1129: case 1132: case 1135: case 1136:
		return 6; // CARMODTYPE_EXHAUST
	case 1025: case 1073: case 1074: case 1075: case 1076: case 1077:
	case 1078: case 1079: case 1080: case 1081: case 1082: case 1083:
	case 1084: case 1085: case 1096: case 1097: case 1098:
		return 7; // CARMODTYPE_WHEELS
	case 1086:
		return 8; // CARMODTYPE_STEREO
	case 1087:
		return 9; // CARMODTYPE_HYDRAULICS
	case 1117: case 1152: case 1153: case 1155: case 1157: case 1160:
	case 1165: case 1166: case 1169: case 1170: case 1171: case 1172:
	case 1173: case 1174: case 1175: case 1179: case 1181: case 1182:
	case 1185: case 1188: case 1189: case 1190: case 1191:
		return 10; // CARMODTYPE_FRONT_BUMPER
	case 1140: case 1141: case 1148: case 1149: case 1150: case 1151:
	case 1154: case 1156: case 1159: case 1161: case 1167: case 1168:
	case 1176: case 1177: case 1178: case 1180: case 1183: case 1184:
	case 1186: case 1187: case 1192: case 1193:
		return 11; // CARMODTYPE_REAR_BUMPER
	case 1143: case 1145:
		return 12; // CARMODTYPE_VENT_RIGHT
	case 1142: case 1144:
		return 13; // CARMODTYPE_VENT_LEFT
	case 1100: case 1123: case 1125:
		return 14; // CARMODTYPE_BULLBARS
	case 1109: case 1110:
		return 15; // CARMODTYPE_REAR_BULLBARS
	case 1115: case 1116:
		return 16; // CARMODTYPE_FRONT_BULLBARS;
	}
	return -1;
}

bool IsVehicleModelIdValid(int iId)
{
	return (iId >= 400 && iId <= 611);
}

void Util_UrlUnencode(char *enc)
{
	char *write_pos = enc;

	while(*enc)
	{
		if(*enc=='%')
		{
			*write_pos = (*++enc>'/'&&*enc<':')?((*enc-('0'))<<4):((*enc-('7'))<<4);
			*write_pos |= (*++enc>'/'&&*enc<':')?(*enc-'0'):(*enc-'7');
		}
		else if (*enc=='+')
			*write_pos= ' ';
		else
			*write_pos= *enc;

		write_pos++; enc++;
	}
	*write_pos='\0';
}

//----------------------------------------------------

char Util_toupper(char c) {return ((c>(char)0x60) && (c<(char)0x7b))? c-0x20:c;}

//----------------------------------------------------

char *Util_stristr(const char *String, const char *Pattern)
{
      char *pptr, *sptr, *start;

      for (start = (char *)String; *start != 0; start++)
      {
            /* find start of pattern in string */
            for ( ; ((*start!=0) && (Util_toupper(*start) != Util_toupper(*Pattern))); start++)
                  ;
            if (0 == *start)
                  return NULL;

            pptr = (char *)Pattern;
            sptr = (char *)start;

            while (Util_toupper(*sptr) == Util_toupper(*pptr))
            {
                  sptr++;
                  pptr++;

                  /* if end of pattern then pattern was found */

                  if (0 == *pptr)
                        return (start);
            }
      }
      return NULL;
}

//----------------------------------------------------

void Util_strupr(char *string)
{
	char *p = string;

	while(*p) {
		*p = Util_toupper(*p);
		p++;
	}
}

//----------------------------------------------------

int Util_wildcmp(char *wild, char *string)
{
	char *cp = 0, *mp = 0;
	
	while((*string) && (*wild != '*'))
	{
		if((*wild != *string) && (*wild != '?'))
		{
			return 0;
		}
		wild++;
		string++;
	}
		
	while (*string)
	{
		if (*wild == '*')
		{
			if (!*++wild)
			{
				return 1;
			}
			mp = wild;
			cp = string+1;
		}
		else if ((*wild == *string) || (*wild == '?'))
		{
			wild++;
			string++;
		}
		else
		{
			wild = mp;
			string = cp++;
		}
	}
		
	while (*wild == '*')
	{
		wild++;
	}

	return !*wild;
}

//----------------------------------------------------

int Util_strnicmp(const char *s1, const char *s2, size_t n)
{

  if (n == 0) return 0;

  do
  {
    if (Util_toupper((unsigned char)*s1) != Util_toupper((unsigned char)*s2++))
      return (int)Util_toupper((unsigned char)*s1) - (int)Util_toupper((unsigned char)*--s2);
    if (*s1++ == 0)
      break;

  } while (--n != 0);

  return 0;
}

//----------------------------------------------------

char *Util_strrev(char *str)
{
      char *p1, *p2;

      if (! str || ! *str)
            return str;
      for (p1 = str, p2 = str + strlen(str) - 1; p2 > p1; ++p1, --p2)
      {
            *p1 ^= *p2;
            *p2 ^= *p1;
            *p1 ^= *p2;
      }
      return str;
}

//----------------------------------------------------

char * Util_itoa(int v, char *s, int r)
{
	int i,neg = 0;
	char *p = s;
	char *q = s;

	if (r < 0 || r > 35) {
		*s = 0;
		return (s);
		}
	if (r == 0) r = 10;
	if (v == 0) {
		*p++ = '0';
		*p = 0;
		return (s);
		}
	if (v < 0) {
		neg = 1;
		v = -v;
		}
	while (v > 0) {
		i = v % r;
		if (i > 9) i += 7;
		*p++ = '0' + i;
		v /= r;
		}
	if (neg) *p++ = '-';
	*p-- = 0;
	q = s;
	while (p > q) {
		i = *q;
		*q++ = *p;
		*p-- = i;
		}
	return (s);
}

//----------------------------------------------------

char * Base64Encoding = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/=";

void Util_Base64Encode( char *cpInput, char *cpOutput )
{
int nIdx[ 4 ];  
while ( '\0' != *cpInput )
{
  nIdx[0] = ((*cpInput) & 0xFC)>>2;
  nIdx[1] = ((*cpInput) & 0x03)<<4;
  cpInput++;
  if ( '\0' != *cpInput )
  {
    nIdx[1] |= ((*cpInput) & 0xF0)>>4;
    nIdx[2]  = ((*cpInput) & 0x0F)<<2;
    cpInput++;
    if ( '\0' != (*cpInput) )
    {
      nIdx[2] |= ((*cpInput) & 0xC0) >> 6;
      nIdx[3]  = (*cpInput) & 0x3F;
      cpInput++;
    }
    else
      nIdx[3] = 64;
  }
  else
  {
    nIdx[2] = 64;
    nIdx[3] = 64;
  }

  *(cpOutput+0) = *(Base64Encoding + nIdx[0]);
  *(cpOutput+1) = *(Base64Encoding + nIdx[1]);
  *(cpOutput+2) = *(Base64Encoding + nIdx[2]);
  *(cpOutput+3) = *(Base64Encoding + nIdx[3]);
  cpOutput += 4;
}

*cpOutput = '\0';

return;
}

//----------------------------------------------------

bool ContainsInvalidNickChars(char * szString)
{
	int x=0;

	while(*szString) {
		if( (*szString >= '0' && *szString <= '9') ||
			(*szString >= 'A' && *szString <= 'Z') ||
			(*szString >= 'a' && *szString <= 'z')  ||
			*szString == ']' || *szString == '[' ||
			*szString == '_'  /* || *szString == '\'' ||
			*szString == '$' || *szString == ':' ||
			*szString == '=' || *szString == '(' ||
			*szString == ')' || *szString == '@' || 
			*szString == '.' */ ) {

			szString++;
		} else {
			return true;
		}
	}
	return false;
}

//----------------------------------------------------

void ReplaceBadChars(char * szString)
{
	while(*szString) {
		//if(*szString >= 1 && *szString < ' ') *szString = '?';
		if(*szString == '%') *szString = '?';

		szString++;
	}
}

//----------------------------------------------------

int CanFileBeOpenedForReading(char * filename)
{
	FILE *f;
	if(f=fopen(filename,"r")) {
		fclose(f);
		return 1;
	}
	return 0;
}

//----------------------------------------------------