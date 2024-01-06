#include "RKDecoder.h"

#include <cassert>
#include <mutex>

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
    _frameGroup = nullptr;
    _maxFrame = 16;
    _rkPacket = nullptr;
}

void RKDecoder::SetParameter(Any parameter)
{
    assert(false);
}

Any RKDecoder::GetParamter()
{
    assert(false);
    return Any();
}

bool RKDecoder::CreateMPI()
{
    RK_LOG_INFO << "-- Create MPI";
    MppCodingType codecType = CodecTypeToRkType(GetCodecType());
    MPP_RET ret = MPP_OK;
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
    if (MPP_OK != (ret = mpp_create(&_ctx, &_mpi)))
    {
        RK_LOG_ERROR << "mpp_create fail, error is: " << RkMppRetToStr(ret);
        assert(false);
        goto END1;
    }
    if (MPP_OK != (ret = mpp_init(_ctx, MppCtxType::MPP_CTX_DEC, codecType)))
    {
        RK_LOG_ERROR << "mpp_init fail, error is: " << RkMppRetToStr(ret);
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
    MPP_RET ret = MPP_OK;
    MppDecCfg rkCfg = nullptr;
    RK_S64 paramS64;
    mpp_dec_cfg_init(&rkCfg);
    if (MPP_OK != (ret = _mpi->control(_ctx, MPP_DEC_GET_CFG, rkCfg)))
    {
        RK_LOG_ERROR << "MppApi::control fail, cmd is: MPP_DEC_GET_CFG, error is: " << RkMppRetToStr(ret);
        assert(false);
        goto END;
    }
    // todo : something to config, for example base:split_parse
    if (MPP_OK != (ret = _mpi->control(_ctx, MPP_DEC_SET_CFG, rkCfg)))
    {
        RK_LOG_ERROR << "MppApi::control fail, cmd is: MPP_DEC_SET_CFG, error is: " << RkMppRetToStr(ret);
        assert(false);
        goto END;
    }
    paramS64 = (RK_S64)_timeout;
    if (MPP_OK != (ret = _mpi->control(_ctx, MPP_SET_OUTPUT_TIMEOUT, &paramS64)))
    {
        RK_LOG_ERROR << "MppApi::control fail, cmd is: MPP_SET_OUTPUT_TIMEOUT, error is: " << RkMppRetToStr(ret);
        assert(false);
        goto END;
    }
    if (MPP_OK != (ret = mpp_buffer_group_get_internal(&_frameGroup, MPP_BUFFER_TYPE_ION)))
    {
        RK_LOG_ERROR << "mpp_buffer_group_get_internal fail, error is: " << RkMppRetToStr(ret);
        assert(false);
        goto END;
    }
    mpp_dec_cfg_init(&rkCfg);
    return true;
END:
    mpp_dec_cfg_init(&rkCfg);
    return false;
}

bool RKDecoder::InitRkPacket()
{
    RK_LOG_INFO << "-- InitRkPacket";
    MPP_RET ret = MPP_OK;
    if (MPP_OK != (ret = mpp_packet_init(&_rkPacket, nullptr, 0)))
    {
        RK_LOG_ERROR << "mpp_packet_init fail, error is: " << RkMppRetToStr(ret);
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
    assert(false);
    return false;
}

void RKDecoder::Stop()
{
    assert(false);
}

bool RKDecoder::Push(AbstractPack::ptr pack)
{
    std::lock_guard<std::mutex> lock(_mtx);
    MPP_RET ret = MPP_OK;
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
    if (MPP_OK != (ret = _mpi->decode_put_packet(_ctx, _rkPacket)))
    {
        RK_LOG_WARN << "MppApi::decode_put_packet fail, error is: " << RkMppRetToStr(ret);
        assert(false);
        goto END;
    }
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
    static std::string description = "";
    assert(false);
    return description;
}

} // namespace Codec
} // namespace Mmp