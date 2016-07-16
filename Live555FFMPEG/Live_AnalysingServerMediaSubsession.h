#ifndef _ANALYSING_SERVER_MEDIA_SUBSESSION_HH
#define _ANALYSING_SERVER_MEDIA_SUBSESSION_HH

#include <OnDemandServerMediaSubsession.hh>
#include "Live_AnalyserInput.h"

#include <liveMedia.hh>
#include <string>
#include <list> 
#include <iostream>
#include <sstream>

class BaseServerMediaSubsession
{
public:
	static FramedSource* createSource(UsageEnvironment& env, FramedSource * videoES, int format)
	{
		FramedSource* source = NULL;
		
		switch (format)
		{
			case 96 : source = H264VideoStreamDiscreteFramer::createNew(env, videoES); break;
			case 97: source = H265VideoStreamDiscreteFramer::createNew(env, videoES); break;
			case 32: source = MPEG1or2VideoStreamDiscreteFramer::createNew(env, videoES); break;
			case 999: source = MPEG4VideoStreamDiscreteFramer::createNew(env, videoES); break;
		}
		return source;
		
		//return H264VideoStreamDiscreteFramer::createNew(env, videoES);
	}

	static RTPSink* createSink(UsageEnvironment& env, Groupsock * rtpGroupsock, unsigned char rtpPayloadTypeIfDynamic, int format)
	{
		RTPSink* videoSink = NULL;
		
		switch (format)
		{
			case 96 : videoSink = H264VideoRTPSink::createNew(env, rtpGroupsock,rtpPayloadTypeIfDynamic); break;
			case 97: videoSink = H265VideoRTPSink::createNew(env, rtpGroupsock, rtpPayloadTypeIfDynamic); break;
			case 32: videoSink = MPEG1or2VideoRTPSink::createNew(env, rtpGroupsock); break;
			case 999: videoSink = MPEG4ESVideoRTPSink::createNew(env, rtpGroupsock,96); break;
		}
		return videoSink;
		//return videoSink = H264VideoRTPSink::createNew(env, rtpGroupsock,rtpPayloadTypeIfDynamic);
	}
	/*
	char const* getAuxLine(AnalyserSource* source,unsigned char rtpPayloadType)
	{
		const char* auxLine = NULL;
		if (source)
		{
			std::ostringstream os; 
			os << "a=fmtp:" << int(rtpPayloadType) << " ";				
			os << source->getAuxLine();				
			os << "\r\n";				
			auxLine = strdup(os.str().c_str());
		} 
		return auxLine;
	}*/
};


class AnalysingServerMediaSubsession: public OnDemandServerMediaSubsession {

public:
  static AnalysingServerMediaSubsession*
  createNew(UsageEnvironment& env, AnalyserInput& analyserInput, unsigned estimatedBitrate,
	    Boolean iFramesOnly = False,
            double vshPeriod = 5.0
            /* how often (in seconds) to inject a Video_Sequence_Header,
               if one doesn't already appear in the stream */);

protected: // we're a virtual base class
  AnalysingServerMediaSubsession(UsageEnvironment& env, AnalyserInput& AnalyserInput, unsigned estimatedBitrate, Boolean iFramesOnly, double vshPeriod);
  virtual ~AnalysingServerMediaSubsession();

protected:
  AnalyserInput& fAnalyserInput;
  unsigned fEstimatedKbps;

private:
  Boolean fIFramesOnly;
  double fVSHPeriod;

  // redefined virtual functions
  virtual FramedSource* createNewStreamSource(unsigned clientSessionId, unsigned& estBitrate);
  virtual RTPSink* createNewRTPSink(Groupsock* rtpGroupsock, unsigned char rtpPayloadTypeIfDynamic, FramedSource* inputSource);

};


// -----------------------------------------
//    ServerMediaSubsession for Multicast
// -----------------------------------------
class MulticastServerMediaSubsession : public PassiveServerMediaSubsession , public BaseServerMediaSubsession
{
	public:
		static MulticastServerMediaSubsession* createNew(UsageEnvironment& env
									, struct in_addr destinationAddress
									, Port rtpPortNum, Port rtcpPortNum
									, int ttl
									, unsigned char rtpPayloadType
									, AnalyserInput* replicator
									, int format);
		
	protected:
		MulticastServerMediaSubsession(AnalyserInput* replicator, RTPSink* rtpSink, RTCPInstance* rtcpInstance) 
				: PassiveServerMediaSubsession(*rtpSink, rtcpInstance), m_replicator(replicator), m_rtpSink(rtpSink) {};			

		virtual char const* sdpLines() ;
		//virtual char const* getAuxSDPLine(RTPSink* rtpSink,FramedSource* inputSource);
		
	protected:
		AnalyserInput* m_replicator;
		RTPSink* m_rtpSink;
		std::string m_SDPLines;
};

#endif

