#include "RkBufferContext.h"

#include <mutex>
#include <unordered_map>

#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/mman.h>

// 
// Note : This file is modified from osal/allocator/allocator_dma_heap.c
//        Date : 2024-03-21
//        Hash : 3b2784384468f13fb7659a47058024d990048cfa
// Copyright 2015 Rockchip Electronics Co. LTD
//

//
// Hint     :  DMA HEAP 已经加入 Linux 内核及安卓, 不过还比较"新颖",暂时还是将其视为平台特异性处理
// See also :  https://source.android.google.cn/docs/core/architecture/kernel/dma-buf-heaps
// Note     :  按照计划, linux (v4l2 or drm) dma buf 和 android ION 应当会逐渐废弃
//

namespace Mmp
{
namespace Codec
{

struct rk_dma_heap_allocation_data 
{
    RK_U64 len;
    RK_U32 fd;
    RK_U32 fd_flags;
    RK_U64 heap_flags;
};

#define RK_DMA_HEAP_IOC_MAGIC              'H'
#define RK_DMA_HEAP_IOCTL_ALLOC            _IOWR(RK_DMA_HEAP_IOC_MAGIC, 0x0, struct rk_dma_heap_allocation_data)

enum RkDmaHeapFlag
{
    DMA_FLAG_NONE       = 0,
    DMA_HEAP_CMA        = (1 << 0),
    DMA_HEAP_CACHABLE   = (1 << 1),
    DMA_HEAP_DMA32      = (1 << 2),
    DMA_HEAP_NUM        = (DMA_HEAP_CMA | DMA_HEAP_CACHABLE | DMA_HEAP_DMA32)
};

constexpr char kRkHeapPath[] = "/dev/dma_heap/";

const std::string GetRkDmaHeapName(uint8_t flags)
{
    static std::unordered_map<uint8_t /* flags */, std::string /* heap name */> kLookup = 
    {
        {DMA_FLAG_NONE,                                       "system-uncached"},
        {DMA_HEAP_CMA,                                        "cma-uncached"},
        {DMA_HEAP_CACHABLE,                                   "system"},
        {DMA_HEAP_CMA | DMA_HEAP_CACHABLE,                    "cma"},
        {DMA_HEAP_DMA32,                                      "system-uncached-dma32"},
        {DMA_HEAP_DMA32 | DMA_HEAP_CMA,                       "cma-uncached"},
        {DMA_HEAP_DMA32 | DMA_HEAP_CACHABLE,                  "system-dma32"},
        {DMA_HEAP_DMA32 | DMA_HEAP_CACHABLE | DMA_HEAP_DMA32, "cma"}
    };
    if (kLookup.count(flags))
    {
        return kLookup[flags];
    }
    else
    {
        assert(false);
        return "";
    }
}

static bool DrmIoCtl(int fd, int req, void* arg)
{
    int ret = -1;
    do 
    {
        ret = ioctl(fd, req, arg);
    } while (ret == -1 && (errno == EINTR || errno == EAGAIN));

    return ret >= 0;    
}

class RkDmaHeapDevice
{
public:
    using ptr = std::shared_ptr<RkDmaHeapDevice>;
public:
    RkDmaHeapDevice();
    ~RkDmaHeapDevice();
public:
    static RkDmaHeapDevice::ptr RkDmaHeapDeviceSingleton();
public:
    bool  Allocate(size_t len, int& fd, uint8_t flags);
    void  DeAllocate(int fd);
    void* Map(int fd, size_t len);
    void  UnMap(void* ptr, size_t len);
private:
    int  Open(uint8_t flags);
    void Close(uint8_t flags);
private:
    std::mutex _mtx;
    int _dmaHeapFds[DMA_HEAP_NUM] = {0};
};

RkDmaHeapDevice::ptr RkDmaHeapDevice::RkDmaHeapDeviceSingleton()
{
    static RkDmaHeapDevice::ptr gDevice = std::make_shared<RkDmaHeapDevice>();
    return gDevice;
}

RkDmaHeapDevice::RkDmaHeapDevice()
{
    
}

RkDmaHeapDevice::~RkDmaHeapDevice()
{
    for (uint8_t flags = 0; flags<DMA_HEAP_NUM; flags++)
    {
        close(flags);
    }
}

bool RkDmaHeapDevice::Allocate(size_t len, int& fd, uint8_t flags)
{
    bool ret = false;
    
    rk_dma_heap_allocation_data data = {};
    {
        data.len = len;
        data.fd_flags = O_RDWR | O_CLOEXEC;
        data.heap_flags = 0;
    }
    ret = DrmIoCtl(Open(flags), RK_DMA_HEAP_IOCTL_ALLOC, &data);
    if (ret)
    {
        fd = data.fd;
    }
    return ret;
}

void RkDmaHeapDevice::DeAllocate(int fd)
{
    assert(fd >= 0);
    close(fd);
}

void* RkDmaHeapDevice::Map(int fd, size_t len)
{
    int flags = PROT_READ;
    if (fcntl(fd, F_GETFL) & O_RDWR)
    {
        flags |= PROT_WRITE;
    }
    return mmap(nullptr, len, flags, MAP_SHARED, fd, 0);
}

void RkDmaHeapDevice::UnMap(void* ptr, size_t len)
{
    munmap(ptr, len);
}

int RkDmaHeapDevice::Open(uint8_t flags)
{
    std::lock_guard<std::mutex> lock(_mtx);
    if (_dmaHeapFds[flags] <= 0)
    {
        std::string devPath = kRkHeapPath + GetRkDmaHeapName(flags);
        _dmaHeapFds[flags] = open(devPath.c_str(), O_RDONLY | O_CLOEXEC);
        if (_dmaHeapFds[flags] <= 0)
        {
            assert(false);
            MMP_LOG_ERROR << "open " << devPath << " fail, error is: " << strerror(errno);
            return -1;
        }
    }
    return _dmaHeapFds[flags];
}

void RkDmaHeapDevice::Close(uint8_t flags)
{
    std::lock_guard<std::mutex> lock(_mtx);
    if (_dmaHeapFds[flags] > 0)
    {
        close(_dmaHeapFds[flags]);
        _dmaHeapFds[flags] = 0;
    }
}

} // namespace Codec
} // namespace Mmp

namespace Mmp
{
namespace Codec
{

RkBufferContext::RkBufferContext()
{
    _fd = -1;
    _data = nullptr;
    _len = 0;
    _flags = 0;
}

RkBufferContext::~RkBufferContext()
{
    if (_data)
    {
        RkDmaHeapDevice::RkDmaHeapDeviceSingleton()->UnMap(_data, _len);
    }
    if (_fd >= 0)
    {
        RkDmaHeapDevice::RkDmaHeapDeviceSingleton()->DeAllocate(_fd);
    }
}

void* RkBufferContext::Malloc(size_t size)
{
    std::lock_guard<std::mutex> lock(_mtx);
    if (_fd == -1) // Hint : 可重入
    {
        if (!RkDmaHeapDevice::RkDmaHeapDeviceSingleton()->Allocate(size, _fd, _flags))
        {
            assert(false);
            goto END;
        }
    }
    _len = size;
    return _data;
/* END1: */
    RkDmaHeapDevice::RkDmaHeapDeviceSingleton()->DeAllocate(_fd);
END:
    _fd = -1;
    _len = 0;
    return nullptr;
}

void* RkBufferContext::Resize(void* data, size_t size)
{
    std::lock_guard<std::mutex> lock(_mtx);
    // Hint : not support resize
    assert(false);
    return _data;
}

void* RkBufferContext::GetAddress(uint64_t offset)
{
    std::lock_guard<std::mutex> lock(_mtx);
    if (!_data)
    {
        _data = RkDmaHeapDevice::RkDmaHeapDeviceSingleton()->Map(_fd, _len);
    }
    return _data ? (uint8_t*)_data + offset : nullptr;
}

const std::string& RkBufferContext::Tag()
{
    static const std::string tag = "RkBufferContext";
    return tag;
}

void RkBufferContext::Sync()
{
    std::lock_guard<std::mutex> lock(_mtx);
    // TODO : cache buffer 应该需要手动处理 CPU 缓存的问题
}

void RkBufferContext::Map()
{
    std::lock_guard<std::mutex> lock(_mtx);
    if (!_data)
    {
        _data = RkDmaHeapDevice::RkDmaHeapDeviceSingleton()->Map(_fd, _len);
        assert(_data);
    }
}

void RkBufferContext::UnMap()
{
    std::lock_guard<std::mutex> lock(_mtx);
    if (_data)
    {
        RkDmaHeapDevice::RkDmaHeapDeviceSingleton()->UnMap(_data, _len);
        _data = nullptr;
    }
}

int RkBufferContext::GetFd()
{
    std::lock_guard<std::mutex> lock(_mtx);
    return _fd;
}

} // namespace Codec
} // namespace Mmp