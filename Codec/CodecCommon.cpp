#include "CodecCommon.h"

#include <cassert>

#include "PNG/PNGUtil.h"

#ifdef USE_VAAPI
#include "VAAPI/VADevice.h"
#endif /* USE_VAAPI */

namespace Mmp
{
namespace Codec
{

const std::string CodecTypeToStr(CodecType type)
{
    switch (type)
    {
        case CodecType::PNG:    return "PNG";
        case CodecType::H264:   return "H264";
        case CodecType::H265:   return "H265";
        case CodecType::VP8:    return "VP8";
        case CodecType::VP9:    return "VP9";
        case CodecType::AV1:    return "AV1";
        default: assert(false); return "UNKNOWN";
    }
}

} // namespace Codec
} // namespace Mmp
