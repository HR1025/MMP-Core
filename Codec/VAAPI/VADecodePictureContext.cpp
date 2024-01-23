#include "VADecodePictureContext.h"

#include "VADecoder.h"

namespace Mmp
{
namespace Codec
{

VADecodePictureContext::VADecodePictureContext()
{
    surface = VA_INVALID_ID;
}

void VADecodePictureContext::SetVADecoder(std::shared_ptr<VADecoder> decoder)
{
    _decoder = decoder;
}

VADecodePictureContext::~VADecodePictureContext()
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

} // namespace Codec
} // namespace Mmp