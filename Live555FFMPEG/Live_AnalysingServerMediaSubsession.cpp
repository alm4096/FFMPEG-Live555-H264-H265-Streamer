//==========================================================================
// Definitions.h allows you to select an encoder 
//    - Choose between H264, H265, MP4, and MP2
//    - Choose between multicast and unicast
//==========================================================================

#include "stdafx.h"
#include "Definitions.h"
#include "Live_AnalysingServerMediaSubsession.h"
#include "FFMPEGClass.h"

#include <MPEG4ESVideoRTPSink.hh>
#include <MPEG4VideoStreamFramer.hh>
#include <MPEG4VideoStreamDiscreteFramer.hh>

#include <H265VideoRTPSink.hh>
#include <H265VideoStreamFramer.hh>
#include <H265VideoStreamDiscreteFramer.hh>

#include <H264VideoRTPSink.hh>
#include <H264VideoStreamFramer.hh>
#include <H264VideoStreamDiscreteFramer.hh>

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



FramedSource* AnalysingServerMediaSubsession::createNewStreamSource(unsigned /*clientSessionId*/, unsigned& estBitrate) {
	estBitrate = fEstimatedKbps;
	
	// Create a framer for the Video Elementary Stream:
	//LOG_MSG("Create Net Stream Source [%d]", estBitrate);
	//fAnalyserInput.videoSource()->
	if (fAnalyserInput.GetEncoderType() == AV_CODEC_ID_MPEG4) {
		return MPEG4VideoStreamDiscreteFramer::createNew(envir(), fAnalyserInput.videoSource());
	}
	if (fAnalyserInput.GetEncoderType() == AV_CODEC_ID_MPEG2VIDEO) {
		return MPEG1or2VideoStreamDiscreteFramer::createNew(envir(), fAnalyserInput.videoSource());
	}
	if (fAnalyserInput.GetEncoderType() == AV_CODEC_ID_H265) {
		return H265VideoStreamDiscreteFramer::createNew(envir(), fAnalyserInput.videoSource());
	}
	if (fAnalyserInput.GetEncoderType() == AV_CODEC_ID_H264) {
		return H264VideoStreamDiscreteFramer::createNew(envir(), fAnalyserInput.videoSource());
	}
	/*
#ifdef MPEG4ENCODING
	return MPEG4VideoStreamDiscreteFramer::createNew(envir(), fAnalyserInput.videoSource());
#endif
#ifdef H265ENCODING
	return H265VideoStreamDiscreteFramer::createNew(envir(), fAnalyserInput.videoSource());
#endif
#ifdef H264ENCODING
	return H264VideoStreamDiscreteFramer::createNew(envir(), fAnalyserInput.videoSource());
#endif
#if defined(MP2ENCODING)  || defined(MP1ENCODING)
	return MPEG1or2VideoStreamDiscreteFramer::createNew(envir(), fAnalyserInput.videoSource());
#endif
	*/
}

RTPSink* AnalysingServerMediaSubsession ::createNewRTPSink(Groupsock* rtpGroupsock, unsigned char /*rtpPayloadTypeIfDynamic*/, FramedSource* /*inputSource*/) {
	setVideoRTPSinkBufferSize();

	if (fAnalyserInput.GetEncoderType() == AV_CODEC_ID_MPEG4) {
		return MPEG4ESVideoRTPSink::createNew(envir(), rtpGroupsock, 96);
	}
	if (fAnalyserInput.GetEncoderType() == AV_CODEC_ID_MPEG2VIDEO) {
		return MPEG1or2VideoRTPSink::createNew(envir(), rtpGroupsock);
	}
	if (fAnalyserInput.GetEncoderType() == AV_CODEC_ID_H265) {
		return H265VideoRTPSink::createNew(envir(), rtpGroupsock, 96);
	}
	if (fAnalyserInput.GetEncoderType() == AV_CODEC_ID_H264) {
		return H264VideoRTPSink::createNew(envir(), rtpGroupsock, 96);
	}

	/*
#ifdef MPEG4ENCODING
	return MPEG4ESVideoRTPSink::createNew(envir(), rtpGroupsock, 96);
#endif
#ifdef H265ENCODING
	return H265VideoRTPSink::createNew(envir(), rtpGroupsock, 96);
#endif
#ifdef H264ENCODING
	return H264VideoRTPSink::createNew(envir(), rtpGroupsock, 96);
#endif
#if defined(MP2ENCODING)  || defined(MP1ENCODING)
	return MPEG1or2VideoRTPSink::createNew(envir(), rtpGroupsock);
#endif*/
}

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




