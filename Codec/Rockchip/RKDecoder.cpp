#include "RKDecoder.h"

#include <cassert>
#include <chrono>
#include <memory>
#include <mutex>
#include <thread>

#include "rockchip/mpp_frame.h"

#include "Common/Promise.h"

#include "RKCommon.h"
#include "RKUtil.h"
#include "RKTranslator.h"

namespace Mmp
{
namespace Codec
{

RKDecoder::RKDecoder()
{
    _ctx = nullptr;
    _mpi = nullptr;
    _timeout = 100;
    _maxFrame = 16;
    _rkPacket = nullptr;
    _frmGrp = nullptr;
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
    // todo : something to config, for example base:split_parse
    if (RK_OP_FAIL(_mpi->control(_ctx, MPP_DEC_SET_CFG, rkCfg)))
    {
        RK_LOG_ERROR << "MppApi::control fail, cmd is: "<< MpiCmdToStr(MPP_DEC_SET_CFG) << " , error is: " << RkMppRetToStr(rkRet);
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
    if (!ReinitDecConfig())
    {
        RK_LOG_WARN << "ReinitDecConfig fail";
        assert(false);
        goto END1;
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
                        if (RK_OP_FAIL(mpp_buffer_group_get_internal(&_frmGrp, MPP_BUFFER_TYPE_DRM)))
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
                    if (RK_OP_FAIL(mpp_buffer_group_limit_config(_frmGrp, bufSize, 24)))
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
            }
            // TODO
            // RK_LOG_INFO << "Pop";
            mpp_frame_deinit(&frame);
        }
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
    std::lock_guard<std::mutex> lock(_mtx);
    MPP_RET rkRet = MPP_OK;
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
    // RK_LOG_INFO << "Push";
    return true;
END:
    return false;
}

bool RKDecoder::Pop(AbstractFrame::ptr& frame)
{
    assert(false);
    return false;
}

bool RKDecoder::CanPush()
{
    assert(false);
    return false;
}

bool RKDecoder::CanPop()
{
    assert(false);
    return false;
}

const std::string& RKDecoder::Description()
{
    static std::string description = "RKDecoder";
    return description;
}

} // namespace Codec
} // namespace Mmp