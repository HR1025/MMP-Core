//
// AbstractEGLWindow.h
//
// Library: GPU
// Package: Windows
// Module:  EGL
// 

#pragma once

#include <cstdint>
#include <string>
#include <utility>

#include "EGLCommon.h"
#include "AbstractWindows.h"

namespace Mmp
{

/**
 * @brief EGL 窗口通用接口定义
 */
class AbstractEGLWindow : public AbstractWindows
{
public:
    using Version = std::pair<uint32_t /* major */, uint32_t /* minor */>;
    virtual ~AbstractEGLWindow() = default;
public:
    /**
     * @brief   设置 surface 类型
     * @sa      GLSurfaceType
     * @note    默认为 GLSurfaceType::WINDOWS
     */
    virtual void SetSurfaceType(GLSurfaceType surfaceType) = 0;
    /**
     * @brief   获取 surface 类型
     * @sa      GLSurfaceType
     */
    virtual GLSurfaceType GetSurfaceType() = 0;
    /**
     * @brief  获取 GPU 产商信息
     */
    virtual const std::string GetVendor() = 0;
    /**
     * @brief 获取 Client API 的类型
     * @sa    GLMode
     */
    virtual GLMode GetGLMode() = 0;
    /**
     * @brief  获取版本
     */
    virtual const Version GetVersion() = 0;
private: /* override AbstractWindows function */
    bool SetAPIType(APIType type) override;
    bool SetRenderMode(bool onScreen) override;
    WindowInfo GetInfo() override;
};

} // namespace Mmp