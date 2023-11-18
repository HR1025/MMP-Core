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

namespace Mmp
{

class SharedData
{
public:
    using ptr = std::shared_ptr<SharedData>;
public:
    /**
     * @note `nullptr` means to use default memory allocate method
     */
    explicit SharedData(size_t size, AbstractAllocateMethod::ptr allocateMethod = nullptr);
    virtual ~SharedData();
public:
    void     SetCapacity(size_t size);
    size_t   GetCapcaity();
    void     SetSize(size_t size);
    size_t   GetSize();
    virtual void*    GetData(uint64_t offset = 0);
    AbstractAllocateMethod::ptr GetAllocateMethod();
protected:
    SharedData(const SharedData&);
    SharedData& operator=(const SharedData&);
    SharedData(const SharedData&&);
    SharedData& operator=(const SharedData&&);
private:
    void*  _data;
    size_t _capacity;
    size_t _size;
private:
    AbstractAllocateMethod::ptr _allocateMethod;
};

} // namespace Mmp