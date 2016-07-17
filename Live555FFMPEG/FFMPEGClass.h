//==============================================================================//
// FFMPEG ENCODER CLASS EXAMPLE                                                 //
//------------------------------------------------------------------------------//
// Notes: This C++ program is designed to simplify talking with the FFMPEG      //
// library. It provides some basic functions to encode a video from your code.  //
//                                                                              //
// Usage: Use this project as a template for your own program, or add the       //
// FFMPEG class to your project                                                 //
//==============================================================================//

#ifndef _DEFINITIONS
#include "Definitions.h"
#endif // !_DEFINITIONS

#pragma once

#include <string>

typedef struct {
	long DataLength;
	char * Data;
}Return_M;

class Live555Class;
#pragma once

//=============================
// Includes
//-----------------------------
// FFMPEG is writen in C so we need to use extern "C"
//-----------------------------
extern "C" {
	//#define INT64_C(x) (x ## LL)
	//#define UINT64_C(x) (x ## ULL)
	
	#include <stdlib.h>
	#include <stdio.h>
	#include <string.h>
	#include <math.h>
	#include <libavutil/opt.h>
	#include <libavutil/mathematics.h>
	#include <libavformat/avformat.h>
	#include <libswscale/swscale.h>
	#include <libswresample/swresample.h>
	#include <libavutil/imgutils.h>
	#include <libavcodec/avcodec.h>
	
	static int sws_flags = SWS_BICUBIC;
}

#include "Live_AnalyserSource.h"

class Encoder_Notify_List_Node {
public:
  Encoder_Notify_List_Node(TaskScheduler  *t, AnalyserSource *s){task = t; source = s;Next = NULL;};
  Encoder_Notify_List_Node(TaskScheduler  *t, AnalyserSource *s, Encoder_Notify_List_Node *N){task = t; source = s;Next = N;};
  TaskScheduler  *task;
  AnalyserSource *source;
  Encoder_Notify_List_Node *Next;
};

typedef struct {
	char * dataPointer;
	int dataSize;
	int FrameID;
}FrameStructure;

//=============================
// FFMPEG Class
//-----------------------------
class FFMPEG
{
public:
	//-----------------------------
	//Define our subroutines that we want the outside world to access
	//-----------------------------

	FFMPEG(void);
	~FFMPEG();

	void Start();
	void Stop();

	void SetPriority( int aPriority );
	int GetPriority() const;

	bool IsDone();
	DWORD GetReturnValue();

	int GetVideoWidth(void) {return m_AVIMOV_WIDTH;}
	int GetVideoHeight(void) {return m_AVIMOV_HEIGHT;}
	
	unsigned int GetFrameID();
	char GetFrame(u_int8_t** FrameBuffer, unsigned int *FrameSize, unsigned int *FrameID);
	char ReleaseFrame();

	void SendNewFrame(char * RGBFrame);

	void RegisterRTSP_Source(TaskScheduler* task, AnalyserSource* Asrc);
	void Un_RegisterRTSP_Source(AnalyserSource* Asrc);

	int Get_Bitrate();

	void PassRTSPAddress(char * addr);

	void SetVideoResolution(int width, int height);

	int GetEncoderType(void){ return m_selected_encoder; }

	int	m_AVIMOV_BPS;

	//Live555 Setup
	void SetRTSPPort(int PortNo);
	void SetRTSPUserandPassword(char * Username, char * Password);
	void SetRTSPAddress(char * streamname);
	void SetRTSPDescription(char * streamdescription);
	void SetEncoder(int m_enc);
	void SetMulticast(void);
	void SetUnicast(void);
	void SetRtpPortNum(int m_rtpnum);

private:
	//Mutex Functions
	int MUTEX_LOCK(HANDLE, DWORD,std::string);

	inline int MUTEX_LOCK( HANDLE * m_Mutex ) {
		return (WaitForSingleObject( *m_Mutex, 10 ) == WAIT_OBJECT_0 ? 1 : 0);
	}

	inline int MUTEX_UNLOCK( HANDLE * m_Mutex ) {
		return (ReleaseSemaphore( *m_Mutex , 1 , NULL) == 0);
	}

	HANDLE mFrameMtx;
	HANDLE mLiveMtx;

	int mVideoWidth;
	int mVideoHeight;

protected:

	static unsigned long WINAPI Link( void *aParam );
	virtual DWORD Function();
	bool IsStopping() const;

private:
	void LoadSQLDatabase();
	void SetDefaultVals();
	char * mInternalFrameBuff1;
	char * mInternalFrameBuff2;
	void SetupVideo(char * filename, int Width, int Height, int FPS, int GOB, int BitPerSecond, int);
	void WriteDummyFrame();
	void WriteFrame(char * RGBFrame);
	void CloseVideo(void);
	
	char * mNewFrame;
	char * mActiveFrame;
	
	bool m_AVIMutexWriteFrame;
	
	char * m_FrameBufferLive555;
	int m_FrameBufferLive555Size;
	int m_FrameBufferLive555ID;
	int m_FrameID2;

	bool mGrabbingFrame;
	bool mNoNewData;

	AVPacket pkt;

	HANDLE * m_FrameStructMutex;
	HANDLE mHandle;
	DWORD mID;
	bool mStop;
	DWORD mReturnValue;	

//#if defined(H264ENCODING) || defined(H265ENCODING)
	char * InitialPacketBuff;
	int InitialPacketBuffSize;
//#endif


private:
	//-----------------------------
	//Define our subroutines that only our class can access
	//-----------------------------

	Live555Class * mLive555Class;

	int m_sws_flags;
	int	m_AVIMOV_FPS;
	int	m_AVIMOV_GOB;
	int m_frame_count;
	int	m_AVIMOV_WIDTH;
	int	m_AVIMOV_HEIGHT;
	int m_RTP_Payload_Size;
	int m_FrameID;
	int m_LastSentFrameID;
	bool m_AVIMutex;
	int m_selected_encoder;
	int m_multicast;

    double m_video_time;

	char *m_filename;

	void CloseCodec(void);
	void WriteFrame(void);
	void SetupCodec(const char *filename, int codec_id);

	void *sws_ctx_void;
	AVFrame *m_frame, *m_framesrc;
    AVCodecContext *m_c;
	AVStream *m_video_st;
	AVOutputFormat *m_fmt;
	AVFormatContext *m_oc;
	AVCodec *m_video_codec;
    AVPicture m_src_picture, m_dst_picture;
	double PCFreq;
	__int64 CounterStart;
	void StartCounter();
	double GetCounter();

	FrameStructure * mFrameStructure;
	int mFrameStructureSent;
	Encoder_Notify_List_Node* Notify_Head_Node;
	unsigned int mFrameNo;
};

