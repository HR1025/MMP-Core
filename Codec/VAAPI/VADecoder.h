//
// VADecoder.h
//
// Library: Codec
// Package: VAAPI
// Module:  VAAPI
// 

#pragma once

#include <mutex>
#include <atomic>
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
class VADecoder : public AbstractDecoder, public std::enable_shared_from_this<VADecoder>
{
public:
    using ptr = std::shared_ptr<VADecoder>;
public:
    VADecoder();
    virtual ~VADecoder() = default;
public:
    bool Init() override;
    void Uninit() override;
    void SetParameter(Any parameter) override;
    Any GetParamter() override;
    bool Push(AbstractPack::ptr pack) override;
    bool Pop(AbstractFrame::ptr& frame) override;
    bool CanPush() override;
    bool CanPop() override;
    const std::string& Description() override;
public:
    /**
     * @brief 初始化 VADecoder 
     */
    bool VaInit();
    /**
     * @brief 重置  VADecoder
     */
    void VaUninit();
    /**
     * @brief 增加引用计数 
     */
    void AddReference();
    /**
     * @brief 减少引用计数 
     */
    void DelReference();
private:
    bool CreateContext();
    void DestroyContext();
    bool CreateVaConfig();
    void DestroyVaConfig();
public: /* Common Interface */
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
    bool CommitVaDecodeCommand(VADecodePictureContext::ptr picContext);
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
    std::atomic<bool>    _needUninit;
    std::atomic<int64_t> _reference;
    VaDecoderParams      _params;
};

} // namespace Codec
} // namespace Mmp
