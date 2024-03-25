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
namespace Codec
{

class RkStreamFrame : public StreamFrame
{
public:
    using ptr = std::shared_ptr<RkStreamFrame>;
public:
    RkStreamFrame(const PixelsInfo& info, AbstractAllocateMethod::ptr allocateMethod);
    ~RkStreamFrame();
public:
    MppFrame frame;
};

RkStreamFrame::RkStreamFrame(const PixelsInfo& info, AbstractAllocateMethod::ptr allocateMethod)
    : StreamFrame(info, allocateMethod)
{
    frame = nullptr;
}

RkStreamFrame::~RkStreamFrame()
{
    if (frame)
    {
        mpp_frame_deinit(&frame);
    }
}

RKDecoder::RKDecoder()
{
    _ctx = nullptr;
    _mpi = nullptr;
    _rkPacket = nullptr;
    _fastOutput = false;
    _poolSize = 24;
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
    paramS64 = 1;
    if (RK_OP_FAIL(_mpi->control(_ctx, MPP_SET_INPUT_TIMEOUT, &paramS64)))
    {
        RK_LOG_ERROR << "MppApi::control fail, cmd is:" << MpiCmdToStr(MPP_SET_INPUT_TIMEOUT)  << " , error is: " << RkMppRetToStr(rkRet);
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
    _runing = true;
    _thread = std::make_shared<TaskQueue>();
    _thread->Start();
    _thread->Commit(std::make_shared<Promise<void>>([this]() -> void
    {
        RK_LOG_INFO << "RK_DEC_THD begin";
        do
        {
            MppFrame frame = nullptr;
            MPP_RET rkRet = MPP_RET::MPP_OK;
            {
                std::lock_guard<std::mutex> lock(_mtx);
                if (_mpi)
                {
                    if (RK_OP_FAIL(_mpi->decode_get_frame(_ctx, &frame)))
                    {
                        if (rkRet != MPP_RET::MPP_ERR_TIMEOUT)
                        {
                            RK_LOG_WARN << "MppApi::decode_get_frame fail, error is: " << RkMppRetToStr(rkRet);
                            assert(false);
                        }
                    }
                }
            }
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
                MppBufferGroup frmGrp = _poolContext ? _poolContext->GetFrameGroup() : nullptr;
                if (frmGrp)
                {
                    if (RK_OP_FAIL(mpp_buffer_group_clear(frmGrp)))
                    {
                        RK_LOG_WARN << "mpp_buffer_group_clear fail, error is: " << RkMppRetToStr(rkRet);
                        assert(false);
                        std::this_thread::sleep_for(std::chrono::milliseconds(1));
                    }
                }
                _poolContext = std::make_shared<RkBufferPoolContext>();
                _poolContext->Init(mpp_frame_get_buf_size(frame), _poolSize);
                frmGrp = _poolContext->GetFrameGroup();
                {
                    std::lock_guard<std::mutex> lock(_mtx);
                    if (_mpi)
                    {
                        if (RK_OP_FAIL(_mpi->control(_ctx, MPP_DEC_SET_EXT_BUF_GROUP, frmGrp)))
                        {
                            RK_LOG_WARN << "MppApi::control fail cmd is:" << MpiCmdToStr(MPP_DEC_SET_EXT_BUF_GROUP) <<", error is: " << RkMppRetToStr(rkRet);
                            assert(false);
                            break;
                        }
                        if (RK_OP_FAIL(_mpi->control(_ctx, MPP_DEC_SET_INFO_CHANGE_READY, nullptr)))
                        {
                            RK_LOG_WARN << "MppApi::control fail cmd is:" << MpiCmdToStr(MPP_DEC_SET_INFO_CHANGE_READY) <<", error is: " << RkMppRetToStr(rkRet);
                            assert(false);
                            break;
                        }
                    }
                }

                continue;
            }
            {
                std::lock_guard<std::mutex> bufLock(_bufMtx);
                RkBufferContext::ptr alloc = _poolContext->GetRkBufferContextByFd(mpp_buffer_get_index(mpp_frame_get_buffer(frame)));
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
                RkStreamFrame::ptr streamFrame = std::make_shared<RkStreamFrame>(info, alloc);
                streamFrame->frame = frame;
                _buffers.push_back(streamFrame);
            }
        } while (_runing);
        RK_LOG_INFO << "RK_DEC_THD end";
    }));
    return true;
}

void RKDecoder::Stop()
{
    std::lock_guard<std::mutex> lock(_mtx);
    if (!_thread)
    {
        assert(false);
    }
    _runing = false;
    _thread->Stop();
    _thread.reset();
}

bool RKDecoder::Push(AbstractPack::ptr pack)
{
    
    MPP_RET rkRet = MPP_OK;
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
    }
    do
    {
        {
            std::lock_guard<std::mutex> lock(_mtx);
            if (RK_OP_FAIL(_mpi->decode_put_packet(_ctx, _rkPacket)))
            {
                if (MPP_ERR_BUFFER_FULL != rkRet)
                {
                    RK_LOG_WARN << "MppApi::decode_put_packet fail, error is: " << RkMppRetToStr(rkRet);
                    assert(false);
                    goto END;
                }
            }
        }
        if (MPP_SUCCESS == rkRet)
        {
            break;
        }
        else if (MPP_ERR_BUFFER_FULL == rkRet)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
    } while (_runing);
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