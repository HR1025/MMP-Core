#include "OpenH264Decoder.h"

#include <cassert>

#include "OpenH264Util.h"

namespace Mmp
{
namespace Codec
{

OpenH264Decoder::OpenH264Decoder()
{
    _decoder = nullptr;
}

void OpenH264Decoder::SetParameter(Any parameter)
{
    
}

Any OpenH264Decoder::GetParamter()
{
    return Any();
}

bool OpenH264Decoder::Init()
{
    assert(!_decoder);
    if (WelsCreateDecoder(&_decoder) != 0)
    {
        OPENH264_LOG_ERROR << "WelsCreateDecoder fail";
        assert(false);
        goto END;
    }
    {
        int logLevel = WELS_LOG_DETAIL;
        WelsTraceCallback logCallback = OpenH264LogCallBack;
        _decoder->SetOption(DECODER_OPTION_TRACE_LEVEL, &logLevel);
        _decoder->SetOption(DECODER_OPTION_TRACE_CALLBACK, (void *)&logCallback);
    }
    {
        SDecodingParam param = {};
        param.eEcActiveIdc = ERROR_CON_DISABLE;
        param.sVideoProperty.eVideoBsType = VIDEO_BITSTREAM_AVC;
        if (_decoder->Initialize(&param) != cmResultSuccess)
        {
            OPENH264_LOG_ERROR << "CWelsDecoder::Initialize fail";
            assert(false);
            goto END1;
        }
    }
    return true;
END1:
    WelsDestroyDecoder(_decoder);
    _decoder = nullptr;
END:
    return false;
}

void OpenH264Decoder::Uninit()
{
    assert(_decoder);
    WelsDestroyDecoder(_decoder);
    _decoder = nullptr;
}

bool OpenH264Decoder::Start()
{
    return true;
}

void OpenH264Decoder::Stop()
{
    
}

bool OpenH264Decoder::Push(AbstractPack::ptr pack)
{
    assert(false);
    return false;
}

bool OpenH264Decoder::Pop(AbstractFrame::ptr& frame) 
{
    assert(false);
    return false;
}

bool OpenH264Decoder::CanPush() 
{
    assert(false);
    return false;
}

bool OpenH264Decoder::CanPop()
{
    assert(false);
    return false;
}

void OpenH264Decoder::SetListener(OnStatusChange onStatusChange)
{
    assert(false);
}

void OpenH264Decoder::DelListener()
{
    assert(false);
}

const std::string& OpenH264Decoder::Description()
{
    assert(false);
}

} // namespace Codec
} // namespace Mmp