//
// RkBufferContext.h
//
// Library: Common
// Package: Codec
// Module:  Rockchip
// 
//

#pragma once

#include "RKCommon.h"

#include "Common/DrmAllocateMethod.h"


namespace Mmp
{
namespace Codec
{

class RkBufferContext : public AbstractDeviceAllocateMethod
{
public:
    using ptr = std::shared_ptr<RkBufferContext>;
public:
    RkBufferContext();
    virtual ~RkBufferContext();
public:
    void* Malloc(size_t size) override;
    void* Resize(void* data, size_t size) override;
    void* GetAddress(uint64_t offset) override;
    const std::string& Tag() override;
public:
    void Sync() override;
    void Map() override;
    void UnMap() override;
public:
    int GetFd() override;
private:
    std::mutex _mtx;
    int _fd;
    void* _data;
    size_t _len;
    uint8_t _flags;
};

} // namespace Codec
} // namespace Mmp