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

} // namespace Codec
} // namespace Mmp