//
// VADecoder.h
//
// Library: Codec
// Package: VAAPI
// Module:  VAAPI
// 

#pragma once

#include <mutex>
#include <deque>
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
 * @sa   Reference guide code:
 *       1 - FFmpeg : https://github.com/FFmpeg/FFmpeg
 *       2 - virglrenderer : https://gitlab.freedesktop.org/virgl/virglrenderer/ (also refer to mesa)
 *       3 - libva-h264 : https://github.com/intel/libva-h264 (deprecated)
 *       4 - libva doxgen : http://intel.github.io/libva/
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
    void PushFrame(StreamFrame::ptr frame);
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
private:
    std::mutex _frameBufMtx;
    size_t _frameBufSize;
    std::deque<StreamFrame::ptr> _frameBufs;
};

} // namespace Codec
} // namespace Mmp
