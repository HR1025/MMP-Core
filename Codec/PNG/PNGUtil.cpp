#include "PNGUtil.h"

#include <cassert>
#include <memory.h>

#include "Common/AllocateMethodFactory.h"

namespace Mmp
{
namespace Codec
{

void PngAllocateMethod::Register()
{
    AllocateMethodFactory::DefaultFactory().RegisterAllocateMethodClass("LodePngAllocateMethod", new Instantiator<PngAllocateMethod,AbstractAllocateMethod>);
}

PngAllocateMethod::PngAllocateMethod()
{
    data = nullptr;
    size = 0;
    _tag = "lodepng";
}

PngAllocateMethod::~PngAllocateMethod()
{
    // Hint : 资源的实际回收由 PngAllocateMethod 析构负责,而非 PngAllocateMethod::Free
    if (data)
    {
        free(data);
    }
    else
    {
        assert(false);
    }
}

void* PngAllocateMethod::Malloc(size_t size)
{
    assert(size == this->size);
    return data;
}

void* PngAllocateMethod::Resize(void* data, size_t size)
{
    assert(false);
    return data;
}

void* PngAllocateMethod::GetAddress(uint64_t offset)
{
    return (uint8_t *)data + offset;
}

const std::string& PngAllocateMethod::Tag()
{
    return _tag;
}

} // namespace Codec
} // namespace Mmp