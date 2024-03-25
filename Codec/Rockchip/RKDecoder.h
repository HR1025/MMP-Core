//
// RKDecoder.h
//
// Library: Common
// Package: Codec
// Module:  Rockchip
// 
// Note : This file is modified from mpp/test/mpi_dec_test.c
//        Date : 2024-01-06
//        Hash : 5f1cd0f12d643f017aea61a196b3b766df511bf6
// Copyright 2015 Rockchip Electronics Co. LTD
//

#pragma once

#include <mutex>

#include "RKCommon.h"
#include "RkBufferPoolContext.h"
#include "Common/TaskQueue.h"

namespace Mmp
{
namespace Codec
{

class RKDecoder : public AbstractDecoder
{
public:
    using ptr = std::shared_ptr<RKDecoder>();
public:
    RKDecoder();
    virtual ~RKDecoder() = default;
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
public:
    virtual CodecType GetCodecType() = 0;
private:
    bool CreateMPI();
    void DestroyMPI();
    bool ReinitDecConfig();
    bool InitRkPacket();
    void UnitRkPacket();
private:
    std::mutex      _mtx;
    MppApi*         _mpi;
    MppCtx          _ctx;
    bool            _sync;
    uint64_t        _poolSize;
    bool            _fastOutput;
    uint64_t        _timeout;
    uint32_t        _maxFrame;
    MppPacket       _rkPacket;
private:
    std::atomic<bool>  _runing;
    std::mutex         _thMtx;
    TaskQueue::ptr     _thread;
private:
    std::mutex                    _bufMtx;
    std::deque<StreamFrame::ptr>  _buffers;
    RkBufferPoolContext::ptr      _poolContext;
};

} // namespace Codec
} // namespace Mmp