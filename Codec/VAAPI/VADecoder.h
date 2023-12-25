//
// VADecoder.h
//
// Library: Codec
// Package: VAAPI
// Module:  VAAPI
// 

#pragma once

#include <mutex>
#include <vector>
#include <memory>

#include "VACommon.h"
#include "VADevice.h"

namespace Mmp
{
namespace Codec
{

/**
 * @todo 支持以及完善缓冲区机制 
 */
class VADecoder : public AbstractDecoder
{
public:
    using ptr = std::shared_ptr<VADecoder>();
public:
    VADecoder();
    virtual ~VADecoder() = default;
public:
    void SetParameter(Any parameter) override;
    Any GetParamter() override;
    bool Push(AbstractPack::ptr pack) override;
    bool Pop(AbstractFrame::ptr& frame) override;
    bool CanPush() override;
    bool CanPop() override;
    void SetListener(OnStatusChange onStatusChange) override;
    void DelListener() override;
    const std::string& Description() override;
public:
    bool VaInit();
    void VaUninit();
private:
    bool CreateContext();
    void DestroyContext();
    bool CreateVaConfig();
    void DestroyVaConfig();
protected: /* Common Interface */
    VaDecoderParams GetDecoderParams();
    void SetDecoderParams(const VaDecoderParams& param);
    VASurfaceID CreateVaSurface(const std::vector<VASurfaceAttrib>& attributes);
    void DestroyVaSurface(VASurfaceID surfaceId);
    VAImage CreateVaImage(VASurfaceID surfaceId);
    void DestroyVaImage(VAImage image);
    VABufferID CreateVaParamBuffer(VABufferType type, void* data, size_t size);
    void DestroyVaParamBuffer(VABufferID buffer);
    VABufferID CreateVaSliceParamBuffer(VABufferType type, void* data, size_t size);
    void DestroyVaSliceParamBuffer(VABufferID buffer);
    VABufferID CreateVaSliceDataBuffer(void* data, size_t size);
    void DestroyVaSliceDataBuffer(VABufferID buffer);
    bool CommitVaDecodeCommand(VaDecodePictureContext::ptr picContext);
protected: /* Hook */
    virtual void StartFrame(const Any& context) = 0;
    virtual void DecodedBitStream(const Any& context) = 0;
    virtual void EndFrame(const Any& context) = 0;
protected:  /* Event */
    virtual void OnVaDecoderParamsChange(const VaDecoderParams& oldValue, const VaDecoderParams& newValue);
protected:
    bool           _isInited;
    VADisplay      _display;
    VAContextID    _context;
    VAConfigID     _config;
private:
    VaDecoderParams  _params;
};

} // namespace Codec
} // namespace Mmp
