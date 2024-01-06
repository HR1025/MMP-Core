#include "RKTranslator.h"

#include <cassert>

namespace Mmp
{
namespace Codec
{

MppCodingType CodecTypeToRkType(CodecType type)
{
    switch (type) 
    {
        case CodecType::H264: return MppCodingType::MPP_VIDEO_CodingAVC;
        case CodecType::H265: return MppCodingType::MPP_VIDEO_CodingHEVC;
        case CodecType::VP8:  return MppCodingType::MPP_VIDEO_CodingVP8;
        case CodecType::VP9:  return MppCodingType::MPP_VIDEO_CodingVP9;
        case CodecType::AV1:  return MppCodingType::MPP_VIDEO_CodingAV1;
        default:
            assert(false);
            return MppCodingType::MPP_VIDEO_CodingUnused;
    }
}

} // namespace Codec
} // namespace Mmp