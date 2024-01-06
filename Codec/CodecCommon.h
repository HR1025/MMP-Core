//
// CodecCommon.h
//
// Library: Codec
// Package: Codec
// Module:  Codec
// 

#pragma once

#include <iostream>
#include <string>

#include "Common/Any.h"
#include "Common/MediaType.h"
#include "Common/PixelsInfo.h"
#include "Common/PixelFormat.h"
#include "Common/LogMessage.h"
#include "Common/AbstractSharedData.h"
#include "Common/AbstractPack.h"
#include "Common/AbstractFrame.h"
#include "Common/AbstractPicture.h"
#include "Common/NormalSharedData.h"
#include "Common/NormalPack.h"
#include "Common/NormalFrame.h"
#include "Common/NormalPicture.h"

#include "StreamFrame.h"

#define CODEC_LOG_TRACE   MMP_MLOG_TRACE("CODEC")    
#define CODEC_LOG_DEBUG   MMP_MLOG_DEBUG("CODEC")    
#define CODEC_LOG_INFO    MMP_MLOG_INFO("CODEC")     
#define CODEC_LOG_WARN    MMP_MLOG_WARN("CODEC")     
#define CODEC_LOG_ERROR   MMP_MLOG_ERROR("CODEC")    
#define CODEC_LOG_FATAL   MMP_MLOG_FATAL("CODEC")    

namespace Mmp
{
namespace Codec
{

/**
 * @brief 编解码器名称
 */
enum class CodecType
{
    PNG,
    H264,
    H265,
    VP8,
    VP9,
    AV1
};
const std::string CodecTypeToStr(CodecType type);
extern std::ostream& operator<<(std::ostream& os, CodecType type);

} // namespace Codec
} // namespace Mmp

#include "PNG/PNGCommon.h"