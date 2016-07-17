#ifndef _ANALYSER_INPUT_HH
#define _ANALYSER_INPUT_HH

#include <MediaSink.hh>
#include "FFMPEGClass.h"


class AnalyserInput: public Medium {
public:
  static AnalyserInput* createNew(UsageEnvironment& env, FFMPEG *Encoder);

  FramedSource* videoSource();
  int GetEncoderType();

private:
  AnalyserInput(UsageEnvironment& env, FFMPEG *Encoder); // called only by createNew()
  virtual ~AnalyserInput();

private:
  friend class WISVideoOpenFileSource;
  static Boolean fHaveInitialized;
  static int fOurVideoFileNo;
  static FramedSource* fOurVideoSource;
  FFMPEG *m_Encoder;
};

// Functions to set the optimal buffer size for RTP sink objects.
// These should be called before each RTPSink is created.
#define VIDEO_MAX_FRAME_SIZE 300000
inline void setVideoRTPSinkBufferSize() { OutPacketBuffer::maxSize = VIDEO_MAX_FRAME_SIZE; }

#endif