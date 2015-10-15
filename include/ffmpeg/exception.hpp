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
        
        #if defined(_WIN32) && _MSC_VER <= 1800
            const char *what() const;
        #else
            const char *what() const noexcept;
        #endif
        FfmpegErrorCode::Type getErrorCode() const;
    
    private:
        std::string ErrorMessage;
        FfmpegErrorCode::Type ErrorCode;
};

#endif
