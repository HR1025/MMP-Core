//
// AbstractSharedDataPool.h
//
// Library: Common
// Package: Memory
// Module:  Pool
// 

#pragma once

#include <memory>

#include "AbstractSharedData.h"

namespace Mmp
{

/**
 * @brief 通用内存池定义 
 */
class AbstractSharedDataPool
{
public:
    using ptr = std::shared_ptr<AbstractSharedDataPool>;
public:
    AbstractSharedDataPool() = default;
    virtual ~AbstractSharedDataPool() = default;
public:
    /**
     * @brief 增加内存池内存块数量 
     */
    virtual void AddCapacity(size_t n) = 0;
    /**
     * @brief 减少内存池内存块数量
     * @note  延迟生效 
     */
    virtual void DelCapacity(size_t n) = 0;
    /**
     * @brief 内存池内存块数量
     */
    virtual size_t Capacity() = 0;
    /**
     * @brief 当前被使用的内存块数量 
     */
    virtual size_t Used() = 0;
    /**
     * @brief 当前可用的内存块数量
     */
    virtual size_t Available() = 0;
    /**
     * @brief  当前实际已分配的内存块数量 
     */
    virtual size_t Allocated() = 0;
    /**
     * @brief      请求内存块
     * @param[in]  waitTimeMs : 等待时间, waitTimeMs == 0 为阻塞等待
     */
    virtual AbstractSharedData::ptr Request(uint32_t waitTimeMs = 0) = 0;
};

} // namespace Mmp