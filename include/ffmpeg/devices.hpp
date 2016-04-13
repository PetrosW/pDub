#ifndef DEVICES_HPP
#define DEVICES_HPP

#include <cassert>
#include <cmath>
#include <utility>
#include "error_codes.hpp"
#include <ffmpeg/packet_sizes.hpp>
#include <omp.h>

extern "C" {
    #include <libavcodec/avcodec.h>
    #include <libavformat/avformat.h>
}

typedef struct {
    int16_t ChannelLeft[PACKET_WAV_SAMPLE_COUNT];
    int16_t ChannelRight[PACKET_WAV_SAMPLE_COUNT];
    uint16_t SampleCount;
} SampleBuffer_t;

typedef struct {
    double ChannelLeft[PACKET_MP3_SAMPLE_COUNT];
    double ChannelRight[PACKET_MP3_SAMPLE_COUNT];
    uint16_t SampleCount;
} OutputBuffer_t;

// Input track wrapper for handling with necessary buffers
class InputDevice
{
    public:
        InputDevice(AVFormatContext *InputTrack);
        ~InputDevice();
        
        int32_t getSamples(uint16_t SampleCount, OutputBuffer_t &OutputBuffer);
        int32_t skipSamples(uint64_t SampleCount);
        int32_t mixSamples(uint16_t SampleCount, OutputBuffer_t &MixWithBuffer, double MultiplicationConst);
    
    private:
        AVFormatContext *InputTrack;
        SampleBuffer_t SampleBuffer_Input;
        AVPacket Packet;
};

class OutputDevice
{
    public:
        OutputDevice(AVFormatContext *Container, AVCodecContext *CodecContext, uint8_t StreamIndex);
        ~OutputDevice();
        std::pair<FfmpegErrorCode::Type, int32_t> putSamples(OutputBuffer_t &InputBuffer, AVRational &CodecBase, AVRational &StreamBase);
        std::pair<FfmpegErrorCode::Type, int32_t> flushDevice(AVRational &CodecBase, AVRational &StreamBase);
        bool AllocFrameSuccess;
        bool AllocFrameBufferSuccess;
        int64_t Pts;
    
    private:
        AVFormatContext *Container;
        AVCodecContext *CodecContext;
        OutputBuffer_t SampleBuffer_Output;
        uint8_t StreamIndex_Audio;
        AVPacket Packet;
        AVFrame *Frame;
};

#endif
