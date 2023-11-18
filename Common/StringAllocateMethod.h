//
// StringAllocateMethod.h
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

class StringAllocateMethod : public AbstractAllocateMethod
{
public:
    void* Malloc(size_t size) override;
    void* Resize(void* data, size_t size) override;
    void* GetAddress(uint64_t offset) override;
    const std::string& Tag() override;
public:
    std::string _data;
};

} // namespace Mmp