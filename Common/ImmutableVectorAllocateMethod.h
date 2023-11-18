//
// ImmutableVectorAllocateMethod.h
//
// Library: Common
// Package: Memory
// Module:  Memory Allocate
// 

#pragma once

#include "AbstractAllocateMethod.h"

#include <cassert>
#include <cstdint>
#include <vector>

namespace Mmp
{

/**
 * @brief 基于 STL vector 实现, 利用vector内存连续性以及C++ RAII特性 
 */
template<typename V = uint8_t>
class ImmutableVectorAllocateMethod : public AbstractAllocateMethod
{
public:
    void* Malloc(size_t size) override
    {
        // Hint : nothing to do
        return reinterpret_cast<void*>(container.data());
    }
    void* Resize(void* /* data */, size_t /* size */) override
    {
        assert(false);
        return reinterpret_cast<void*>(container.data());
    }
    void* GetAddress(uint64_t offset) override
    {
        return reinterpret_cast<uint8_t*>(container.data()) + offset;
    }
    const std::string& Tag() override
    {
        static std::string tag = "ImmutableVectorAllocateMethod";
        return tag;
    }
public:
    std::vector<V> container;
};

} // namespace Mmp