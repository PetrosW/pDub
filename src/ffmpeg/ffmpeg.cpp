#include <ffmpeg/ffmpeg.hpp>

Ffmpeg_t::Ffmpeg_t() : Packet{}, SampleCount(0)
{
    av_register_all();
}

void Ffmpeg_t::cleanUp_SplitTrack(FfmpegCleanUpLevelCode::Type Level, bool CloseInput)
{
    switch (Level)
    {
        case FfmpegCleanUpLevelCode::LEVEL_AVIO:
            avio_closep(&Container_Out->pb);
        
        case FfmpegCleanUpLevelCode::LEVEL_AVFORMAT_CONTEXT:
            avformat_free_context(Container_Out);
        
        case FfmpegCleanUpLevelCode::LEVEL_AVFORMAT_INPUT:
            if (CloseInput) avformat_close_input(&Container_In);
        break;
        
        default:
        return;
    }
}

void Ffmpeg_t::splitTrack(std::string FileName, uint64_t SplitDuration_1, uint64_t SplitDuration_2)
{
    if (!SplitDuration_1) throw FfmpegException_t(FfmpegErrorCode::SPLIT_DURATION_1_EMPTY, 0);
    
    std::string SplitFile_1 = FileName;
    std::string SplitFile_2 = FileName;
    
    SplitFile_1.replace(SplitFile_1.find_last_of("."), 1, "_1.");
    SplitFile_2.replace(SplitFile_2.find_last_of("."), 1, "_2.");
    
    // Transforming miliseconds to sample count (44.1 samples = 1 ms when 44100 KHz sampling freq)
    SplitDuration_1 = std::llround(SplitDuration_1 * 44.1);
    SplitDuration_2 = std::llround(SplitDuration_2 * 44.1);
    
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
        cleanUp_SplitTrack(FfmpegCleanUpLevelCode::LEVEL_AVFORMAT_INPUT);
        throw FfmpegException_t(FfmpegErrorCode::CONTAINER_IN_STREAM_INFO, ErrCode);
    }
    
    if (Container_In->nb_streams > 1) fprintf(stderr, "Warning: more than 1 stream in input file %s detected. Selecting first stream.", FileName.c_str() );
    
    if (Container_In->streams[0]->codec->codec_type != AVMEDIA_TYPE_AUDIO)
    {
        cleanUp_SplitTrack(FfmpegCleanUpLevelCode::LEVEL_AVFORMAT_INPUT);
        throw FfmpegException_t(FfmpegErrorCode::CONTAINER_IN_STREAM_NOT_AUDIO, 0);
    }
    
    av_dump_format(Container_In, 0, FileName.c_str(), 0);
    
    writePacketsToFile(SplitFile_1, SplitDuration_1);
    writePacketsToFile(SplitFile_2, SplitDuration_2);
    
    avformat_close_input(&Container_In);
}

void Ffmpeg_t::writePacketsToFile(std::string &SplitFile, uint64_t SplitDuration)
{
    int32_t ErrCode = avformat_alloc_output_context2(&Container_Out, nullptr, nullptr, SplitFile.c_str() );
    if (ErrCode < 0)
    {
        cleanUp_SplitTrack(FfmpegCleanUpLevelCode::LEVEL_AVFORMAT_INPUT);
        throw FfmpegException_t(FfmpegErrorCode::CONTAINER_OUT_ALLOC, ErrCode);
    }
    
    AVStream *Stream_Out = avformat_new_stream(Container_Out, Container_In->streams[0]->codec->codec);
    if (!Stream_Out)
    {
        cleanUp_SplitTrack(FfmpegCleanUpLevelCode::LEVEL_AVFORMAT_CONTEXT);
        throw FfmpegException_t(FfmpegErrorCode::CONTAINER_OUT_NEW_STREAM, 0);
    }
    
    ErrCode = avcodec_copy_context(Stream_Out->codec, Container_In->streams[0]->codec);
    if (ErrCode < 0)
    {
        cleanUp_SplitTrack(FfmpegCleanUpLevelCode::LEVEL_AVFORMAT_CONTEXT);
        throw FfmpegException_t(FfmpegErrorCode::CONTAINER_OUT_COPY_CODEC_CONTEXT, ErrCode);
    }
    
    Stream_Out->time_base = Container_In->streams[0]->time_base;
    Stream_Out->codec->codec_tag = 0;
    if (Container_Out->oformat->flags & AVFMT_GLOBALHEADER) Stream_Out->codec->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;
    
    if (!(Container_Out->oformat->flags & AVFMT_NOFILE) )
    {
        ErrCode = avio_open(&Container_Out->pb, SplitFile.c_str(), AVIO_FLAG_WRITE);
        if (ErrCode < 0)
        {
            cleanUp_SplitTrack(FfmpegCleanUpLevelCode::LEVEL_AVFORMAT_CONTEXT);
            throw FfmpegException_t(FfmpegErrorCode::CONTAINER_OUT_OPEN_FILE, ErrCode);
        }
    }
    
    ErrCode = avformat_write_header(Container_Out, nullptr);
    if (ErrCode < 0)
    {
        cleanUp_SplitTrack(FfmpegCleanUpLevelCode::LEVEL_AVIO);
        throw FfmpegException_t(FfmpegErrorCode::CONTAINER_OUT_WRITE_HEADER, ErrCode);
    }
    
    av_init_packet(&Packet);
    
    /* ##### CONTAINER READY ##### */
    
    uint64_t Duration = 0;
    
    /* End it when:
     * 1) one of the functions fails
     * 2) EOF
     * 3) we reached the desired duration
     * ----------------------------------
     * (Duration < SplitDuration) || (!SplitDuration)
     * - SplitDuration = 0 -> (Duration < SplitDuration) == false -> until EOF
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
                cleanUp_SplitTrack(FfmpegCleanUpLevelCode::LEVEL_AVIO);
                throw FfmpegException_t(FfmpegErrorCode::CONTAINER_IN_READ_FRAME, ErrCode);
            }
        }
        
        if (!SplitDuration) // When flushing rest of the audio, need to start counting pts and dts from 0
        {
            Packet.pts = Duration;
            Packet.dts = Duration;
        }
        
        if (SampleCount)
        {
            uint8_t TmpBuffer[4092];
            
            if (Packet.duration < 1024)
            {
                int64_t PacketDuration = Packet.duration;
                memcpy(TmpBuffer, Packet.data, PacketDuration << 2);
                av_free_packet(&Packet);
                
                const bool Condition = ( (PacketDuration + SampleCount) > 1024);
                
                ErrCode = av_new_packet(&Packet, (Condition ? 4096 : (PacketDuration + SampleCount) << 2) );
                if (ErrCode)
                {
                    cleanUp_SplitTrack(FfmpegCleanUpLevelCode::LEVEL_AVIO);
                    throw FfmpegException_t(FfmpegErrorCode::PACKET_ALLOC, ErrCode);
                }
                
                Packet.stream_index = 0;
                Packet.pts = Duration;
                Packet.dts = Duration;
                
                memcpy(Packet.data, SampleBuffer, SampleCount << 2);
                memcpy(Packet.data + (SampleCount << 2), TmpBuffer, (Condition ? (1024 - SampleCount) << 2 : PacketDuration << 2) );
                
                if (Condition)
                {
                    memcpy(SampleBuffer, TmpBuffer + ( (1024 - SampleCount) << 2), (PacketDuration - (1024 - SampleCount) ) << 2 );
                    Packet.duration = 1024;
                    SampleCount = PacketDuration - (1024 - SampleCount);
                }
                else
                {
                    Packet.duration = PacketDuration + SampleCount;
                    SampleCount = 0;
                }
            }
            else
            {
                memcpy(TmpBuffer, Packet.data + ( (1024 - SampleCount) << 2), SampleCount << 2);
                memmove(Packet.data + (SampleCount << 2), Packet.data, (1024 - SampleCount) << 2);
                memcpy(Packet.data, SampleBuffer, SampleCount << 2);
                memcpy(SampleBuffer, TmpBuffer, SampleCount << 2);
            }
        }
        
        if ( SplitDuration && ( (Duration + Packet.duration) > SplitDuration) )
        {
            Packet.duration = SplitDuration - Duration;
            Packet.size = Packet.duration << 2; // Packet.duration * 4, 1 sample for 1 channel = 16 bits * 2 channels = 4B
            
            SampleCount = 1024 - Packet.duration; // 1024 samples form one packet
            std::memcpy(SampleBuffer, Packet.data + (Packet.duration << 2), SampleCount << 2);
        }
        
        Packet.pos = -1;
        Duration += Packet.duration;
        
        ErrCode = av_interleaved_write_frame(Container_Out, &Packet);
        if (ErrCode < 0)
        {
            cleanUp_SplitTrack(FfmpegCleanUpLevelCode::LEVEL_AVIO);
            throw FfmpegException_t(FfmpegErrorCode::CONTAINER_OUT_WRITE_FRAME, ErrCode);
        }
        
        av_free_packet(&Packet);
    }
    
    // When SplitDuration == 0 (flushing rest of the audio) and SampleBuffer holds samples, we want to empty the buffer
    if (!SplitDuration && SampleCount)
    {
        ErrCode = av_new_packet(&Packet, SampleCount << 2);
        if (ErrCode < 0)
        {
            cleanUp_SplitTrack(FfmpegCleanUpLevelCode::LEVEL_AVIO);
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
            cleanUp_SplitTrack(FfmpegCleanUpLevelCode::LEVEL_AVIO);
            throw FfmpegException_t(FfmpegErrorCode::CONTAINER_OUT_WRITE_FRAME, ErrCode);
        }
        
        av_free_packet(&Packet);
        SampleCount = 0;
    }
    
    ErrCode = av_write_trailer(Container_Out);
    if (ErrCode != 0)
    {
        cleanUp_SplitTrack(FfmpegCleanUpLevelCode::LEVEL_AVIO);
        throw FfmpegException_t(FfmpegErrorCode::CONTAINER_OUT_WRITE_TRAILER, ErrCode);
    }
    
    cleanUp_SplitTrack(FfmpegCleanUpLevelCode::LEVEL_AVIO, false);
}
