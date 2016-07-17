#pragma once

#include "Live555UsageEnv.h"
#include "FFMPEGClass.h"
#include <string.h>

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

	void SetRTSPPort(int PortNo) {
		RTSPPort = PortNo;
	}

	void SetRTSPUserandPassword(char * Username, char * Password) {
		strcpy_s(RTSPUser, Username);
		strcpy_s(RTSPPass, Password);
	}

	void SetRTSPAddress(char * streamname) {
		strcpy_s(RTSP_Address, streamname);
	}

	void SetRTSPDescription(char * streamdescription) {
		strcpy_s(RTSP_Description, streamdescription);
	}

	void SetEncoder(int m_enc) {
		m_selected_encoder = m_enc;
	}
	void SetMulticast(void) {
		m_multicast = 1;
	}
	void SetUnicast(void) {
		m_multicast = 0;
	}
	void SetRtpPortNum(int m_rtpnum) {
		m_rtpPortNum = m_rtpnum;
	}
protected:

	static unsigned long WINAPI Link( void *aParam );
	virtual DWORD Function();
	bool IsStopping() const;

private:
	void SetDefaultValues();
	int RTSPPort;
	char RTSPUser[100];
	char RTSPPass[100];
	char RTSP_Address[1024];
	char RTSP_Description[1024];

	int m_selected_encoder;
	int m_multicast;
	int m_rtpPortNum;

	DWORD LiveSingleStart();
	HANDLE mHandle;
	DWORD mID;
	bool mStop;
	DWORD mReturnValue;	
	FFMPEG * m_Encoder;
	char Stop_RTSP_Loop;
};
