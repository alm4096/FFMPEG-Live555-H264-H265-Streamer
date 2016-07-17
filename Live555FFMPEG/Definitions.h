//==========================================================================
// Definitions.h allows you to select an encoder 
//    - Choose between H264, H265, MP4, and MP2
//    - Choose between multicast and unicast
//==========================================================================

#pragma once
#ifndef _DEFINITIONS
#define _DEFINITIONS

//#include <windows.h>
#include <assert.h> 
#define ASSERT assert

#define FF_API_PRIVATE_OPT 1

//Used for MUTEX INFINITE Wait fails. Don't change
#define __LOGMUTEXFUNCTION__ std::string(__FUNCTION__+std::string(" ")).append(std::string(__FILE__+std::string(" ")).append(std::to_string((long long)__LINE__)))

#endif
