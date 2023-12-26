//
// NormalPack.h
//
// Library: Common
// Package: Memory
// Module:  Pack
// 

#pragma once

#include "AbstractPack.h"

namespace Mmp
{

/**
 * @brief 默认压缩数据实现 
 */
class NormalPack : public AbstractPack
{
public:
    using ptr = std::shared_ptr<NormalPack>;
public:
    explicit NormalPack(size_t size, AbstractAllocateMethod::ptr allocateMethod = nullptr);
    ~NormalPack();
public:
    void     SetCapacity(size_t size) override;
    size_t   GetCapcaity() override;
    void     SetSize(size_t size) override;
    size_t   GetSize() override;
    void*    GetData(uint64_t offset = 0) override;
    AbstractAllocateMethod::ptr GetAllocateMethod() override;
private:
    AbstractSharedData::ptr _data;
};

} // namespace Mmp