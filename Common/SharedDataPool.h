//
// SharedDataPool.h
//
// Library: Common
// Package: Memory
// Module:  Pool
// 

#pragma once

#include <mutex>
#include <atomic>
#include <cstdint>
#include <unordered_set>
#include <unordered_map>
#include <condition_variable>

#include "AbstractSharedDataPool.h"

namespace Mmp
{


class SharedDataPool : public AbstractSharedDataPool, public std::enable_shared_from_this<SharedDataPool>
{
public:
    using ptr = std::shared_ptr<SharedDataPool>;
public:
    explicit SharedDataPool(size_t dataSize, size_t capacity, AbstractAllocateMethod::ptr allocateMethod = nullptr);
    virtual ~SharedDataPool() = default;
public:
    void AddCapacity(size_t n) override;
    void DelCapacity(size_t n) override;
    size_t Capacity() override;
    size_t Used() override;
    size_t Available() override;
    size_t Allocated() override;
    AbstractSharedData::ptr Request(uint32_t waitTimeMs = 0) override;
private:
    std::mutex _mtx;
    std::condition_variable _cond;
    std::unordered_map<uint32_t /* slot */, AbstractSharedData::ptr> _buffers;
    std::unordered_set<uint32_t /* slot */> _occupys;
    std::unordered_set<uint32_t /* slot */> _idles;
private:
    std::atomic<size_t> _usedSlot;
    std::atomic<size_t> _availableSlot;
    std::atomic<size_t> _allocatedSlot;
    std::atomic<size_t> _capacity;
private:
    AbstractAllocateMethod::ptr _allocateMethod;
    size_t _dataSize;
    size_t _curSlot;
};

}; // namespace Mmp