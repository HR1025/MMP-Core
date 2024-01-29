//
// VADecoderContext.h
//
// Library: Codec
// Package: VAAPI
// Module:  VAAPI
// 

#pragma once

#include <mutex>

#include "VACommon.h"
#include "VADecodePictureContext.h"

namespace Mmp
{
namespace Codec
{

class VADecoderContext
{
public:
    using ptr = std::shared_ptr<VADecoderContext>;
public:
    explicit VADecoderContext(const VaDecoderParams& params);
    ~VADecoderContext();
public:
    bool Init();
public:
    VASurfaceID CreateVaSurface(const std::vector<VASurfaceAttrib>& attributes);
    void DestroyVaSurface(VASurfaceID surfaceId);
    VABufferID CreateVaParamBuffer(VABufferType type, void* data, size_t size);
    void DestroyVaParamBuffer(VABufferID buffer);
    VABufferID CreateVaSliceParamBuffer(VABufferType type, void* data, size_t size);
    void DestroyVaSliceParamBuffer(VABufferID buffer);
    VABufferID CreateVaSliceDataBuffer(void* data, size_t size);
    void DestroyVaSliceDataBuffer(VABufferID buffer);
    bool CommitVaDecodeCommand(VADecodePictureContext::ptr picContext);
    bool SyncVaSurface(VASurfaceID surfaceId);
    bool MapVaSurface(VASurfaceID surfaceId, VAImage& image, void*& address);
    void UnMapVaSurface(const VAImage& image);
private:
    bool CreateContext();
    void DestroyContext();
    bool CreateVaConfig();
    void DestroyVaConfig();
private:
    void Uninit();
private:
    bool            _isInited;
    VAImageFormat   _imageFormat;
    VaDecoderParams _params;
    VADisplay       _display;
    VAContextID     _context;
    VAConfigID      _config;
};

} // namespace Codec
} // namespace Mmp
