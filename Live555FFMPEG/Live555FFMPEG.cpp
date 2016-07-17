//==========================================================================
// Live555FFMPEG.cpp : Defines the entry point for the console application.
//==========================================================================
// Main Example for Live555 and FFMPEG
//
//==========================================================================

#include "stdafx.h"
#include "FFMPEGClass.h"
#include "libavcodec\avcodec.h"

int main(int argc, char* argv[])
{
	//Some local variables
	int mVidWidth = 640;
	int mVidHeight = 480;

	//Create Class
	FFMPEG * mFFMPEG = new FFMPEG();
	
	//Setup Encoder
	mFFMPEG->SetVideoResolution(mVidWidth, mVidHeight);	//Set Frame Resolution
	mFFMPEG->m_AVIMOV_BPS = 5000000;		//Set Bitrate (this is extremely important to get correct)

	/*
	//--------------------
	//Optional settings -- Uncomment to use
	//--------------------
	mFFMPEG->SetRTSPPort(8554);
	mFFMPEG->SetRTSPUserandPassword("admin", "admin");
	mFFMPEG->SetRTSPAddress("multicast");
	mFFMPEG->SetRTSPDescription("This is the stream description");
	mFFMPEG->SetRtpPortNum(20000);

	//Encoder options
	mFFMPEG->SetEncoder(AV_CODEC_ID_H264);
	//Other encoders
	// AV_CODEC_ID_H265
	// AV_CODEC_ID_H264
	// AV_CODEC_ID_MPEG4
	// AV_CODEC_ID_MPEG2VIDEO

	mFFMPEG->SetMulticast();
	mFFMPEG->SetUnicast();
	//--------------------
	*/

	//Start Encoding and Streaming
	mFFMPEG->Start();

	//Temporary Frame
	unsigned char * TempRGBFrame = new unsigned char[mVidWidth * mVidHeight * 3];

	//Main Random Frame Generation loop
	while (1) {
		for (int i = 0; i < mVidWidth * mVidHeight * 3; i++) {
			TempRGBFrame[i] = rand() % 256;
		}
		mFFMPEG->SendNewFrame((char*)TempRGBFrame);
		Sleep(1);
	}

	//Clean up
	mFFMPEG->Stop();
	while (!mFFMPEG->IsDone()) {
		Sleep(1);
	}
	delete mFFMPEG;
	delete[] TempRGBFrame;
	return 0;
}

