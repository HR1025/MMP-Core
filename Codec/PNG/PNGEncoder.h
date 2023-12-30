//
// PngEncoder.h
//
// Library: Codec
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
    bool Pop(AbstractPack::ptr& pack) override;
    bool CanPush() override;
    bool CanPop() override;
    const std::string& Description() override;
private:
    bool IsAllowEncodePixelFormat(PixelFormat format);
private:
    std::recursive_mutex _mutex;
    PNGEncoderParameter  _setting;
    std::atomic<bool>    _inEncoding;
    NormalPack::ptr      _pack;
};

} // namespace Codec
} // namespace Mmp