// *****************************************************************************
//
//     Copyright (c) 2007, Pleora Technologies Inc., All rights reserved.
//
// *****************************************************************************

#include "stdafx.h"

#include "Thread.h"
#include "PvSimpleUISampleDlg.h"
#include "fff.h"
#include "PvBufferWriter.h"
#include "FFMPEGClass.h"
#include <Math.h>

static RGB_T iron[128] =  {
				{0,   0,  0},
				{0,   0,  0},
				{0,   0,  36},
				{0,   0,  51},
				{0,   0,  66},
				{0,   0,  81},
				{2,   0,  90},
				{4,   0,  99},
				{7,   0, 106},
				{11,   0, 115},
				{14,   0, 119},
				{20,   0, 123},
				{27,   0, 128},
				{33,   0, 133},
				{41,   0, 137},
				{48,   0, 140},
				{55,   0, 143},
				{61,   0, 146},
				{66,   0, 149},
				{72,   0, 150},
				{78,   0, 151},
				{84,   0, 152},
				{91,   0, 153},
				{97,   0, 155},
				{104,   0, 155},
				{110,   0, 156},
				{115,   0, 157},
				{122,   0, 157},
				{128,   0, 157},
				{134,   0, 157},
				{139,   0, 157},
				{146,   0, 156},
				{152,   0, 155},
				{157,   0, 155},
				{162,   0, 155},
				{167,   0, 154},
				{171,   0, 153},
				{175,   1, 152},
				{178,   1, 151},
				{182,   2, 149},
				{185,   4, 149},
				{188,   5, 147},
				{191,   6, 146},
				{193,   8, 144},
				{195,  11, 142},
				{198,  13, 139},
				{201,  17, 135},
				{203,  20, 132},
				{206,  23, 127},
				{208,  26, 121},
				{210,  29, 116},
				{212,  33, 111},
				{214,  37, 103},
				{217,  41,  97},
				{219,  46,  89},
				{221,  49,  78},
				{223,  53,  66},
				{224,  56,  54},
				{226,  60,  42},
				{228,  64,  30},
				{229,  68,  25},
				{231,  72,  20},
				{232,  76,  16},
				{234,  78,  12},
				{235,  82,  10},
				{236,  86,   8},
				{237,  90,   7},
				{238,  93,   5},
				{239,  96,   4},
				{240, 100,   3},
				{241, 103,   3},
				{241, 106,   2},
				{242, 109,   1},
				{243, 113,   1},
				{244, 116,   0},
				{244, 120,   0},
				{245, 125,   0},
				{246, 129,   0},
				{247, 133,   0},
				{248, 136,   0},
				{248, 139,   0},
				{249, 142,   0},
				{249, 145,   0},
				{250, 149,   0},
				{251, 154,   0},
				{252, 159,   0},
				{253, 163,   0},
				{253, 168,   0},
				{253, 172,   0},
				{254, 176,   0},
				{254, 179,   0},
				{254, 184,   0},
				{254, 187,   0},
				{254, 191,   0},
				{254, 195,   0},
				{254, 199,   0},
				{254, 202,   1},
				{254, 205,   2},
				{254, 208,   5},
				{254, 212,   9},
				{254, 216,  12},
				{255, 219,  15},
				{255, 221,  23},
				{255, 224,  32},
				{255, 227,  39},
				{255, 229,  50},
				{255, 232,  63},
				{255, 235,  75},
				{255, 238,  88},
				{255, 239, 102},
				{255, 241, 116},
				{255, 242, 134},
				{255, 244, 149},
				{255, 245, 164},
				{255, 247, 179},
				{255, 248, 192},
				{255, 249, 203},
				{255, 251, 216},
				{255, 253, 228},
				{255, 254, 239},
				{255, 255, 249},
				{255, 255, 249},
				{255, 255, 249},
				{255, 255, 249},
				{255, 255, 249},
				{255, 255, 249},
				{255, 255, 249},
				{255, 255, 249}
};

static RGB_T rain[128] =  {
			{0,   0,   0},
			{0,   0,   0},
			{15,   0,  15},
			{31,   0,  31},
			{47,   0,  47},
			{63,   0,  63},
			{79,   0,  79},
			{95,   0,  95},
			{111,   0, 111},
			{127,   0, 127},
			{143,   0, 143},
			{159,   0, 159},
			{175,   0, 175},
			{191,   0, 191},
			{207,   0, 207},
			{223,   0, 223},
			{239,   0, 239},
			{255,   0, 255},
			{239,   0, 250},
			{223,   0, 245},
			{207,   0, 240},
			{191,   0, 236},
			{175,   0, 231},
			{159,   0, 226},
			{143,   0, 222},
			{127,   0, 217},
			{111,   0, 212},
			{95,   0, 208},
			{79,   0, 203},
			{63,   0, 198},
			{47,   0, 194},
			{31,   0, 189},
			{15,   0, 184},
			{0,   0, 180},
			{0,  15, 184},
			{0,  31, 189},
			{0,  47, 194},
			{0,  63, 198},
			{0,  79, 203},
			{0,  95, 208},
			{0, 111, 212},
			{0, 127, 217},
			{0, 143, 222},
			{0, 159, 226},
			{0, 175, 231},
			{0, 191, 236},
			{0, 207, 240},
			{0, 223, 245},
			{0, 239, 250},
			{0, 255, 255},
			{0, 245, 239},
			{0, 236, 223},
			{0, 227, 207},
			{0, 218, 191},
			{0, 209, 175},
			{0, 200, 159},
			{0, 191, 143},
			{0, 182, 127},
			{0, 173, 111},
			{0, 164,  95},
			{0, 155,  79},
			{0, 146,  63},
			{0, 137,  47},
			{0, 128,  31},
			{0, 119,  15},
			{0, 110,   0},
			{15, 118,   0},
			{30, 127,   0},
			{45, 135,   0},
			{60, 144,   0},
			{75, 152,   0},
			{90, 161,   0},
			{105, 169,  0},
			{120, 178,  0},
			{135, 186,  0},
			{150, 195,  0},
			{165, 203,  0},
			{180, 212,  0},
			{195, 220,  0},
			{210, 229,  0},
			{225, 237,  0},
			{240, 246,  0},
			{255, 255,  0},
			{251, 240,  0},
			{248, 225,  0},
			{245, 210,  0},
			{242, 195,  0},
			{238, 180,  0},
			{235, 165,  0},
			{232, 150,  0},
			{229, 135,  0},
			{225, 120,  0},
			{222, 105,  0},
			{219,  90,  0},
			{216,  75,  0},
			{212,  60,  0},
			{209,  45,  0},
			{206,  30,  0},
			{203,  15,  0},
			{200,   0,  0},
			{202,  11,  11},
			{205,  23,  23},
			{207,  34,  34},
			{210,  46,  46},
			{212,  57,  57},
			{215,  69,  69},
			{217,  81,  81},
			{220,  92,  92},
			{222, 104, 104},
			{225, 115, 115},
			{227, 127, 127},
			{230, 139, 139},
			{232, 150, 150},
			{235, 162, 162},
			{237, 173, 173},
			{240, 185, 185},
			{242, 197, 197},
			{245, 208, 208},
			{247, 220, 220},
			{250, 231, 231},
			{252, 243, 243},
			{252, 243, 243},
			{252, 243, 243},
			{252, 243, 243},
			{252, 243, 243},
			{252, 243, 243},
			{252, 243, 243},
			{252, 243, 243}
};

// ==========================================================================
Thread::Thread(PvSimpleUISampleDlg *aDlg, PvDisplayWnd *aDisplayWnd, PvDevice *aDevice, PvPipeline *aPipeline, int w, int h, int type, int windowing, int l_AVIMOV_BPS, bool bFlipped)
    : mMainDlg( aDlg )
    , mDisplayWnd( aDisplayWnd )
    , mDevice( aDevice )
    , mPipeline( aPipeline )
	, mHandle( 0 )
	, mID( 0 )
	, mStop( false )
	, mbSaveImage( false )
	, mbSaveAsBMP( false )
    , mbSaveAsRAW( false)
    , mbSaveAsSEQ( false )
	, mReturnValue( 0 )
    , mbFlipped ( bFlipped )
{
	m_AVIMOV_BPS = l_AVIMOV_BPS;

	SYSTEMTIME st;
	int i;
	SetPalette(1);

	uint32_t width = (uint32_t)w;
	uint32_t height = (uint32_t)h;
	m_CamType = type;

    if (m_CamType == CAM_TYPE_A3XX) {
		height = 240;
    }
	else if (m_CamType == CAM_TYPE_A6XX) {
		height = 480;
        if (windowing == 1)
    		height = 240;
        else if (windowing == 2)
    		height = 120;
    }
	else if (m_CamType == CAM_TYPE_T1K) {
		height = 768;
    }

	mHeight = height;
	mWidth = width;

	PvImage* pImage = mDispBuffer.GetImage();
	pImage->Alloc(width, height, PvPixelRGB8Packed);

	SetArea(CPoint(width/2 - 5, height/2 - 5), CSize(10, 10));
	m_AreaAvg = 0;
    m_bAreaActive = true;
	SetIRFormat(0); // Signal mode is default

	m_SpotPT.x = mWidth / 2;
	m_SpotPT.y = mHeight / 2;

	m_ScaleLow = 0;
	m_ScaleHigh = 0;
	m_bManualAdjust = false;
	m_bAdjustOnce = false;
	m_TrigCount1 = 0;
	m_TrigCount2 = 0;
   	m_LineState1 = 0;
    m_LineState2 = 0;
	m_TrigCount = 0; // Total trig count
    m_StartTime = 0;

    miCurrentSEQImage = 0;
    miSeqFrames = 10;

    // Create name of log file
	GetLocalTime(&st);
    m_logFile.Format(_T("Sensor_%04d%02d%02d_%02d%02d%02d.log"), st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);
}

// ==========================================================================
Thread::~Thread()
{
	if ( mHandle != INVALID_HANDLE_VALUE )
	{
		Stop();
	}
}

// ==========================================================================
void Thread::Start()
{
	mHandle = ::CreateThread(
		NULL,				// Security attributes
		0,					// Stack size, 0 is default
		Link,				// Start address
		this,				// Parameter
		0,					// Creation flags
		&mID );				// Thread ID
}

// ==========================================================================
void Thread::Stop()
{
	ASSERT( mHandle != INVALID_HANDLE_VALUE );

	mStop = true;

    // Stop stream thread
	if ( mPipeline != NULL )
	{
        if ( mPipeline->IsStarted() )
        {
		    mPipeline->Stop();
        }
    }

	DWORD lRetVal = ::WaitForSingleObject( mHandle, INFINITE );
	ASSERT( lRetVal != WAIT_TIMEOUT  );

	::CloseHandle( mHandle );
	mHandle = INVALID_HANDLE_VALUE;

	mID = 0;
}
// ==========================================================================
void Thread::SaveImage(int fmt)
{
	if (fmt == FMT_FFF)
		mbSaveImage = true;
	else if (fmt == FMT_BMP)
		mbSaveAsBMP = true;
	else if (fmt == FMT_RAW)
		mbSaveAsRAW = true;
    else if (fmt == FMT_SEQ) {
        miCurrentSEQImage = 0;
		mbSaveAsSEQ = true;
    }
}
// ==========================================================================
void Thread::SetPalette( int aPalette )
{
	int i;
	for (i = 0; i < 128; i++) {
		if (aPalette == 0) {
			mPalette[i].r = i;
			mPalette[i].g = i;
			mPalette[i].b = i;
		}
		else if (aPalette == 1) {
			mPalette[i].r = iron[i].r;
			mPalette[i].g = iron[i].g;
			mPalette[i].b = iron[i].b;
		}
		else if (aPalette == 2) {
			mPalette[i].r = rain[i].r;
			mPalette[i].g = rain[i].g;
			mPalette[i].b = rain[i].b;
		}
	}
}

// ==========================================================================
void Thread::SetPriority( int aPriority )
{
	ASSERT( mHandle != INVALID_HANDLE_VALUE );
	::SetThreadPriority( mHandle, aPriority );
}

// ==========================================================================
int Thread::GetPriority() const
{
	ASSERT( mHandle != INVALID_HANDLE_VALUE );
	return ::GetThreadPriority( mHandle );
}

// ==========================================================================
bool Thread::IsStopping() const
{
	ASSERT( mHandle != INVALID_HANDLE_VALUE );
	return mStop;
}

// ==========================================================================
bool Thread::IsDone()
{
	if ( ( mHandle == INVALID_HANDLE_VALUE ) ||
		 ( mID == 0 ) )
	{
		return true;
	}

	return ( ::WaitForSingleObject( mHandle, 0 ) == WAIT_OBJECT_0 );
}

void Thread::SaveAsBitmap(PvBuffer *pBuf)
{
	PvResult lResult;
	PvBufferWriter bufferWriter;

	CString capFile = m_SnapshotPath + _T("\\GEVDemo");
	::CreateDirectory(capFile, NULL);
	COleDateTime dt = COleDateTime::GetCurrentTime();
	// Construct a unique file name
	CString fileName;
	fileName.Format(_T("%d%02d%02d_%02d%02d%02d.bmp"),
		dt.GetYear(), dt.GetMonth(), dt.GetDay(),
		dt.GetHour(), dt.GetMinute(), dt.GetSecond());
	capFile += _T("\\") + fileName;

	lResult = bufferWriter.Store(pBuf, PvString((LPCTSTR)capFile));
	if ( lResult.IsFailure() )
    {
		AfxMessageBox((LPCTSTR)lResult.GetCodeString());
	}
}

void Thread::Log(double sensor1, double sensor2, double spot)
{
	SYSTEMTIME st;

	CString logFile = m_SnapshotPath + _T("\\GEVDemo");
	::CreateDirectory(logFile, NULL);

	// Construct file name
	logFile += _T("\\") + m_logFile;
	GetLocalTime(&st);

	CFile f;
	if (f.Open(logFile, CFile::modeCreate | CFile::modeNoTruncate | CFile::modeReadWrite | CFile::shareDenyNone))
	{
		char str[80];
        sprintf_s(str, 80, "%04d-%02d-%02d %02d:%02d:%02d %.2f %.2f %.2f\r\n", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond, sensor1, sensor2, spot);
		f.SeekToEnd();
		f.Write(str, (UINT)strlen(str));
		f.Close();
	}
}

// ==========================================================================
void Thread::SaveAsRaw(PvBuffer *pBuf)
{
	uint8_t* pSrc;
	CString capFile = m_SnapshotPath + _T("\\GEVDemo");
	::CreateDirectory(capFile, NULL);
	COleDateTime dt = COleDateTime::GetCurrentTime();
    CFile file;
    CFileException e;          
	// Construct a unique file name
	CString fileName;

	fileName.Format(_T("%d%02d%02d_%02d%02d%02d.raw"),
		dt.GetYear(), dt.GetMonth(), dt.GetDay(),
		dt.GetHour(), dt.GetMinute(), dt.GetSecond());
	capFile += _T("\\") + fileName;

    // Try to create new file
    if (!file.Open(capFile, CFile::modeCreate |
                          CFile::modeReadWrite |
                          CFile::typeBinary, &e)) {
		return;
    }

    try 
    {
        // Write to file
        UINT sz = mHeight * mWidth * 2;
    	pSrc = (uint8_t*)pBuf->GetDataPointer();
		file.Write(pSrc, sz);
    }
    catch (CFileException* e) 
    {
        if (e->m_cause == CFileException::diskFull)
        {
            try {
                file.Close();
                file.Remove(capFile);
            }
            catch (CFileException*) {}
        }
    }
}

// ==========================================================================
void Thread::ShowTSens(PvBuffer *pBuf)
{
	int nPixelSize = mHeight * mWidth * 2;
	FLIRFILEHEAD* pHeader;
	FLIRFILEINDEX* pIndex;
	uint8_t* pSrc;
	DWORD dwNumUsedIndex;
    int i;

    pSrc = (uint8_t*)pBuf->GetDataPointer();

	// Pointer to FFF header
    pHeader = (FLIRFILEHEAD *)&pSrc[nPixelSize + sizeof(FPGA_HEADER)];
	// Pointer to start of index table
	pIndex = (FLIRFILEINDEX *)&pSrc[nPixelSize + sizeof(FPGA_HEADER) + sizeof(FLIRFILEHEAD)];

    dwNumUsedIndex = pHeader->dwNumUsedIndex;

    if (dwNumUsedIndex > 2)
        return;

	for (i = 0; i < (int)dwNumUsedIndex; i++) 
	{
		if (pIndex[i].wMainType == FFF_TAGID_BasicData) 
		{
			PBYTE pData;
			BI_DATA_T* pBI;
            int j;

            pData = (PBYTE)&pIndex[dwNumUsedIndex];
			pBI = (BI_DATA_T *)pData;
            for (j = 0; j < 20; j++) {
                if (pBI->ExtendedImageInfo.tsData[j].fTSTemp > 100.0f)
                    TRACE("TS: %s = %.4f\r\n", pBI->ExtendedImageInfo.tsData[j].pzTSName, pBI->ExtendedImageInfo.tsData[j].fTSTemp);
            }
		}
    }
}

// ==========================================================================
void Thread::SaveImage(PvBuffer *pBuf)
{
	FPGA_HEADER* pFPGA;
	FLIRFILEHEAD* pHeader;
	FLIRFILEINDEX* pIndex;
	uint8_t* pSrc;
    CFile file;
    CFileException e;          
	int nPixelSize = mHeight * mWidth * 2;
	int i;
	WORD trgflags = 0;
	GEOMETRIC_INFO_T geom;
	DWORD dwNumUsedIndex;

	pSrc = (uint8_t*)pBuf->GetDataPointer();
	pFPGA = (FPGA_HEADER*)&pSrc[nPixelSize];

	CString capFile = m_SnapshotPath + _T("\\GEVDemo");
	::CreateDirectory(capFile, NULL);
	COleDateTime dt = COleDateTime::GetCurrentTime();
	// Construct a unique file name
	CString fileName;
	fileName.Format(_T("%d%02d%02d_%02d%02d%02d.img"),
		dt.GetYear(), dt.GetMonth(), dt.GetDay(),
		dt.GetHour(), dt.GetMinute(), dt.GetSecond());
	capFile += _T("\\") + fileName;

	// Pointer to FFF header
    pHeader = (FLIRFILEHEAD *)&pSrc[nPixelSize + sizeof(FPGA_HEADER)];
	// Pointer to start of index table
	pIndex = (FLIRFILEINDEX *)&pSrc[nPixelSize + sizeof(FPGA_HEADER) + sizeof(FLIRFILEHEAD)];

    // Try to create new file
    if (!file.Open(capFile, CFile::modeCreate |
                          CFile::modeReadWrite |
                          CFile::typeBinary, &e)) {
		return;
    }

	dwNumUsedIndex = pHeader->dwNumUsedIndex;
	ULONG offs = sizeof(FLIRFILEHEAD) + (sizeof(FLIRFILEINDEX) * dwNumUsedIndex);

	for (i = 0; i < (int)dwNumUsedIndex; i++) {
		pIndex[i].dwChecksum = 0;
		if (pIndex[i].wMainType == FFF_TAGID_Pixels) {
			pIndex[i].dwDataSize = pBuf->GetSize() + sizeof(GEOMETRIC_INFO_T);
		}
		pIndex[i].dwDataPtr = offs;
		offs += pIndex[i].dwDataSize;
	}

    try 
    {
        // Write to file
        file.Write(pHeader, sizeof(FLIRFILEHEAD));
		file.Write(pIndex, sizeof(FLIRFILEINDEX) * dwNumUsedIndex);

		for (i = 0; i < (int)dwNumUsedIndex; i++) 
		{
			if (pIndex[i].wMainType == FFF_TAGID_BasicData) 
			{
				PBYTE pData;
				BI_DATA_T* pBI;
				PvImage* pImage;

				pData = (PBYTE)&pIndex[dwNumUsedIndex];
				pBI = (BI_DATA_T *)pData;
				pImage = pBuf->GetImage();
				pBI->GeometricInfo.imageHeight = pImage->GetHeight();

				pBI->ImageInfo.imageTime = (unsigned long)m_ts;
				pBI->ImageInfo.imageMilliTime = m_ms;
				pBI->ImageInfo.timeZoneBias = m_tzBias;

				pBI->PresentParameters.level = mLevel;
				pBI->PresentParameters.span = mSpan;

                // Transfer trig info - if any
				if ((pFPGA->dp1_trig_type | pFPGA->dp2_trig_type) & 
						(FPGA_TRIG_TYPE_MARK	| 
						 FPGA_TRIG_TYPE_MARK_START |
						 FPGA_TRIG_TYPE_MARK_STOP)
						 )
				{
        			// Trig flag bits - description
		        	// Bit 15: 1=This trig info is relevant 
		            //         0=Look for trig info in pixel data instead
			        // Bit 5:  1=Stop trig type
			        // Bit 4:  1=Start trig type
			        // Bit 3:  0=Device 1=Serial port trig (or LPT)
			        // Bit 2:  0=TTL       1=OPTO 
			        // Bit 1:  0=Negative  1=Positive
			        // Bit 0:  0=No trig   1=Trigged
                    trgflags = 0x8000; // Trig info is relevant 
					if ((pFPGA->dp1_trig_type | pFPGA->dp2_trig_type) & FPGA_TRIG_TYPE_MARK)
						trgflags |= 0x0001; // Normal trig mark
					if ((pFPGA->dp1_trig_type | pFPGA->dp2_trig_type) & FPGA_TRIG_TYPE_MARK_START)
						trgflags |= 0x0010; // Start trig type
					if ((pFPGA->dp1_trig_type | pFPGA->dp2_trig_type) & FPGA_TRIG_TYPE_MARK_STOP)
						trgflags |= 0x0020; // Stop trig type
				}

                pBI->ImageInfo.trigFlags = trgflags;
                pBI->ImageInfo.trigCount = m_TrigCount;
                pBI->ImageInfo.trigHit = 0;
                pBI->ImageInfo.trigInfoType = 1;

				// Save a copy of geometric data
				memcpy(&geom, pData, sizeof(GEOMETRIC_INFO_T));

				// Write the modified Basic Image data block to file
				file.Write(pData, pIndex[i].dwDataSize);
			}
		}

		file.Write(&geom, sizeof(GEOMETRIC_INFO_T));
        file.Write(pSrc, pBuf->GetAcquiredSize());
        file.Close();
    }
    catch (CFileException* e) 
    {
        if (e->m_cause == CFileException::diskFull)
        {
            try {
                file.Close();
                file.Remove(capFile);
            }
            catch (CFileException*) {}
        }
    }

}

// ==========================================================================
void Thread::SaveSEQ(PvBuffer *pBuf)
{
	FPGA_HEADER* pFPGA;
	FLIRFILEHEAD* pHeader;
	FLIRFILEINDEX* pIndex;
	uint8_t* pSrc;
    CFileException e;          
	int nPixelSize = mHeight * mWidth * 2;
	int i;
	WORD trgflags = 0;
	GEOMETRIC_INFO_T geom;
	DWORD dwNumUsedIndex;

	pSrc = (uint8_t*)pBuf->GetDataPointer();
	pFPGA = (FPGA_HEADER*)&pSrc[nPixelSize];

    if (miCurrentSEQImage == 0)
    {
    	m_SEQFileName = m_SnapshotPath + _T("\\GEVDemo");
	    ::CreateDirectory(m_SEQFileName, NULL);
    	COleDateTime dt = COleDateTime::GetCurrentTime();
	    // Construct a unique file name
    	CString fileName;
	    fileName.Format(_T("%d%02d%02d_%02d%02d%02d.seq"),
		    dt.GetYear(), dt.GetMonth(), dt.GetDay(),
    		dt.GetHour(), dt.GetMinute(), dt.GetSecond());
	    m_SEQFileName += _T("\\") + fileName;

        // Try to create new file
        if (!m_SEQFile.Open(m_SEQFileName, CFile::modeCreate |
                          CFile::modeReadWrite |
                          CFile::typeBinary, &e)) 
        {
            mbSaveAsSEQ = false;
		    return;
        }
    }

	// Pointer to FFF header
    pHeader = (FLIRFILEHEAD *)&pSrc[nPixelSize + sizeof(FPGA_HEADER)];
	// Pointer to start of index table
	pIndex = (FLIRFILEINDEX *)&pSrc[nPixelSize + sizeof(FPGA_HEADER) + sizeof(FLIRFILEHEAD)];

	dwNumUsedIndex = pHeader->dwNumUsedIndex;
	ULONG offs = sizeof(FLIRFILEHEAD) + (sizeof(FLIRFILEINDEX) * dwNumUsedIndex);

	for (i = 0; i < (int)dwNumUsedIndex; i++) {
		pIndex[i].dwChecksum = 0;
		if (pIndex[i].wMainType == FFF_TAGID_Pixels) {
			pIndex[i].dwDataSize = nPixelSize + sizeof(GEOMETRIC_INFO_T);
		}
		pIndex[i].dwDataPtr = offs;
		offs += pIndex[i].dwDataSize;
	}

    try 
    {
        // Write to file
        m_SEQFile.Write(pHeader, sizeof(FLIRFILEHEAD));
		m_SEQFile.Write(pIndex, sizeof(FLIRFILEINDEX) * dwNumUsedIndex);

		for (i = 0; i < (int)dwNumUsedIndex; i++) 
		{
			if (pIndex[i].wMainType == FFF_TAGID_BasicData) 
			{
				PBYTE pData;
				BI_DATA_T* pBI;

				pData = (PBYTE)&pIndex[dwNumUsedIndex];
				pBI = (BI_DATA_T *)pData;
				pBI->GeometricInfo.imageHeight = mHeight; 

				pBI->ImageInfo.imageTime = (unsigned long)m_ts;
				pBI->ImageInfo.imageMilliTime = m_ms;
				pBI->ImageInfo.timeZoneBias = m_tzBias;

				pBI->PresentParameters.level = mLevel;
				pBI->PresentParameters.span = mSpan;

                // Transfer trig info - if any
				if ((pFPGA->dp1_trig_type | pFPGA->dp2_trig_type) & 
						(FPGA_TRIG_TYPE_MARK	| 
						 FPGA_TRIG_TYPE_MARK_START |
						 FPGA_TRIG_TYPE_MARK_STOP)
						 )
				{
        			// Trig flag bits - description
		        	// Bit 15: 1=This trig info is relevant 
		            //         0=Look for trig info in pixel data instead
			        // Bit 5:  1=Stop trig type
			        // Bit 4:  1=Start trig type
			        // Bit 3:  0=Device 1=Serial port trig (or LPT)
			        // Bit 2:  0=TTL       1=OPTO 
			        // Bit 1:  0=Negative  1=Positive
			        // Bit 0:  0=No trig   1=Trigged
                    trgflags = 0x8000; // Trig info is relevant 
					if ((pFPGA->dp1_trig_type | pFPGA->dp2_trig_type) & FPGA_TRIG_TYPE_MARK)
						trgflags |= 0x0001; // Normal trig mark
					if ((pFPGA->dp1_trig_type | pFPGA->dp2_trig_type) & FPGA_TRIG_TYPE_MARK_START)
						trgflags |= 0x0010; // Start trig type
					if ((pFPGA->dp1_trig_type | pFPGA->dp2_trig_type) & FPGA_TRIG_TYPE_MARK_STOP)
						trgflags |= 0x0020; // Stop trig type
				}

                pBI->ImageInfo.trigFlags = trgflags;
                pBI->ImageInfo.trigCount = m_TrigCount;
                pBI->ImageInfo.trigHit = 0;
                pBI->ImageInfo.trigInfoType = 1;

				// Save a copy of geometric data
				memcpy(&geom, pData, sizeof(GEOMETRIC_INFO_T));

				// Write the modified Basic Image data block to file
				m_SEQFile.Write(pData, pIndex[i].dwDataSize);
			}
		}

		m_SEQFile.Write(&geom, sizeof(GEOMETRIC_INFO_T));
        m_SEQFile.Write(pSrc, nPixelSize);
        miCurrentSEQImage++;
    }
    catch (CFileException* e) 
    {
        if (e->m_cause == CFileException::diskFull)
        {
            try {
                m_SEQFile.Close();
                m_SEQFile.Remove(m_SEQFileName);
            }
            catch (CFileException*) {}
        }
        miCurrentSEQImage = 0;
        mbSaveAsSEQ = false;
    }

    if (miCurrentSEQImage >= miSeqFrames) {
        m_SEQFile.Close();
        mbSaveAsSEQ = false;
    }
}

#define POW_OVERFLOW   100000.0
#define POW_UNDERFLOW  1e-6

// ==========================================================================
void Thread::SaveTauSEQ(PvBuffer *pBuf)
{
    FFF_FILE_HEADER     head; 
	BI_DATA_T           BI;
	uint8_t* pSrc;
    CFileException e;          
	int nPixelSize = mHeight * mWidth * 2;
	GEOMETRIC_INFO_T geom;

	pSrc = (uint8_t*)pBuf->GetDataPointer();

    if (miCurrentSEQImage == 0)
    {
    	m_SEQFileName = m_SnapshotPath + _T("\\GEVDemo");
	    ::CreateDirectory(m_SEQFileName, NULL);
    	COleDateTime dt = COleDateTime::GetCurrentTime();
	    // Construct a unique file name
    	CString fileName;
	    fileName.Format(_T("%d%02d%02d_%02d%02d%02d.seq"),
		    dt.GetYear(), dt.GetMonth(), dt.GetDay(),
    		dt.GetHour(), dt.GetMinute(), dt.GetSecond());
	    m_SEQFileName += _T("\\") + fileName;

        // Try to create new file
        if (!m_SEQFile.Open(m_SEQFileName, CFile::modeCreate |
                          CFile::modeReadWrite |
                          CFile::typeBinary, &e)) 
        {
            mbSaveAsSEQ = false;
		    return;
        }
    }

    // Initiate header
    memset(&head, 0, sizeof(FFF_FILE_HEADER));
    strcpy_s(head.header.szFormatID, 4, FFF_EXT);      // File format ID (= "FFF")
    strcpy_s(head.header.szOrigin, 16, "ATAU_RBFO");   // Program/unit which created file
    head.header.dwVersion = FILE_FORMAT_VERSION;       // Major+minor (1+1)
    // Offset to index data, from beginning of file
    head.header.dwIndexOff = sizeof(FLIRFILEHEAD);
    head.header.dwNumUsedIndex = 2;                    // Number of index entries
    head.header.dwNextID = 3;                          // Next free index, starting with 1
    head.header.usSwapPattern = FFF_HDR_SWAP_PATTERN;  // Swap pattern 1=LSBF

    // Basic image data tag
    head.btag.wMainType   = FFF_TAGID_BasicData;
    head.btag.wSubType    = 0;
    head.btag.dwVersion   = BASICDATAREV;
    head.btag.dwIndexID   = 1;
    head.btag.dwDataSize  = sizeof(BI_DATA_T);
    head.btag.dwDataPtr   = sizeof(FFF_FILE_HEADER);
    head.btag.dwParent    = 0;
    head.btag.dwObjectNr  = 0;

    // Image pixel tag
    head.itag.wMainType  = FFF_TAGID_Pixels;
    head.itag.wSubType   = FFF_Pixels_LE;
    head.itag.dwVersion  = IMAGEPIXELREV;
    head.itag.dwIndexID  = 2;
    head.itag.dwDataPtr  = sizeof(FFF_FILE_HEADER) + sizeof(BI_DATA_T);
    head.itag.dwDataSize = nPixelSize + sizeof(GEOMETRIC_INFO_T);

    memset(&BI, 0, sizeof(BI_DATA_T));
    BI.ObjectParameters.objectDistance = 2.0f; // Target distance 2 meters
    BI.ObjectParameters.relHum = 0.5; // 50% relative hunidity
    BI.ObjectParameters.estAtmTransm = 1.0;
    BI.ObjectParameters.emissivity = 1.0;
    BI.ObjectParameters.extOptTransm = 1.0;
    BI.ObjectParameters.ambTemp = 295.15f;
    BI.ObjectParameters.extOptTemp = 295.15f;
    BI.ObjectParameters.atmTemp = 295.15f; 

    float B = (float)mMainDlg->m_B;
    float F = (float)mMainDlg->m_F;
    float R = static_cast<float>(mMainDlg->m_W4WT * (exp(B / mMainDlg->m_WT) - F));
    BI.AdjustParameters.normGain = 1.0f / (static_cast<float>(mMainDlg->m_R / R));
    BI.AdjustParameters.normOffset = (long)(-mMainDlg->m_O);

    BI.CalibParameters.R = R;
    BI.CalibParameters.B = B;
    BI.CalibParameters.F = F;
    BI.CalibParameters.alpha1 = 0.006569f;
    BI.CalibParameters.alpha2 = 0.01262f;
    BI.CalibParameters.beta1 = -0.002276f;
    BI.CalibParameters.beta2 = -0.00667f;
    BI.CalibParameters.X = 1.9f;

    BI.CalibParameters.tmax = 5000.0f;
    BI.CalibParameters.tmin = 0.0f;

    BI.CalibParameters.tmaxClip = static_cast<float>(POW_OVERFLOW);
    BI.CalibParameters.tmaxWarn = static_cast<float>(POW_OVERFLOW);
    BI.CalibParameters.tmaxSaturated = static_cast<float>(POW_OVERFLOW);

    BI.CalibParameters.tminClip = static_cast<float>(POW_UNDERFLOW);
    BI.CalibParameters.tminWarn = static_cast<float>(POW_UNDERFLOW);
    BI.CalibParameters.tminSaturated = static_cast<float>(POW_UNDERFLOW);

    BI.ImageInfo.imageTime = (unsigned long)m_ts;
	BI.ImageInfo.imageMilliTime = m_ms;
	BI.ImageInfo.timeZoneBias = m_tzBias;

	BI.PresentParameters.level = mLevel;
	BI.PresentParameters.span = mSpan;

    BI.AdjustParameters.ipixUnderflow = 0;
    BI.AdjustParameters.ipixOverflow = 0xFFFF;

    memset(&geom, 0, sizeof(GEOMETRIC_INFO_T));
    geom.firstValidX = 0;
    geom.firstValidY = 0;
    geom.lastValidX = mWidth - 1;
    geom.lastValidY = mHeight - 1;
    geom.imageHeight = mHeight;
    geom.imageWidth = mWidth;
    geom.pixelSize = 2;
    memcpy(&BI.GeometricInfo, &geom, sizeof(GEOMETRIC_INFO_T));

    try 
    {
        // Write header to file
        m_SEQFile.Write(&head, sizeof(FFF_FILE_HEADER));

		// Write the modified Basic Image data block to file
		m_SEQFile.Write(&BI, sizeof(BI_DATA_T));

		// Write geometry block to file
		m_SEQFile.Write(&geom, sizeof(GEOMETRIC_INFO_T));
		// Write pixel data block to file
		m_SEQFile.Write(pSrc, pBuf->GetAcquiredSize());

        miCurrentSEQImage++;
    }
    catch (CFileException* e) 
    {
        if (e->m_cause == CFileException::diskFull)
        {
            try {
                m_SEQFile.Close();
                m_SEQFile.Remove(m_SEQFileName);
            }
            catch (CFileException*) {}
        }
        miCurrentSEQImage = 0;
        mbSaveAsSEQ = false;
    }

    if (miCurrentSEQImage >= miSeqFrames) {
        m_SEQFile.Close();
        mbSaveAsSEQ = false;
    }
}

// ==========================================================================
void Thread::SaveTAUImage(PvBuffer *pBuf)
{
    FFF_FILE_HEADER     head; 
	BI_DATA_T           BI;
	uint8_t* pSrc;
    CFile file;
    CFileException e;          
	int nPixelSize = mHeight * mWidth * 2;
	GEOMETRIC_INFO_T geom;

	pSrc = (uint8_t*)pBuf->GetDataPointer();

	CString capFile = m_SnapshotPath + _T("\\GEVDemo");
	::CreateDirectory(capFile, NULL);
	COleDateTime dt = COleDateTime::GetCurrentTime();
	// Construct a unique file name
	CString fileName;
	fileName.Format(_T("%d%02d%02d_%02d%02d%02d.img"),
		dt.GetYear(), dt.GetMonth(), dt.GetDay(),
		dt.GetHour(), dt.GetMinute(), dt.GetSecond());
	capFile += _T("\\") + fileName;

    // Try to create new file
    if (!file.Open(capFile, CFile::modeCreate |
                          CFile::modeReadWrite |
                          CFile::typeBinary, &e)) {
		return;
    }

    // Initiate header
    memset(&head, 0, sizeof(FFF_FILE_HEADER));
    strcpy_s(head.header.szFormatID, 4, FFF_EXT);      // File format ID (= "FFF")
    strcpy_s(head.header.szOrigin, 16, "ATAU_RBFO");   // Program/unit which created file
    head.header.dwVersion = FILE_FORMAT_VERSION;       // Major+minor (1+1)
    // Offset to index data, from beginning of file
    head.header.dwIndexOff = sizeof(FLIRFILEHEAD);
    head.header.dwNumUsedIndex = 2;                    // Number of index entries
    head.header.dwNextID = 3;                          // Next free index, starting with 1
    head.header.usSwapPattern = FFF_HDR_SWAP_PATTERN;  // Swap pattern 1=LSBF

    // Basic image data tag
    head.btag.wMainType   = FFF_TAGID_BasicData;
    head.btag.wSubType    = 0;
    head.btag.dwVersion   = BASICDATAREV;
    head.btag.dwIndexID   = 1;
    head.btag.dwDataSize  = sizeof(BI_DATA_T);
    head.btag.dwDataPtr   = sizeof(FFF_FILE_HEADER);
    head.btag.dwParent    = 0;
    head.btag.dwObjectNr  = 0;

    // Image pixel tag
    head.itag.wMainType  = FFF_TAGID_Pixels;
    head.itag.wSubType   = FFF_Pixels_LE;
    head.itag.dwVersion  = IMAGEPIXELREV;
    head.itag.dwIndexID  = 2;
    head.itag.dwDataPtr  = sizeof(FFF_FILE_HEADER) + sizeof(BI_DATA_T);
    head.itag.dwDataSize = nPixelSize + sizeof(GEOMETRIC_INFO_T);

    memset(&BI, 0, sizeof(BI_DATA_T));
    BI.ObjectParameters.objectDistance = 2.0f; // Target distance 2 meters
    BI.ObjectParameters.relHum = 0.5; // 50% relative hunidity
    BI.ObjectParameters.estAtmTransm = 1.0;
    BI.ObjectParameters.emissivity = 1.0;
    BI.ObjectParameters.extOptTransm = 1.0;
    BI.ObjectParameters.ambTemp = 295.15f;
    BI.ObjectParameters.extOptTemp = 295.15f;
    BI.ObjectParameters.atmTemp = 295.15f; 

    float B = (float)mMainDlg->m_B;
    float F = (float)mMainDlg->m_F;
    float R = static_cast<float>(mMainDlg->m_W4WT * (exp(B / mMainDlg->m_WT) - F));
    BI.AdjustParameters.normGain = 1.0f / (static_cast<float>(mMainDlg->m_R / R));
    BI.AdjustParameters.normOffset = (long)(-mMainDlg->m_O);

    BI.CalibParameters.R = R;
    BI.CalibParameters.B = B;
    BI.CalibParameters.F = F;
    BI.CalibParameters.alpha1 = 0.006569f;
    BI.CalibParameters.alpha2 = 0.01262f;
    BI.CalibParameters.beta1 = -0.002276f;
    BI.CalibParameters.beta2 = -0.00667f;
    BI.CalibParameters.X = 1.9f;

    BI.CalibParameters.tmax = 5000.0f;
    BI.CalibParameters.tmin = 0.0f;

    BI.CalibParameters.tmaxClip = static_cast<float>(POW_OVERFLOW);
    BI.CalibParameters.tmaxWarn = static_cast<float>(POW_OVERFLOW);
    BI.CalibParameters.tmaxSaturated = static_cast<float>(POW_OVERFLOW);

    BI.CalibParameters.tminClip = static_cast<float>(POW_UNDERFLOW);
    BI.CalibParameters.tminWarn = static_cast<float>(POW_UNDERFLOW);
    BI.CalibParameters.tminSaturated = static_cast<float>(POW_UNDERFLOW);

    BI.ImageInfo.imageTime = (unsigned long)m_ts;
	BI.ImageInfo.imageMilliTime = m_ms;
	BI.ImageInfo.timeZoneBias = m_tzBias;

	BI.PresentParameters.level = mLevel;
	BI.PresentParameters.span = mSpan;

    BI.AdjustParameters.ipixUnderflow = 0;
    BI.AdjustParameters.ipixOverflow = 0xFFFF;

    memset(&geom, 0, sizeof(GEOMETRIC_INFO_T));
    geom.firstValidX = 0;
    geom.firstValidY = 0;
    geom.lastValidX = mWidth - 1;
    geom.lastValidY = mHeight - 1;
    geom.imageHeight = mHeight;
    geom.imageWidth = mWidth;
    geom.pixelSize = 2;
    memcpy(&BI.GeometricInfo, &geom, sizeof(GEOMETRIC_INFO_T));

    try 
    {
        // Write header to file
        file.Write(&head, sizeof(FFF_FILE_HEADER));

		// Write the modified Basic Image data block to file
		file.Write(&BI, sizeof(BI_DATA_T));

		// Write geometry block to file
		file.Write(&geom, sizeof(GEOMETRIC_INFO_T));
		// Write pixel data block to file
		file.Write(pSrc, pBuf->GetAcquiredSize());
    }
    catch (CFileException* e) 
    {
        if (e->m_cause == CFileException::diskFull)
        {
            try {
                file.Close();
                file.Remove(capFile);
            }
            catch (CFileException*) {}
        }
    }

}
// ==========================================================================
unsigned long WINAPI Thread::Link( void *aParam )
{
	Thread *lThis = reinterpret_cast<Thread *>( aParam );
	lThis->mReturnValue = lThis->Function();
	return lThis->mReturnValue;
}

// ==========================================================================
DWORD Thread::GetReturnValue()
{
	return mReturnValue;
}

// ==========================================================================
void Thread::SetIRFormat(int fmt)
{
	m_IRFormat = fmt;

	if (m_CamType == CAM_TYPE_ATAU)
    {
        m_MinLimit = 500;
		m_MaxLimit = 65535;
    }
	else if (m_CamType == CAM_TYPE_T1K)
    {
        m_MinLimit = 0;
		m_MaxLimit = 65535;
    }
    else
    {
    	if (fmt == 0) {
	    	m_MinLimit = 3000;
		    m_MaxLimit = 62000;
    	} 
	    else if (fmt == 1) { // 0.1 K
		    m_MinLimit = 2000; // 200 K or -73 deg Celsius
    		m_MaxLimit = 62000; // 6200 K
	    }
    	else if (fmt == 2) { // 0.01 K
	    	m_MinLimit = 3000; // 30 K or -243 deg Celsius
		    m_MaxLimit = 62000; // 620 K
    	}
    }
}

void Thread::setSpot(CPoint cl)
{
	int32_t posX, posY, szX, szY;  

	// All coordinates for child windows are client coordinates
	// (relative to the upper-left corner of the parent window's client area)
	mDisplayWnd->GetPosition(posX, posY, szX,  szY);
	// cl is Client coordinates

	m_SpotPT.x = (LONG)((float)(cl.x * mWidth) / szX);
	m_SpotPT.y = (LONG)((float)(cl.y * mHeight) / szY);

	m_SpotPT.x = __max(0, m_SpotPT.x);
	m_SpotPT.x = __min((mWidth - 1), m_SpotPT.x);

	m_SpotPT.y = __max(0, m_SpotPT.y);
	m_SpotPT.y = __min((mHeight - 1), m_SpotPT.y);

    if (mbFlipped) {
        m_SpotPT.x = mWidth - m_SpotPT.x - 1;
    	m_SpotPT.y = mHeight - m_SpotPT.y - 1;
    }
	//TRACE("Spot %d x %d\r\n", m_SpotPT.x, m_SpotPT.y);
}

void Thread::CalcArea(uint16_t* pBuf)
{
	int x,y;
	int64_t sum = 0;
	USHORT amax = 0;
	USHORT amin = 0xFFFF;
	USHORT sample;
	DWORD  sampleCnt = 0;
    CPoint areaUL; // Upper left point

    if (!m_bAreaActive)
        return;

    areaUL = m_AreaUL;
    if (mbFlipped) {
        areaUL.x = mWidth - areaUL.x - 1;
    	areaUL.y = mHeight - areaUL.y - 1;
    }

	// Calculate area measurement values
	for (y = 0; y < m_AreaSize.cy; y++) 
	{
		for (x = 0; x < m_AreaSize.cx; x++)	
		{
			sample = pBuf[(mWidth * (areaUL.y + y)) + areaUL.x + x];

			if ((sample < m_MinLimit) || (sample > m_MaxLimit))
				continue;

			sampleCnt++;
			sum += sample;

			if (sample < amin) amin = sample;
			if (sample > amax) amax = sample;
		}
	}

	m_AreaMin = amin;
	m_AreaMax = amax;
	if (sampleCnt > 0)
		m_AreaAvg = (USHORT)(sum / sampleCnt);
}

void Thread::DrawArea(PvBuffer& Buf)
{
    int i, row;
    uint8_t* pDst;
    uint8_t* pDstA;
    uint8_t* pDstB;
    PvImage* pImg;

    if (!m_bAreaActive)
        return;

    pImg = Buf.GetImage();
	pDst = Buf.GetDataPointer();
    // Draw upper horizontal line
    pDstA = &pDst[m_AreaUL.y * (pImg->GetWidth() * 3)];
    pDstB = &pDst[(m_AreaUL.y + 1) * (pImg->GetWidth() * 3)];
    for (i = m_AreaUL.x; i < (m_AreaUL.x + m_AreaSize.cx); i++)
    {
        pDstA[i * 3] = 0xFF;
        pDstA[i * 3 + 1] = 0;
        pDstA[i * 3 + 2] = 0;

        pDstB[i * 3] = 0xFF;
        pDstB[i * 3 + 1] = 0;
        pDstB[i * 3 + 2] = 0;
    }

    for (row = m_AreaUL.y; row <= (m_AreaUL.y + m_AreaSize.cy); row++)
    {
        pDstA = &pDst[row * (pImg->GetWidth() * 3)];
        pDstA[m_AreaUL.x * 3] = 0xFF;
        pDstA[m_AreaUL.x * 3+1] = 0;
        pDstA[m_AreaUL.x * 3+2] = 0;
        pDstA[(m_AreaUL.x+1) * 3] = 0xFF;
        pDstA[(m_AreaUL.x+1) * 3+1] = 0;
        pDstA[(m_AreaUL.x+1) * 3+2] = 0;

        pDstA[(m_AreaUL.x + m_AreaSize.cx) * 3] = 0xFF;
        pDstA[(m_AreaUL.x + m_AreaSize.cx) * 3+1] = 0;
        pDstA[(m_AreaUL.x + m_AreaSize.cx) * 3+2] = 0;
        pDstA[(m_AreaUL.x + m_AreaSize.cx - 1) * 3] = 0xFF;
        pDstA[(m_AreaUL.x + m_AreaSize.cx - 1) * 3+1] = 0;
        pDstA[(m_AreaUL.x + m_AreaSize.cx - 1) * 3+2] = 0;
    }
    
    // Draw lower horizontal line
    pDstA = &pDst[(m_AreaUL.y + m_AreaSize.cy) * (pImg->GetWidth() * 3)];
    pDstB = &pDst[(m_AreaUL.y + m_AreaSize.cy - 1) * (pImg->GetWidth() * 3)];
    for (i = m_AreaUL.x; i < (m_AreaUL.x + m_AreaSize.cx); i++)
    {
        pDstA[i * 3] = 0xFF;
        pDstA[i * 3 + 1] = 0;
        pDstA[i * 3 + 2] = 0;

        pDstB[i * 3] = 0xFF;
        pDstB[i * 3 + 1] = 0;
        pDstB[i * 3 + 2] = 0;
    }
}

double Thread::getFPS()
{
    DWORD lDelta = ::GetTickCount() - m_StartTime;
    double fps;
    fps = (double) m_FrameCount * 1000 / (double) lDelta;
    return fps;
}

void Thread::resetStats()
{
    m_StartTime = GetTickCount();
    m_FrameCount = 0;
}

// ==========================================================================
DWORD Thread::Function()
{
	mFFMPEG = new FFMPEG(NULL);
	mFFMPEG->mVideoHeight = mWidth * 480/640 ;
	mFFMPEG->mVideoWidth = mWidth;
	mFFMPEG->m_AVIMOV_BPS = m_AVIMOV_BPS;
	mFFMPEG->Start();

	ASSERT( mDisplayWnd != NULL );
	ASSERT( mMainDlg != NULL );

    // Timestamp used to limit display rate
	DWORD lPrevious = 0;
	unsigned long lSize, i;
	DWORD ts;

	ts = GetTickCount();
    m_StartTime = ts;
    m_FrameCount = 0;
    lPrevious = ::GetTickCount();

	for ( ;; )
	{
		// Check if we were signaled to terminate
        if ( IsStopping() )
		{
			break;
		}

		PvBuffer *lBuffer = NULL;
        PvResult  lOperationResult;

        // Try retrieving a buffer, using default timeout
        PvResult lResult = mPipeline->RetrieveNextBuffer( &lBuffer, 1000, &lOperationResult );
        if ( lResult.IsOK() )
        {
            if ( lOperationResult.IsOK() )
            {
                mm_timer.GetUTCTime(m_ts, m_ms, m_tzBias);

                // Here we have successfully received a valid block of data (such as an image)
                m_FrameCount++;
	            // We limit refresh rate to ~ 30 fps max
                DWORD lCurrent = ::GetTickCount();
	            long lDelta = ( lCurrent - lPrevious ) - ( 1000 / 30 );
    	        if ( lDelta > 0 )
	            {
		    		FPGA_HEADER* pFPGA;
			    	USHORT maxpix, minpix;
		            lPrevious = ::GetTickCount();

    				uint8_t* pDst;
	    			uint16_t* pSrc;
		    		pSrc = (uint16_t*)lBuffer->GetDataPointer();
			    	pDst = mDispBuffer.GetDataPointer();

				    if (!m_bManualAdjust || m_bAdjustOnce)
				    {
					    // Find min and max
					    minpix = 0xFFFF;
					    maxpix = 0;
					    i = 0;

					    lSize = mHeight * mWidth; // Number of usable pixels
					    while (i < lSize) 
					    {
						    if ((pSrc[i] > m_MinLimit) && (pSrc[i] < minpix))
							    minpix = pSrc[i];

						    if ((pSrc[i] < m_MaxLimit) && (pSrc[i] > maxpix))
							    maxpix = pSrc[i];

						    i++;
					    }

					    m_ScaleLow = minpix;
					    m_ScaleHigh = maxpix;
					    m_bAdjustOnce = false;
				    }
				    else
				    {
					    minpix = m_ScaleLow;
					    maxpix = m_ScaleHigh;
				    }

				    // Get buffer value for spot
				    m_SpotValue = pSrc[(mWidth * m_SpotPT.y) + m_SpotPT.x];

                    int y = mHeight >> 1;
                    int col = (mWidth >> 1) + 1;
                    DWORD val;
                    val = (DWORD)pSrc[ (y-1)*mWidth + col ] +
                          (DWORD)pSrc[ (y-1)*mWidth + col + 1 ] +
                          (DWORD)pSrc[ y*mWidth + col ] + 
                          (DWORD)pSrc[ y*mWidth + col + 1 ];
                    m_CenterSpotValue = val >> 2;

			    	if (GetTickCount() - ts > 500) {
				    	CalcArea(pSrc);
					    ts = GetTickCount();
				    }

				    mSpan = maxpix - minpix + 1;
				    mLevel = (maxpix + minpix) / 2;

				    unsigned long x = 0;
				    UCHAR nsample = 0; // Normalized sample value (between 0 and 127)
				    USHORT sample;
				    i = 0;
				    float span = (float)(maxpix - minpix + 1);
				    lSize = mDispBuffer.GetSize();
                    if (mbFlipped)
                        x = x = mWidth * mHeight;
				    while (i < lSize) 
				    {
					    sample = pSrc[x];
                        if (mbFlipped)
                            x--;
                        else 
                            x++;

    					if (sample < minpix)
	    					nsample = 0;
		    			else if (sample > maxpix)
			    			nsample = 127;
				    	else {
					        nsample = (UCHAR) (((sample - minpix) / span) * 0xFF);
						    nsample = nsample >> 1;
					    }

					    pDst[i++] = mPalette[nsample].r;
					    pDst[i++] = mPalette[nsample].g;
    					pDst[i++] = mPalette[nsample].b;
	    			}

                    if (m_CamType != CAM_TYPE_ATAU)
                    {
		    		    // Check trig count
			    	    pFPGA = (FPGA_HEADER*)&pSrc[mHeight * mWidth];
				        if (pFPGA->dp1_trig_type & FPGA_TRIG_TYPE_MARK)	{
                           //TRACE("Trig\n");
					        m_TrigCount1++;
                            m_TrigCount++;
				        }
                        //TRACE("State: %d Frame count %d\n", pFPGA->dp1_trig_state, pFPGA->frame_cnt_LSB + (pFPGA->frame_cnt_MSB << 8));
                        m_LineState1 = pFPGA->dp1_trig_state ? 1 : 0;
                        m_LineState2 = pFPGA->dp2_trig_state ? 1 : 0;

				        if (pFPGA->dp2_trig_type & FPGA_TRIG_TYPE_MARK) {
					        m_TrigCount2++;
                            m_TrigCount++;
				        }
                    }

					
                    DrawArea(mDispBuffer);

					
					mFFMPEG->SendNewFrame((char*)pDst);

                    // Display buffer
                    mDisplayWnd->Display( mDispBuffer );
                }

                mMainDlg->AlarmCheck();
                mMainDlg->TrigCheck();
	    		if (mbSaveAsBMP) {
		    		// Image save is requested
			    	mbSaveAsBMP = false;
				    SaveAsBitmap(&mDispBuffer);
			    }

			    if (mbSaveImage) {
				    // Image save is requested
    				mbSaveImage = false;
                    if (m_CamType == CAM_TYPE_ATAU)
		    		    SaveTAUImage(lBuffer);
                    else
				        SaveImage(lBuffer);
			    }
                else if (mbSaveAsSEQ) {
				    // Sequence save is requested
                    if (m_CamType == CAM_TYPE_ATAU)
		    		    SaveTauSEQ(lBuffer);
                    else
		    		    SaveSEQ(lBuffer);
                }

			    if (mbSaveAsRAW) {
				    // Image save is requested
    				mbSaveAsRAW = false;
	    			SaveAsRaw(lBuffer);
		    	}

                //ShowTSens(lBuffer);
            }

            // Release buffer back to the pipeline
            mPipeline->ReleaseBuffer( lBuffer );
		}
        else {
            //TRACE("Frame buffer error - sleep\r\n");
            Sleep(500);
        }
	}


	mFFMPEG->Stop();
	while (!mFFMPEG->IsDone()) {
		Sleep(1);
	}
	delete mFFMPEG;

	return 0;
}

