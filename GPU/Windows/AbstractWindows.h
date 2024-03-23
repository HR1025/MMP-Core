//
// AbstractWindows.h
//
// Library: GPU
// Package: Windows
// Module:  Windows
// 

#pragma once

#include <memory>
#include <string>

#include "Common/Any.h"
#include "WindowsCommon.h"

namespace Mmp
{

/**
 * @brief 通用窗口接口定义
 * @todo  1 - 允许外部设置 on-screen 时的窗口大小
 *        2 - 接入系统输入部分 (如果有的话)
 *        3 - (是否需要) 支持导入外部 context
 *        4 - 窗口事件处理 (如 窗口大小变更 、窗口拖拽 等)
 */
class AbstractWindows
{
public:
    using ptr = std::shared_ptr<AbstractWindows>;
    virtual ~AbstractWindows() = default;
public:
    /**
     * @brief   开启
     * @return  是否成功
     */
    virtual bool Open() = 0;
    /**
     * @brief   关闭
     * @return  是否成功
     */
    virtual bool Close() = 0;
    /**
     * @brief        将 window 绑定到渲染线程
     * @param[in]    bind : true 为绑定, false 为解绑
     * @note         此接口只能在渲染线程调用
     */
    virtual bool BindRenderThread(bool bind) = 0;
    /**
     * @brief      设置 API 类型
     * @param[in]  type
     * @return     true 代表设置成功
     * @note       不同的 windows 可设置的 APIType 各不相同
     */
    virtual bool SetAPIType(APIType type) = 0;
    /**
     * @brief      设置渲染模式
     * @param[in]  onScreen : true : on-screen render, fasle : off-screen render
     * @return     是否设置成功
     */
    virtual bool SetRenderMode(bool onScreen) = 0;
    /**
     * @brief 获取窗口信息
     */
    virtual WindowInfo GetInfo() = 0;
    /**
     * @brief 双缓冲时,将 backend frame buffer 交换至 fornt frame buffer
     * @note  只有在 onScreen render 才需要进行, offScreen render 调用时无效
     */
    virtual void  Swap() = 0;
    /**
     * @brief  获取接口
     * @param  funcName : 函数名称
     * @return function addresss (if available)
     * @note   不同的 windows 可能具有不同的 extensions, 可以使用 GetProcAddress 获取特定接口
     */
    virtual void* GetProcAddress(const std::string& funcName);
    /**
     * @brief    设置原生属性
     */
    virtual void Set(const std::string& key, Any value);
    /**
     * @brief    获取原生属性
     */
    virtual Any Get(const std::string& key);
};

} // namespace