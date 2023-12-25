#include "SegmentAllocateMethod.h"

#include <cassert>

namespace Mmp
{

SegmentAllocateMethod::SegmentAllocateMethod()
{
    _sharedData = nullptr;
    _offset     = 0;
    _length     = 0;
    _tag        = "SegmentAllocateMethod";
}

SegmentAllocateMethod::~SegmentAllocateMethod()
{
    // nothing to do
}

void* SegmentAllocateMethod::Malloc(size_t size)
{
    assert(_sharedData);
    assert(_offset + _length <= static_cast<uint64_t>(_sharedData->GetSize()));
    assert(size == (size_t)_length);
    uint8_t* data =  reinterpret_cast<uint8_t*>(_sharedData->GetData(_offset));
    return reinterpret_cast<void*>(data);
}

void* SegmentAllocateMethod::Resize(void* /* data */, size_t /* size */)
{
    assert(false);
    return nullptr;
}

void* SegmentAllocateMethod::GetAddress(uint64_t offset)
{
    uint8_t* data =  reinterpret_cast<uint8_t*>(_sharedData->GetData(_offset));
    return data ? data + offset : nullptr;
}

const std::string& SegmentAllocateMethod::Tag()
{
    return _tag;
}

void SegmentAllocateMethod::SetRange(uint64_t offset, uint64_t length, const AbstractSharedData::ptr& sharedData)
{
    _offset     = offset;
    _length     = length;
    _sharedData = sharedData;
}

} // namespace Mmp