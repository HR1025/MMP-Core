#include "DrmAllocateMethod.h"

#include <cstddef>
#include <cstdint>
#include <cassert>
#include <memory>
#include <mutex>
#include <string>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <drm/drm.h>
#include <drm/drm_mode.h>
#include <linux/dma-buf.h>

#include <Poco/Environment.h>

#include "LogMessage.h"

namespace Mmp
{

static std::string GetDrmNode()
{
    if (Poco::Environment::has("MMP_DRM_DEVICE"))
    {
        return Poco::Environment::get("MMP_DRM_DEVICE");
    }
    else
    {
        return "/dev/dri/card0";
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

class DrmDevice
{
public:
    using ptr = std::shared_ptr<DrmDevice>;
public:
    DrmDevice();
    ~DrmDevice();
public:
    static DrmDevice::ptr DrmDeviceSingleton();
public:
    bool  Allocate(size_t len, int& fd);
    void  DeAllocate(int fd);
    void* Map(int fd, size_t len);
    void  UnMap(void* ptr, size_t len);
private:
    bool Open();
    void Close();
private:
    int _fd = -1;
};

DrmDevice::ptr DrmDevice::DrmDeviceSingleton()
{
    static DrmDevice::ptr gDrmDevice = std::make_shared<DrmDevice>();
    return gDrmDevice;
}

DrmDevice::DrmDevice()
{
    Open();
}

DrmDevice::~DrmDevice()
{
    Close();
}

bool DrmDevice::Allocate(size_t len, int& fd)
{
    static auto destroyDumb = [this](int handle) -> void 
    {
        drm_mode_destroy_dumb dmdd;
        dmdd.handle = handle;
        DrmIoCtl(_fd, DRM_IOCTL_MODE_DESTROY_DUMB, &dmdd);
    };

    int handle = -1;
    // create dump buffer
    {
        drm_mode_create_dumb dmcd = {};
        dmcd.bpp = 8;
        dmcd.width = len;
        dmcd.height = 1;
        dmcd.flags = 0;
        if (!DrmIoCtl(_fd, DRM_IOCTL_MODE_CREATE_DUMB, &dmcd))
        {
            assert(false);
            goto END;
        }
        handle = dmcd.handle;
    }
    // map handle to fd
    {
        drm_prime_handle dph = {};
        dph.handle = handle;
        dph.fd = -1;
        dph.flags = 0;
        if (!DrmIoCtl(_fd, DRM_IOCTL_PRIME_HANDLE_TO_FD, &dph))
        {
            assert(false);
            goto END1;
        }
        fd = dph.fd;
    }
    destroyDumb(handle);
    assert(fd >= 0);
    return true;
END1:
    destroyDumb(handle);
END:
    return false;
}

void DrmDevice::DeAllocate(int fd)
{
    assert(fd >= 0);
    close(fd);
}

void* DrmDevice::Map(int fd, size_t len)
{
    int flags = PROT_READ;
    if (fcntl(fd, F_GETFL) & O_RDWR)
    {
        flags |= PROT_WRITE;
    }
    return mmap(nullptr, len, flags, MAP_SHARED, fd, 0);
}

void DrmDevice::UnMap(void* ptr, size_t len)
{
    munmap(ptr, len);
}

bool DrmDevice::Open()
{
    if (_fd >= 0)
    {
        return true;
    }
    std::string drmNode = GetDrmNode();
    _fd = open(drmNode.c_str(), O_RDWR | O_CLOEXEC);
    if (_fd < 0)
    {
        MMP_LOG_ERROR << "open " << drmNode << " fail";
        assert(false);
    }
    return _fd >= 0;
}

void DrmDevice::Close()
{
    if (_fd < 0)
    {
        return;
    }
    close(_fd);
    _fd = -1;
}

} // namespace Mmp

namespace Mmp
{

DrmAllocateMethod::DrmAllocateMethod()
{
    _fd = -1;
    _data = nullptr;
    _len = 0;
}

DrmAllocateMethod::~DrmAllocateMethod()
{
    if (_data)
    {
        DrmDevice::DrmDeviceSingleton()->UnMap(_data, _len);
    }
    if (_fd >= 0)
    {
        DrmDevice::DrmDeviceSingleton()->DeAllocate(_fd);
    }
}

void* DrmAllocateMethod::Malloc(size_t size)
{
    std::lock_guard<std::mutex> lock(_mtx);
    if (!DrmDevice::DrmDeviceSingleton()->Allocate(size, _fd))
    {
        assert(false);
        goto END;
    }
    _data = DrmDevice::DrmDeviceSingleton()->Map(_fd, size);
    if (_data == nullptr)
    {
        goto END1;
    }
    _len = size;
    return _data;
END1:
    DrmDevice::DrmDeviceSingleton()->DeAllocate(_fd);
END:
    _fd = -1;
    _len = 0;
    return nullptr;
}

void* DrmAllocateMethod::Resize(void* data, size_t size)
{
    std::lock_guard<std::mutex> lock(_mtx);
    // Hint : not support resize
    assert(false);
    return _data;
}

void* DrmAllocateMethod::GetAddress(uint64_t offset)
{
    std::lock_guard<std::mutex> lock(_mtx);
    return _data ? (uint8_t*)_data + offset : nullptr;
}

const std::string& DrmAllocateMethod::Tag()
{
    static const std::string tag = "DrmAllocateMethod";
    return tag;
}

void DrmAllocateMethod::Sync()
{
    std::lock_guard<std::mutex> lock(_mtx);
}

void DrmAllocateMethod::Map()
{
    std::lock_guard<std::mutex> lock(_mtx);
    if (!_data)
    {
        _data = DrmDevice::DrmDeviceSingleton()->Map(_fd, _len);
        assert(_data);
    }
}

void DrmAllocateMethod::UnMap()
{
    std::lock_guard<std::mutex> lock(_mtx);
    if (_data)
    {
        DrmDevice::DrmDeviceSingleton()->UnMap(_data, _len);
        _data = nullptr;
    }
}

int DrmAllocateMethod::GetFd()
{
    std::lock_guard<std::mutex> lock(_mtx);
    return _fd;
}

} // namespace Mmp