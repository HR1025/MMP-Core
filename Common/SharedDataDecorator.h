//
// SharedDataDecorator.h
//
// Library: Common
// Package: Memory
// Module:  SharedData
// 

#pragma once

#include "AbstractSharedData.h"

namespace Mmp
{

class SharedDataDecorator : public AbstractSharedData
{
public:
    using ptr = std::shared_ptr<SharedDataDecorator>;
public:
    explicit SharedDataDecorator(AbstractSharedData::ptr sharedData);
    ~SharedDataDecorator() = default;
public:
    void     SetCapacity(size_t size) override;
    size_t   GetCapcaity() override;
    void     SetSize(size_t size) override;
    size_t   GetSize() override;
    void*    GetData(uint64_t offset = 0) override;
    AbstractAllocateMethod::ptr GetAllocateMethod() override;
private:
    AbstractSharedData::ptr _sharedData;
};

} // namespace Mmp