#include "stdafx.h"
#include "Definitions.h"
#include "Live_AnalysingServerMediaSubsession.h"
#include "FFMPEGClass.h"


#ifdef MPEG4ENCODING
#include <MPEG4ESVideoRTPSink.hh>
#include <MPEG4VideoStreamFramer.hh>
#include <MPEG4VideoStreamDiscreteFramer.hh>

AnalysingServerMediaSubsession* AnalysingServerMediaSubsession::createNew(UsageEnvironment& env, AnalyserInput& wisInput, unsigned estimatedBitrate,
	Boolean iFramesOnly,
	double vshPeriod) {
		return new AnalysingServerMediaSubsession(env, wisInput, estimatedBitrate,
			iFramesOnly, vshPeriod);
}

AnalysingServerMediaSubsession
	::AnalysingServerMediaSubsession(UsageEnvironment& env, AnalyserInput& analyserInput,	unsigned estimatedBitrate, Boolean iFramesOnly, double vshPeriod) 
	: OnDemandServerMediaSubsession(env, True /*reuse the first source*/),

	fAnalyserInput(analyserInput), fIFramesOnly(iFramesOnly), fVSHPeriod(vshPeriod) {
		fEstimatedKbps = (estimatedBitrate + 500)/1000;

}

AnalysingServerMediaSubsession
	::~AnalysingServerMediaSubsession() {
}

FramedSource* AnalysingServerMediaSubsession ::createNewStreamSource(unsigned /*clientSessionId*/, unsigned& estBitrate) {
	estBitrate = fEstimatedKbps;

	// Create a framer for the Video Elementary Stream:
	//LOG_MSG("Create Net Stream Source [%d]", estBitrate);

	return MPEG4VideoStreamDiscreteFramer::createNew(envir(), fAnalyserInput.videoSource());
}

RTPSink* AnalysingServerMediaSubsession ::createNewRTPSink(Groupsock* rtpGroupsock, unsigned char /*rtpPayloadTypeIfDynamic*/, FramedSource* /*inputSource*/) {
	setVideoRTPSinkBufferSize();
	return MPEG4ESVideoRTPSink::createNew(envir(), rtpGroupsock, 96);
}
#endif

#ifdef H265ENCODING
#include <H265VideoRTPSink.hh>
#include <H265VideoStreamFramer.hh>
#include <H265VideoStreamDiscreteFramer.hh>

AnalysingServerMediaSubsession* AnalysingServerMediaSubsession::createNew(UsageEnvironment& env, AnalyserInput& wisInput, unsigned estimatedBitrate,
	Boolean iFramesOnly,
	double vshPeriod) {
	return new AnalysingServerMediaSubsession(env, wisInput, estimatedBitrate,
		iFramesOnly, vshPeriod);
}

AnalysingServerMediaSubsession
::AnalysingServerMediaSubsession(UsageEnvironment& env, AnalyserInput& analyserInput,	unsigned estimatedBitrate, Boolean iFramesOnly, double vshPeriod) 
: OnDemandServerMediaSubsession(env, True /*reuse the first source*/),

fAnalyserInput(analyserInput), fIFramesOnly(iFramesOnly), fVSHPeriod(vshPeriod) {
	fEstimatedKbps = (estimatedBitrate + 500)/1000;

}

AnalysingServerMediaSubsession
::~AnalysingServerMediaSubsession() {
}

FramedSource* AnalysingServerMediaSubsession ::createNewStreamSource(unsigned /*clientSessionId*/, unsigned& estBitrate) {
	estBitrate = fEstimatedKbps;

	// Create a framer for the Video Elementary Stream:
	//LOG_MSG("Create Net Stream Source [%d]", estBitrate);

	return H265VideoStreamDiscreteFramer::createNew(envir(), fAnalyserInput.videoSource());
}

RTPSink* AnalysingServerMediaSubsession ::createNewRTPSink(Groupsock* rtpGroupsock, unsigned char /*rtpPayloadTypeIfDynamic*/, FramedSource* /*inputSource*/) {
	setVideoRTPSinkBufferSize();
	return H265VideoRTPSink::createNew(envir(), rtpGroupsock, 96);
}
#endif
#ifdef H264ENCODING
#include <H264VideoRTPSink.hh>
#include <H264VideoStreamFramer.hh>
#include <H264VideoStreamDiscreteFramer.hh>

AnalysingServerMediaSubsession* AnalysingServerMediaSubsession::createNew(UsageEnvironment& env, AnalyserInput& wisInput, unsigned estimatedBitrate,
	Boolean iFramesOnly,
	double vshPeriod) {
		return new AnalysingServerMediaSubsession(env, wisInput, estimatedBitrate,
			iFramesOnly, vshPeriod);
}

AnalysingServerMediaSubsession
	::AnalysingServerMediaSubsession(UsageEnvironment& env, AnalyserInput& analyserInput,	unsigned estimatedBitrate, Boolean iFramesOnly, double vshPeriod) 
	: OnDemandServerMediaSubsession(env, True /*reuse the first source*/),

	fAnalyserInput(analyserInput), fIFramesOnly(iFramesOnly), fVSHPeriod(vshPeriod) {
		fEstimatedKbps = (estimatedBitrate + 500)/1000;

}

AnalysingServerMediaSubsession
	::~AnalysingServerMediaSubsession() {
}

FramedSource* AnalysingServerMediaSubsession ::createNewStreamSource(unsigned /*clientSessionId*/, unsigned& estBitrate) {
	estBitrate = fEstimatedKbps;

	// Create a framer for the Video Elementary Stream:
	//LOG_MSG("Create Net Stream Source [%d]", estBitrate);

	return H264VideoStreamDiscreteFramer::createNew(envir(), fAnalyserInput.videoSource());
}

RTPSink* AnalysingServerMediaSubsession ::createNewRTPSink(Groupsock* rtpGroupsock, unsigned char /*rtpPayloadTypeIfDynamic*/, FramedSource* /*inputSource*/) {
	setVideoRTPSinkBufferSize();
	return H264VideoRTPSink::createNew(envir(), rtpGroupsock, 96);
}
#endif

#ifdef MULTICASTENABLE

// -----------------------------------------
//    ServerMediaSubsession for Multicast
// -----------------------------------------
MulticastServerMediaSubsession* MulticastServerMediaSubsession::createNew(UsageEnvironment& env
									, struct in_addr destinationAddress
									, Port rtpPortNum, Port rtcpPortNum
									, int ttl
									, unsigned char rtpPayloadType
									, AnalyserInput* replicator
									, int format) 
{ 
	// Create a source
	FramedSource* source = replicator->videoSource();			
	FramedSource* videoSource = createSource(env, source, format);

	// Create RTP/RTCP groupsock
	Groupsock* rtpGroupsock = new Groupsock(env, destinationAddress, rtpPortNum, ttl);
	Groupsock* rtcpGroupsock = new Groupsock(env, destinationAddress, rtcpPortNum, ttl);

	// Create a RTP sink
	RTPSink* videoSink = createSink(env,rtpGroupsock, rtpPayloadType, format);

	// Create 'RTCP instance'
	const unsigned maxCNAMElen = 100;
	unsigned char CNAME[maxCNAMElen+1];
	gethostname((char*)CNAME, maxCNAMElen);
	CNAME[maxCNAMElen] = '\0'; 
	RTCPInstance* rtcpInstance = RTCPInstance::createNew(env, rtcpGroupsock,  500, CNAME, videoSink, NULL);

	// Start Playing the Sink
	videoSink->startPlaying(*videoSource, NULL, NULL);
	
	return new MulticastServerMediaSubsession(replicator, videoSink, rtcpInstance);
}
		
char const* MulticastServerMediaSubsession::sdpLines() 
{
	if (m_SDPLines.empty())
	{
		// Ugly workaround to give SPS/PPS that are from the RTPSink 
		m_SDPLines.assign(PassiveServerMediaSubsession::sdpLines());
		//m_SDPLines.append(getAuxSDPLine(m_rtpSink,NULL));
	}
	return m_SDPLines.c_str();
}
/*
char const* MulticastServerMediaSubsession::getAuxSDPLine(RTPSink* rtpSink,FramedSource* inputSource)
{
	return this->getAuxLine(dynamic_cast<AnalyserSource*>(m_replicator->videoSource()), rtpSink->rtpPayloadType());
}*/

#endif

#if defined(MP2ENCODING)  || defined(MP1ENCODING)
#include <MPEG1or2VideoRTPSink.hh>
#include <MPEG1or2VideoStreamFramer.hh>
#include <MPEG1or2VideoStreamDiscreteFramer.hh>

AnalysingServerMediaSubsession* AnalysingServerMediaSubsession::createNew(UsageEnvironment& env, AnalyserInput& wisInput, unsigned estimatedBitrate,
	Boolean iFramesOnly,
	double vshPeriod) {
		return new AnalysingServerMediaSubsession(env, wisInput, estimatedBitrate,
			iFramesOnly, vshPeriod);
}

AnalysingServerMediaSubsession
	::AnalysingServerMediaSubsession(UsageEnvironment& env, AnalyserInput& analyserInput,	unsigned estimatedBitrate, Boolean iFramesOnly, double vshPeriod) 
	: OnDemandServerMediaSubsession(env, True /*reuse the first source*/),

	fAnalyserInput(analyserInput), fIFramesOnly(iFramesOnly), fVSHPeriod(vshPeriod) {
		fEstimatedKbps = (estimatedBitrate + 500)/1000;

}

AnalysingServerMediaSubsession
	::~AnalysingServerMediaSubsession() {
}

FramedSource* AnalysingServerMediaSubsession ::createNewStreamSource(unsigned /*clientSessionId*/, unsigned& estBitrate) {
	estBitrate = fEstimatedKbps;

	// Create a framer for the Video Elementary Stream:
	//LOG_MSG("Create Net Stream Source [%d]", estBitrate);

	return MPEG1or2VideoStreamDiscreteFramer::createNew(envir(), fAnalyserInput.videoSource());
}

RTPSink* AnalysingServerMediaSubsession ::createNewRTPSink(Groupsock* rtpGroupsock, unsigned char /*rtpPayloadTypeIfDynamic*/, FramedSource* /*inputSource*/) {
	setVideoRTPSinkBufferSize();
	/*
	struct in_addr destinationAddress;
	destinationAddress.s_addr = inet_addr("239.255.12.42");

	rtpGroupsock->addDestination(destinationAddress,8888);
	rtpGroupsock->multicastSendOnly();
	*/
	// 32 or 33 for MPG2  https://en.wikipedia.org/wiki/RTP_audio_video_profile
	return MPEG1or2VideoRTPSink::createNew(envir(), rtpGroupsock);
}
#endif


