#include "OpenH264Util.h"

namespace Mmp
{
namespace Codec
{

void OpenH264LogCallBack(void */* ctx */, int level, const char *msg)
{
    if (level >= WELS_LOG_DETAIL)
    {
        OPENH264_LOG_TRACE << msg;
    }
    else if (level >= WELS_LOG_DEBUG)
    {
        OPENH264_LOG_DEBUG << msg;
    }
    else if (level >= WELS_LOG_INFO)
    {
        OPENH264_LOG_INFO << msg;
    }
    else if (level >= WELS_LOG_WARNING)
    {
        OPENH264_LOG_WARN << msg;
    }
    else if (level >= WELS_LOG_ERROR)
    {
        OPENH264_LOG_ERROR << msg;
    }
    else
    {
        OPENH264_LOG_FATAL << msg;
    }
}

PixelFormat OpenH264PixFormatToMppPixFormat(EVideoFormatType type)
{
    switch (type)
    {
        case EVideoFormatType::videoFormatRGB: return PixelFormat::RGB888;
        case EVideoFormatType::videoFormatRGBA: return PixelFormat::RGBA8888;
        case EVideoFormatType::videoFormatRGB565: return PixelFormat::RGB565;
        case EVideoFormatType::videoFormatBGR: return PixelFormat::BGR888;
        case EVideoFormatType::videoFormatBGRA: return PixelFormat::BGRA8888;
        case EVideoFormatType::videoFormatABGR: return PixelFormat::ABGR8888;
        case EVideoFormatType::videoFormatARGB: return PixelFormat::ARGB8888;
        case EVideoFormatType::videoFormatI420: return PixelFormat::YUV420P;
        default:
            assert(false);
            return PixelFormat::NV12;
    }
}

} // namespace Codec
} // namespace Mmp