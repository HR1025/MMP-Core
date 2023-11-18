#include "NormalAllocateMethod.h"

#include <cassert>
#include <memory.h>

namespace Mmp
{

NormalAllocateMethod::NormalAllocateMethod()
{
    _data = nullptr;
    _size = 0;
}

NormalAllocateMethod::~NormalAllocateMethod()
{
    if (_data)
    {
        free(_data);
    }
}

void* NormalAllocateMethod::Malloc(size_t size)
{
    // Hint : 只允许分配一次内存
    if (!_data)
    {
        _data = malloc(size);
        _size = size;
        assert(_data);
    }
    else
    {
        assert(size == _size);
    }
    return _data;
}

void* NormalAllocateMethod::Resize(void* data, size_t size)
{
    // Todo : 是否允许重设内存大小,在多对象使用时可能导致内存越界问题
    assert(data);
    if (size != 0)
    {
        assert(_data == data);
        _data = realloc(data, size);
        assert(_data);
    }
    else
    {
        free(_data);
        _data = nullptr;
    }
    _size = size;
    return _data;
}

void* NormalAllocateMethod::GetAddress(uint64_t offset)
{
    return (uint8_t*)_data + offset;
}

const std::string& NormalAllocateMethod::Tag()
{
    static const std::string tag = "NormalAllocateMethod";
    return tag;
}

} // namespace Mmp