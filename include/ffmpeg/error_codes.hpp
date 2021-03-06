#ifndef ERROR_CODES_HPP
#define ERROR_CODES_HPP

#include <cstdint>

namespace FfmpegErrorCode {
    enum Type:uint8_t {
        NO_ERROR_OCCURED,
        SPLIT_DURATION_1_EMPTY,
        CONTAINER_IN_ALLOC,
        CONTAINER_IN_OPEN,
        CONTAINER_IN_STREAM_INFO,
        CONTAINER_IN_STREAM_NOT_AUDIO,
        CONTAINER_IN_READ_FRAME,
        CONTAINER_OUT_ALLOC,
        CONTAINER_OUT_NEW_STREAM,
        CONTAINER_OUT_COPY_CODEC_CONTEXT,
        CONTAINER_OUT_OPEN_FILE,
        CONTAINER_OUT_WRITE_HEADER,
        CONTAINER_OUT_WRITE_FRAME,
        CONTAINER_OUT_WRITE_TRAILER,
        PACKET_ALLOC,
        FRAME_ALLOC,
        FRAME_ALLOC_BUFFER,
        CODEC_IN_NOT_FOUND,
        CODEC_IN_OPEN,
        RESAMPLE_ALLOC,
        RESAMPLE_INIT,
        RESAMPLE_CONVERT,
        RESAMPLE_BUFFER_ALLOC,
        DECODE,
        ENCODE
    };
}


#endif
