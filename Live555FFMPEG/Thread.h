// *****************************************************************************
//
//     Copyright (c) 2007, Pleora Technologies Inc., All rights reserved.
//
// *****************************************************************************

#pragma once

#include "PvDisplayWnd.h"
#include "PvBuffer.h"
#include "PvDevice.h"

#include "mmsystem.h"
#include <sys/timeb.h>

class CMMTiming
{
public:
	CMMTiming()
   {
		timeBeginPeriod(1);  // sets MM time resolution
     	m_StartTime = timeGetTime();  // picks up the start time
        _ftime64_s( &m_tb );     // and the offset within the current second
   }
   virtual ~CMMTiming()
   {
      timeEndPeriod(1);
   }

   void CMMTiming::GetLocalTime(time_t &timestamp, int &millisec, short &timeZoneBias)
   {
	   DWORD tm = timeGetTime() + m_tb.millitm;

	   if (tm < m_StartTime)
	   {
		   // Wrap around - usually takes 47 days!!
		   m_StartTime = timeGetTime();
		   _ftime64_s( &m_tb );
	   }

     // Coordinated UTC time in seconds since 1970-01-01 00:00
     timestamp = m_tb.time + 
		   ((tm - m_StartTime) / 1000)
		   - (m_tb.timezone * 60);

	 timeZoneBias = 0;

     // Correct for daylight savings
     if (m_tb.dstflag)
       timestamp += 3600;

     // Milliseconds since last second
     millisec = (tm - m_StartTime) % 1000;
   }

   void CMMTiming::GetUTCTime(time_t &timestamp, int &millisec, short &timeZoneBias)
   {
		DWORD tm = timeGetTime() + m_tb.millitm;

		if (tm < m_StartTime)
		{
		   // Wrap around - usually takes 47 days!!
		   m_StartTime = timeGetTime();
		   _ftime64_s( &m_tb );
		}

		// Coordinated UTC time in seconds since 1970-01-01 00:00
		timestamp =  m_tb.time + ((tm - m_StartTime) / 1000);
		timeZoneBias = m_tb.timezone;

		// Correct for daylight savings
		if (m_tb.dstflag)
			timestamp += 3600;

		// Milliseconds since last second
		millisec = (tm - m_StartTime) % 1000;
   }

protected:
	DWORD		m_StartTime;
    __timeb64   m_tb;
};

class PvSimpleUISampleDlg;
class FFMPEG;

#define FMT_FFF		0
#define FMT_BMP		1
#define FMT_RAW     2
#define FMT_SEQ     3

typedef struct {
	BYTE r;
	BYTE g;
	BYTE b;
} RGB_T;

class Thread
{
public:

	Thread(PvSimpleUISampleDlg *aDlg, PvDisplayWnd *aDisplayWnd, PvDevice *aDevice, PvPipeline *aPipeline, int w, int h, int type, int windowing, int l_AVIMOV_BPS, bool bFlipped = false);
	~Thread();

	void Start();
	void Stop();

	void SetPriority( int aPriority );
	int GetPriority() const;

	bool IsDone();
	DWORD GetReturnValue();
	void SetPalette( int aPalette );
	void SaveImage(int fmt);
    void SetSequenceFrames(UINT nFrames) { miSeqFrames = nFrames; }
    bool IsRecording() { return mbSaveAsSEQ; }
	USHORT getSpot() { return (USHORT)m_SpotValue; }
	USHORT getCenterSpot() { return (USHORT)m_CenterSpotValue; }
	USHORT getScaleLo() { return m_ScaleLow; }
	USHORT getScaleHi() { return m_ScaleHigh; }
	void SetManualAdjust( bool bManual ) { m_bManualAdjust = bManual; }
	void AdjustOnce() { m_bAdjustOnce = true; }
	void setScaleLo(USHORT lo) { m_ScaleLow = lo; }
	void setScaleHi(USHORT hi) { m_ScaleHigh = hi; }
	DWORD getTrigCount(int port) { return port == 1 ? m_TrigCount1 : m_TrigCount2; }
	DWORD getLineState(int port) { return port == 1 ? m_LineState1 : m_LineState2; }
	void SetIRFormat( int fmt );
	void SetArea(CPoint ul, CSize sz) { m_AreaUL = ul; m_AreaSize = sz; }
	void setSpot(CPoint cl); // Client coordinates
    void setAreaActive(bool f) { m_bAreaActive = f; }
	USHORT getAreaAvg() { return m_AreaAvg;}
	USHORT getAreaMin() { return m_AreaMin;}
	USHORT getAreaMax() { return m_AreaMax;}
	CSize  getAreaSize() { return m_AreaSize; }
	CPoint getAreaUL() { return m_AreaUL; }
    void Log(double sensor1, double sensor2, double spot);
	double getFPS();
    DWORD getFrameCount() { return m_FrameCount; }
    void resetStats();

	CString m_SnapshotPath;

protected:

	static unsigned long WINAPI Link( void *aParam );
	virtual DWORD Function();

	bool IsStopping() const;

private:
	FFMPEG * mFFMPEG;
	void  CalcArea(uint16_t* pBuf);
	void  SaveImage(PvBuffer *pBuf);
    void  SaveSEQ(PvBuffer *pBuf);
    void  SaveTauSEQ(PvBuffer *pBuf);
    void  SaveTAUImage(PvBuffer *pBuf);
	void  SaveAsBitmap(PvBuffer *pBuf);
    void  SaveAsRaw(PvBuffer *pBuf);
    void  DrawArea(PvBuffer& Buf);
    void  ShowTSens(PvBuffer *pBuf);
	
	int m_AVIMOV_BPS;

	HANDLE mHandle;
	DWORD mID;

	bool mStop;
	bool mbSaveImage;
	bool mbSaveAsBMP;
    bool mbSaveAsRAW;
    bool mbSaveAsSEQ;
    int  miCurrentSEQImage;
    int  miSeqFrames;
   	CString m_SEQFileName;
    CFile m_SEQFile;
    CMMTiming mm_timer;
    time_t m_ts;
    int m_ms;
    short m_tzBias;

	DWORD mReturnValue;

	PvBuffer mDispBuffer;
	// We do not own these objects
	PvSimpleUISampleDlg *mMainDlg;
	PvDisplayWnd *mDisplayWnd;
    PvDevice *mDevice;
    PvPipeline * mPipeline;

	RGB_T mPalette[128];
	int mHeight;
	int mWidth;
 	int m_CamType;
    bool mbFlipped;

	USHORT  m_MinLimit;
	USHORT  m_MaxLimit;

	ULONG	mLevel;
	ULONG	mSpan;

	CPoint	m_AreaUL; // Area upper left
	CSize	m_AreaSize; // Area width/height
	USHORT  m_AreaAvg;
	USHORT  m_AreaMin;
	USHORT  m_AreaMax;
    bool    m_bAreaActive;

	CPoint  m_SpotPT; // Spot position

	DWORD   m_CenterSpotValue;
	DWORD	m_SpotValue;
	USHORT  m_ScaleLow;
	USHORT  m_ScaleHigh;
	bool 	m_bManualAdjust;
	bool    m_bAdjustOnce;
	DWORD   m_LineState1;
    DWORD   m_LineState2;
	DWORD   m_TrigCount1;
	DWORD   m_TrigCount2;
    DWORD   m_TrigCount;
	int		m_IRFormat;
    DWORD   m_StartTime;
    DWORD   m_FrameCount;

    CString m_logFile;
};
