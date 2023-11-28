#include "VACommon.h"

namespace Mmp
{
namespace Codec
{

VaDecoderParams::VaDecoderParams()
{
    profile = VAProfileNone;
    format = PixelFormat::NV12;
    level = 0;
    width = 0;
    height = 0;
    maxReference = 0;
    flag = MMP_VA_DECODE_FALG_NEED_MEMORY_NONE;
    flag |= MMP_VA_DECODE_FALG_NEED_MEMORY_TYPE;
}

bool operator==(const VaDecoderParams& left, const VaDecoderParams& right)
{
    return left.profile == right.profile && left.format == right.format && left.level == right.level &&
           left.width == right.width && left.height == right.height && left.maxReference == right.maxReference &&
           left.flag == right.flag;
}

bool operator!=(const VaDecoderParams& left, const VaDecoderParams& right)
{
    return !(left == right);
}

VaDecodePictureContext::VaDecodePictureContext()
{
    surface = VA_INVALID_ID;
}

} // namespace Codec
} // namespace Mmp