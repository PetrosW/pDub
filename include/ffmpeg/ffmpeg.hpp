#ifndef FFMPEG_HPP
#define FFMPEG_HPP

#include <algorithm>
#include <climits>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <functional>
#include <list>
#include <map>
#include <set>
#include <string>
#include <utility>
#include <vector>
#include "exception.hpp"

class Record_1
{
    public:
        uint32_t Id;
        uint32_t StartTime;
        uint32_t EndTime;
        std::string Name;
};

extern "C" {
    #include <libavcodec/avcodec.h>
    #include <libavformat/avformat.h>
    #include <libswresample/swresample.h>
}

#define PACKET_WAV_SAMPLE_COUNT 1024 // 1024 samples form one packet

namespace FfmpegCleanUpLevelCode_SplitTrack {
    enum Type:uint8_t {
        // for Ffmpeg_t::cleanUp_SplitTrack()
        LEVEL_AVIO,
        LEVEL_AVFORMAT_CONTEXT,
        LEVEL_AVFORMAT_INPUT
    };
}

namespace FfmpegCleanUpLevelCode_ConvertAudio {
    enum Type:uint8_t {
        // for Ffmpeg_t::cleanUp_ConvertAudio()
        LEVEL_PACKET,
        LEVEL_AVIO,
        LEVEL_AVFORMAT_OUTPUT,
        LEVEL_RESAMPLE_CLOSE,
        LEVEL_RESAMPLE_FREE,
        LEVEL_FRAME,
        LEVEL_AVCODEC_CLOSE,
        LEVEL_AVFORMAT_INPUT
    };
}

namespace FfmpegExportComponents {
    enum :uint8_t {
        // for Ffmpeg::exportProject()
        AUDIO_ONLY = 0x01,
        VIDEO_ONLY = 0x02,
        AUDIO_VIDEO = AUDIO_ONLY | VIDEO_ONLY
    };
}

class Interval_t
{
    public:
        Interval_t(uint32_t S, uint32_t E) : StartTime(S), EndTime(E) {};
        uint32_t StartTime;
        uint32_t EndTime;
};

struct Comparator_Interval_t {
    bool operator()(const Interval_t &lhs, const Interval_t &rhs)
    {
        return (lhs.StartTime < rhs.StartTime);
    }
};

typedef struct {
    uint64_t SampleCount;
    std::list<uint32_t> Recordings;
} AudioTask_t;

class Ffmpeg_t
{
    public:
        Ffmpeg_t();
        void splitTrack(std::string FileName, uint32_t SplitDuration);
        uint64_t getAudioDuration(std::string FileName);
        std::pair<std::vector<double>, std::vector<double> > getSamplesForWaveformPlotting(std::string FileName);
        void convertInputAudio(std::string FileName, std::string Id);
        void exportProject(std::map<uint16_t, Record_1 *> &Recordings, std::string OutputFile, uint32_t Start, uint32_t End, uint8_t ExportComponents);
    
    private:
        AVFormatContext *Container_In;
        AVFormatContext *Container_Out;
        
        AVPacket Packet;
        
        uint8_t StreamIndex;
        uint8_t SampleBuffer[(PACKET_WAV_SAMPLE_COUNT - 1) * 4];
        uint16_t SampleCount;
        uint32_t Duration;
        uint8_t *ResamplingBuffer[1];
        int32_t ResamplingBufferSize;
        
        void initInputFileAudio(std::string &FileName, AVFormatContext **Container = nullptr);
        void writePacketsToFile(std::string &SplitFile, uint32_t SplitDuration);
        void cleanUp_SplitTrack(FfmpegCleanUpLevelCode_SplitTrack::Type Level, bool CloseInput = 1);
        void cleanUp_ConvertAudio(FfmpegCleanUpLevelCode_ConvertAudio::Type Level, AVFrame **Frame = nullptr, SwrContext **ResampleContext = nullptr);
        void separateChannelSamples(int16_t *SamplePtr, std::vector<double> &Channel1, std::vector<double> &Channel2, int16_t SampleCountXChannels, bool Restart);
        void compareMinMaxAndSwap(std::function<bool(int16_t, int16_t)> SampleComparator, int16_t **MinMaxValue, int16_t Sample, int16_t **SwapMinMaxValueWith);
        void prepareOutputPacketAndWriteIt(AVPacket &Packet_Out, std::vector<uint8_t> &SampleFifo, AVFrame *Frame, SwrContext *ResampleContext, bool FreePacket);
        int32_t resample_AndStore(SwrContext *ResampleContext, AVFrame *Frame, std::vector<uint8_t> &SampleFifo);
        int32_t resample_JustStore(SwrContext *, AVFrame *Frame, std::vector<uint8_t> &SampleFifo);
};

#endif
