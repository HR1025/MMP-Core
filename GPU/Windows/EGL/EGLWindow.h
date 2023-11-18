//
// EGLWindow.h
//
// Library: GPU
// Package: Windows
// Module:  EGL
// 

#pragma once

#include <set>
#include <string>
#include <vector>

#include <EGL/egl.h>

#include "AbstractEGLWindow.h"

namespace Mmp
{

/**
 * @brief EGL 窗口
 * @note  EGLWindow 最多只能被继承一次,否则容易造成由于继承造成的子类膨胀问题,
 *        装饰器相关的拓展派生类考虑使用组合的方式实现
 */
class EGLWindow : public AbstractEGLWindow
{
public: /* override AbstractWindows function */
    bool Open() override;
    bool Close() override;
    bool BindRenderThread(bool bind) override;
    void Swap() override;
public: /* override AbstractEGLWindow function */
    void SetSurfaceType(GLSurfaceType surfaceType) override;
    GLSurfaceType GetSurfaceType() override;
    const std::string GetVendor() override; 
    GLMode GetGLMode() override;
    const Version GetVersion() override;
    bool CheckExtension(const std::string& extension);
public:
    EGLWindow();
protected:
    /**
     * @brief 获取所有 EGL 可选配置
     * @sa    OnChooseEGLConfig
     */
    virtual std::vector<EGLConfig> GetAllEGLConfigs();
protected: /* EGLWindow Event share data */
    /* in */ EGLDisplay    _eglDisplay;
    /* in */ EGLConfig     _eglConfig;
private: /* EGLWindow Event */
    /**
     * @brief 获取 EGLDisplay
     */
    virtual EGLDisplay OnOpenDisplay() = 0;
    /**
     * @brief 选择 EGLConfig
     * @note  一个显示器可能支持多种输出配置,这些配置放置在 EGLConfig 中,
     *        派生类需要从中选取一个自己认为最合适的配置
     */
    virtual EGLConfig  OnChooseEGLConfig() = 0;
    /**
     * @brief 创建 surface
     * @note  派生类可以根据 GLSurfaceType 创建合适的 surface,
     *        进一步在知道具体的 platform 时可以使用 EGL extension
     *        更好地创建 surface
     */
    virtual EGLSurface OnCreateSurface() = 0;
private:
    void Reset();
    /**
     * @brief 根据 _eglConfig 进行判断,确认 OpenGL ES 的版本为 3 还是 2
     * @note  仅当 _glMode == OPENGL_ESx 时需要调用
     */
    void CheckOpenGLESVersion();
    /**
     * @brief      打印 EGL config 的一些配置值
     * @param[in]  eglConfig
     * @sa         eglspec.1.5.pdf - Table 3.1: EGLConfig attributes.
     */
    void LogEGLConfig(EGLConfig eglConfig);
protected:
    GLSurfaceType _surfaceType;
    EGLSurface    _eglSurface;
private:
    EGLContext    _eglContext;
    std::string   _vendor;
    GLMode        _glMode;
    uint32_t      _versionMajor;
    uint32_t      _versionMinor;
private:
   std::set<std::string> _extensions; 
};

} // namespace Mmp 