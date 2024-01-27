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
#include "VADecoderContext.h"

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
protected: /* Common Interface */
    VaDecoderParams GetDecoderParams();
    void SetDecoderParams(const VaDecoderParams& param);
    VADecoderContext::ptr GetContext();
protected: /* Hook */
    virtual void StartFrame(const Any& context) = 0;
    virtual void DecodedBitStream(const Any& context) = 0;
    virtual void EndFrame(const Any& context) = 0;
protected:  /* Event */
    virtual void OnVaDecoderParamsChange(const VaDecoderParams& oldValue, const VaDecoderParams& newValue);
private:
    VADecoderContext::ptr _contex;
    VaDecoderParams       _params;
};

} // namespace Codec
} // namespace Mmp
