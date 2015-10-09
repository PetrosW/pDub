#ifndef EXCEPTION_HPP
#define EXCEPTION_HPP

#include <cstdint>
#include <exception>
#include <string>
#include "error_codes.hpp"

extern "C" {
    #include <libavutil/error.h>
}

class FfmpegException_t : public std::exception
{
    public:
        FfmpegException_t(FfmpegErrorCode::Type ErrCode, int32_t FfmpegErrCode);
        
        const char *what() const noexcept;
        FfmpegErrorCode::Type getErrorCode() const;
    
    private:
        std::string ErrorMessage;
        FfmpegErrorCode::Type ErrorCode;
};

#endif
