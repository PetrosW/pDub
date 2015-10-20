#ifndef FFMPEG_HPP
#define FFMPEG_HPP

#include <algorithm>
#include <climits>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <string>
#include <utility>
#include <vector>
#include "exception.hpp"

extern "C" {
    #include <libavcodec/avcodec.h>
    #include <libavformat/avformat.h>
}

#define PACKET_WAV_SAMPLE_COUNT 1024 // 1024 samples form one packet

namespace FfmpegCleanUpLevelCode {
    enum Type:uint8_t {
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
        void splitTrack(std::string FileName, uint64_t SplitDuration);
        uint64_t getAudioDuration(std::string FileName);
        std::pair<std::vector<double>, std::vector<double> > getSamplesForWaveformPlotting(std::string FileName);
    
    private:
        //AVCodec *Codec_In;
        //AVCodec *Codec_Out;
        
        //AVCodecContext *CodecContext_In;
        //AVCodecContext *CodecContext_Out;
        
        AVFormatContext *Container_In;
        AVFormatContext *Container_Out;
        
        AVPacket Packet;
        
        uint8_t SampleBuffer[(PACKET_WAV_SAMPLE_COUNT - 1) * 4];
        uint16_t SampleCount;
        
        void initInputFileAudio(std::string &FileName);
        void writePacketsToFile(std::string &SplitFile, uint64_t SplitDuration);
        void cleanUp_SplitTrack(FfmpegCleanUpLevelCode::Type Level, bool CloseInput = 1);
        void separateChannelSamples(int16_t *SamplePtr, std::vector<double> &Channel1, std::vector<double> &Channel2, int16_t SampleCountXChannels, bool Restart);
        void separateChannelSamples2(int16_t *SamplePtr, std::vector<double> &Channel1, std::vector<double> &Channel2, int16_t SampleCountXChannels, bool Restart);
};

#endif
