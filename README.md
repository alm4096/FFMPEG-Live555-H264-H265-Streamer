# FFMPEG-Live555-H264-H265-Streamer
H264, H265 and more Encoder and Multicast/Unicast Streamer (example of how to use Live555 and FFMPEG). A threaded class for H264 and H265 streaming in Visual Studio using FFMPEG and LIVE555

## Credits
This project uses the following (and would not work at all without them):
- LIVE555 http://www.live555.com/
- FFMPEG https://ffmpeg.org/

This project simply provides an easy to use 'example' of how to use FFMPEG and LIVE555 together to encode and stream video for Windows programmers using Visual Studio.

## Project Ultimatum
This project aims to do the following:
- Provide a simple threaded class for H264 / H265 encoding and streaming using FFMPEG and LIVE555 (other formats can be added as needed)
- Minimum fuss class setup and usage (it should be as easy as creating the class and passing a frame to get working, keep it simple!)
- For use only with Visual Studio 2010+ development environment (exclusively for Windows only)

Please keep this Git inline with the ultimatum. I encourage anyone to create their own copy of this Git and take this project in another direction.

Send me a message if you would like to collaborate.

## Planned Features
- Clean up code
- Allow 'users' (i.e. programmer using class) to directly change encoding options
- Make the following an option instead of being hard coded:
  - Encoding format (H264, H265, etc)
  - Unicast / Multicast
  - Encoding presets (for H264 and H265)
- Update to latest FFMPEG and LIVE555 versions
- Check for memory leaks
  
## Current Status: First Release V1.0
- V1.0 Initial Release 7th July 2016:
   - Encodes and streams in H264, H265, MP4, and MP2
   - Streams in Multicast and Unicast
   - Options mostly hardcoded
   - Project file only works in VS2013 and above (mixup with creating the solution file, will be fixed soon)
