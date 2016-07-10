#include "stdafx.h"
#include "Live_AnalyserInput.h"
#include "Live_AnalyserSource.h"

////////// WISInput implementation //////////

AnalyserInput* AnalyserInput::createNew(UsageEnvironment& env, FFMPEG *Encoder) {
  if (!fHaveInitialized) {
    fHaveInitialized = True;
  }

  return new AnalyserInput(env, Encoder);
}


FramedSource* AnalyserInput::videoSource() {
  if (fOurVideoSource == NULL || AnalyserSource::GetRefCount() == 0) {
    fOurVideoSource = AnalyserSource::createNew(envir(), m_Encoder);
  }
  return fOurVideoSource;
}


AnalyserInput::AnalyserInput(UsageEnvironment& env, FFMPEG *Encoder): Medium(env), m_Encoder(Encoder) {
}

AnalyserInput::~AnalyserInput() {
  /* When we get destroyed, make sure our source is also destroyed.. */
  if (fOurVideoSource != NULL && AnalyserSource::GetRefCount() != 0) {
    AnalyserSource::handleClosure(fOurVideoSource);
  }
}


//global inits, not sure this should be there...

Boolean AnalyserInput::fHaveInitialized = False;
int AnalyserInput::fOurVideoFileNo = -1;
FramedSource* AnalyserInput::fOurVideoSource = NULL;


