//
// VACommon.h
//
// Library: Common
// Package: Codec
// Module:  VAAPI
// 

#pragma once

// TODO : move common part from the Codec/VAAPI to Codec, for example H264StartFrameContext and etc

#include "Common/Any.h"
#include "Common/Common.h"
#include "Common/PixelFormat.h"

#include <va/va.h>
#if MMP_PLATFORM(LINUX)
#include <va/va_drm.h>
#endif
#ifdef USE_X11
#include <va/va_x11.h>
#endif /* USE_X11 */

#include "Codec/CodecCommon.h"
#include "Codec/AbstractEncoder.h"
#include "Codec/AbstractDecorder.h"
#include "Codec/H264/H264Common.h"

#define VAAPI_LOG_TRACE   MMP_MLOG_TRACE("VAAPI")    
#define VAAPI_LOG_DEBUG   MMP_MLOG_DEBUG("VAAPI")    
#define VAAPI_LOG_INFO    MMP_MLOG_INFO("VAAPI")     
#define VAAPI_LOG_WARN    MMP_MLOG_WARN("VAAPI")     
#define VAAPI_LOG_ERROR   MMP_MLOG_ERROR("VAAPI")    
#define VAAPI_LOG_FATAL   MMP_MLOG_FATAL("VAAPI")   

namespace Mmp
{
namespace Codec
{

enum MmpVaDecodeFlag : uint64_t
{
    MMP_VA_DECODE_FALG_NEED_MEMORY_NONE = 0,
    MMP_VA_DECODE_FALG_NEED_MEMORY_TYPE = 1 << 1U,
    MMP_VA_DECODE_FALG_NEED_PIX_FORMAT  = 1 << 2U
};

class VaDecoderParams
{
public:
    VaDecoderParams();
    ~VaDecoderParams() = default;
public:
    VAProfile                          profile;
    PixelFormat                        format;
    uint32_t                           level;
    uint32_t                           width;
    uint32_t                           height;
    uint32_t                           maxReference;
    uint64_t /* MmpVaDecodeFlag */     flag;
};
bool operator==(const VaDecoderParams& left, const VaDecoderParams& right);
bool operator!=(const VaDecoderParams& left, const VaDecoderParams& right);

class VaDecodePictureContext
{
public:
    using ptr = std::shared_ptr<VaDecodePictureContext>;
public:
    VaDecodePictureContext();
public:
    Any                      opaque;
    VASurfaceID              surface;
    std::vector<VABufferID>  paramBuffers;
    std::vector<VABufferID>  sliceBuffers;
};

} // namespace Codec
} // namespace Mmp