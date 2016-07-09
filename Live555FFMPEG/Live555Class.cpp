// *****************************************************************************
//
//     
//
// *****************************************************************************

#include "stdafx.h"
#include "Live555Class.h"
#include "Live_AnalysingServerMediaSubsession.h"
#include <exception>
#ifdef _DEBUG
//#include "DebugHeader.h"
#endif
// ==========================================================================
Live555Class::Live555Class( FFMPEG * a_Encoder )
	: mHandle( INVALID_HANDLE_VALUE )
	, mID( 0 )
	, mStop( false )
	, mReturnValue ( 0 )
	, m_Encoder (a_Encoder)
{	
	SetDefaultValues();

//	TRACE("Live555 Init\n");
}


void Live555Class::SetDefaultValues() {
	RTSPPort=554;
	RTSPUser[0]=0;
	RTSPPass[0]=0;
}

// ==========================================================================
Live555Class::~Live555Class()
{
	if ( mHandle != INVALID_HANDLE_VALUE )
	{
		Stop();
	}
}

// ==========================================================================
void Live555Class::Start()
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
void Live555Class::Stop()
{
	ASSERT( mHandle != INVALID_HANDLE_VALUE );
	mStop = true;
	Stop_RTSP_Loop=char(0xFF);
	DWORD lRetVal = ::WaitForSingleObject( mHandle, 15000 );
	ASSERT( lRetVal != WAIT_TIMEOUT  );

	::CloseHandle( mHandle );
	mHandle = INVALID_HANDLE_VALUE;

	mID = 0;
}

// ==========================================================================
void Live555Class::SetPriority( int aPriority )
{
	ASSERT( mHandle != INVALID_HANDLE_VALUE );
	::SetThreadPriority( mHandle, aPriority );
}

// ==========================================================================
int Live555Class::GetPriority() const
{
	ASSERT( mHandle != INVALID_HANDLE_VALUE );
	return ::GetThreadPriority( mHandle );
}

// ==========================================================================
bool Live555Class::IsStopping() const
{
	ASSERT( mHandle != INVALID_HANDLE_VALUE );
	return mStop;
}

// ==========================================================================
bool Live555Class::IsDone()
{
	if ( ( mHandle == INVALID_HANDLE_VALUE ) ||
		( mID == 0 ) )
	{
		return true;
	}

	return ( ::WaitForSingleObject( mHandle, 0 ) == WAIT_OBJECT_0 );
}

// ==========================================================================
unsigned long WINAPI Live555Class::Link( void *aParam )
{
	Live555Class *lThis = reinterpret_cast<Live555Class *>( aParam );
	lThis->mReturnValue = lThis->Function();
	return lThis->mReturnValue;
}

// ==========================================================================
DWORD Live555Class::GetReturnValue()
{
	return mReturnValue;
}

// ==========================================================================
DWORD Live555Class::Function()
{
	while (mStop==false) {
		LiveSingleStart();
		if (mStop) {
			break;
		}
		Sleep(5000);
	}

	return 0;
}


DWORD Live555Class::LiveSingleStart() {
//	TRACE("Live555 Started\n");
	Stop_RTSP_Loop=0;
	TaskScheduler    *scheduler;
	UsageEnvironment *env ;
	char RTSP_Address[1024];
	RTSP_Address[0]=0x00;

	if (m_Encoder == NULL){
//		TRACE("No Video Encoder registered for the RTSP Encoder\n");
		return 0;
	}
	char* url;
	scheduler = BasicTaskScheduler::createNew();
	env = BasicUsageEnvironment::createNew(*scheduler);

	UserAuthenticationDatabase* authDB = NULL;
	RTSPServer* rtspServer =NULL;
	if ((RTSPUser[0]==0x00) || (RTSPPass[0]==0x00)) {
		rtspServer = RTSPServer::createNew(*env, RTSPPort);
	}
	else {
		// To implement client access control to the RTSP server, do the following:

		authDB = new UserAuthenticationDatabase;
		authDB->addUserRecord(RTSPUser, RTSPPass);

		rtspServer = RTSPServer::createNew(*env, RTSPPort, authDB);

	}

	// Create the RTSP server:
	ServerMediaSession* sms;
	UNREFERENCED_PARAMETER(sms);
#ifdef MULTICASTENABLE
	unsigned short rtpPortNum = 20000;
	unsigned short rtcpPortNum = rtpPortNum+1;
	unsigned char ttl = 5;
	struct in_addr destinationAddress;
	unsigned short rtspPort = 8554;
	unsigned short rtspOverHTTPPort = 8080;
#endif


	AnalyserInput* inputDevice;


	if (rtspServer == NULL) {
//		TRACE("LIVE555: Failed to create RTSP server: %s\n", env->getResultMsg());
		return 0;
	}
	else {
		char const* descriptionString = "Session streamed by \"IMC Server\"";



		// Initialize the WIS input device:
		inputDevice = AnalyserInput::createNew(*env, m_Encoder);
		if (inputDevice == NULL) {
//			TRACE("Live555: Failed to create WIS input device\n");
			return 0;
		}
		else {
			// A MPEG-1 or 2 video elementary stream:
			/* Increase the buffer size so we can handle the high res stream.. */
			OutPacketBuffer::maxSize = 300000;

#ifdef MULTICASTENABLE
			destinationAddress.s_addr = chooseRandomIPv4SSMAddress(*env);	
			ServerMediaSession* sms = ServerMediaSession::createNew(*env, "multicast");
#ifdef H264ENCODING
			sms->addSubsession(MulticastServerMediaSubsession::createNew(*env,destinationAddress, Port(rtpPortNum), Port(rtcpPortNum), ttl, 96, inputDevice,96));
#endif
#ifdef H265ENCODING
			sms->addSubsession(MulticastServerMediaSubsession::createNew(*env,destinationAddress, Port(rtpPortNum), Port(rtcpPortNum), ttl, 96, inputDevice,97));
#endif
#ifdef MP2ENCODING
			sms->addSubsession(MulticastServerMediaSubsession::createNew(*env,destinationAddress, Port(rtpPortNum), Port(rtcpPortNum), ttl, 96, inputDevice,32));
#endif
#ifdef MPEG4ENCODING
			sms->addSubsession(MulticastServerMediaSubsession::createNew(*env, destinationAddress, Port(rtpPortNum), Port(rtcpPortNum), ttl, 96, inputDevice, 999));
#endif

			rtspServer->addServerMediaSession(sms);
			url = rtspServer->rtspURL(sms);
			*env << "Play this stream using the URL \"" << url << "\"\n";
			m_Encoder->PassRTSPAddress(url);
//			TRACE("Play this stream using the URL %s\n", rtspServer->rtspURL(sms));
#else
			// NOTE: This *must* be a Video Elementary Stream; not a Program Stream
			sms = ServerMediaSession::createNew(*env, RTSP_Address, RTSP_Address, descriptionString);
			sms->addSubsession(AnalysingServerMediaSubsession::createNew(*env, *inputDevice, m_Encoder->Get_Bitrate()));

			rtspServer->addServerMediaSession(sms);
			url = rtspServer->rtspURL(sms);
			*env << "Play this stream using the URL \"" << url << "\"\n";
			m_Encoder->PassRTSPAddress(url);
#endif

			//announceStream(rtspServer, sms, streamName, inputFileName);
			//LOG_MSG("Play this stream using the URL %s", rtspServer->rtspURL(sms));

		}
	}

	Stop_RTSP_Loop=0;

	for (;;)
	{
		/* The actual work is all carried out inside the LIVE555 Task scheduler */
		env->taskScheduler().doEventLoop(&Stop_RTSP_Loop); // does not return

		if (mStop) {
			break;
		}
	}
	delete url;

	
	Medium::close(rtspServer); // will also reclaim "sms" and its "ServerMediaSubsession"s
	Medium::close(inputDevice);

	//AM added, fix mem leaks ???
	env->reclaim();
	delete scheduler;

	
	/*
	
	scheduler = BasicTaskScheduler::createNew();
	env = BasicUsageEnvironment::createNew(*scheduler);

	*/
	
	return 0;
}