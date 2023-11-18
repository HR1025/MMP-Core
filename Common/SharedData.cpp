#include "SharedData.h"

#include <cassert>
#include <algorithm>

#include "AllocateMethodFactory.h"

namespace Mmp
{

SharedData::SharedData(size_t size, AbstractAllocateMethod::ptr allocateMethod)
{
    _allocateMethod = allocateMethod ? allocateMethod : AllocateMethodFactory::DefaultFactory().CreateAllocateMethod("NormalAllocateMethod");
    if (size != 0)
    {
        _data     = _allocateMethod->Malloc(size);
        _capacity = size;
        _size     = size;
    }
    else
    {
        _data     = nullptr;
        _capacity = 0;
        _size     = 0;
    }
}

SharedData::~SharedData()
{
    // Hint : 内存回收完全交由 _allocateMethod 进行处理
}

void  SharedData::SetCapacity(size_t size)
{
    // (1) allloc memory
    if (size !=0 && _capacity == 0 /* && size == 0 */)
    {
        assert(!_data);
        _data = _allocateMethod->Malloc(size);
        _capacity = size;
    }
    // (2) realloc
    else if (size !=0 && _capacity != 0)
    {
        _data = _allocateMethod->Resize(_data, _capacity);
        _capacity = size;
        _size = std::min(_capacity, _size);
    }
}

size_t SharedData::GetCapcaity()
{
    return _capacity;
}

void SharedData::SetSize(size_t size)
{
    assert(size <= _capacity);
    _size = std::min(size, _capacity);
}

size_t SharedData::GetSize()
{
    return _size;
}

void* SharedData::GetData(uint64_t offset)
{
    return offset < _capacity ? (uint8_t*)_allocateMethod->GetAddress(offset) : nullptr;
}

AbstractAllocateMethod::ptr SharedData::GetAllocateMethod()
{
    return _allocateMethod;
}

} // namespace Mmp