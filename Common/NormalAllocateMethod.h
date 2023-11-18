//
// NormalAllocateMethod.h
//
// Library: Common
// Package: Memory
// Module:  Memory Allocate
// 

#pragma once

#include "AbstractAllocateMethod.h"

namespace Mmp
{

/**
 * @brief 标准内存分配实现, 基于 C-Style
 */
class NormalAllocateMethod : public AbstractAllocateMethod
{
public:
    using ptr = std::shared_ptr<NormalAllocateMethod>;
public:
    NormalAllocateMethod();
    ~NormalAllocateMethod();
public:
    void* Malloc(size_t size) override;
    void* Resize(void* data, size_t size) override;
    void* GetAddress(uint64_t offset) override;
    const std::string& Tag() override;
private:
    void*   _data;
    size_t  _size;
};

} // namespace Mmp