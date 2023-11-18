//
// EventMessage.h
//
// Library: Event
// Package: Event
// Module:  Message
// 

#pragma once

#include <string>

namespace Mmp
{
namespace Event
{

extern std::string kOpenDrmDevice; /* 打开 DRM 设备 */
#define MMP_OPEN_DRM_DEVICE_ARGS const std::string& deviceName, int drmFd
extern std::string kCloseDrmDevice; /* 关闭 DRM 设备 */
#define MMP_CLOSE_DRM_DEVICE_ARGS const std::string& deviceName, int drmFd

} // namespace Event
} // namespace Mmp