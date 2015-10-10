#ifndef FFMPEG_HPP
#define FFMPEG_HPP

#include <cmath>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <memory>
#include <string>
#include "exception.hpp"

extern "C" {
    #include <libavcodec/avcodec.h>
    #include <libavformat/avformat.h>
}

namespace FfmpegCleanUpLevelCode {
    enum Type:std::uint8_t {
        // for Ffmpeg_t::cleanUp_SplitTrack()
        LEVEL_AVIO,
        LEVEL_AVFORMAT_CONTEXT,
        LEVEL_AVFORMAT_INPUT
    };
}

class Ffmpeg_t
{
    public:
        Ffmpeg_t();
        void splitTrack(std::string FileName, uint64_t SplitDuration_1, uint64_t SplitDuration_2 = 0);
    
    private:
        //AVCodec *Codec_In;
        //AVCodec *Codec_Out;
        
        //AVCodecContext *CodecContext_In;
        //AVCodecContext *CodecContext_Out;
        
        AVFormatContext *Container_In;
        AVFormatContext *Container_Out;
        
        AVPacket Packet;
        
        uint8_t SampleBuffer[4092];
        uint16_t SampleCount;
        
        void writePacketsToFile(std::string &SplitFile, uint64_t SplitDuration);
        void cleanUp_SplitTrack(FfmpegCleanUpLevelCode::Type Level, bool CloseInput = 1);
};

#endif
