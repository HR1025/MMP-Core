//
// VASurfaceAllocateMethod.h
//
// Library: Common
// Package: Codec
// Module:  VAAPI
// 

#pragma once

#include "Common/AbstractDeviceAllocateMethod.h"

#include "VACommon.h"
#include "VADecodePictureContext.h"

namespace Mmp
{
namespace Codec
{

class VASurfaceAllocateMethod : public AbstractDeviceAllocateMethod
{
public:
    using ptr = std::shared_ptr<VASurfaceAllocateMethod>;
public:
    explicit VASurfaceAllocateMethod(VADecodePictureContext::ptr context);
    ~VASurfaceAllocateMethod();
public: /* override AbstractAllocateMethod function */
    void* Malloc(size_t size) override;
    void* Resize(void* data, size_t size) override;
    void* GetAddress(uint64_t offset) override;
    const std::string& Tag() override;
public: /* override AbstractAllocateMethod function */
    void Sync() override;
    void Map() override;
    void UnMap() override;
private:
    VADecodePictureContext::ptr _context;
    bool _isMap;
    void* _data;
    VAImage _image;
};

} // namespace Codec
} // namespace Mmp