/// \file
/// \brief Returns the value from QueryPerformanceCounter.  This is the function RakNet uses to represent time.
///
/// This file is part of RakNet Copyright 2003 Kevin Jenkins.
///
/// Usage of RakNet is subject to the appropriate license agreement.
/// Creative Commons Licensees are subject to the
/// license found at
/// http://creativecommons.org/licenses/by-nc/2.5/
/// Single application licensees are subject to the license found at
/// http://www.rakkarsoft.com/SingleApplicationLicense.html
/// Custom license users are subject to the terms therein.
/// GPL license users are subject to the GNU General Public
/// License as published by the Free
/// Software Foundation; either version 2 of the License, or (at your
/// option) any later version.

#ifndef __GET_TIME_H
#define __GET_TIME_H

#include "RakNetDefines.h"
#include "Export.h"

#include <stdint.h>

/// The namespace RakNet is not consistently used.  It's only purpose is to avoid compiler errors for classes whose names are very common.
/// For the most part I've tried to avoid this simply by using names very likely to be unique for my classes.
namespace RakNet
{
	// Define __GET_TIME_64BIT if you want to use large types for GetTime (takes more bandwidth when you transmit time though!)
	// You would want to do this if your system is going to run long enough to overflow the millisecond counter (over a month)
#if __GET_TIME_64BIT==1
	typedef uint64_t Time;
	typedef uint32_t Time32;
	typedef uint64_t Time64;
#else
	typedef uint32_t Time;
	typedef uint32_t Time32;
	typedef uint64_t Time64;
#endif

#ifdef RAKNET_TIME_EXPERIMENT
	typedef Time64 TimeExp;
#else
	typedef Time TimeExp;
#endif

	/// Returns the value from QueryPerformanceCounter.  This is the function RakNet uses to represent time.
	Time RAK_DLL_EXPORT GetTime(void);
	Time32 RAK_DLL_EXPORT GetTime32(void);
	Time64 RAK_DLL_EXPORT GetTime64(void);
}

#endif
