#pragma once

typedef unsigned int	uint,	*puint;
typedef unsigned short	ushort,	*pushort;
typedef unsigned char	byte,	*pbyte;
typedef signed int		*pint;
typedef signed short	*pshort;
typedef signed char		*pchar;
typedef bool			*pbool;

#ifndef NULL
	#define NULL 0
#endif

#define NewException(a) #a
