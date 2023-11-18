//
// EGLWindowDefault.h
//
// Library: GPU
// Package: Windows
// Module:  EGL
// 

#pragma once

#include "EGLWindow.h"

namespace Mmp
{

/**
 * @brief 默认 EGL 窗口
 * @note  此配置不指定任何平台,故其具有较好的平台兼容性,
 *        但如果明确知道当前的 platform 信息 (X11 、wayland、GBM etc),
 *        则应当将此作为备选方案 (跨平台性好,兼容性差)
 */
class EGLWindowDefault : public EGLWindow 
{
public:
    EGLWindowDefault() = default;
    ~EGLWindowDefault() = default;
private:
    EGLDisplay OnOpenDisplay() override;
    EGLConfig  OnChooseEGLConfig() override;
    /**
     * @note  目前来看无法在 x86 上无法创建出 window 和 pbuffer surface
     */
    EGLSurface OnCreateSurface() override;
};

} // namespace Mmp
