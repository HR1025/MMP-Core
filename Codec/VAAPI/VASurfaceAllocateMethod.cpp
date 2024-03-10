#include "VASurfaceAllocateMethod.h"

#include <cassert>

#include "VADecoderContext.h"

namespace Mmp
{
namespace Codec
{

VASurfaceAllocateMethod::VASurfaceAllocateMethod(VADecodePictureContext::ptr context)
{
    _context = context;
    _data = nullptr;
    _image = {};
    _isMap = false;
}

VASurfaceAllocateMethod::~VASurfaceAllocateMethod()
{
    if (_isMap)
    {
        UnMap();
    }
}

void* VASurfaceAllocateMethod::Malloc(size_t /* size */)
{
    return _data;
}

void* VASurfaceAllocateMethod::Resize(void* /* data */, size_t /* size */)
{
    assert(false);
    return _data;
}

void* VASurfaceAllocateMethod::GetAddress(uint64_t offset)
{
    Map();
    assert(_data);
    return _data ? reinterpret_cast<uint8_t*>(_data) + offset : nullptr;
}

const std::string& VASurfaceAllocateMethod::Tag()
{
    static std::string tag = "VASurfaceAllocateMethod";
    return tag;
}

void VASurfaceAllocateMethod::Sync()
{
    UnMap();
    _context->GetDecoderContext()->SyncVaSurface(_context->surface);
}

void VASurfaceAllocateMethod::Map()
{
    if (!_isMap)
    {
        Sync();
        if (_context->GetDecoderContext()->MapVaSurface(_context->surface, _image, _data))
        {
            _isMap = true;
            MMP_LOG_TRACE << "Map, surface id is: " << _context->surface << " , image id is: " << _image.image_id;
        }
        else
        {
            _image = {};
            _data = nullptr;
            assert(false);
        }
    }
}

void VASurfaceAllocateMethod::UnMap()
{
    if (_isMap)
    {
        MMP_LOG_TRACE << "Unmap, image id is: " << _image.image_id;
        _context->GetDecoderContext()->UnMapVaSurface(_image);
        _image = {};
        _data = nullptr;
        _isMap = false;
    }
}

} // namespace Codec
} // namespace Mmp