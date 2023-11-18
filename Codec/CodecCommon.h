//
// CodecCommon.h
//
// Library: Coedec
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
#include "Common/SharedData.h"
#include "Common/AbstractFrame.h"

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
};
const std::string CodecTypeToStr(CodecType type);
extern std::ostream& operator<<(std::ostream& os, CodecType type);

/**
 * @brief 压缩数据格式
 */
class Pack : public SharedData
{
public:
    using ptr = std::shared_ptr<Pack>;
public:
    explicit Pack(size_t size, AbstractAllocateMethod::ptr allocateMethod = nullptr);
public:
    Any sideData;
};


/**
 * @brief 编码器状态
 * @todo  根据需要完善
 */
enum EncoderStatus
{
    E_SUCESS = 200,
    ED_FAIL = 400,
    EF_NOT_SUPPORT,    // 不支持编码
    EF_DATA_INVALID,   // 非法数据
    EF_OVERSIZE,       // 缓冲区溢出
    EF_UNKOWN,         // 未知错误
};

/**
 * @brief 解码器状态
 * @todo  根据需要完善
 */
enum DecoderStatus
{
    D_SUCESS = 200,
    DF_FAIL = 400,
    DF_NOT_SUPPORT,    // 不支持解码
    DF_DATA_INVALID,   // 非法数据
    DF_OVERSIZE,       // 缓冲区溢出
    DF_UNKOWN,         // 未知错误
};

} // namespace Codec
} // namespace Mmp

#include "PNG/PNGCommon.h"