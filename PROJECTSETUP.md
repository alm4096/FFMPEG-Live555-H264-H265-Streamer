# How to Setup Your Solution Settings
How to add this class to your existing project

## Includes
- Go to project properties.
- Configuration -> VC++ Directories -> Include Directories
- Add the following directories:
   - $(SolutionDir)Includes\live\includes\BasicUsageEnvironment\include
   - $(SolutionDir)Includes\live\includes\groupsock\include
   - $(SolutionDir)Includes\live\includes\liveMedia\include
   - $(SolutionDir)Includes\live\includes\UsageEnvironment\include
   - $(SolutionDir)Includes\ffmpeg2016-04-05\include

## Libraries
- Go to project properties.
- Configuration -> VC++ Directories -> Library Directories
- Add the following directories:
   - $(SolutionDir)\Includes\live\libs
   - $(SolutionDir)Includes\ffmpeg2016-04-05\lib
- Configuration -> Linker -> Input -> Additional Dependencies
- Add the following libraries:
   - Ws2_32.lib
   - avcodec.lib
   - avdevice.lib
   - avfilter.lib
   - avformat.lib
   - avutil.lib
   - postproc.lib
   - swresample.lib
   - swscale.lib
- Now add the following for the DEBUG version only
   - libBasicUsageEnvironmentD.lib
   - libgroupsockD.lib
   - libliveMediaD.lib
   - libUsageEnvironmentD.lib
- Now add the following for the RELEASE version only
   - libBasicUsageEnvironment.lib
   - libgroupsock.lib
   - libliveMedia.lib
   - libUsageEnvironment.lib

## Files:
Add the following Cpp Source files:
- FFMPEGClass.cpp
- Live_AnalyserInput.cpp
- Live_AnalyserSource.cpp
- Live_AnalysingServerMediaSubsession.cpp
- Live555Class.cpp

Add the following H Header files:
- Definitions.h
- FFMPEGClass.h
- Live_AnalyserInput.h
- Live_AnalyserSource.h
- Live_AnalysingServerMediaSubsession.h
- Live555Class.h
- Live555UsageEnv.h
- MutexUtility.h

