//
// AbstractSharedData.h
//
// Library: Common
// Package: Memory
// Module:  SharedData
// 

#pragma once

#include <memory>
#include "AbstractAllocateMethod.h"

namespace Mmp
{

/**
 * @brief 通用内存块定义 
 */
class AbstractSharedData
{
public:
    using ptr = std::shared_ptr<AbstractSharedData>;
public:
    AbstractSharedData() = default;
    virtual ~AbstractSharedData() = default;
public:
    /**
     * @brief   设置内存块分配大小 (字节) 
     */
    virtual void     SetCapacity(size_t size) = 0;
    /**
     * @brief    获取内存块分片大小 (字节) 
     */
    virtual size_t   GetCapcaity() = 0;
    /**
     * @brief    获取内存块使用大小 (字节) 
     */
    virtual size_t   GetSize() = 0;
    /**
     * @brief    设置内存块使用大小 (字节)
     */
    virtual void     SetSize(size_t size) = 0;
    /**
     * @brief      获取内存地址
     * @param[in]  offset : 偏移值 (字节)
     */
    virtual void*    GetData(uint64_t offset = 0) = 0;
    /**
     * @brief      获取内存分配方法 
     */
    virtual AbstractAllocateMethod::ptr GetAllocateMethod() = 0;
};

} // namespace Mmp