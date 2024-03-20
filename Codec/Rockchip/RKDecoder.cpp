#include "RKDecoder.h"

#include <cassert>
#include <chrono>
#include <memory>
#include <mutex>
#include <thread>
#include <fstream>
#include <sstream>

#include "rockchip/mpp_frame.h"
#include "rockchip/mpp_buffer.h"

#include "Common/Promise.h"

#include "RKCommon.h"
#include "RKUtil.h"
#include "RKTranslator.h"

#include "Common/AbstractAllocateMethod.h"

#define ENABLE_RK_DECODE_DUMP 0
#define RK_DECODE_DUMP_SIGNGLE_FILE 1

namespace Mmp
{

class RkDecoderAllocateMethod : public AbstractAllocateMethod
{
public:
    using ptr = std::shared_ptr<RkDecoderAllocateMethod>;
public:
    RkDecoderAllocateMethod();
    ~RkDecoderAllocateMethod();
public:
    void* Malloc(size_t size) override;
    void* Resize(void* data, size_t size) override;
    void* GetAddress(uint64_t offset) override;
    const std::string& Tag() override;
public:
    MppFrame frame;
};

RkDecoderAllocateMethod::RkDecoderAllocateMethod()
{
    frame = nullptr;
}

RkDecoderAllocateMethod::~RkDecoderAllocateMethod()
{
    if (frame)
    {
        mpp_frame_deinit(&frame);
    }
}

void* RkDecoderAllocateMethod::Malloc(size_t /* size */)
{
    if (frame)
    {   
        MppBuffer buf = mpp_frame_get_buffer(frame);
        if (buf)
        {
            return (void*)mpp_buffer_get_ptr(buf);
        }
        else
        {
            assert(false);
            return nullptr;
        }
    }
    else
    {
        return nullptr;
    }
}

void* RkDecoderAllocateMethod::GetAddress(uint64_t offset)
{
    if (frame)
    {        
        return ((uint8_t*)mpp_buffer_get_ptr(mpp_frame_get_buffer(frame)) + offset);
    }
    else
    {
        return nullptr;
    }
}

void* RkDecoderAllocateMethod::Resize(void* /* data */, size_t /* size */)
{
    assert(false);
    return nullptr;
}

const std::string& RkDecoderAllocateMethod::Tag()
{
    static std::string tag = "RkDecoderAllocateMethod";
    return tag;
}

} // namespace Mmp

namespace Mmp
{
namespace Codec
{

RKDecoder::RKDecoder()
{
    _ctx = nullptr;
    _mpi = nullptr;
    _timeout = 1;
    _rkPacket = nullptr;
    _frmGrp = nullptr;
    _fastOutput = false;
    _sync = true;
    if (_sync)
    {
        _bufSize = 24;
        _timeout = 1;
    }
    else
    {
        _bufSize = 24 * 4;
        _timeout = 100;
    }
}

void RKDecoder::SetParameter(Any parameter)
{
}

Any RKDecoder::GetParamter()
{
    return Any();
}

bool RKDecoder::CreateMPI()
{
    RK_LOG_INFO << "-- Create MPI";
    MppCodingType codecType = CodecTypeToRkType(GetCodecType());
    MPP_RET rkRet = MPP_OK;
    if (codecType == MppCodingType::MPP_VIDEO_CodingUnused)
    {
        RK_LOG_ERROR << "Unknown codec type, codec type is: " << GetCodecType();
        assert(false);
        goto END;
    }
    if (mpp_check_support_format(MppCtxType::MPP_CTX_DEC, codecType))
    {
        RK_LOG_ERROR << "Unsupport codec type, codec type is: " << GetCodecType();
        assert(false);
        goto END;
    }
    if (RK_OP_FAIL(mpp_create(&_ctx, &_mpi)))
    {
        RK_LOG_ERROR << "mpp_create fail, error is: " << RkMppRetToStr(rkRet);
        assert(false);
        goto END1;
    }
    ReinitDecConfig();
    if (RK_OP_FAIL(mpp_init(_ctx, MppCtxType::MPP_CTX_DEC, codecType)))
    {
        RK_LOG_ERROR << "mpp_init fail, error is: " << RkMppRetToStr(rkRet);
        assert(false);
        goto END2;
    }
    return true;
END2:
    mpp_destroy(_ctx);
END1:
    _ctx = nullptr;
    _mpi = nullptr;
END:
    return false;
}

void RKDecoder::DestroyMPI()
{
    RK_LOG_INFO << "-- DestroyMPI";
    if (_ctx)
    {
        mpp_destroy(_ctx);
    }
    _ctx = nullptr;
    _mpi = nullptr;
}

bool RKDecoder::ReinitDecConfig()
{
    MPP_RET rkRet = MPP_OK;
    MppDecCfg rkCfg = nullptr;
    RK_S64 paramS64;
    RK_LOG_INFO << "-- ReinitDecConfig";
    mpp_dec_cfg_init(&rkCfg);
    if (RK_OP_FAIL(_mpi->control(_ctx, MPP_DEC_GET_CFG, rkCfg)))
    {
        RK_LOG_ERROR << "MppApi::control fail, cmd is:" << MpiCmdToStr(MPP_DEC_GET_CFG) << " , error is: " << RkMppRetToStr(rkRet);
        assert(false);
        goto END;
    }
    if (RK_OP_FAIL(_mpi->control(_ctx, MPP_DEC_SET_CFG, rkCfg)))
    {
        RK_LOG_ERROR << "MppApi::control fail, cmd is: "<< MpiCmdToStr(MPP_DEC_SET_CFG) << " , error is: " << RkMppRetToStr(rkRet);
        assert(false);
        goto END;
    }
    paramS64 = _fastOutput ? 1 : 0;
    if (RK_OP_FAIL(_mpi->control(_ctx, MPP_DEC_SET_IMMEDIATE_OUT, &paramS64)))
    {
        RK_LOG_ERROR << "MppApi::control fail, cmd is:" << MpiCmdToStr(MPP_DEC_SET_IMMEDIATE_OUT)  << " , error is: " << RkMppRetToStr(rkRet);
        assert(false);
        goto END;
    }
    paramS64 = (RK_S64)_timeout;
    if (RK_OP_FAIL(_mpi->control(_ctx, MPP_SET_OUTPUT_TIMEOUT, &paramS64)))
    {
        RK_LOG_ERROR << "MppApi::control fail, cmd is:" << MpiCmdToStr(MPP_SET_OUTPUT_TIMEOUT)  << " , error is: " << RkMppRetToStr(rkRet);
        assert(false);
        goto END;
    }
    mpp_dec_cfg_deinit(rkCfg);
    return true;
END:
    mpp_dec_cfg_deinit(rkCfg);
    return false;
}

bool RKDecoder::InitRkPacket()
{
    RK_LOG_INFO << "-- InitRkPacket";
    MPP_RET rkRet = MPP_OK;
    if (RK_OP_FAIL(mpp_packet_init(&_rkPacket, nullptr, 0)))
    {
        RK_LOG_ERROR << "mpp_packet_init fail, error is: " << RkMppRetToStr(rkRet);
        assert(false);
        goto END;
    }
    return true;
END:
    return false;
}

void RKDecoder::UnitRkPacket()
{
    RK_LOG_INFO << "-- UnitRkPacket";
    if (_rkPacket)
    {
        mpp_packet_deinit(&_rkPacket);
    }
    _rkPacket = nullptr;
}

bool RKDecoder::Init()
{
    std::lock_guard<std::mutex> lock(_mtx);
    RK_LOG_INFO << "Try to init RKDecoder";
    if (!CreateMPI())
    {
        RK_LOG_ERROR << "CreateMPI fail";
        assert(false);
        goto END;
    }
    if (!InitRkPacket())
    {
        RK_LOG_ERROR << "InitRkPacket fail";
        assert(false);
        goto END1;
    }
    return true;
END1:
    DestroyMPI();
END:
    return false;
}

void RKDecoder::Uninit()
{
    RK_LOG_INFO << "Uninit RKDecoder";
    UnitRkPacket();
    DestroyMPI();
}

bool RKDecoder::Start()
{
    std::lock_guard<std::mutex> lock(_mtx);
    if (_thread)
    {
        assert(false);
        return false;
    }
    if (!_sync)
    {
        _runing = true;
        _thread = std::make_shared<TaskQueue>();
        _thread->Start();
        _thread->Commit(std::make_shared<Promise<void>>([this]() -> void
        {
            RK_LOG_INFO << "RK_DEC_THD begin";
            while (_runing)
            {
                MppFrame frame = nullptr;
                do
                {
                    std::lock_guard<std::mutex> lock(_mtx);
                    MPP_RET rkRet = MPP_RET::MPP_OK;
                    if (!_mpi || !_ctx)
                    {
                        assert(false);
                        break;
                    }
                    if (RK_OP_FAIL(_mpi->decode_get_frame(_ctx, &frame)))
                    {
                        if (rkRet != MPP_RET::MPP_ERR_TIMEOUT)
                        {
                            RK_LOG_WARN << "MppApi::decode_get_frame fail, error is: " << RkMppRetToStr(rkRet);
                            assert(false);
                            break;
                        }
                    }
                } while (0);
                if (!frame)
                {
                    std::this_thread::sleep_for(std::chrono::milliseconds(1));
                    continue;
                }
                if (mpp_frame_get_info_change(frame))
                {
                    {
                        RK_U32 width = mpp_frame_get_width(frame);
                        RK_U32 height = mpp_frame_get_height(frame);
                        RK_U32 horStride = mpp_frame_get_hor_stride(frame);
                        RK_U32 verStride = mpp_frame_get_ver_stride(frame);
                        RK_LOG_INFO << "mpp_frame_get_info_change, " << width << "x" << height << ":" << horStride << "x" << verStride;
                    }
                    MPP_RET rkRet = MPP_RET::MPP_OK;
                    RK_U32 bufSize = mpp_frame_get_buf_size(frame);
                    do
                    {
                        std::lock_guard<std::mutex> lock(_mtx);
                        if (!_mpi)
                        {
                            break;
                        }
                        if (nullptr == _frmGrp) 
                        {
                            if (RK_OP_FAIL(mpp_buffer_group_get_internal(&_frmGrp, MPP_BUFFER_TYPE_ION)))
                            {
                                RK_LOG_WARN << "mpp_buffer_group_get_internal fail, error is: " << RkMppRetToStr(rkRet);
                                assert(false);
                                break;
                            }
                            if (RK_OP_FAIL(_mpi->control(_ctx, MPP_DEC_SET_EXT_BUF_GROUP, _frmGrp)))
                            {
                                RK_LOG_WARN << "MppApi::control fail cmd is:" << MpiCmdToStr(MPP_DEC_SET_EXT_BUF_GROUP) <<", error is: " << RkMppRetToStr(rkRet);
                                assert(false);
                                break;
                            }
                        }
                        else if (RK_OP_FAIL(mpp_buffer_group_clear(_frmGrp)))
                        {
                            RK_LOG_WARN << "mpp_buffer_group_clear fail, error is: " << RkMppRetToStr(rkRet);
                            assert(false);
                            break;
                        }
                        if (RK_OP_FAIL(mpp_buffer_group_limit_config(_frmGrp, bufSize, _bufSize)))
                        {
                            RK_LOG_WARN << "mpp_buffer_group_limit_config fail";
                            assert(false);
                            break;
                        }
                        if (RK_OP_FAIL(_mpi->control(_ctx, MPP_DEC_SET_INFO_CHANGE_READY, nullptr)))
                        {
                            RK_LOG_WARN << "MppApi::control fail cmd is:" << MpiCmdToStr(MPP_DEC_SET_INFO_CHANGE_READY) <<", error is: " << RkMppRetToStr(rkRet);
                            assert(false);
                            break;
                        }
                    } while (0);
                    continue;
                }
                // RK_LOG_INFO << "Pop";
                {
                    std::lock_guard<std::mutex> bufLock(_bufMtx);
                    RkDecoderAllocateMethod::ptr alloc = std::make_shared<RkDecoderAllocateMethod>();
                    alloc->frame = frame;
                    PixelsInfo info;
                    {
                        info.width = (int32_t)mpp_frame_get_hor_stride(frame);
                        info.height = (int32_t)mpp_frame_get_ver_stride(frame);
                        info.bitdepth = 8;
                        info.format = MppFrameFormatToPixformat(mpp_frame_get_fmt(frame));
                    }
                    StreamFrame::ptr streamFrame = std::make_shared<StreamFrame>(info, alloc);
                    _buffers.push_back(streamFrame);
                }
            }
            RK_LOG_INFO << "RK_DEC_THD end";
        }));
    }
    return true;
}

void RKDecoder::Stop()
{
    std::lock_guard<std::mutex> lock(_mtx);
    if (!_sync && !_thread)
    {
        assert(false);
    }
    if (!_sync)
    {
        _runing = false;
        _thread->Stop();
        _thread.reset();
    }    
}

bool RKDecoder::Push(AbstractPack::ptr pack)
{
    std::lock_guard<std::mutex> lock(_mtx);
    MPP_RET rkRet = MPP_OK;
    size_t maxTryTime = 30, curTryTime = 0;
#if ENABLE_RK_DECODE_DUMP
    {
        std::string filePath = "./dump.h264";
        std::ofstream ofs(filePath, std::ios::app);
        ofs.write((char *)pack->GetData(), pack->GetSize());
        ofs.flush();
        ofs.close();
    }
#endif /* ENABLE_RK_DECODE_DUMP */
    {
        mpp_packet_set_data(_rkPacket, pack->GetData());
        mpp_packet_set_size(_rkPacket, pack->GetSize());
        mpp_packet_set_pos(_rkPacket, pack->GetData());
        mpp_packet_set_length(_rkPacket, pack->GetSize());
        if (0 /* todo */)
        {
            mpp_packet_set_eos(_rkPacket);
        }
    }
    if (RK_OP_FAIL(_mpi->decode_put_packet(_ctx, _rkPacket)))
    {
        RK_LOG_WARN << "MppApi::decode_put_packet fail, error is: " << RkMppRetToStr(rkRet);
        assert(false);
        goto END;
    }
    if (!_sync)
    {
        return true;
    }
    do
    {
        MppFrame frame = nullptr;
        MPP_RET rkRet = MPP_RET::MPP_OK;
        if (RK_OP_FAIL(_mpi->decode_get_frame(_ctx, &frame)))
        {
            if (rkRet != MPP_RET::MPP_ERR_TIMEOUT)
            {
                RK_LOG_WARN << "MppApi::decode_get_frame fail, error is: " << RkMppRetToStr(rkRet);
                assert(false);
                break;
            }
        }
        if (!frame)
        {
            curTryTime++;
            if (curTryTime <= maxTryTime)
            {
                std::this_thread::sleep_for(std::chrono::milliseconds(1));
                continue;
            }
            else
            {
                break;
            }
        }
        if (mpp_frame_get_info_change(frame))
        {
            {
                RK_U32 width = mpp_frame_get_width(frame);
                RK_U32 height = mpp_frame_get_height(frame);
                RK_U32 horStride = mpp_frame_get_hor_stride(frame);
                RK_U32 verStride = mpp_frame_get_ver_stride(frame);
                RK_LOG_INFO << "mpp_frame_get_info_change, " << width << "x" << height << ":" << horStride << "x" << verStride;
            }
            MPP_RET rkRet = MPP_RET::MPP_OK;
            RK_U32 bufSize = mpp_frame_get_buf_size(frame);
            if (nullptr == _frmGrp) 
            {
                if (RK_OP_FAIL(mpp_buffer_group_get_internal(&_frmGrp, MPP_BUFFER_TYPE_ION)))
                {
                    RK_LOG_WARN << "mpp_buffer_group_get_internal fail, error is: " << RkMppRetToStr(rkRet);
                    assert(false);
                    break;
                }
                if (RK_OP_FAIL(_mpi->control(_ctx, MPP_DEC_SET_EXT_BUF_GROUP, _frmGrp)))
                {
                    RK_LOG_WARN << "MppApi::control fail cmd is:" << MpiCmdToStr(MPP_DEC_SET_EXT_BUF_GROUP) <<", error is: " << RkMppRetToStr(rkRet);
                    assert(false);
                    break;
                }
            }
            else if (RK_OP_FAIL(mpp_buffer_group_clear(_frmGrp)))
            {
                RK_LOG_WARN << "mpp_buffer_group_clear fail, error is: " << RkMppRetToStr(rkRet);
                assert(false);
                break;
            }
            if (RK_OP_FAIL(mpp_buffer_group_limit_config(_frmGrp, bufSize, _bufSize)))
            {
                RK_LOG_WARN << "mpp_buffer_group_limit_config fail";
                assert(false);
                break;
            }
            if (RK_OP_FAIL(_mpi->control(_ctx, MPP_DEC_SET_INFO_CHANGE_READY, nullptr)))
            {
                RK_LOG_WARN << "MppApi::control fail cmd is:" << MpiCmdToStr(MPP_DEC_SET_INFO_CHANGE_READY) <<", error is: " << RkMppRetToStr(rkRet);
                assert(false);
                break;
            }
            continue;
        }
        {
            std::lock_guard<std::mutex> bufLock(_bufMtx);
            RkDecoderAllocateMethod::ptr alloc = std::make_shared<RkDecoderAllocateMethod>();
            alloc->frame = frame;
            PixelsInfo info;
            {
                info.width = (int32_t)mpp_frame_get_hor_stride(frame);
                info.height = (int32_t)mpp_frame_get_ver_stride(frame);
                info.bitdepth = 8;
                info.format = MppFrameFormatToPixformat(mpp_frame_get_fmt(frame));
            }
#if 0 /* ENABLE_RK_DECODE_DUMP */
            {
                static uint64_t curDumpTime = 0;
#if RK_DECODE_DUMP_SIGNGLE_FILE
                std::string dumpPath = (std::stringstream() << "./" << info.width << "_" << info.height << ".dmp").str();
#else
                std::string dumpPath = (std::stringstream() << "./" << curDumpTime++ << "_" << info.width << "_" << info.height << ".dmp").str();                
#endif /* RK_DECODE_DUMP_SIGNGLE_FILE */
                std::ofstream ofs(dumpPath, std::ios::app);
                ofs.write((char*)mpp_buffer_get_ptr(mpp_frame_get_buffer(frame)), info.width * info.height * 1.5);
                ofs.flush();
                ofs.close();
            }
#endif /* ENABLE_RK_DECODE_DUMP */
            StreamFrame::ptr streamFrame = std::make_shared<StreamFrame>(info, alloc);
            _buffers.push_back(streamFrame);
        }
    } while (1);
    return true;
END:
    return false;
}

bool RKDecoder::Pop(AbstractFrame::ptr& frame)
{
    std::lock_guard<std::mutex> lock(_bufMtx);
    if (_buffers.empty())
    {
        return false;
    }
    else
    {
        frame = _buffers.front();
        _buffers.pop_front();
        return true;
    }
}

bool RKDecoder::CanPush()
{
    return true;
}

bool RKDecoder::CanPop()
{
    std::lock_guard<std::mutex> lock(_bufMtx);
    return _buffers.empty();
}

const std::string& RKDecoder::Description()
{
    static std::string description = "RKDecoder";
    return description;
}

} // namespace Codec
} // namespace Mmp