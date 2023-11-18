#include "StringAllocateMethod.h"

#include <cassert>

namespace Mmp
{

void* StringAllocateMethod::Malloc(size_t size)
{
    assert(size == _data.size());
    return (void*)(_data.data());
}

void* StringAllocateMethod::Resize(void* /* data */, size_t /* size */)
{
    assert(false);
    return (void*)(_data.data());
}

void* StringAllocateMethod::GetAddress(uint64_t offset)
{
    return (uint8_t*)(_data.data()) + offset;
}

const std::string& StringAllocateMethod::Tag()
{
    static const std::string tag = "StringAllocateMethod";
    return tag;
}

} // namespace Mmp