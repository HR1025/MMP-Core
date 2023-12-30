//
// OpenH264Decoder.h
//
// Library: Codec
// Package: Codec
// Module:  OpenH264
// 

#pragma once

#include "OpenH264Common.h"

namespace Mmp
{
namespace Codec
{

class OpenH264Decoder : public AbstractDecoder
{
public:
    using ptr = std::shared_ptr<OpenH264Decoder>();
public:
    OpenH264Decoder();
    virtual ~OpenH264Decoder() = default;
public:
    void SetParameter(Any parameter) override;
    Any GetParamter() override;
    bool Init() override;
    void Uninit() override;
    bool Start() override;
    void Stop() override;
    bool Push(AbstractPack::ptr pack) override;
    bool Pop(AbstractFrame::ptr& frame) override;
    bool CanPush() override;
    bool CanPop() override;
    void SetListener(OnStatusChange onStatusChange) override;
    void DelListener() override;
    const std::string& Description() override;
private:
    ISVCDecoder* _decoder;
};

} // namespace Codec
} // namespace Mmp