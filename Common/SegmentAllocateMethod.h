//
// SegmentAllocateMethod.h
//
// Library: Common
// Package: Memory
// Module:  Memory Allocate
// 

#pragma once

#include <cstdint>
#include <string>

#include "AbstractAllocateMethod.h"
#include "SharedData.h"

namespace Mmp
{

/**
 * @brief SegmentAllocateMethod 本身并不提供内存分配的手段, SegmentAllocateMethod 主要用于
 *        无内存拷贝将 sharedData 拆分成多个内存段
 * @todo  是否有更合适的命名?
 */
class SegmentAllocateMethod : public AbstractAllocateMethod
{
public:
    using ptr = std::shared_ptr<SegmentAllocateMethod>;
public:
    SegmentAllocateMethod();
    ~SegmentAllocateMethod();
public:
    void* Malloc(size_t size) override;
    void* Resize(void* data, size_t size) override;
    void* GetAddress(uint64_t offset) override;
    const std::string& Tag() override;
public:
    /**
     * @brief       设置内存段区间
     * @param[in]   offset : 内存段起始偏移
     * @param[in]   length : 内存段大小
     * @param[in]   sharedData : 内存段
     * @note        offset >= 0 && length >=0 &&  (offset+length) <= sharedData->SetSize()
     */
    void SetRange(uint64_t offset, uint64_t length, const AbstractSharedData::ptr& sharedData);
private:
    AbstractSharedData::ptr _sharedData;
    uint64_t        _offset;
    uint64_t        _length;
    std::string     _tag;
};

} // namespace Mmp