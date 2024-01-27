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

void VADecodePictureContext::SetDecoderContext(VADecoderContext::ptr context)
{
    _context = context;
}

VADecodePictureContext::~VADecodePictureContext()
{
    assert(_context);
    if (_context)
    {
        _context->DestroyVaSurface(surface);
        for (const auto& paramBuffer : paramBuffers)
        {
            _context->DestroyVaParamBuffer(paramBuffer);
        }
        for (const auto& sliceBuffer : sliceBuffers)
        {
            _context->DestroyVaSliceDataBuffer(sliceBuffer);
        }
    }
}

} // namespace Codec
} // namespace Mmp