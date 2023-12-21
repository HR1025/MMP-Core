#include "VATranslator.h"

#include <cassert>

namespace Mmp
{
namespace Codec
{

/**
 * @todo 处理 libva 的版本兼容性问题
 */
VAProfile H264ProfileToVaProfile(H264Profile profile)
{
    switch (profile)
    {
        case MMP_H264_PROFILE_HIGH10: /* return VAProfileH264High10; */
        case MMP_H264_PROFILE_HIGH: return VAProfileH264High;
        case MMP_H264_PROFILE_MAIN: return VAProfileH264Main;
        default:
            return  VAProfileNone;
    }
}

uint32_t H264ChromaFormatToVaChromaFormat(H264ChromaFormat format)
{
    switch (format)
    {
        case H264ChromaFormat::MMP_H264_CHROMA_400: return VA_RT_FORMAT_YUV400;
        case H264ChromaFormat::MMP_H264_CHROMA_420: return VA_RT_FORMAT_YUV420;
        case H264ChromaFormat::MMP_H264_CHROMA_422: return VA_RT_FORMAT_YUV422;
        case H264ChromaFormat::MMP_H264_CHROMA_444: return VA_RT_FORMAT_YUV444;
        default:
            assert(false);
            return 0;
    }
}

uint32_t H264SliceTypeToVaSliceType(H264SliceType type)
{
    switch (type)
    {
        case H264SliceType::MMP_H264_P_SLICE: return 0;
        case H264SliceType::MMP_H264_B_SLICE: return 1;
        case H264SliceType::MMP_H264_I_SLICE: return 2;
        case H264SliceType::MMP_H264_SP_SLICE: return 3;
        case H264SliceType::MMP_H264_SI_SLICE: return 4;
        default:
            assert(false);
            return 0;
    }
}

uint32_t PixelFormatToVaRTFormat(PixelFormat format)
{
    switch (format)
    {
        case PixelFormat::YUV420P: return VA_RT_FORMAT_YUV420;
        case PixelFormat::NV12: return VA_RT_FORMAT_YUV420;
        case PixelFormat::RGBA8888: return VA_RT_FORMAT_RGB32;
        default: return 0;
    }
}

} // namespace Codec
} // namespace Mmp