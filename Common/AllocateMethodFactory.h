//
// AllocateMethod.h
//
// Library: Common
// Package: Memory
// Module:  AllocateMethodFactory
// 

#pragma once

#include <mutex>

#include "AbstractAllocateMethod.h"
#include "Instantiator.h"
#include "DynamicFactory.h"

namespace Mmp
{

/**
 * @brief 内存分配接口工厂
 */
class AllocateMethodFactory
{
public:
    using AllocateMethodInstantiator = AbstractInstantiator<AbstractAllocateMethod>;
public:
    AllocateMethodFactory();
    ~AllocateMethodFactory();
public:
    void RegisterAllocateMethodClass(const std::string& className, AllocateMethodInstantiator* instantiator);
    AbstractAllocateMethod::ptr CreateAllocateMethod(const std::string& className);
    static AllocateMethodFactory& DefaultFactory();
private:
    void RegisterBuiltins();
private:
    std::mutex _mutex;
    DynamicFactory<AbstractAllocateMethod> _allocateMethodFactory;
};

} // namespace Mmp