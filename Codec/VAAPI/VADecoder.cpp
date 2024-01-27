#include "VADecoder.h"

#include <string>

#include "Any.h"
#include "VAUtil.h"
#include "VADevice.h"
#include "VATranslator.h"

namespace Mmp
{
namespace Codec
{

VADecoder::VADecoder()
{
    // TODO : 支持可配置
    _frameBufSize = 5;
}

bool VADecoder::Init()
{
    return true;
}

void VADecoder::Uninit()
{

}

void VADecoder::SetParameter(Any parameter)
{

}

Any VADecoder::GetParamter() 
{
    return Any();
}

bool VADecoder::Pop(AbstractFrame::ptr& frame)
{
    std::lock_guard<std::mutex> lock(_frameBufMtx);
    if (!_frameBufs.empty())
    {
        frame = _frameBufs.front();
        _frameBufs.pop_front();
        return true;
    }
    else
    {
        frame = nullptr;
        return false;
    }
}

bool VADecoder::CanPush()
{
    std::lock_guard<std::mutex> lock(_frameBufMtx);
    return _frameBufs.size() <= _frameBufSize;
}

bool VADecoder::CanPop() 
{
    std::lock_guard<std::mutex> lock(_frameBufMtx);
    return !_frameBufs.empty();
}

const std::string& VADecoder::Description()
{
    static std::string description = "VADecoder";
    return description;
}

bool VADecoder::VaInit()
{
    _contex = std::make_shared<VADecoderContext>(_params);
    return _contex->Init();
}

void VADecoder::VaUninit()
{
    _contex.reset();
}

VaDecoderParams VADecoder::GetDecoderParams()
{
    return _params;
}

void VADecoder::SetDecoderParams(const VaDecoderParams& param)
{
    if (_params != param)
    {
        VaDecoderParams oldParams = _params;
        _params = param;
        VaUninit();
        if (!VaInit())
        {
            assert(false);
        } 
        OnVaDecoderParamsChange(oldParams, param);
    }
}

void VADecoder::PushFrame(StreamFrame::ptr frame)
{
    std::lock_guard<std::mutex> lock(_frameBufMtx);
    _frameBufs.push_back(frame);
}

VADecoderContext::ptr VADecoder::GetContext()
{
    assert(_contex);
    return _contex;
}

void VADecoder::OnVaDecoderParamsChange(const VaDecoderParams& oldValue, const VaDecoderParams& newValue)
{

}

} // namespace Codec
} // namespace Mmp