#include "PixelFormat.h"

#include <cassert>

namespace Mmp
{

float BytePerPixel(PixelFormat format)
{
    switch (format)
    {
        case PixelFormat::YUV420P:    return 1.5f;
        case PixelFormat::NV12:       return 1.5f;
        case PixelFormat::RGB565:     return 2.0f;
        case PixelFormat::ARGB1555:   return 2.0f;
        case PixelFormat::RGBA5551:   return 2.0f;
        case PixelFormat::RGB888:     return 3.0f;
        case PixelFormat::ARGB8888:   return 4.0f;
        case PixelFormat::RGBA8888:   return 4.0f;;
        case PixelFormat::BGR565:     return 2.0f;
        case PixelFormat::ABGR1555:   return 2.0f;
        case PixelFormat::BRGA5551:   return 2.0f;
        case PixelFormat::BGR888:     return 3.0f;
        case PixelFormat::BGRA8888:   return 4.0f;
        case PixelFormat::ABGR8888:   return 4.0f;
        case PixelFormat::HSV888:     return 3.0f;
        default: assert(false);       return 0.0f;
    }
}

const std::string PixelFormatToStr(PixelFormat format)
{
    switch (format)
    {
        case PixelFormat::YUV420P:    return "YUV420P";
        case PixelFormat::NV12:       return "NV12";
        case PixelFormat::RGB565:     return "RGB565";
        case PixelFormat::ARGB1555:   return "ARGB1555";
        case PixelFormat::RGBA5551:   return "RGBA5551";
        case PixelFormat::RGB888:     return "RGB888";
        case PixelFormat::ARGB8888:   return "ARGB8888";
        case PixelFormat::RGBA8888:   return "RGBA8888";
        case PixelFormat::BGR565:     return "BGR565";
        case PixelFormat::ABGR1555:   return "ABGR1555";
        case PixelFormat::BRGA5551:   return "BRGA5551";
        case PixelFormat::BGR888:     return "BGR888";
        case PixelFormat::BGRA8888:   return "BGRA8888";
        case PixelFormat::ABGR8888:   return "ABGR8888";
        case PixelFormat::HSV888:     return "HSV888";
        default: assert(false);       return "UNKNOWN";
    }
}

std::ostream& operator<<(std::ostream& os, PixelFormat format)
{
    os << PixelFormatToStr(format);
    return os;
}

} // namespace Mmp