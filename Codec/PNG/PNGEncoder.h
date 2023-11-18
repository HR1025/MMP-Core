//
// PngEncoder.h
//
// Library: Coedec
// Package: Encoder
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

class PngEncoder : public AbstractEncoder
{
public:
    PngEncoder();
    ~PngEncoder();
public:
    void SetParameter(Any parameter) override;
    Any GetParamter() override;
    bool Push(AbstractFrame::ptr frame) override;
    bool Pop(Pack::ptr& pack) override;
    bool CanPush() override;
    bool CanPop() override;
    void SetListener(OnStatusChange onStatusChange) override;
    void DelListener() override;
    const std::string& Description() override;
private:
    bool IsAllowEncodePixelFormat(PixelFormat format);
private:
    std::recursive_mutex _mutex;
    PNGEncoderParameter  _setting;
    std::atomic<bool>    _inEncoding;
    Pack::ptr            _pack;
    OnStatusChange       _onStatusChange;
};

} // namespace Codec
} // namespace Mmp