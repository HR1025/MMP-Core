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

bool VADecoder::Push(AbstractPack::ptr pack)
{
    return false;
}

bool VADecoder::Pop(AbstractFrame::ptr& frame)
{
    return false;
}

bool VADecoder::CanPush()
{
    return false;
}

bool VADecoder::CanPop() 
{
    return false;
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