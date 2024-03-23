//
// AbstractDeviceAllocateMethod.h
//
// Library: Common
// Package: Memory
// Module:  Memory Allocate
// 

#pragma once

#include "AbstractAllocateMethod.h"
#include <string>

namespace Mmp
{

/**
 * @brief 抽象通用设备内存分配接口定义
 */
class AbstractDeviceAllocateMethod : public AbstractAllocateMethod
{
public:
    using ptr = std::shared_ptr<AbstractDeviceAllocateMethod>;
public:
    const static std::string kSync;
    const static std::string kMap;
    const static std::string kUnmap;
public:
    AbstractDeviceAllocateMethod() = default;
    virtual ~AbstractDeviceAllocateMethod() = default;
public:
    /**
     * @brief 内存同步
     */
    virtual void Sync() = 0;
    /**
     * @brief 内存映射
     */
    virtual void Map() = 0;
    /**
     * @brief 取消内存映射
     */
    virtual void UnMap() = 0;
    /**
     * @brief 获取 fd
     * @note  目前仅在 linux 上使用, 实际获取 dma buf fd
     * @todo  更好的平台一致性 ?
     */
    virtual int GetFd();
public:
    /**
     * @param[in] key :
     *                  kSync --> Sync
     *                  kMap  --> Map
     *                  kUnmap --> kUnmap
     */
    bool Set(const std::string& key, const Any& value) override;
};

} // namespace Mmp