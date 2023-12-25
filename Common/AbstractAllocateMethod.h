//
// AllocateMethod.h
//
// Library: Common
// Package: Memory
// Module:  Memory Allocate
// 

#pragma once

#include <cstdint>
#include <string>
#include <memory>

namespace Mmp
{

/**
 * @brief 抽象通用内存分配接口定义
 */
class AbstractAllocateMethod
{
public:
    using ptr = std::shared_ptr<AbstractAllocateMethod>;
public:
    AbstractAllocateMethod() = default;
    virtual ~AbstractAllocateMethod() = default;
public:
    /**
     * @sa malloc
     */
    virtual void* Malloc(size_t size) = 0;
    /**
     * @sa realloc
     */
    virtual void* Resize(void* data, size_t size) = 0;
    /**
     * @brief 获取内存地址
     */
    virtual void* GetAddress(uint64_t offset) = 0;
    /**
     * @brief 标签
     */
    virtual const std::string& Tag() = 0;
};

} // namespace Mmp