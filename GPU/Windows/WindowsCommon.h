//
// WindowsCommon.h
//
// Library: GPU
// Package: Windows
// Module:  Windows
// 

#pragma once

#include <cstdint>
#include <string>
#include <iostream>

#include "Common/LogMessage.h"

#define WIN_LOG_TRACE   MMP_MLOG_TRACE("GPU")    
#define WIN_LOG_DEBUG   MMP_MLOG_DEBUG("GPU")    
#define WIN_LOG_INFO    MMP_MLOG_INFO("GPU")     
#define WIN_LOG_WARN    MMP_MLOG_WARN("GPU")     
#define WIN_LOG_ERROR   MMP_MLOG_ERROR("GPU")    
#define WIN_LOG_FATAL   MMP_MLOG_FATAL("GPU")    

namespace Mmp
{

/**
 * @brief 窗口所关联的 API 类型
 */
enum class APIType
{
    OPENGL,
    OPENGL_ES3,
    D3D11,
    VULKAN,
    UNKNOWN,
};
const std::string APITypeToStr(APIType type);
extern std::ostream& operator<<(std::ostream& os, APIType type);

/**
 * @brief 窗口描述信息
 */
struct WindowInfo
{
public:
    WindowInfo();
public:
    APIType   apiType;
    uint32_t  vMajor;
    uint32_t  vMinor;
};

} // namespace Mmp