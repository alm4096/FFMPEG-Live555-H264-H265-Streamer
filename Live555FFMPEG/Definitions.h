#pragma once
#ifndef _DEFINITIONS
#define _DEFINITIONS


//#include <windows.h>
#include <assert.h> 
#define ASSERT assert

#define FF_API_PRIVATE_OPT 1

//FFMPEG Class Controls
#define FFMPEGENCODING
#define MULTICASTENABLE

//============================
//Choose one and only one encoder
//============================
//#define H264ENCODING
//#define H265ENCODING
//#define MP2ENCODING
#define MPEG4ENCODING
//============================

//#define Live555Debug

//Used for MUTEX INFINITE Wait fails. Don't change
#define __LOGMUTEXFUNCTION__ std::string(__FUNCTION__+std::string(" ")).append(std::string(__FILE__+std::string(" ")).append(std::to_string((long long)__LINE__)))

#endif
