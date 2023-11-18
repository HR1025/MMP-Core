//
// WindowFactory.h
//
// Library: GPU
// Package: Windows
// Module:  EGL
// 

#pragma once
#include <mutex>

#include "Common/Instantiator.h"
#include "Common/DynamicFactory.h"
#include "AbstractWindows.h"

namespace Mmp
{

/**
 * @brief 窗口工厂
 */
class WindowFactory
{
public:
    using AbstractWindowInstantiator = AbstractInstantiator<AbstractWindows>;
public:
    WindowFactory();
    ~WindowFactory();
    /**
     * @brief      注册窗口创建器到工厂
     * @param[in]  className
     * @param[in]  instantiator
     * @note       AbstractWindowInstantiator 需通过 new 的方式进行创建
     */
    void registerClass(const std::string& className, AbstractWindowInstantiator* instantiator);
    /**
     * @brief 创建窗口
     * @note  预注册 EGL 窗口创建器
     *        - EGLWindowDefault
     *        - X11Window
     *        - SDLWindow
     *        - DXGIWindow
     *        - EGLWindowGBM
     *        - VulkanWindow
     */
    AbstractWindows::ptr createWindow(const std::string& className);
    /**
     * @brief 获取窗口工厂默认单例
     */
    static WindowFactory& DefaultFactory();
private:
    void RegisterBuiltins();
private:
    std::mutex _mutex;
    DynamicFactory<AbstractWindows> _windowFactory;
};

} // namespace Mmp