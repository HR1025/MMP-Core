//
// Instantiator.h
//
// Library: Common
// Package: Factory
// Module:  DynamicFactory
// 

#pragma once

#include <map>
#include <mutex>
#include <string>

#include "Instantiator.h"

namespace Mmp
{

/**
 * @brief 动态抽象工厂
 * @note  通过 register 和 unregister 将不同实例创建挂载到工厂上
 * @sa    AbstractInstantiator
 */
template<class Base>
class DynamicFactory
{
public:
    using BaseInstantiator = AbstractInstantiator<Base>;
    using FactoryMap       = std::map<std::string /* class name */, BaseInstantiator*>;
public:
    /**
     * @brief      注册
     * @param[in]  className
     * @param[in]  baseInstantiator
     * @sa         BaseInstantiator
     * @note       BaseInstantiator 需使用 new 
     */
    void registerClass(const std::string& className, BaseInstantiator* baseInstantiator)
    {
        std::lock_guard<std::mutex> lock(_mutex);
        _factoryMap[className] = baseInstantiator;
    }
    /**
     * @brief      取消注册
     * @param[in]  className
     */
    void UnregisterClass(const std::string& className)
    {
        std::lock_guard<std::mutex> lock(_mutex);
        if (_factoryMap.count(className) == 1)
        {
            delete _factoryMap[className];
            _factoryMap.erase(className);
        }
    } 
    /**
     * @brief      是否已注册
     * @param[in]  className
     */
    bool IsClassRegister(const std::string& className)
    {
        std::lock_guard<std::mutex> lock(_mutex);
        return _factoryMap.count(className) == 1;
    }
    /**
     * @brief       创建类实例
     * @param[in]   className
     */
    std::shared_ptr<Base> CreateInstance(const std::string& className)
    {
        std::lock_guard<std::mutex> lock(_mutex);
        if (_factoryMap.count(className) == 1)
        {
            return _factoryMap[className]->CreateInstance();
        }
        return nullptr;
    }
private:
    std::mutex _mutex;
    FactoryMap _factoryMap;
};

} // namespace Mmp