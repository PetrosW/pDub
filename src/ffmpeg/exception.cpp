#include <ffmpeg/exception.hpp>

FfmpegException_t::FfmpegException_t(FfmpegErrorCode::Type ErrCode, int32_t FfmpegError) : ErrorCode(ErrCode)
{
    if (FfmpegError < 0)
    {
        char TmpBuffer[AV_ERROR_MAX_STRING_SIZE] = {0};
        av_strerror(FfmpegError, TmpBuffer, AV_ERROR_MAX_STRING_SIZE);
        ErrorMessage = TmpBuffer;
    }
}

const char *FfmpegException_t::what() const noexcept
{
    return ErrorMessage.c_str();
}

FfmpegErrorCode::Type FfmpegException_t::getErrorCode() const
{
    return ErrorCode;
}
