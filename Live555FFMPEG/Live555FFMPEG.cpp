// Live555FFMPEG.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "FFMPEGClass.h"

int main(int argc, char* argv[])
{
	//Create Class
	FFMPEG * mFFMPEG = new FFMPEG();
	
	//Setup Encoder
	mFFMPEG->mVideoHeight = 480;
	mFFMPEG->mVideoWidth = 640;
	mFFMPEG->m_AVIMOV_BPS = 1000000;
	
	//Start Encoding and Streaming
	mFFMPEG->Start();

	//Temporary Frame
	unsigned char * TempRGBFrame = new unsigned char[640 * 480 * 3];

	//Main Random Frame Generation loop
	while (1) {
		for (int i = 0; i < 640 * 480 * 3; i++) {
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

