//==============================================================================//
// FFMPEG ENCODER CLASS EXAMPLE                                                 //
//------------------------------------------------------------------------------//
// Notes: This C++ program is designed to simplify talking with the FFMPEG      //
// library. It provides some basic functions to encode a video from your code.  //
//                                                                              //
// Usage: Use this project as a template for your own program, or add the       //
// FFMPEG class to your project                                                 //
//==============================================================================//
// Definitions.h allows you to select an encoder 
//    - Choose between H264, H265, MP4, and MP2
//    - Choose between multicast and unicast
//==========================================================================



//=============================
// Includes
#include "stdafx.h"
#include "FFMPEGClass.h"
#include "Definitions.h"
#include "MutexUtility.h"
#include <climits>
#include "Live555Class.h"
#include <libavutil/error.h>

#define _NOBUFFFRAMES 150
//=============================
// Our constructor
//-----------------------------
// Create any necessary blocks of memory and init variables
//-----------------------------
FFMPEG::FFMPEG()
	: mHandle( INVALID_HANDLE_VALUE )
	, mID( 0 )
	, mStop( false )
	, mReturnValue ( 0 )
{
	SetDefaultVals();
	sws_ctx_void = NULL;

	mLive555Class = new Live555Class(this);

	av_init_packet(&pkt);

	mInternalFrameBuff1=new char [640*512*4];
	mInternalFrameBuff2=new char [640*512*4];
	
	m_FrameBufferLive555 = new char[300000];
	m_FrameBufferLive555Size=0;
	m_FrameBufferLive555ID=0;
	m_FrameID2=1;

	m_FrameID=1;
	m_filename = new char [1024];
	m_AVIMutex = false;
	mFrameNo=0;
	mFrameStructureSent=-1;
	mFrameStructure=new FrameStructure[_NOBUFFFRAMES];
	m_FrameStructMutex = new HANDLE[_NOBUFFFRAMES];
	for (int i=0;i<_NOBUFFFRAMES;i++){
		mFrameStructure[i].dataPointer=new char[100000]; //We can do this since the rtp packet size is limited to 12000;
		mFrameStructure[i].FrameID=-1;
		mFrameStructure[i].dataSize=0;
		m_FrameStructMutex[i] = CreateSemaphore(NULL,1,1,NULL);
	}
	m_LastSentFrameID=0;
	
	mNewFrame=NULL;
	mActiveFrame=NULL;
	Notify_Head_Node=NULL;
	mNoNewData=true;
	mGrabbingFrame=false;
	
	PCFreq = 0.0;
	CounterStart = 0;
	
	SetEncoder(AV_CODEC_ID_H264);
	SetMulticast();

//#if defined(H264ENCODING) || defined(H265ENCODING)
	InitialPacketBuff=new char [10000000];
	InitialPacketBuffSize=0;
//#endif
	m_AVIMutexWriteFrame=false;
	
	//Create semaphores not Mutexes (in windows land semaphores are equivalent to Mutexes)
	mFrameMtx=CreateSemaphore(NULL,1,1,NULL);
	mLiveMtx=CreateSemaphore(NULL,1,1,NULL);

	avcodec_register_all();

	LoadSQLDatabase();
}

void FFMPEG::SetRtpPortNum(int m_rtpnum) {
	mLive555Class->SetRtpPortNum(m_rtpnum);
}
void FFMPEG::SetEncoder(int m_enc) {
	m_selected_encoder = m_enc;
	mLive555Class->SetEncoder(m_enc);
}
void FFMPEG::SetMulticast(void) {
	m_multicast = 1;
	mLive555Class->SetMulticast();
}
void FFMPEG::SetUnicast(void) {
	m_multicast = 0;
	mLive555Class->SetUnicast();
}

void FFMPEG::SetVideoResolution(int width, int height) {
	mVideoHeight = height;
	mVideoWidth = width;

	delete[] mInternalFrameBuff1;
	delete[] mInternalFrameBuff2;

	mInternalFrameBuff1 = new char[height * width * 3];
	mInternalFrameBuff2 = new char[height * width * 3];
}

void FFMPEG::SendNewFrame(char * RGBFrame) {
	//Pass data if we are ready
	if (MUTEX_LOCK(&mFrameMtx)) {

		if (mActiveFrame!=NULL){
			//Do nothing...
		}
		else {
			//Copy frame RGB data
			memcpy(mInternalFrameBuff1, RGBFrame, mVideoWidth * mVideoHeight * 3); //(set size)
			mActiveFrame=mInternalFrameBuff1;

		}
	}
	MUTEX_UNLOCK(&mFrameMtx);
}

//=============================
// Our Destructor
//-----------------------------
// Delete any allocated memory
//-----------------------------
FFMPEG::~FFMPEG()
{
	delete mLive555Class;

	delete [] m_filename;

	Encoder_Notify_List_Node* Tmp_Node;
	while (Notify_Head_Node != NULL){
		Tmp_Node = Notify_Head_Node;
		Notify_Head_Node = Notify_Head_Node->Next;
		delete Tmp_Node;
	}

//#if defined(H264ENCODING) || defined(H265ENCODING)
	delete [] InitialPacketBuff;
//#endif

	for (int i=0;i<_NOBUFFFRAMES;i++) {	
		if (mFrameStructure[i].dataPointer!=NULL) {
			delete [] mFrameStructure[i].dataPointer;
			mFrameStructure[i].dataPointer=NULL;
			mFrameStructure[i].dataSize=0;
		}
	}
	delete [] mFrameStructure;
	delete [] m_FrameStructMutex;

	delete [] mInternalFrameBuff1;
	delete [] mInternalFrameBuff2;
	delete [] m_FrameBufferLive555;
	if ( mHandle != INVALID_HANDLE_VALUE )
	{
		Stop();
	}

}


// ==========================================================================
void FFMPEG::Start()
{
	mHandle = ::CreateThread(
		NULL,				// Security attributes
		0,					// Stack size, 0 is default
		Link,				// Start address
		this,				// Parameter
		0,					// Creation flags
		&mID );				// Thread ID
}

// ==========================================================================
void FFMPEG::Stop()
{
	ASSERT( mHandle != INVALID_HANDLE_VALUE );

	mStop = true;
	DWORD lRetVal = ::WaitForSingleObject( mHandle, 10000 );
	ASSERT( lRetVal != WAIT_TIMEOUT  );

	::CloseHandle( mHandle );
	mHandle = INVALID_HANDLE_VALUE;

	mID = 0;
}

// ==========================================================================
void FFMPEG::SetPriority( int aPriority )
{
	ASSERT( mHandle != INVALID_HANDLE_VALUE );
	::SetThreadPriority( mHandle, aPriority );
}

// ==========================================================================
int FFMPEG::GetPriority() const
{
	ASSERT( mHandle != INVALID_HANDLE_VALUE );
	return ::GetThreadPriority( mHandle );
}

// ==========================================================================
bool FFMPEG::IsStopping() const
{
	ASSERT( mHandle != INVALID_HANDLE_VALUE );
	return mStop;
}

// ==========================================================================
bool FFMPEG::IsDone()
{
	if ( ( mHandle == INVALID_HANDLE_VALUE ) ||
		 ( mID == 0 ) )
	{
		return true;
	}

	return ( ::WaitForSingleObject( mHandle, 0 ) == WAIT_OBJECT_0 );
}

// ==========================================================================
unsigned long WINAPI FFMPEG::Link( void *aParam )
{
	FFMPEG *lThis = reinterpret_cast<FFMPEG *>( aParam );
	lThis->mReturnValue = lThis->Function();
	return lThis->mReturnValue;
}

// ==========================================================================
DWORD FFMPEG::GetReturnValue()
{
	return mReturnValue;
}

void FFMPEG::SetDefaultVals(){
	//Set Defaults
	m_RTP_Payload_Size=30000;	
	m_AVIMOV_FPS=10;		//Movie frames per second
	m_AVIMOV_GOB=3;		//I frames per no of P frames, see note below!
	m_AVIMOV_BPS=1000000; //Bits per second, if this is too low then movie will become garbled or grey (H264 and H265 are funny about this)
}

void FFMPEG::LoadSQLDatabase(){
	SetDefaultVals();
}

// ==========================================================================
DWORD FFMPEG::Function()
{
	//Some inits
	int mFuncFPS=25;
	mFuncFPS=m_AVIMOV_FPS;

	SetupVideo("Output.avi", mVideoWidth, mVideoHeight, mFuncFPS, m_AVIMOV_GOB, m_AVIMOV_BPS, m_RTP_Payload_Size);

	//Start live555
	mLive555Class->Start();

	char * RecycleFrame=NULL;
	
	StartCounter();
	double mWaitTimeout=1000.00/double(mFuncFPS);
	double mProcessTime=0.00;

	int tempInt = 0;

	StartCounter();

	//Main frame loop (recycles frames if we don't get enough)
	for (;;)
	{
		//If we are stopping then exit
		if (mStop) {
			break;
		}

		// We limit refresh rate to ~ X fps max
		double mWaitTime = GetCounter();
		
		if ((mWaitTime>=(mWaitTimeout-mProcessTime))){
			//Count how long each frame takes to process
			StartCounter();

			//Not sure why this is here, might be able to remove.
			tempInt = (tempInt + 1) % 100;

			if (MUTEX_LOCK(&mFrameMtx)) {
				//If we have a frame then process
				if (mActiveFrame!=NULL){
					memcpy(mInternalFrameBuff2, mInternalFrameBuff1, mVideoWidth * mVideoHeight * 3); //(set size)
					
					mNewFrame=mInternalFrameBuff2;				
					mActiveFrame = NULL;

					//Unlock mutex here
					MUTEX_UNLOCK(&mFrameMtx);

					WriteFrame(mNewFrame);
				
					RecycleFrame = mNewFrame;
					mProcessTime=(mProcessTime*50+GetCounter())/51;
				}
				else if  (RecycleFrame!=NULL) {
					//Recycle frame
					MUTEX_UNLOCK(&mFrameMtx);

					mNewFrame=RecycleFrame;				
					WriteFrame(mNewFrame);
					mProcessTime=(mProcessTime*50+GetCounter())/51;
				}
			}
			else if  (RecycleFrame!=NULL) {
				//Recycle frame
				MUTEX_UNLOCK(&mFrameMtx);

				mNewFrame=RecycleFrame;				
				WriteFrame(mNewFrame);
				mProcessTime=(mProcessTime*50+GetCounter())/51;
			}
			MUTEX_UNLOCK(&mFrameMtx);
		}

		Sleep(1);
	}

	//Clean up
	mLive555Class->Stop();
	while(!mLive555Class->IsDone()) {Sleep(1);}
	CloseVideo();

	return 0;
}

//use performance counters instead of sleeps (much more accurate)
void FFMPEG::StartCounter()
{
    LARGE_INTEGER li;
	if (!QueryPerformanceFrequency(&li)) {
//		TRACE("\nPerformance Counter Failed\n");
	}
    PCFreq = double(li.QuadPart)/1000.0;

    QueryPerformanceCounter(&li);
    CounterStart = li.QuadPart;
}
double FFMPEG::GetCounter()
{
    LARGE_INTEGER li;
    QueryPerformanceCounter(&li);
    return double(li.QuadPart-CounterStart)/PCFreq;
}

//=============================
// Setup Video Codec
//-----------------------------
// Sets up the video codec
//-----------------------------
void FFMPEG::SetupCodec(const char *filename, int codec_id)
{
	//---------------------
	//The basic flow for opening a video is:
	//---------------------
	//Register FFMPEG -> Allocate Media Context -> Allocate Stream Format Context -> 
	//Allocate Codec Context -> Allocate Frames -> Open Media File -> Start Writing Frames!!

	//If file is already open then don't allow user to open it again
	if (m_AVIMutex) {return;}
	
	//Some inits
	int ret;
	m_sws_flags = SWS_BICUBIC;
	m_frame_count=0;
	
	//You must call these subroutines otherwise you get errors!!
	avcodec_register_all();
	av_register_all();

	// Add the audio and video streams using the default format codecs
	// and initialize the codecs.
	m_video_st = NULL;

	// Add an output stream.
	{

		//find the encoder
		m_video_codec = avcodec_find_encoder((AVCodecID)codec_id);  //avcodec_find_encoder(m_fmt->video_codec);
		if (!(m_video_codec)) {
			return;
		}

		m_c = avcodec_alloc_context3(m_video_codec);
		if (!(m_c)) {
			return;
		}

		//Setup fundumental video stream parameters
		//m_c->codec_id = m_fmt->video_codec;
		m_c->width = m_AVIMOV_WIDTH;			//Note Resolution must be a multiple of 2!!
		m_c->height = m_AVIMOV_HEIGHT;		//Note Resolution must be a multiple of 2!!
		m_c->time_base.den = m_AVIMOV_FPS;		//Frames per second
		m_c->time_base.num = 1;
		m_c->gop_size = m_AVIMOV_GOB;		// Intra frames per x P frames


		if ((m_selected_encoder==AV_CODEC_ID_MPEG4) || (m_selected_encoder==AV_CODEC_ID_MPEG2VIDEO)) {
			m_c->max_b_frames = 1;
		}

		m_c->pix_fmt       = AV_PIX_FMT_YUV420P;//Do not change this, H264 needs YUV format not RGB
		//m_c->rtp_payload_size = m_RTP_Payload_Size;// 30000 This is the NAL unit size!
		m_c->bit_rate = m_AVIMOV_BPS;			//Bits Per Second 

		if ((codec_id == AV_CODEC_ID_H264) || (codec_id == AV_CODEC_ID_H265)){
			av_opt_set(m_c->priv_data, "preset", "fast", 0); //change between ultrafast,superfast, veryfast, faster, fast, medium, slow, slower, veryslow, placebo
		}

		av_opt_set_int(m_c->priv_data, "slice-max-size", m_RTP_Payload_Size, 0); //Replaces RTP Payload size

		//m_c->rtp_payload_size = m_RTP_Payload_Size;// 30000 This is the NAL unit size!
	}

	
	// Now that all the parameters are set, we can open the audio and
	// video codecs and allocate the necessary encode buffers.
	{
		//Open the codec
		ret = avcodec_open2(m_c, m_video_codec, NULL);
		if (ret < 0) {
			return;
		}

		//allocate and init a re-usable frame
		//m_frame = avcodec_alloc_frame();
		m_frame = av_frame_alloc();
		if (!m_frame) {
			return;
		}
		
		//Allocate RGB frame that we can pass to the YUV frame
		//ret = avpicture_alloc(&m_src_picture, AV_PIX_FMT_RGB24, m_c->width, m_c->height);
		m_framesrc = av_frame_alloc();
		if (!m_framesrc) {
			return;
		}
	

		//Actually use the frame
		m_framesrc->format = m_c->pix_fmt;
		m_framesrc->width = m_c->width;
		m_framesrc->height = m_c->height;

		ret = av_image_alloc(m_framesrc->data, m_framesrc->linesize, m_c->width, m_c->height,
			AV_PIX_FMT_RGB24, 24);
		if (ret < 0) {
			return;
		}
	
		//Actually use the frame
		m_frame->format = m_c->pix_fmt;
		m_frame->width = m_c->width;
		m_frame->height = m_c->height;

		ret = av_image_alloc(m_frame->data, m_frame->linesize, m_c->width, m_c->height,
			m_c->pix_fmt, 32);
		if (ret < 0) {
			return;
		}
		
	}

	if (ret < 0) {
		return;
	}

	//Set frame count to zero
	if (m_frame)
		m_frame->pts = 0;
	
	//Frame is initalised!
	m_AVIMutex=true;

}

//=============================
// Write Video Frame
//-----------------------------
// Writes a Video Frame that is stored in m_src_picture
//-----------------------------
void FFMPEG::WriteFrame(void){

	//If video is not initalised then don't write frame
	if (!m_AVIMutex) {return;}

	if (!MUTEX_LOCK(&mLiveMtx)) {
		return;
	}
	
	//Inits
	int ret;

	struct SwsContext *sws_ctx = (struct SwsContext *)sws_ctx_void;


	//If we haven't already done so init the context of the frame conversion from RGB to YUV
	if (!sws_ctx) {
		sws_ctx = sws_getContext(m_c->width, m_c->height, AV_PIX_FMT_RGB24,
			m_c->width, m_c->height, AV_PIX_FMT_YUV420P,
									sws_flags, NULL, NULL, NULL);
		if (!sws_ctx) {
			//mMainDlg->mSystemLog->AddEntry( "FFMPEG::WriteFrame - Failed sws_getContext" );
			MUTEX_UNLOCK(&mLiveMtx);
			return;
		}
	}

	//Convert RGB frame (m_src_picture) to and YUV frame (m_dst_picture)
	sws_scale(sws_ctx,
		(uint8_t* const*)m_framesrc->data, m_framesrc->linesize,
		0, m_c->height, (uint8_t* const*)m_frame->data, m_frame->linesize);
	
	//Pass back...
	sws_ctx_void = (void*)sws_ctx;

	//Some inits for encoding the frame
	int got_packet;
	av_init_packet(&pkt);
	pkt.data = NULL;    // packet data will be allocated by the encoder
	pkt.size = 0;
	
	//Encode the frame
	ret = avcodec_encode_video2(m_c, &pkt, m_frame, &got_packet);
	if (ret < 0) {
		MUTEX_UNLOCK(&mLiveMtx);
		char temp2[1024];
		av_strerror(ret,temp2,1024);
		//mMainDlg->mSystemLog->AddEntry( "FFMPEG::WriteFrame - Failed to encode the frame" );
		return;
	}

	//If size of encoded frame is zero, it means the image was buffered.
	if (!ret && got_packet && pkt.size) {

		if ((m_selected_encoder==AV_CODEC_ID_H265) || (m_selected_encoder==AV_CODEC_ID_H264)) {

			if (INT_MAX<10000000) {
				__debugbreak();
			}

			if (InitialPacketBuffSize + pkt.size>10000000)  {
				//We should have never got here...
				InitialPacketBuffSize = 0;
				__debugbreak();
				//Something VERY VERY Bad is about to happen...
			}
			memcpy(&InitialPacketBuff[InitialPacketBuffSize], pkt.data, pkt.size);
			InitialPacketBuffSize += pkt.size;



			bool LoopTheLoop = true;

			//Break the encoded data into induvidual NAL units
			while (LoopTheLoop) {
				LoopTheLoop = false;

				int NalStart = -1;
				for (int k = 0; k<InitialPacketBuffSize - 4; k++) {
					if ((InitialPacketBuff[k] == 0x00) && (InitialPacketBuff[k + 1] == 0x00) && (InitialPacketBuff[k + 2] == 0x01)) {
						NalStart = k + 3;
						break;
					}
				}

				int NalEnd = -1;
				if (NalStart != -1) {
					//We've found the start of a nal unit
					for (int k = NalStart; k<InitialPacketBuffSize - 4; k++) {
						if (((InitialPacketBuff[k + 1] == 0x00) && (InitialPacketBuff[k + 2] == 0x00) && (InitialPacketBuff[k + 3] == 0x01)) ||
							((InitialPacketBuff[k + 1] == 0x00) && (InitialPacketBuff[k + 2] == 0x00) && (InitialPacketBuff[k + 3] == 0x00))
							)
						{
							NalEnd = k + 1;
							LoopTheLoop = true;
							break;
						}
					}
				}

				if (LoopTheLoop) {

					mFrameStructureSent = (mFrameStructureSent + 1) % _NOBUFFFRAMES;

					//DO NOT CHANGE THIS TO ANYTHING BUT INFINITE, otherwise frames will be corrupted
					if (MUTEX_LOCK(m_FrameStructMutex[mFrameStructureSent], INFINITE, __LOGMUTEXFUNCTION__)) {

						if (mFrameStructure[mFrameStructureSent].dataSize != 0) {
							//__debugbreak();
							//TRACE("FFMPEG Problem!!!\n");
							//Live555 is not getting packets
						}

						if ((NalEnd - NalStart)<100000) {
							memcpy(mFrameStructure[mFrameStructureSent].dataPointer, &InitialPacketBuff[NalStart], NalEnd - NalStart);
							mFrameStructure[mFrameStructureSent].dataSize = NalEnd - NalStart;
						}
						else {
							memcpy(mFrameStructure[mFrameStructureSent].dataPointer, &InitialPacketBuff[NalStart], 100000);
							mFrameStructure[mFrameStructureSent].dataSize = 100000;
							//TRACE("NAL PACKET TOO BIG!!!\n");
							//__debugbreak();
						}


						InitialPacketBuffSize -= NalEnd;
						memmove(&InitialPacketBuff[0], &InitialPacketBuff[NalEnd], InitialPacketBuffSize);

						m_FrameID = (m_FrameID % 32760) + 1;
						mFrameStructure[mFrameStructureSent].FrameID = (m_FrameID);

						UNLOCK(m_FrameStructMutex[mFrameStructureSent]);

						if (Notify_Head_Node != NULL) {
							//notify anyone waiting that a new frame is here..
							Encoder_Notify_List_Node* Tmp_Head_Node = Notify_Head_Node;
							while (Tmp_Head_Node != NULL) {
								Tmp_Head_Node->task->triggerEvent(AnalyserSource::eventTriggerId, Tmp_Head_Node->source);
								Tmp_Head_Node = Tmp_Head_Node->Next;

							}
						}
					}
				}
			}
		}
		else {

			mFrameStructureSent=(mFrameStructureSent+1)%_NOBUFFFRAMES;

			//DO NOT CHANGE THIS TO ANYTHING BUT INFINITE, otherwise frames will be corrupted
			if (MUTEX_LOCK(m_FrameStructMutex[mFrameStructureSent], INFINITE, __LOGMUTEXFUNCTION__)) {

				if (mFrameStructure[mFrameStructureSent].dataSize != 0) {
					//__debugbreak();
					//TRACE("FFMPEG Problem!!!\n");
					//Live555 is not getting packets
				}

				if (pkt.size < 100000) {
					memcpy(mFrameStructure[mFrameStructureSent].dataPointer, pkt.data, pkt.size);
					mFrameStructure[mFrameStructureSent].dataSize = pkt.size;
					m_FrameID = (m_FrameID % 32760) + 1;
					mFrameStructure[mFrameStructureSent].FrameID = (m_FrameID);
				}
				else {
					//				TRACE("ERROR! Packet too big");
					memcpy(mFrameStructure[mFrameStructureSent].dataPointer, pkt.data, 100000);
					mFrameStructure[mFrameStructureSent].dataSize = 100000;
					m_FrameID = (m_FrameID % 32760) + 1;
					mFrameStructure[mFrameStructureSent].FrameID = (m_FrameID);
				}
				UNLOCK(m_FrameStructMutex[mFrameStructureSent]);

				if (Notify_Head_Node != NULL) {
					//notify anyone waiting that a new frame is here..
					Encoder_Notify_List_Node* Tmp_Head_Node = Notify_Head_Node;
					while (Tmp_Head_Node != NULL) {
						Tmp_Head_Node->task->triggerEvent(AnalyserSource::eventTriggerId, Tmp_Head_Node->source);
						Tmp_Head_Node = Tmp_Head_Node->Next;

					}
				}
			}
		}

		av_packet_unref(&pkt);

		//if non-zero then it means that there was something wrong writing the frame to
		//the file
		if (ret != 0) {
			MUTEX_UNLOCK(&mLiveMtx);
			return;
		}
	} 
	else {
		ret = 0;
	}
	
	//Increment Frame counter
	m_frame_count++;
	m_frame->pts += av_rescale_q(1, m_c->time_base, m_c->time_base);
	MUTEX_UNLOCK(&mLiveMtx);
}


int FFMPEG::MUTEX_LOCK( HANDLE m_Mutex, DWORD atimeout, std::string functionname ) {
	//if timeout is infinite then log a locked up thread
	if (atimeout==INFINITE) {
		//Wait 60 seconds for a lock
		int ret = (WaitForSingleObject( m_Mutex, 60000 ) == WAIT_OBJECT_0 ? 1 : 0);

		//Check if we got a lock
		if (ret==1) {
			//We got a lock so it's okay
			return ret;
		}
		else {

			//Stop so we know something went wrong
			__debugbreak();

			//Log the error
			functionname+=" - Mutex Lock Failed (INFINITE Lock)";
			//mMainDlg->SetNewLogEntry(functionname);

			//Wait infinitely
			return (WaitForSingleObject( m_Mutex, atimeout ) == WAIT_OBJECT_0 ? 1 : 0);	
		}
	}
	else {
		return (WaitForSingleObject( m_Mutex, atimeout ) == WAIT_OBJECT_0 ? 1 : 0);
	}
}

//=============================
// Setup Video
//-----------------------------
// Sets up the Video and Opens the Video File
//-----------------------------
void FFMPEG::SetupVideo(char * filename, int Width, int Height, int FPS, int GOB, int BitPerSecond, int RTPPSize) {
	
	//Copy filename to local string
	sprintf_s(m_filename,1024,filename);
	
	//Set movie parameters
	m_AVIMOV_WIDTH=Width;	//Movie width
	m_AVIMOV_HEIGHT=Height;	//Movie height
	m_AVIMOV_FPS=FPS;		//Movie frames per second
	m_AVIMOV_GOB=GOB;		//I frames per no of P frames, see note below!
	m_AVIMOV_BPS=BitPerSecond; //Bits per second, if this is too low then movie will become garbled
	m_RTP_Payload_Size = RTPPSize;

	//Pass Parameters to Setup Codec as H264 file
	SetupCodec(m_filename, m_selected_encoder);
	/*
#ifdef MPEG4ENCODING
	SetupCodec(m_filename, AV_CODEC_ID_MPEG4);
#endif
#ifdef MP2ENCODING
	SetupCodec(m_filename, AV_CODEC_ID_MPEG2VIDEO);
#endif
#ifdef MP1ENCODING
	SetupCodec(m_filename, AV_CODEC_ID_MPEG1VIDEO);
#endif
#ifdef H264ENCODING	
	SetupCodec(m_filename,AV_CODEC_ID_H264);
#endif
#ifdef H265ENCODING	
	SetupCodec(m_filename,AV_CODEC_ID_H265);
#endif
#ifdef MJPEGENCODING
	SetupCodec(m_filename, AV_CODEC_ID_MJPEG);
#endif
	
	*/

	//------------
	//NOTE: on GOB
	//------------
	//An I frame is an entire frame stored. A P frame is only a partial frame that is 
	//encoded based on the previous frame. 
	//
	//Think of I frames as refreshing the entire video, and P frames as just encoding the 
	//moving bits of the frame.
	//
	//A GOB of 10 means that for every 10 P frames there is only 1 I frame, i.e. the frame is
	//only 'refreshed' every 10 frames.
	//------------
}

//=============================
// Close Codec
//-----------------------------
// Close Video Codec
//-----------------------------
void FFMPEG::CloseCodec(void){

	//If video is not initalised then don't close frame
	if (!m_AVIMutex) {return;}

	//Set open flag clear
	m_AVIMutex=false;

	//Swsfree context
	struct SwsContext *sws_ctx = (struct SwsContext *)sws_ctx_void;

	//Free context
	sws_freeContext(sws_ctx);
	sws_ctx_void = NULL;


	//Close Video codec
	avcodec_close(m_c);
	av_free(m_c);
	m_c = NULL;
	
	//Free our frames
    //av_free(m_src_picture.data[0]);
	//->
	av_freep(&m_framesrc->data[0]);
	av_frame_free(&m_framesrc);
	av_freep(&m_frame->data[0]);
	av_frame_free(&m_frame);
	
	m_frame = NULL;
}

void FFMPEG::SetRTSPPort(int PortNo) {
	mLive555Class->SetRTSPPort(PortNo);
}

void FFMPEG::SetRTSPUserandPassword(char * Username, char * Password) {
	mLive555Class->SetRTSPUserandPassword(Username, Password);
}

void FFMPEG::SetRTSPAddress(char * streamname) {
	mLive555Class->SetRTSPAddress(streamname);
}

void FFMPEG::SetRTSPDescription(char * streamdescription) {
	mLive555Class->SetRTSPDescription(streamdescription);
}

//=============================
// Close Codec
//-----------------------------
// Close Video Codec
//-----------------------------
void FFMPEG::CloseVideo(void){
	
	//Close Video Codec and write end of file
	CloseCodec();	
}



//=============================
// Write Dummy Frame
//-----------------------------
// Processes a Randomly Generated RGB frame
//-----------------------------
void FFMPEG::WriteDummyFrame(){

	char * tempDummyFrame=new char [m_c->height * m_c->width*3];

	//Step through height of frame
	for (int x=0;x<m_c->height * m_c->width*3;x++) {  //Height Loop

		//Save RGB frame to FFMPEG's source frame
		tempDummyFrame[x] = char(rand() * 255);
	}
	
	//Send frame off to FFMPEG for encoding
	WriteFrame(tempDummyFrame);
	
	delete [] tempDummyFrame;
}



//=============================
// Write Frame
//-----------------------------
// Processes an RGB frame supplied by the user
//-----------------------------
void FFMPEG::WriteFrame(char * RGBFrame) {
	
	//For video recording

	//If video is not initalised then don't write frame
	if (!m_AVIMutex) {return;}
	
	//Data should be in the format RGBRGBRGB...etc and should be Width*Height*3 long
	
	//Step through height of frame
	for (int y=0;y<m_c->height;y++) {  //Height Loop

		//Step through width of frame
		for (int x=0;x<m_c->width;x++) { //Width Loop

			//Save RGB frame to FFMPEG's source frame
			m_framesrc->data[0][y * m_framesrc->linesize[0] + x*3+0] = RGBFrame[(y*m_c->width+x)*3+0];  //Red Channel
			m_framesrc->data[0][y * m_framesrc->linesize[0] + x*3+1] = RGBFrame[(y*m_c->width+x)*3+1];  //Green Channel
			m_framesrc->data[0][y * m_framesrc->linesize[0] + x*3+2] = RGBFrame[(y*m_c->width+x)*3+2];  //Blue Channel
		}
	}

	//Send frame off to FFMPEG for encoding
	WriteFrame();
}


int FFMPEG::Get_Bitrate(){
	return m_AVIMOV_BPS;
}



unsigned int FFMPEG::GetFrameID(){
	
	if (mStop) {
		return 0; //We're stopping so tell Live555
	}

	if (mFrameStructure[m_LastSentFrameID].dataSize==0) {
		//If there's no data available then just return now.
		return m_FrameID2;
	}

	m_FrameID2=(m_FrameID2%32760)+1;
	return m_FrameID2;
}

char FFMPEG::GetFrame(u_int8_t** FrameBuffer, unsigned int *FrameSize, unsigned int *FrameID){
	if (mStop){
		*FrameBuffer=NULL;
		return -1;
	}

	//Wait for FFMPEG
	if (mFrameStructure[m_LastSentFrameID].dataSize==0) {
		*FrameBuffer=NULL;
		*FrameSize=0;
		return 1;
	}

	if(LOCK( m_FrameStructMutex[m_LastSentFrameID],0 )) {
		
		memcpy(m_FrameBufferLive555,mFrameStructure[m_LastSentFrameID].dataPointer,mFrameStructure[m_LastSentFrameID].dataSize);
		m_FrameBufferLive555Size=mFrameStructure[m_LastSentFrameID].dataSize;
		m_FrameBufferLive555ID=m_FrameID2;
		mFrameStructure[m_LastSentFrameID].dataSize=0;
		UNLOCK( m_FrameStructMutex[m_LastSentFrameID] );
		m_LastSentFrameID=(m_LastSentFrameID+1)%_NOBUFFFRAMES;

		*FrameBuffer = (u_int8_t*)m_FrameBufferLive555;
		*FrameSize = m_FrameBufferLive555Size;
		*FrameID = m_FrameBufferLive555ID;
	}
	else {
		*FrameBuffer=NULL;
		*FrameSize=0;
		return 1;	
	}
	
	return 1;
}

//WE only have two frames so work with that.....
char FFMPEG::ReleaseFrame(){

	if (mStop){
		return -1;
	}

	return 1;
}

void FFMPEG::RegisterRTSP_Source(TaskScheduler* task, AnalyserSource* Asrc){

	/* add the new source to the notify list.. */
	Notify_Head_Node = new Encoder_Notify_List_Node(task, Asrc, Notify_Head_Node);

}
void FFMPEG::Un_RegisterRTSP_Source(AnalyserSource* Asrc){
	Encoder_Notify_List_Node* Tmp_Head_Node = Notify_Head_Node;
	if (Notify_Head_Node->source == Asrc){
		Notify_Head_Node = Notify_Head_Node->Next;
	}
	else {
		while (Tmp_Head_Node != NULL && Tmp_Head_Node->Next != NULL){
			if (Tmp_Head_Node->Next->source == Asrc){
				Encoder_Notify_List_Node* Tmp_Node = Tmp_Head_Node->Next;
				Tmp_Head_Node->Next = Tmp_Head_Node->Next->Next;
				delete Tmp_Node;
				/* stop the search.. */
				Tmp_Head_Node = NULL;
			}
			else {
				Tmp_Head_Node = Tmp_Head_Node->Next;
			}
		}
	}
}

void FFMPEG::PassRTSPAddress(char * addr) 
{
	//mMainDlg->SetRTSPAddress(addr); 
}