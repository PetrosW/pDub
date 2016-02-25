#include <ffmpeg/ffmpeg.hpp>

Ffmpeg_t::Ffmpeg_t() : Packet{}, StreamIndex(0), SampleCount(0), ResamplingBuffer{nullptr}, ResamplingBufferSize(0)
{
    av_register_all();
}

void Ffmpeg_t::cleanUp_SplitTrack(FfmpegCleanUpLevelCode_SplitTrack::Type Level, bool CloseInput)
{
    switch (Level)
    {
        case FfmpegCleanUpLevelCode_SplitTrack::LEVEL_AVIO:
            avio_closep(&Container_Out->pb);
        
        case FfmpegCleanUpLevelCode_SplitTrack::LEVEL_AVFORMAT_CONTEXT:
            avformat_free_context(Container_Out);
        
        case FfmpegCleanUpLevelCode_SplitTrack::LEVEL_AVFORMAT_INPUT:
            if (CloseInput) avformat_close_input(&Container_In);
        break;
        
        default:
        return;
    }
}

void Ffmpeg_t::cleanUp_ConvertAudio(FfmpegCleanUpLevelCode_ConvertAudio::Type Level, AVFrame **Frame, SwrContext **ResampleContext)
{
    switch (Level)
    {
        case FfmpegCleanUpLevelCode_ConvertAudio::LEVEL_PACKET:
            av_packet_unref(&Packet);
        
        case FfmpegCleanUpLevelCode_ConvertAudio::LEVEL_AVIO:
            avio_closep(&Container_Out->pb);
        
        case FfmpegCleanUpLevelCode_ConvertAudio::LEVEL_AVFORMAT_OUTPUT:
            avformat_free_context(Container_Out);
        
        case FfmpegCleanUpLevelCode_ConvertAudio::LEVEL_RESAMPLE_CLOSE:
            if (*ResampleContext) swr_close(*ResampleContext);
        
        case FfmpegCleanUpLevelCode_ConvertAudio::LEVEL_RESAMPLE_FREE:
            if (*ResampleContext) swr_free(ResampleContext);
        
        case FfmpegCleanUpLevelCode_ConvertAudio::LEVEL_FRAME:
            av_frame_free(Frame);
        
        case FfmpegCleanUpLevelCode_ConvertAudio::LEVEL_AVCODEC_CLOSE:
            avcodec_close(Container_In->streams[StreamIndex]->codec);
        
        case FfmpegCleanUpLevelCode_ConvertAudio::LEVEL_AVFORMAT_INPUT:
            avformat_close_input(&Container_In);
        break;
        
        default:
        return;
    }
}

std::pair<std::vector<double>, std::vector<double> > Ffmpeg_t::getSamplesForWaveformPlotting(std::string FileName)
{
    initInputFileAudio(FileName);
    
    std::vector<double> Channel1, Channel2;
    
    int32_t ErrCode = 0;
    while ( !(ErrCode = av_read_frame(Container_In, &Packet) ) )
    {
        int16_t *SamplePtr = reinterpret_cast<int16_t *>(Packet.data);
        separateChannelSamples(SamplePtr, Channel1, Channel2, Packet.duration << 1, false);
        av_packet_unref(&Packet);
    }
    
    avformat_close_input(&Container_In);
    separateChannelSamples(nullptr, Channel1, Channel2, 0, true);
    
    if (ErrCode != AVERROR_EOF) throw FfmpegException_t(FfmpegErrorCode::CONTAINER_IN_READ_FRAME, ErrCode);
    
    return std::make_pair(Channel1, Channel2);
}

void Ffmpeg_t::separateChannelSamples(int16_t *SamplePtr, std::vector<double> &Channel1, std::vector<double> &Channel2, int16_t SampleCountXChannels, bool Restart)
{
    bool IsFirstChannel = 1;
    
    static int16_t MinMax_Channel1[] = {SHRT_MAX, SHRT_MIN}, MinMax_Channel2[] = {SHRT_MAX, SHRT_MIN};
    static int16_t *Max_Channel1 = MinMax_Channel1 + 1, *Max_Channel2 = MinMax_Channel2 + 1;
    static int16_t *Min_Channel1 = MinMax_Channel1, *Min_Channel2 = MinMax_Channel2;
    
    if (Restart)
    {
        SampleCount = 0;
        // Does it work correctly after multiple runs?
        *Max_Channel1 = *Max_Channel2 = SHRT_MIN;
        *Min_Channel1 = *Min_Channel2 = SHRT_MAX;
        return;
        
        // Should I flush MinMax_Channels or discard? Less than 1ms -> discard
    }
    
    for (int16_t i = 0; i < SampleCountXChannels; i++)
    {
        if (IsFirstChannel)
        {
            compareMinMaxAndSwap(std::less<int16_t>(), &Max_Channel1, SamplePtr[i], &Min_Channel1);
            compareMinMaxAndSwap(std::greater<int16_t>(), &Min_Channel1, SamplePtr[i], &Max_Channel1);
        }
        else
        {
            compareMinMaxAndSwap(std::less<int16_t>(), &Max_Channel2, SamplePtr[i], &Min_Channel2);
            compareMinMaxAndSwap(std::greater<int16_t>(), &Min_Channel2, SamplePtr[i], &Max_Channel2);
            
            SampleCount++;
            
            if (SampleCount == 44) // 44 samples ~ 1 ms
            {
                Channel1.push_back(MinMax_Channel1[0]);
                Channel1.push_back(MinMax_Channel1[1]);
                Channel2.push_back(MinMax_Channel2[0]);
                Channel2.push_back(MinMax_Channel2[1]);
                
                *Min_Channel1 = *Min_Channel2 = SHRT_MAX;
                *Max_Channel1 = *Max_Channel2 = SHRT_MIN;
                SampleCount = 0;
            }
        }
        
        IsFirstChannel = !IsFirstChannel;
    }
}

void Ffmpeg_t::compareMinMaxAndSwap(std::function<bool(int16_t, int16_t)> SampleComparator, int16_t **MinMaxValue, int16_t Sample, int16_t **SwapMinMaxValueWith)
{
    if (SampleComparator(**MinMaxValue, Sample) )
    {
        **MinMaxValue = Sample;
        
        if (*MinMaxValue < *SwapMinMaxValueWith)
        {
            std::swap(**MinMaxValue, **SwapMinMaxValueWith);
            std::swap(*MinMaxValue, *SwapMinMaxValueWith);
        }
    }
}

void Ffmpeg_t::convertInputAudio(std::string FileName, std::string Id)
{
    bool DoResample = false;
    
    initInputFileAudio(FileName);
    
    AVCodecContext *CodecContext_In = Container_In->streams[StreamIndex]->codec;
    if (!CodecContext_In->channel_layout) CodecContext_In->channel_layout = av_get_default_channel_layout(CodecContext_In->channels);
    
    if ( (CodecContext_In->sample_rate != 44100) || ( (CodecContext_In->sample_fmt != AV_SAMPLE_FMT_S16P) && (CodecContext_In->sample_fmt != AV_SAMPLE_FMT_S16) ) 
       || (CodecContext_In->channel_layout != AV_CH_LAYOUT_STEREO) ) DoResample = true;
        
    if ( (!DoResample) && (CodecContext_In->codec_id == AV_CODEC_ID_PCM_S16LE) )
    {
        printf("Info: nothing needs to be done, codecs and sample rate/format fit\n");
        avformat_close_input(&Container_In);
        return;
    }
    
    AVCodec *Codec_In = avcodec_find_decoder(CodecContext_In->codec_id);
    if ( (!Codec_In) || (Codec_In->id < AV_CODEC_ID_FIRST_AUDIO) || (Codec_In->id >= AV_CODEC_ID_FIRST_SUBTITLE) )
    {
        cleanUp_ConvertAudio(FfmpegCleanUpLevelCode_ConvertAudio::LEVEL_AVFORMAT_INPUT);
        throw FfmpegException_t(FfmpegErrorCode::CODEC_IN_NOT_FOUND, 0);
    }
    
    if (avcodec_open2(CodecContext_In, avcodec_find_decoder(CodecContext_In->codec_id), nullptr) )
    {
        cleanUp_ConvertAudio(FfmpegCleanUpLevelCode_ConvertAudio::LEVEL_AVFORMAT_INPUT);
        throw FfmpegException_t(FfmpegErrorCode::CODEC_IN_OPEN, 0);
    }
    
    int32_t ErrCode = 0;
    SwrContext *ResampleContext = nullptr;
    int32_t (Ffmpeg_t::*resampleFunction)(SwrContext *, AVFrame *, std::vector<uint8_t>&) =
        (DoResample ? &Ffmpeg_t::resample_AndStore : &Ffmpeg_t::resample_JustStore);
    
    AVFrame *Frame = av_frame_alloc();
    if (!Frame)
    {
        cleanUp_ConvertAudio(FfmpegCleanUpLevelCode_ConvertAudio::LEVEL_AVCODEC_CLOSE);
        throw FfmpegException_t(FfmpegErrorCode::FRAME_ALLOC, 0);
    }
    
    if (DoResample)
    {
        ResampleContext = swr_alloc_set_opts(nullptr, AV_CH_LAYOUT_STEREO, AV_SAMPLE_FMT_S16, 44100,
                                             CodecContext_In->channel_layout, CodecContext_In->sample_fmt, CodecContext_In->sample_rate, 0, nullptr);
        if (!ResampleContext)
        {
            cleanUp_ConvertAudio(FfmpegCleanUpLevelCode_ConvertAudio::LEVEL_FRAME, &Frame);
            throw FfmpegException_t(FfmpegErrorCode::RESAMPLE_ALLOC, 0);
        }
        
        ErrCode = swr_init(ResampleContext);
        if (ErrCode < 0)
        {
            cleanUp_ConvertAudio(FfmpegCleanUpLevelCode_ConvertAudio::LEVEL_RESAMPLE_FREE, &Frame, &ResampleContext);
            throw FfmpegException_t(FfmpegErrorCode::RESAMPLE_INIT, ErrCode);
        }
    }
    
    ErrCode = avformat_alloc_output_context2(&Container_Out, nullptr, "wav", nullptr);
    if (ErrCode < 0)
    {
        cleanUp_ConvertAudio(FfmpegCleanUpLevelCode_ConvertAudio::LEVEL_RESAMPLE_CLOSE, &Frame, &ResampleContext);
        throw FfmpegException_t(FfmpegErrorCode::CONTAINER_OUT_ALLOC, ErrCode);
    }
    
    AVStream *Stream_Out = avformat_new_stream(Container_Out, avcodec_find_decoder(AV_CODEC_ID_PCM_S16LE) );
    if (!Stream_Out)
    {
        cleanUp_ConvertAudio(FfmpegCleanUpLevelCode_ConvertAudio::LEVEL_AVFORMAT_OUTPUT, &Frame, &ResampleContext);
        throw FfmpegException_t(FfmpegErrorCode::CONTAINER_OUT_NEW_STREAM, 0);
    }
    
    Stream_Out->id = 0;
    Stream_Out->time_base = {1, 44100};
    Stream_Out->codec->sample_fmt = AV_SAMPLE_FMT_S16;
    Stream_Out->codec->sample_rate = 44100;
    Stream_Out->codec->bit_rate = 705600;
    Stream_Out->codec->channel_layout = AV_CH_LAYOUT_STEREO;
    Stream_Out->codec->channels = 2;
    Stream_Out->codec->codec_tag = 0;
    
    if (Container_Out->oformat->flags & AVFMT_GLOBALHEADER) Stream_Out->codec->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;
    
    ErrCode = avio_open(&Container_Out->pb, Id.c_str(), AVIO_FLAG_WRITE); // co bude v Id?
    if (ErrCode < 0)
    {
        cleanUp_ConvertAudio(FfmpegCleanUpLevelCode_ConvertAudio::LEVEL_AVFORMAT_OUTPUT, &Frame, &ResampleContext);
        throw FfmpegException_t(FfmpegErrorCode::CONTAINER_OUT_OPEN_FILE, ErrCode);
    }
    
    ErrCode = avformat_write_header(Container_Out, nullptr);
    if (ErrCode < 0)
    {
        cleanUp_ConvertAudio(FfmpegCleanUpLevelCode_ConvertAudio::LEVEL_AVIO, &Frame, &ResampleContext);
        throw FfmpegException_t(FfmpegErrorCode::CONTAINER_OUT_WRITE_HEADER, ErrCode);
    }
    
    AVPacket Packet_Out = {0};
    av_init_packet(&Packet_Out);
    Duration = 0;
    ResamplingBufferSize = 0;
    int32_t DecodedAmount;
    int32_t WasFrameDecoded;
    std::vector<uint8_t> SampleFifo;
    
    /* ##### INIT DONE, BOTH CONTAINERS OPEN & READY ##### */
    
    while (!ErrCode)
    {
        ErrCode = av_read_frame(Container_In, &Packet);
        if (ErrCode < 0)
        {
            if (ErrCode == AVERROR_EOF) break;
            else
            {
                cleanUp_ConvertAudio(FfmpegCleanUpLevelCode_ConvertAudio::LEVEL_AVIO, &Frame, &ResampleContext);
                throw FfmpegException_t(FfmpegErrorCode::CONTAINER_IN_READ_FRAME, ErrCode);
            }
        }
        
        if (Packet.stream_index != StreamIndex)
        {
            av_packet_unref(&Packet);
            continue;
        }
        
        DecodedAmount = 0;
        
        do {
            ErrCode = avcodec_decode_audio4(CodecContext_In, Frame, &WasFrameDecoded, &Packet);
            if (ErrCode < 0)
            {
                cleanUp_ConvertAudio(FfmpegCleanUpLevelCode_ConvertAudio::LEVEL_PACKET, &Frame, &ResampleContext);
                throw FfmpegException_t(FfmpegErrorCode::DECODE, ErrCode);
            }
            
            DecodedAmount += ErrCode;
            if (WasFrameDecoded)
            {
                ErrCode = (this->*resampleFunction)(ResampleContext, Frame, SampleFifo);
                if (ErrCode < 0)
                {
                    cleanUp_ConvertAudio(FfmpegCleanUpLevelCode_ConvertAudio::LEVEL_PACKET, &Frame, &ResampleContext);
                    throw FfmpegException_t(FfmpegErrorCode::RESAMPLE_CONVERT, 0);
                }
            }
        } while (DecodedAmount < Packet.size);
        
        while (SampleFifo.size() >= PACKET_WAV_SAMPLE_COUNT << 2) // 4096
        {
            ErrCode = av_new_packet(&Packet_Out, PACKET_WAV_SAMPLE_COUNT << 2);
            if (ErrCode < 0)
            {
                cleanUp_ConvertAudio(FfmpegCleanUpLevelCode_ConvertAudio::LEVEL_AVIO, &Frame, &ResampleContext);
                throw FfmpegException_t(FfmpegErrorCode::PACKET_ALLOC, ErrCode);
            }
            
            prepareOutputPacketAndWriteIt(Packet_Out, SampleFifo, Frame, ResampleContext, true);
        }
        
        av_packet_unref(&Packet);
        ErrCode = 0;
    }
    
    // Input file processing done, flushing decoder
    Packet.data = nullptr;
    Packet.size = 0;
    
    do {
        avcodec_decode_audio4(CodecContext_In, Frame, &WasFrameDecoded, &Packet);
        if (WasFrameDecoded)
        {
            ErrCode = (this->*resampleFunction)(ResampleContext, Frame, SampleFifo);
            if (ErrCode < 0)
            {
                cleanUp_ConvertAudio(FfmpegCleanUpLevelCode_ConvertAudio::LEVEL_AVIO, &Frame, &ResampleContext);
                throw FfmpegException_t(FfmpegErrorCode::RESAMPLE_CONVERT, 0);
            }
        }
    } while (WasFrameDecoded);
    
    // Flushing ResampleContext
    if (DoResample)
    {
        do {
            ErrCode = swr_convert(ResampleContext, ResamplingBuffer, ResamplingBufferSize, nullptr, 0);
            if (ErrCode > 0) SampleFifo.insert(SampleFifo.end(), *ResamplingBuffer, *ResamplingBuffer + (ErrCode << 2) );
            else if (ErrCode < 0)
            {
                cleanUp_ConvertAudio(FfmpegCleanUpLevelCode_ConvertAudio::LEVEL_AVIO, &Frame, &ResampleContext);
                throw FfmpegException_t(FfmpegErrorCode::RESAMPLE_CONVERT, 0);
            }
        } while (ErrCode > 0);
        
        av_freep(ResamplingBuffer);
    }
    
    // Flushing SampleFifo
    while (!SampleFifo.empty() )
    {
        if (SampleFifo.size() >= PACKET_WAV_SAMPLE_COUNT << 2) ErrCode = av_new_packet(&Packet_Out, PACKET_WAV_SAMPLE_COUNT << 2);
        else ErrCode = av_new_packet(&Packet_Out, SampleFifo.size() );
        
        if (ErrCode < 0)
        {
            cleanUp_ConvertAudio(FfmpegCleanUpLevelCode_ConvertAudio::LEVEL_AVIO, &Frame, &ResampleContext);
            throw FfmpegException_t(FfmpegErrorCode::PACKET_ALLOC, ErrCode);
        }
        
        prepareOutputPacketAndWriteIt(Packet_Out, SampleFifo, Frame, ResampleContext, false);
    }
    
    ErrCode = av_write_trailer(Container_Out);
    if (ErrCode)
    {
        cleanUp_ConvertAudio(FfmpegCleanUpLevelCode_ConvertAudio::LEVEL_AVIO, &Frame, &ResampleContext);
        throw FfmpegException_t(FfmpegErrorCode::CONTAINER_OUT_WRITE_TRAILER, ErrCode);
    }
    
    cleanUp_ConvertAudio(FfmpegCleanUpLevelCode_ConvertAudio::LEVEL_AVIO, &Frame, &ResampleContext);
}

void Ffmpeg_t::prepareOutputPacketAndWriteIt(AVPacket &Packet_Out, std::vector<uint8_t> &SampleFifo, AVFrame *Frame, SwrContext *ResampleContext, bool FreePacket)
{
    Packet_Out.duration = Packet_Out.size >> 2;
    Packet_Out.stream_index = 0;
    Packet_Out.pts = Duration;
    Packet_Out.dts = Duration;
    Packet_Out.pos = -1;
    
    Duration += Packet_Out.duration;
    
    memcpy(Packet_Out.data, SampleFifo.data(), Packet_Out.size);
    SampleFifo.erase(SampleFifo.begin(), SampleFifo.begin() + Packet_Out.size);
    
    int32_t ErrCode = av_interleaved_write_frame(Container_Out, &Packet_Out);
    av_packet_unref(&Packet_Out);
    if (ErrCode < 0)
    {
        if (FreePacket) cleanUp_ConvertAudio(FfmpegCleanUpLevelCode_ConvertAudio::LEVEL_PACKET, &Frame, &ResampleContext);
        else cleanUp_ConvertAudio(FfmpegCleanUpLevelCode_ConvertAudio::LEVEL_AVIO, &Frame, &ResampleContext);
        throw FfmpegException_t(FfmpegErrorCode::CONTAINER_OUT_WRITE_FRAME, ErrCode);
    }
}

int32_t Ffmpeg_t::resample_AndStore(SwrContext *ResampleContext, AVFrame *Frame, std::vector<uint8_t> &SampleFifo)
{
    int32_t ErrCode = 0;
    int32_t GeneratedSamplesCount = av_rescale_rnd(Frame->nb_samples, 44100, Container_In->streams[StreamIndex]->codec->sample_rate, AV_ROUND_UP);
    
    if (ResamplingBufferSize < GeneratedSamplesCount)
    {
        av_freep(ResamplingBuffer);
        ErrCode = av_samples_alloc(ResamplingBuffer, nullptr, 2, GeneratedSamplesCount, AV_SAMPLE_FMT_S16, 0);
        ResamplingBufferSize = GeneratedSamplesCount;
    }
    
    if (ErrCode >= 0)
    {
        ErrCode = swr_convert(ResampleContext, ResamplingBuffer, ResamplingBufferSize, const_cast<const uint8_t**>(Frame->data), Frame->nb_samples);
        if (ErrCode > 0) SampleFifo.insert(SampleFifo.end(), *ResamplingBuffer, *ResamplingBuffer + (ErrCode << 2) );
        else av_freep(ResamplingBuffer);
    }
    else
    {
        cleanUp_ConvertAudio(FfmpegCleanUpLevelCode_ConvertAudio::LEVEL_PACKET, &Frame, &ResampleContext);
        throw FfmpegException_t(FfmpegErrorCode::RESAMPLE_BUFFER_ALLOC, 0);
    }
    
    return ErrCode;
}

int32_t Ffmpeg_t::resample_JustStore(SwrContext *, AVFrame *Frame, std::vector<uint8_t> &SampleFifo)
{
    if (Container_In->streams[StreamIndex]->codec->sample_fmt == AV_SAMPLE_FMT_S16)
        SampleFifo.insert(SampleFifo.end(), Frame->data[0], Frame->data[0] + (Frame->nb_samples << 2) );
    else
    {
        uint16_t BytesPerChannel = Frame->nb_samples;
        for (uint16_t i = 0; i < BytesPerChannel; i++)
        {
            SampleFifo.push_back(Frame->data[0][(i << 1)]);
            SampleFifo.push_back(Frame->data[0][(i << 1) + 1]);
            SampleFifo.push_back(Frame->data[1][(i << 1)]);
            SampleFifo.push_back(Frame->data[1][(i << 1) + 1]);
        }
    }
    
    return 0;
}

uint64_t Ffmpeg_t::getAudioDuration(std::string FileName)
{
    initInputFileAudio(FileName);
    
    double SampleRate = Container_In->streams[0]->codec->sample_rate;
    uint64_t Duration_miliseconds = std::llround( (Container_In->streams[0]->duration / SampleRate) * 1000);
    
    avformat_close_input(&Container_In);
    
    return Duration_miliseconds;
}

void Ffmpeg_t::initInputFileAudio(std::string &FileName)
{
    Container_In = avformat_alloc_context();
    if (!Container_In) throw FfmpegException_t(FfmpegErrorCode::CONTAINER_IN_ALLOC, 0);
    
    int32_t ErrCode = avformat_open_input(&Container_In, FileName.c_str(), nullptr, nullptr);
    if (ErrCode < 0)
    {
        avformat_free_context(Container_In);
        throw FfmpegException_t(FfmpegErrorCode::CONTAINER_IN_OPEN, ErrCode);
    }
    
    ErrCode = avformat_find_stream_info(Container_In, nullptr);
    if (ErrCode < 0)
    {
        cleanUp_SplitTrack(FfmpegCleanUpLevelCode_SplitTrack::LEVEL_AVFORMAT_INPUT);
        throw FfmpegException_t(FfmpegErrorCode::CONTAINER_IN_STREAM_INFO, ErrCode);
    }
    
    StreamIndex = 0;
    if (Container_In->nb_streams > 1)
    {
        fprintf(stderr, "Warning: more than 1 stream in input file %s detected. Selecting first audio stream.\n", FileName.c_str() );
        while ( (StreamIndex < Container_In->nb_streams) && (Container_In->streams[StreamIndex]->codec->codec_type != AVMEDIA_TYPE_AUDIO) ) StreamIndex++;
    }
    
    if ( (StreamIndex == Container_In->nb_streams) || (Container_In->streams[StreamIndex]->codec->codec_type != AVMEDIA_TYPE_AUDIO) )
    {
        cleanUp_SplitTrack(FfmpegCleanUpLevelCode_SplitTrack::LEVEL_AVFORMAT_INPUT);
        throw FfmpegException_t(FfmpegErrorCode::CONTAINER_IN_STREAM_NOT_AUDIO, 0);
    }
    
    av_init_packet(&Packet);
}

void Ffmpeg_t::splitTrack(std::string FileName, uint32_t SplitDuration)
{
    if (!SplitDuration) throw FfmpegException_t(FfmpegErrorCode::SPLIT_DURATION_1_EMPTY, 0);
    
    std::string SplitFile_1, SplitFile_2;
    SplitFile_1 = SplitFile_2 = FileName;
    
    // Creating file names for split tracks
    SplitFile_1.replace(SplitFile_1.find_last_of("."), 1, "_1.");
    SplitFile_2.replace(SplitFile_2.find_last_of("."), 1, "_2.");
    
    // Transforming miliseconds to sample count (44.1 samples = 1 ms when 44100 KHz sampling freq)
    SplitDuration = std::lround(SplitDuration * 44.1);
    
    initInputFileAudio(FileName);
    
    av_dump_format(Container_In, 0, FileName.c_str(), 0);
    
    writePacketsToFile(SplitFile_1, SplitDuration);
    writePacketsToFile(SplitFile_2, 0);
    
    avformat_close_input(&Container_In);
}

void Ffmpeg_t::writePacketsToFile(std::string &SplitFile, uint32_t SplitDuration)
{
    int32_t ErrCode = avformat_alloc_output_context2(&Container_Out, nullptr, nullptr, SplitFile.c_str() );
    if (ErrCode < 0)
    {
        cleanUp_SplitTrack(FfmpegCleanUpLevelCode_SplitTrack::LEVEL_AVFORMAT_INPUT);
        throw FfmpegException_t(FfmpegErrorCode::CONTAINER_OUT_ALLOC, ErrCode);
    }
    
    AVStream *Stream_Out = avformat_new_stream(Container_Out, avcodec_find_decoder(Container_In->streams[0]->codec->codec_id) );
    if (!Stream_Out)
    {
        cleanUp_SplitTrack(FfmpegCleanUpLevelCode_SplitTrack::LEVEL_AVFORMAT_CONTEXT);
        throw FfmpegException_t(FfmpegErrorCode::CONTAINER_OUT_NEW_STREAM, 0);
    }
    
    ErrCode = avcodec_copy_context(Stream_Out->codec, Container_In->streams[0]->codec);
    if (ErrCode < 0)
    {
        cleanUp_SplitTrack(FfmpegCleanUpLevelCode_SplitTrack::LEVEL_AVFORMAT_CONTEXT);
        throw FfmpegException_t(FfmpegErrorCode::CONTAINER_OUT_COPY_CODEC_CONTEXT, ErrCode);
    }
    
    // Output stream has the same time base as input since they share properties
    Stream_Out->time_base = Container_In->streams[0]->time_base;
    Stream_Out->codec->codec_tag = 0;
    if (Container_Out->oformat->flags & AVFMT_GLOBALHEADER) Stream_Out->codec->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;
    
    // If output needs a file, open it
    if (!(Container_Out->oformat->flags & AVFMT_NOFILE) )
    {
        ErrCode = avio_open(&Container_Out->pb, SplitFile.c_str(), AVIO_FLAG_WRITE);
        if (ErrCode < 0)
        {
            cleanUp_SplitTrack(FfmpegCleanUpLevelCode_SplitTrack::LEVEL_AVFORMAT_CONTEXT);
            throw FfmpegException_t(FfmpegErrorCode::CONTAINER_OUT_OPEN_FILE, ErrCode);
        }
    }
    
    // Writing header info about streams to file
    ErrCode = avformat_write_header(Container_Out, nullptr);
    if (ErrCode < 0)
    {
        cleanUp_SplitTrack(FfmpegCleanUpLevelCode_SplitTrack::LEVEL_AVIO);
        throw FfmpegException_t(FfmpegErrorCode::CONTAINER_OUT_WRITE_HEADER, ErrCode);
    }
    
    /* ##### CONTAINER READY ##### */
    
    Duration = 0;
    
    /* End it when:
     * 1) one of the functions fails
     * 2) EOF
     * 3) we reached the desired duration
     * ----------------------------------
     * (Duration < SplitDuration) || (!SplitDuration)
     * - SplitDuration == 0 -> (Duration < SplitDuration) == false -> until EOF
     * - SplitDuration > 0 -> until (Duration < SplitDuration) == false -> until desired duration reached
     */
    while ( (!ErrCode) && ( (Duration < SplitDuration) || (!SplitDuration) ) )
    {
        ErrCode = av_read_frame(Container_In, &Packet);
        if (ErrCode < 0)
        {
            if (ErrCode == AVERROR_EOF) continue;
            else
            {
                cleanUp_SplitTrack(FfmpegCleanUpLevelCode_SplitTrack::LEVEL_AVIO);
                throw FfmpegException_t(FfmpegErrorCode::CONTAINER_IN_READ_FRAME, ErrCode);
            }
        }
        
        if (!SplitDuration) // When flushing rest of the audio, need to start counting pts and dts from 0
        {
            Packet.pts = Duration;
            Packet.dts = Duration;
        }
        
        /* SampleBuffer is not empty, which means the file was split
         * somewhere in the packet, so the remaining samples are in the SampleBuffer.
         * They need to be put before samples from the currently obtained packet.
         */
        if (SampleCount)
        {
            uint8_t TmpBuffer[(PACKET_WAV_SAMPLE_COUNT - 1) * 4]; // 1 sample for 1 channel = 16 bits * 2 channels = 4B
            const uint16_t PacketMaxSampleCountMinusSampleCount = PACKET_WAV_SAMPLE_COUNT - SampleCount; // It repeats, saving some time
            
            /* If the currently obtained packet is not full, it means it's the last one. There's not
             * enough space allocated in the packet to hold any additional samples from the SampleBuffer,
             * therefore we need to allocated a completely new packet with enough space.
             */
            if (Packet.duration < PACKET_WAV_SAMPLE_COUNT)
            {
                int32_t PacketDuration = Packet.duration;
                memcpy(TmpBuffer, Packet.data, PacketDuration << 2); // == PacketDuration * 4
                av_packet_unref(&Packet);
                
                /* Are samples from the obtained packet and SampleBuffer together larger than a packet?
                 * Saving some time since it repeats
                 */
                const bool Condition = ( (PacketDuration + SampleCount) > PACKET_WAV_SAMPLE_COUNT);
                
                // Allocate exact space if data is smaller than a regular packet, or allocate packet size if it's larger
                ErrCode = av_new_packet(&Packet, (Condition ? 4096 : (PacketDuration + SampleCount) << 2) );
                if (ErrCode)
                {
                    cleanUp_SplitTrack(FfmpegCleanUpLevelCode_SplitTrack::LEVEL_AVIO);
                    throw FfmpegException_t(FfmpegErrorCode::PACKET_ALLOC, ErrCode);
                }
                
                Packet.stream_index = 0;
                Packet.pts = Duration;
                Packet.dts = Duration;
                
                // Copying samples from SampleBuffer followed by samples from the last packet
                memcpy(Packet.data, SampleBuffer, SampleCount << 2);
                memcpy(Packet.data + (SampleCount << 2), TmpBuffer, (Condition ? PacketMaxSampleCountMinusSampleCount << 2 : PacketDuration << 2) );
                
                // If true, data > packet, meaning there are samples left in the SampleBuffer, needs to be flushed
                if (Condition)
                {
                    memcpy(SampleBuffer, TmpBuffer + (PacketMaxSampleCountMinusSampleCount << 2), (PacketDuration - PacketMaxSampleCountMinusSampleCount) << 2 );
                    Packet.duration = PACKET_WAV_SAMPLE_COUNT;
                    SampleCount = PacketDuration - PacketMaxSampleCountMinusSampleCount;
                }
                else // Everything was able to fit into the last allocated packet
                {
                    Packet.duration = PacketDuration + SampleCount;
                    SampleCount = 0;
                }
            }
            /* Obtained packet is full, which means we need to copy out SampleCount samples from the end to TmpBuffer,
             * shift 1024 - SampleCount samples from the beginning to the end of the packet, copy in samples from SampleBuffer
             * to the beginning of the packet and overwrite samples in SampleBuffer with those in TmpBuffer.
             * Improvement: instead of the last overwrite, buffers are just switched
             */
            else
            {
                memcpy(TmpBuffer, Packet.data + (PacketMaxSampleCountMinusSampleCount << 2), SampleCount << 2);
                memmove(Packet.data + (SampleCount << 2), Packet.data, PacketMaxSampleCountMinusSampleCount << 2);
                memcpy(Packet.data, SampleBuffer, SampleCount << 2);
                std::swap(TmpBuffer, SampleBuffer); // == memcpy(SampleBuffer, TmpBuffer, SampleCount << 2);
            }
        }
        
        /* When we're proccesing the first file (SplitDuration > 0) and we would exceed the desired
         * duration if we were to write the whole packet, it's necessary to write only a part of the packet
         */
        if ( SplitDuration && ( (Duration + Packet.duration) > SplitDuration) )
        {
            Packet.duration = SplitDuration - Duration;
            Packet.size = Packet.duration << 2;
            
            // Put the leftover samples in the SampleBuffer
            SampleCount = PACKET_WAV_SAMPLE_COUNT - Packet.duration;
            std::memcpy(SampleBuffer, Packet.data + (Packet.duration << 2), SampleCount << 2);
        }
        
        Packet.pos = -1;
        Duration += Packet.duration;
        
        ErrCode = av_interleaved_write_frame(Container_Out, &Packet);
        if (ErrCode < 0)
        {
            cleanUp_SplitTrack(FfmpegCleanUpLevelCode_SplitTrack::LEVEL_AVIO);
            throw FfmpegException_t(FfmpegErrorCode::CONTAINER_OUT_WRITE_FRAME, ErrCode);
        }
        
        av_packet_unref(&Packet);
    }
    
    /* When SplitDuration == 0 (flushing rest of the audio) and SampleBuffer holds samples,
     * we want to empty the buffer. Since input file is already depleted,
     * we need to allocate a new packet.
     */
    if (!SplitDuration && SampleCount)
    {
        ErrCode = av_new_packet(&Packet, SampleCount << 2);
        if (ErrCode < 0)
        {
            cleanUp_SplitTrack(FfmpegCleanUpLevelCode_SplitTrack::LEVEL_AVIO);
            throw FfmpegException_t(FfmpegErrorCode::PACKET_ALLOC, ErrCode);
        }
        
        Packet.duration = SampleCount;
        Packet.stream_index = 0;
        Packet.pts = Duration;
        Packet.dts = Duration;
        Packet.pos = -1;
        
        memcpy(Packet.data, SampleBuffer, SampleCount << 2);
        
        ErrCode = av_interleaved_write_frame(Container_Out, &Packet);
        if (ErrCode < 0)
        {
            cleanUp_SplitTrack(FfmpegCleanUpLevelCode_SplitTrack::LEVEL_AVIO);
            throw FfmpegException_t(FfmpegErrorCode::CONTAINER_OUT_WRITE_FRAME, ErrCode);
        }
        
        av_packet_unref(&Packet);
        SampleCount = 0;
    }
    
    // Finish the stream data input
    ErrCode = av_write_trailer(Container_Out);
    if (ErrCode)
    {
        cleanUp_SplitTrack(FfmpegCleanUpLevelCode_SplitTrack::LEVEL_AVIO);
        throw FfmpegException_t(FfmpegErrorCode::CONTAINER_OUT_WRITE_TRAILER, ErrCode);
    }
    
    cleanUp_SplitTrack(FfmpegCleanUpLevelCode_SplitTrack::LEVEL_AVIO, false);
}
