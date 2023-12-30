//
// OpenH264Common.h
//
// Library: Codec
// Package: Codec
// Module:  OpenH264
// 

#pragma once

#include "Codec/CodecCommon.h"
#include "Codec/AbstractEncoder.h"
#include "Codec/AbstractDecorder.h"

#include "wels/codec_api.h"
#include "wels/codec_ver.h"

#define OPENH264_LOG_TRACE   MMP_MLOG_TRACE("OPENH264")    
#define OPENH264_LOG_DEBUG   MMP_MLOG_DEBUG("OPENH264")    
#define OPENH264_LOG_INFO    MMP_MLOG_INFO("OPENH264")     
#define OPENH264_LOG_WARN    MMP_MLOG_WARN("OPENH264")     
#define OPENH264_LOG_ERROR   MMP_MLOG_ERROR("OPENH264")    
#define OPENH264_LOG_FATAL   MMP_MLOG_FATAL("OPENH264")  

namespace Mmp
{
namespace Codec
{

} // namespace Codec
} // namespace Mmp