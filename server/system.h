/*

	SA:MP Multiplayer Modification
	Copyright 2004-2005 SA:MP Team

	file:
		system.h
	desc:
		System defines header file.

    Version: $Id: system.h,v 1.6 2006/05/07 15:35:32 kyeman Exp $

*/

#ifndef _SYSTEM_H
#define _SYSTEM_H

typedef unsigned long DWORD;

//----------------------------------------------------

#pragma pack(1)
typedef struct _RGBA {
	unsigned char r,g,b,a;
} RGBA, *PRGBA;

#pragma pack(1)
typedef struct _VECTOR {
	float X,Y,Z;
} VECTOR, *PVECTOR;

#pragma pack(1)
typedef struct _VECTOR2D {
	float X,Y;
} VECTOR2D, *PVECTOR2D;

#pragma pack(1)
typedef struct _MATRIX4X4 {
	VECTOR right;
	DWORD  flags;
	VECTOR up;
	float  pad_u;
	VECTOR at;
	float  pad_a;
	VECTOR pos;
	float  pad_p;
} MATRIX4X4, *PMATRIX4X4;

//----------------------------------------------------

#ifdef _WIN32
# include <windows.h>
#else
typedef unsigned long ULONG;
typedef ULONG *PULONG;
typedef unsigned short USHORT;
typedef USHORT *PUSHORT;
typedef unsigned char UCHAR;
typedef UCHAR *PUCHAR;
typedef char *PSZ;
#ifndef FALSE
#define FALSE               0
#endif
#ifndef TRUE
#define TRUE                1
#endif
#define far
#define near
#define FAR                 far
#define NEAR                near
#ifndef CONST
#define CONST  const
#endif

typedef int                 BOOL;
typedef unsigned char       BYTE;
typedef unsigned short      WORD;
typedef float               FLOAT;
typedef FLOAT               *PFLOAT;
typedef BOOL near           *PBOOL;
typedef BOOL far            *LPBOOL;
typedef BYTE near           *PBYTE;
typedef BYTE far            *LPBYTE;
typedef char                CHAR;
typedef char				*PCHAR;
typedef int near            *PINT;
typedef int far             *LPINT;
typedef WORD near           *PWORD;
typedef WORD far            *LPWORD;
typedef long far            *LPLONG;
typedef DWORD near          *PDWORD;
typedef DWORD far           *LPDWORD;
typedef void far            *LPVOID;
typedef CONST void far      *LPCVOID;
typedef int                 INT;
typedef unsigned int        UINT;
typedef unsigned int        *PUINT;
#endif //!Win32

#endif // _SYSTEM_H


