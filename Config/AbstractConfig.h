//
// AbstractConfig.h
//
// Library: Config
// Package: Config
// Module:  Config
// 

#pragma once

#include <memory>
#include <string>

#include "Common/Any.h"

namespace Mmp
{
namespace Config
{
/**
 * @brief MMP 全局配置
 * @note  只能在 Init 前配置一次
 */
class AbstractConfig
{
public:
    using ptr = std::shared_ptr<AbstractConfig>;
public:
    virtual ~AbstractConfig() = default;
public:
    static AbstractConfig* ConfigSingleton();
public:
    /**
     * @brief       加载配置
     * @param[in]   iniPath : mmp.ini 路径, 为空加载默认配置
     */
    virtual void Load(const std::string& iniPath = "") = 0;
    /**
     * @brief       初始化
     * @note        耗时操作
     */
    virtual void Init() = 0;
    /**
     * @brief      重置
     * @note       有且仅在进程退出时调用
     */
    virtual void Uninit() = 0;
    /**
     * @brief       设置属性
     * @param[in]   section : 节
     * @param[in]   key : 键
     * @param[in]   value : 值, 具体类型由键确认
     */
    virtual void Set(const std::string& section, const std::string& key, Any value) = 0;
    /**
     * @brief   获取属性
     * @sa      Set
     */
    virtual Any Get(const std::string& section, const std::string& key) = 0;
};

} // namespace Config
} // namespace Mmp