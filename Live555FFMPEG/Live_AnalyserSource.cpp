/**********
This library is free software; you can redistribute it and/or modify it under
the terms of the GNU Lesser General Public License as published by the
Free Software Foundation; either version 2.1 of the License, or (at your
option) any later version. (See <http://www.gnu.org/copyleft/lesser.html>.)

This library is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for
more details.

You should have received a copy of the GNU Lesser General Public License
along with this library; if not, write to the Free Software Foundation, Inc.,
51 Franklin Street, Fifth Floor, Boston, MA 02110-1301  USA
**********/
// "liveMedia"
// Copyright (c) 1996-2011 Live Networks, Inc.  All rights reserved.
// A template for a MediaSource encapsulating an audio/video input device
//
// NOTE: Sections of this code labeled "%%% TO BE WRITTEN %%%" are incomplete, and need to be written by the programmer
// (depending on the features of the particular device).
// Implementation

#include "stdafx.h"
#include "Live_AnalyserSource.h"
#include <GroupsockHelper.hh> // for "gettimeofday()"
#include "FFMPEGClass.h"
#include <atltrace.h>
//#define TRACE ATLTRACE
#ifdef _DEBUG
//#include "DebugHeader.h"
#endif
AnalyserSource* AnalyserSource::createNew(UsageEnvironment& env, FFMPEG * E_Source) {
  return new AnalyserSource(env, E_Source);
}


EventTriggerId AnalyserSource::eventTriggerId = 0;

unsigned AnalyserSource::referenceCount = 0;

AnalyserSource::AnalyserSource(UsageEnvironment& env, FFMPEG * E_Source) : FramedSource(env), Encoding_Source(E_Source) {
  if (referenceCount == 0) {
    // Any global initialization of the device would be done here:
    
  }
  ++referenceCount;

  // Any instance-specific initialization of the device would be done here:
  Last_Sent_Frame_ID = 0;

  /* register us with the Encoding thread so we'll get notices when new frame data turns up.. */
  Encoding_Source->RegisterRTSP_Source(&(env.taskScheduler()), this);

  // We arrange here for our "deliverFrame" member function to be called
  // whenever the next frame of data becomes available from the device.
  //
  // If the device can be accessed as a readable socket, then one easy way to do this is using a call to
  //     envir().taskScheduler().turnOnBackgroundReadHandling( ... )
  // (See examples of this call in the "liveMedia" directory.)
  //
  // If, however, the device *cannot* be accessed as a readable socket, then instead we can implement is using 'event triggers':
  // Create an 'event trigger' for this device (if it hasn't already been done):
  if (eventTriggerId == 0) {
    eventTriggerId = envir().taskScheduler().createEventTrigger(deliverFrame0);
  }
}

AnalyserSource::~AnalyserSource() {
  // Any instance-specific 'destruction' (i.e., resetting) of the device would be done here:
  
  /* de-register this source from the Encoding thread, since we no longer need notices.. */
  Encoding_Source->Un_RegisterRTSP_Source(this);

  --referenceCount;
  if (referenceCount == 0) {
    // Any global 'destruction' (i.e., resetting) of the device would be done here:
    
    // Reclaim our 'event trigger'
    envir().taskScheduler().deleteEventTrigger(eventTriggerId);
    eventTriggerId = 0;
  }
  
}

unsigned AnalyserSource::GetRefCount() {
  return referenceCount;
}

void AnalyserSource::doGetNextFrame() {
  // This function is called (by our 'downstream' object) when it asks for new data.
  //LOG_MSG("Do Next Frame..");
  // Note: If, for some reason, the source device stops being readable (e.g., it gets closed), then you do the following:
  //if (0 /* the source stops being readable */ /*%%% TO BE WRITTEN %%%*/) {
  unsigned int FrameID = Encoding_Source->GetFrameID();
  //TRACE("FrameID %d\n",FrameID);
  if (FrameID == 0){
    //LOG_MSG("No Data. Close");
#ifdef Live555Debug
	TRACE("CLOSING!!!!\n");  
#endif
	Encoding_Source->ReleaseFrame();
    handleClosure(this);
    return;
  }



  // If a new frame of data is immediately available to be delivered, then do this now:
  if (Last_Sent_Frame_ID != FrameID){
    deliverFrame();
	//TRACE("LIVE555 Frame ID Same\n");  
	//DEBUG_MSG("Frame ID: %d",FrameID);
  }

  // No new data is immediately available to be delivered.  We don't do anything more here.
  // Instead, our event trigger must be called (e.g., from a separate thread) when new data becomes available.
}

void AnalyserSource::deliverFrame0(void* clientData) {
  ((AnalyserSource*)clientData)->deliverFrame();
}

void AnalyserSource::deliverFrame() {
  // This function is called when new frame data is available from the device.
  // We deliver this data by copying it to the 'downstream' object, using the following parameters (class members):
  // 'in' parameters (these should *not* be modified by this function):
  //     fTo: The frame data is copied to this address.
  //         (Note that the variable "fTo" is *not* modified.  Instead,
  //          the frame data is copied to the address pointed to by "fTo".)
  //     fMaxSize: This is the maximum number of bytes that can be copied
  //         (If the actual frame is larger than this, then it should
  //          be truncated, and "fNumTruncatedBytes" set accordingly.)
  // 'out' parameters (these are modified by this function):
  //     fFrameSize: Should be set to the delivered frame size (<= fMaxSize).
  //     fNumTruncatedBytes: Should be set iff the delivered frame would have been
  //         bigger than "fMaxSize", in which case it's set to the number of bytes
  //         that have been omitted.
  //     fPresentationTime: Should be set to the frame's presentation time
  //         (seconds, microseconds).  This time must be aligned with 'wall-clock time' - i.e., the time that you would get
  //         by calling "gettimeofday()".
  //     fDurationInMicroseconds: Should be set to the frame's duration, if known.
  //         If, however, the device is a 'live source' (e.g., encoded from a camera or microphone), then we probably don't need
  //         to set this variable, because - in this case - data will never arrive 'early'.
  // Note the code below.

  if (!isCurrentlyAwaitingData()) {
#ifdef Live555Debug
	  TRACE("LIVE555 Not Ready for Data\n");  
#endif
	  return; // we're not ready for the data yet
  }


  static u_int8_t* newFrameDataStart;
  static unsigned newFrameSize = 0;

  /* get the data frame from the Encoding thread.. */
  if (Encoding_Source->GetFrame(&newFrameDataStart, &newFrameSize, &Last_Sent_Frame_ID)){
    if ((newFrameDataStart!=nullptr) && (newFrameSize!=0)) {
		/* This should never happen, but check anyway.. */
		if (newFrameSize > fMaxSize) {
		  fFrameSize = fMaxSize;
		  fNumTruncatedBytes = newFrameSize - fMaxSize;
		  __debugbreak();
		} else {
		  fFrameSize = newFrameSize;
		}
		gettimeofday(&fPresentationTime, NULL); // If you have a more accurate time - e.g., from an encoder - then use that instead.
		//fDurationInMicroseconds
		// If the device is *not* a 'live source' (e.g., it comes instead from a file or buffer), then set "fDurationInMicroseconds" here.
		/* move the data to be sent off.. */
		memmove(fTo, newFrameDataStart, fFrameSize);

		/* release the Mutex we had on the Frame's buffer.. */
		Encoding_Source->ReleaseFrame();
	}
	else {
		//AM Added, something bad happened
		//ALTRACE("LIVE555: FRAME NULL\n");
		//Encoding_Source->ReleaseFrame();
#ifdef Live555Debug
		TRACE("LIVE555 No Frame Data\n");  
#endif
		return;

		//fFrameSize=0;
		//fTo=NULL;
		//__debugbreak();
		//TRACE("CLOSING!!!!\n");  
		//handleClosure(this);
		
	}
  }
  else {
    //LOG_MSG("Closing Connection due to Frame Error..");
#ifdef Live555Debug
	  TRACE("CLOSING!!!!\n");  
#endif
	  Encoding_Source->ReleaseFrame();
	  __debugbreak();
	  handleClosure(this);
  }

  
  // After delivering the data, inform the reader that it is now available:
  FramedSource::afterGetting(this);
}


// The following code would be called to signal that a new frame of data has become available.
// This (unlike other "LIVE555 Streaming Media" library code) may be called from a separate thread.
#if 0 /* This has been written into the encoding thread and is auto called after a source is registered with the encoding thread. */
void signalNewAnalyserFrameData() {
  TaskScheduler* ourScheduler = NULL; //%%% TO BE WRITTEN %%%
  AnalyserSource* ourDevice  = NULL; //%%% TO BE WRITTEN %%%

  if (ourScheduler != NULL) { // sanity check
    ourScheduler->triggerEvent(AnalyserSource::eventTriggerId, ourDevice);
  }
}
#endif
