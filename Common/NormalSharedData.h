//
// NormalNormalSharedData.h
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

class NormalSharedData : public AbstractSharedData
{
public:
    using ptr = std::shared_ptr<NormalSharedData>;
public:
    /**
     * @note `nullptr` means to use default memory allocate method
     */
    explicit NormalSharedData(size_t size, AbstractAllocateMethod::ptr allocateMethod = nullptr);
    ~NormalSharedData();
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