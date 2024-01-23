//
// DrmAllocateMethod.h
//
// Library: Common
// Package: Memory
// Module:  Memory Allocate
// 
// Note : osal/allocator/allocator_drm.c (mpp:https://github.com/rockchip-linux/mpp)
//        Date : 2024-01-09
//        Hash : 5f1cd0f12d643f017aea61a196b3b766df511bf6
// Copyright 2010 Rockchip Electronics S.LSI Co. LTD
//

#pragma once

#include <mutex>

#include "AbstractDeviceAllocateMethod.h"

namespace Mmp
{

/**
 * @brief 基于 DRM 内存分配实现, 用于图形相关
 */
class DrmAllocateMethod : public AbstractDeviceAllocateMethod
{
public:
    using ptr = std::shared_ptr<DrmAllocateMethod>;
public:
    DrmAllocateMethod();
    ~DrmAllocateMethod();
public:
    void* Malloc(size_t size) override;
    void* Resize(void* data, size_t size) override;
    void* GetAddress(uint64_t offset) override;
    const std::string& Tag() override;
public:
    void Sync() override;
    void Map() override;
    void UnMap() override;
private:
    std::mutex _mtx;
    int _fd;
    void* _data;
    size_t _len;
};

} // namespace Mmp