#include "VaH264SliceDecodingProcess.h"

namespace Mmp
{
namespace Codec
{

VaH264DecodePictureContext::VaH264DecodePictureContext()
{
    surface = VA_INVALID_ID;
}

void VaH264DecodePictureContext::SetVADecoder(VADecoder::ptr decoder)
{
    _decoder = decoder;
}

VaH264DecodePictureContext::~VaH264DecodePictureContext()
{
    assert(_decoder);
    if (_decoder)
    {
        _decoder->DestroyVaSurface(surface);
        for (const auto& paramBuffer : paramBuffers)
        {
            _decoder->DestroyVaParamBuffer(paramBuffer);
        }
        for (const auto& sliceBuffer : sliceBuffers)
        {
            _decoder->DestroyVaSliceDataBuffer(sliceBuffer);
        }
    }
}

H264PictureContext::ptr VaH264SliceDecodingProcess::CreatePictureContext()
{
    return std::make_shared<VaH264DecodePictureContext>();
}

} // namespace Codec
} // namespace Mmp