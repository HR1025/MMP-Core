//
// PNGDecorder.h
//
// Library: Coedec
// Package: Decorder
// Module:  PNG
// 

#pragma once

#include <atomic>
#include <mutex>

#include "PNGCommon.h"

namespace Mmp
{
namespace Codec
{
/**
 * @note base on `lodepng`
 */
class PngDecoder : public AbstractDecoder
{
public:
    PngDecoder();
    ~PngDecoder();
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
private:
    /**
     * @note only support RGB888 or ARGB8888
     */
    bool IsAllowDecodePixelFormat(const PngDecoderParameter& setting);
private:
    std::recursive_mutex  _mutex;
    PngDecoderParameter   _setting;
    std::atomic<bool>     _inDecoding;
    NormalPicture::ptr    _picture;
    OnStatusChange        _onStatusChange;
};
} // namespace Codec
} // namespace Mmp