#pragma once

#include "Live555UsageEnv.h"

#include "FFMPEGClass.h"


class Live555Class
{
public:

	Live555Class( FFMPEG * a_Encoder );
	~Live555Class();

	void Start();
	void Stop();

	void SetPriority( int aPriority );
	int GetPriority() const;

	bool IsDone();
	DWORD GetReturnValue();


protected:

	static unsigned long WINAPI Link( void *aParam );
	virtual DWORD Function();
	bool IsStopping() const;

private:
	void SetDefaultValues();
	int RTSPPort;
	char RTSPUser[100];
	char RTSPPass[100];


	DWORD LiveSingleStart();
	HANDLE mHandle;
	DWORD mID;
	bool mStop;
	DWORD mReturnValue;	
	FFMPEG * m_Encoder;
	char Stop_RTSP_Loop;
};
