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
        case CodecType::PNG: return "PNG";
        default: assert(false); return "UNKNOWN";
    }
}

} // namespace Codec
} // namespace Mmp
