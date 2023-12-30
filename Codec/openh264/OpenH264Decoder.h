//
// OpenH264Decoder.h
//
// Library: Codec
// Package: Codec
// Module:  OpenH264
// 

#pragma once

#include <mutex>
#include <deque>

#include "OpenH264Common.h"
#include "Common/SharedDataPool.h"

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
    const std::string& Description() override;
private:
    std::mutex           _mtx;
    ISVCDecoder*         _decoder;
    SharedDataPool::ptr  _pool;
    PixelsInfo           _info;
    size_t               _bufSize;
private:
    std::mutex                    _bufMtx;
    std::deque<StreamFrame::ptr>  _buffers;
};

} // namespace Codec
} // namespace Mmp