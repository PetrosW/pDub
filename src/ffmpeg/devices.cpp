#include <ffmpeg/devices.hpp>

/* ##### InputDevice ##### */

InputDevice::InputDevice(AVFormatContext *InputTrack) : InputTrack(InputTrack), SampleBuffer_Input({ {0}, {0}, 0}), Packet{}
{
    av_init_packet(&Packet);
}

InputDevice::~InputDevice()
{
    avformat_close_input(&InputTrack);
}

int32_t InputDevice::getSamples(uint16_t SampleCount, OutputBuffer_t &OutputBuffer)
{
    assert(SampleCount <= PACKET_MP3_SAMPLE_COUNT);
    
    uint16_t Count = (SampleBuffer_Input.SampleCount > SampleCount ? SampleCount : SampleBuffer_Input.SampleCount);
    
    assert( (OutputBuffer.SampleCount + Count) <= PACKET_MP3_SAMPLE_COUNT);
    
    // parallel
    for (uint16_t i = 0; i < Count; i++)
    {
        OutputBuffer.ChannelLeft[OutputBuffer.SampleCount + i] = SampleBuffer_Input.ChannelLeft[i] / 32768.0;
        OutputBuffer.ChannelRight[OutputBuffer.SampleCount + i] = SampleBuffer_Input.ChannelRight[i] / 32768.0;
    }
    OutputBuffer.SampleCount += Count;
    
    memmove(SampleBuffer_Input.ChannelLeft, SampleBuffer_Input.ChannelLeft + Count, (SampleBuffer_Input.SampleCount - Count) * sizeof(int16_t) );
    memmove(SampleBuffer_Input.ChannelRight, SampleBuffer_Input.ChannelRight + Count, (SampleBuffer_Input.SampleCount - Count) * sizeof(int16_t) );
    SampleBuffer_Input.SampleCount -= Count;
    
    SampleCount -= Count;
    
    // If what's in buffer is not enough, get more samples from file
    while (SampleCount > 0)
    {
        int32_t ErrCode = av_read_frame(InputTrack, &Packet);
        if (ErrCode < 0) return ErrCode;
        
        uint16_t PacketLength = Packet.duration;
        int16_t *TmpPtr = reinterpret_cast<int16_t *>(Packet.data);
        
        Count = (SampleCount >= PACKET_WAV_SAMPLE_COUNT ? PACKET_WAV_SAMPLE_COUNT : SampleCount);
        
        // parallel
        for (uint16_t i = 0; i < Count; i++)
        {
            OutputBuffer.ChannelLeft[OutputBuffer.SampleCount + i] = TmpPtr[i << 1] / 32768.0;
            OutputBuffer.ChannelRight[OutputBuffer.SampleCount + i] = TmpPtr[(i << 1) + 1] / 32768.0;
        }
        
        OutputBuffer.SampleCount += Count;
        SampleCount -= Count;
        PacketLength -= Count;
        
        assert( ( (PacketLength >= 0) && (SampleCount == 0) ) || ( (PacketLength == 0) && (SampleCount > 0) ) );
        // Store the remaining samples from the packet in the buffer
        if (PacketLength)
        {
            // parallel
            uint16_t j = 0;
            for (uint16_t i = Count; i < Packet.duration; i++, j++)
            {
                SampleBuffer_Input.ChannelLeft[j] = TmpPtr[i << 1];
                SampleBuffer_Input.ChannelRight[j] = TmpPtr[(i << 1) + 1];
            }
            SampleBuffer_Input.SampleCount = PacketLength;
        }
        
        av_packet_unref(&Packet);
    }
    
    return 0;
}

int32_t InputDevice::skipSamples(uint64_t SampleCount)
{
    // Nenastane, nebot skip bude vzdy jen na zacatku, kdy je buffer prazdny
    /*if (SampleBuffer_Input.SampleCount > SampleCount)
    {
        SampleBuffer_Input.SampleCount -= SampleCount;
        SampleCount = 0;
    }
    else
    {
        SampleCount -= SampleBuffer_Input.SampleCount;
        SampleBuffer_Input.SampleCount = 0;
    }*/
    
    while (SampleCount > 0)
    {
        int32_t ErrCode = av_read_frame(InputTrack, &Packet);
        if (ErrCode < 0) return ErrCode;
        
        if (SampleCount < PACKET_WAV_SAMPLE_COUNT)
        {
            int16_t *TmpPtr = reinterpret_cast<int16_t *>(Packet.data);
            
            //parallel
            for (uint16_t i = SampleCount; i < Packet.duration; i++)
            {
                SampleBuffer_Input.ChannelLeft[i] = TmpPtr[i << 1];
                SampleBuffer_Input.ChannelRight[i] = TmpPtr[(i << 1) + 1];
            }
            SampleBuffer_Input.SampleCount = Packet.duration - SampleCount;
            SampleCount = 0;
        }
        else SampleCount -= PACKET_WAV_SAMPLE_COUNT;
        
        av_packet_unref(&Packet);
    }
    
    return 0;
}

int32_t InputDevice::mixSamples(uint16_t SampleCount, OutputBuffer_t &MixWithBuffer, double MultiplicationConst)
{
    OutputBuffer_t TmpBuffer = { {0.0}, {0.0}, 0};
    int32_t ErrCode = getSamples(SampleCount, TmpBuffer);
    if (ErrCode) return ErrCode;
    
    // parallel
    for (uint16_t i = 0; i < SampleCount; i++)
    {
        MixWithBuffer.ChannelLeft[i] += (TmpBuffer.ChannelLeft[i]/* * MultiplicationConst*/);
        MixWithBuffer.ChannelRight[i] += (TmpBuffer.ChannelRight[i]/* * MultiplicationConst*/);
        
        if (MixWithBuffer.ChannelLeft[i] > 0.999969482421875) MixWithBuffer.ChannelLeft[i] = 0.999969482421875;
        if (MixWithBuffer.ChannelRight[i] > 0.999969482421875) MixWithBuffer.ChannelRight[i] = 0.999969482421875;
        
        if (MixWithBuffer.ChannelLeft[i] < -1.0) MixWithBuffer.ChannelLeft[i] = -1.0;
        if (MixWithBuffer.ChannelRight[i] < -1.0) MixWithBuffer.ChannelRight[i] = -1.0;
    }
    
    return 0;
}

/* ##### OutputDevice ##### */

OutputDevice::OutputDevice(AVFormatContext *Container, AVCodecContext *CodecContext, uint8_t StreamIndex)
    : Pts(0), Container(Container), CodecContext(CodecContext), SampleBuffer_Output({ {0.0}, {0.0}, 0}), StreamIndex_Audio(StreamIndex), Packet{}
{
     Frame = av_frame_alloc();
     if (!Frame) AllocFrameSuccess = false;
     else
     {
         AllocFrameSuccess = true;
        
         Frame->format = AV_SAMPLE_FMT_S16P;
         Frame->channel_layout = AV_CH_LAYOUT_STEREO;
         Frame->nb_samples = PACKET_MP3_SAMPLE_COUNT;
         
         int32_t ErrCode = av_frame_get_buffer(Frame, 0);
         if (ErrCode) AllocFrameBufferSuccess = false;
         else AllocFrameBufferSuccess = true;
         
         av_init_packet(&Packet);
     }
}

OutputDevice::~OutputDevice()
{
    if (Frame) av_frame_free(&Frame);
}

std::pair<FfmpegErrorCode::Type, int32_t> OutputDevice::putSamples(OutputBuffer_t &InputBuffer, AVRational &CodecBase, AVRational &StreamBase)
{
    assert(InputBuffer.SampleCount <= PACKET_MP3_SAMPLE_COUNT);
    
    if ( (InputBuffer.SampleCount + SampleBuffer_Output.SampleCount) >= PACKET_MP3_SAMPLE_COUNT)
    {
        uint16_t AdditionalSamples = PACKET_MP3_SAMPLE_COUNT - SampleBuffer_Output.SampleCount;
        
        //printf("lol\n");
        //int32_t ErrCode = av_frame_get_buffer(Frame, 0);
        
        //av_frame_make_writable(Frame);
        // Don't want to put the whole exportProject() into a try/catch block
        //if (ErrCode) return std::make_pair(FfmpegErrorCode::FRAME_ALLOC_BUFFER, 0);
        
        int16_t *ChLeftPtr = reinterpret_cast<int16_t *>(Frame->data[0]);
        int16_t *ChRightPtr = reinterpret_cast<int16_t *>(Frame->data[1]);
        
        // parallel
        for (uint16_t i = 0; i < SampleBuffer_Output.SampleCount; i++)
        {
            ChLeftPtr[i] = SampleBuffer_Output.ChannelLeft[i] * 32768.0;
            ChRightPtr[i] = SampleBuffer_Output.ChannelRight[i] * 32768.0;
        }
            
        // parallel
        for (uint16_t i = 0; i < AdditionalSamples; i++)
        {
            ChLeftPtr[SampleBuffer_Output.SampleCount + i] = InputBuffer.ChannelLeft[i] * 32768.0;
            ChRightPtr[SampleBuffer_Output.SampleCount + i] = InputBuffer.ChannelRight[i] * 32768.0;
        }
        
        // Store unused samples in SampleBuffer_Output
        SampleBuffer_Output.SampleCount = InputBuffer.SampleCount - AdditionalSamples;
        
        memcpy(SampleBuffer_Output.ChannelLeft, InputBuffer.ChannelLeft + AdditionalSamples, SampleBuffer_Output.SampleCount * sizeof(double) );
        memcpy(SampleBuffer_Output.ChannelRight, InputBuffer.ChannelRight + AdditionalSamples, SampleBuffer_Output.SampleCount * sizeof(double) );
        
        InputBuffer.SampleCount = 0;
        
        // Encode the frame
        int32_t WasFrameEncoded;
        
        int32_t ErrCode = avcodec_encode_audio2(CodecContext, &Packet, Frame, &WasFrameEncoded);
        av_frame_make_writable(Frame);
        
        if (ErrCode) return std::make_pair(FfmpegErrorCode::ENCODE, ErrCode);
        
        if (WasFrameEncoded)
        {
            // This one is a dummy to av_packet_rescale_ts to be quiet
            Packet.pts = Packet.dts = Pts;
            av_packet_rescale_ts(&Packet, CodecBase, StreamBase);
            Packet.stream_index = StreamIndex_Audio;
            Packet.pts = Packet.dts = Pts;
            Pts += Packet.duration;
            
            ErrCode = av_interleaved_write_frame(Container, &Packet);
            if (ErrCode) return std::make_pair(FfmpegErrorCode::CONTAINER_OUT_WRITE_FRAME, ErrCode);
            
            av_packet_unref(&Packet);
        }
        
    }
    else // Store in SampleBuffer_Output
    {
        memcpy(SampleBuffer_Output.ChannelLeft + SampleBuffer_Output.SampleCount, InputBuffer.ChannelLeft, InputBuffer.SampleCount * sizeof(double) );
        memcpy(SampleBuffer_Output.ChannelRight + SampleBuffer_Output.SampleCount, InputBuffer.ChannelRight, InputBuffer.SampleCount * sizeof(double) );
        
        SampleBuffer_Output.SampleCount += InputBuffer.SampleCount;
        InputBuffer.SampleCount = 0;
    }
    
    return std::make_pair(FfmpegErrorCode::NO_ERROR_OCCURED, 0);
}

std::pair<FfmpegErrorCode::Type, int32_t> OutputDevice::flushDevice(AVRational &CodecBase, AVRational &StreamBase)
{
    int32_t ErrCode;
    static int32_t WasFrameEncoded = 1;
    
    if (SampleBuffer_Output.SampleCount)
    {
        Frame->nb_samples = SampleBuffer_Output.SampleCount;
        //ErrCode = av_frame_get_buffer(Frame, 0);
        
        // Don't want to put the whole exportProject() into a try/catch block
        //if (ErrCode) return std::make_pair(FfmpegErrorCode::FRAME_ALLOC_BUFFER, 0);
        
        int16_t *ChLeftPtr = reinterpret_cast<int16_t *>(Frame->data[0]);
        int16_t *ChRightPtr = reinterpret_cast<int16_t *>(Frame->data[1]);
        
        // parallel
        for (uint16_t i = 0; i < SampleBuffer_Output.SampleCount; i++)
        {
            ChLeftPtr[i] = SampleBuffer_Output.ChannelLeft[i] * 32768.0;
            ChRightPtr[i] = SampleBuffer_Output.ChannelRight[i] * 32768.0;
        }
        
        // Encode the frame
        ErrCode = avcodec_encode_audio2(CodecContext, &Packet, Frame, &WasFrameEncoded);
        av_frame_unref(Frame);
        
        if (ErrCode) return std::make_pair(FfmpegErrorCode::ENCODE, ErrCode);
        
        if (WasFrameEncoded)
        {
            // This one is a dummy to av_packet_rescale_ts to be quiet
            Packet.pts = Packet.dts = Pts;
            av_packet_rescale_ts(&Packet, CodecBase, StreamBase);
            Packet.stream_index = StreamIndex_Audio;
            Packet.pts = Packet.dts = Pts;
            Pts += Packet.duration;
            
            ErrCode = av_interleaved_write_frame(Container, &Packet);
            if (ErrCode) return std::make_pair(FfmpegErrorCode::CONTAINER_OUT_WRITE_FRAME, ErrCode);
            
            av_packet_unref(&Packet);
        }
        
        SampleBuffer_Output.SampleCount = 0;
        WasFrameEncoded = 1;
        
        return std::make_pair(FfmpegErrorCode::NO_ERROR_OCCURED, 1);
    }
    // Flushing encoder
    if (WasFrameEncoded)
    {
        ErrCode = avcodec_encode_audio2(CodecContext, &Packet, nullptr, &WasFrameEncoded);
        if (ErrCode) return std::make_pair(FfmpegErrorCode::ENCODE, ErrCode);
        
        if ( (WasFrameEncoded) && (Packet.duration) )
        {
            // This one is a dummy to av_packet_rescale_ts to be quiet
            Packet.pts = Packet.dts = Pts;
            av_packet_rescale_ts(&Packet, CodecBase, StreamBase);
            Packet.stream_index = StreamIndex_Audio;
            Packet.pts = Packet.dts = Pts;
            Pts += Packet.duration;
            
            ErrCode = av_interleaved_write_frame(Container, &Packet);
            if (ErrCode) return std::make_pair(FfmpegErrorCode::CONTAINER_OUT_WRITE_FRAME, ErrCode);
        }
        else WasFrameEncoded = 0;
        
        av_packet_unref(&Packet);
        return std::make_pair(FfmpegErrorCode::NO_ERROR_OCCURED, 1);
    }
    
    // Flush the interleaved queues
    av_interleaved_write_frame(Container, nullptr);
    WasFrameEncoded = 1;
    
    return std::make_pair(FfmpegErrorCode::NO_ERROR_OCCURED, 0);
}
