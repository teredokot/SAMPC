/// Define __GET_TIME_64BIT to have RakNetTime use a 64, rather than 32 bit value.  A 32 bit value will overflow after about 5 weeks.
/// However, this doubles the bandwidth use for sending times, so don't do it unless you have a reason to.
/// Disabled by default.
#ifndef __GET_TIME_64BIT
//#define __GET_TIME_64BIT
#endif

/// Workaround for http://support.microsoft.com/kb/274323
/// If two calls between RakNet::GetTime() happen farther apart than this time in microseconds, this delta will be returned instead
/// Note: This will cause ID_TIMESTAMP to be temporarily inaccurate if you set a breakpoint that pauses the UpdateNetworkLoop() thread in RakPeer
/// To enable, set GET_TIME_SPIKE_LIMIT to (non-zero) or (0) to disable 
#ifndef GET_TIME_SPIKE_LIMIT
#define GET_TIME_SPIKE_LIMIT 0
#endif

/// For RakNet time rollover experiment
#ifndef RAKNET_TIME_EXPERIMENT
#define RAKNET_TIME_EXPERIMENT
#endif

/// Makes RakNet threadsafe
/// Define this if you use the same instance of RakPeer from multiple threads
/// Otherwise leave it undefined, since it makes things an order of magnitude slower.
/// Disabled by default
#ifndef _RAKNET_THREADSAFE
//#define _RAKNET_THREADSAFE
#endif

/// Can interrupt a Sleep() if a message is incoming.  Useful to define if you pass a large sleep value to RakPeer::Initialize
#ifndef USE_WAIT_FOR_MULTIPLE_EVENTS
// #define USE_WAIT_FOR_MULTIPLE_EVENTS
#endif

/// Define __BITSTREAM_NATIVE_END to NOT support endian swapping in the BitStream class.  This is faster and is what you should use
/// unless you actually plan to have different endianness systems connect to each other
/// Enabled by default.
#ifndef __BITSTREAM_NATIVE_END
#define __BITSTREAM_NATIVE_END
#endif

/// Maximum (stack) size to use with _alloca before using new and delete instead.
#ifndef MAX_ALLOCA_STACK_ALLOCATION
#define MAX_ALLOCA_STACK_ALLOCATION 1048576
#endif
