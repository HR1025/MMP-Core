//
// SharedData.h
//
// Library: Common
// Package: Memory
// Module:  SharedData
// 

#pragma once

#include <memory>

#include "AbstractAllocateMethod.h"
#include "AbstractSharedData.h"

namespace Mmp
{

class SharedData : public AbstractSharedData
{
public:
    using ptr = std::shared_ptr<SharedData>;
public:
    /**
     * @note `nullptr` means to use default memory allocate method
     */
    explicit SharedData(size_t size, AbstractAllocateMethod::ptr allocateMethod = nullptr);
    ~SharedData();
public:
    void     SetCapacity(size_t size) override;
    size_t   GetCapcaity() override;
    void     SetSize(size_t size) override;
    size_t   GetSize() override;
    void*    GetData(uint64_t offset = 0) override;
    AbstractAllocateMethod::ptr GetAllocateMethod() override;
private:
    void*  _data;
    size_t _capacity;
    size_t _size;
private:
    AbstractAllocateMethod::ptr _allocateMethod;
};

} // namespace Mmp