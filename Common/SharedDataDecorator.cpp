#include "SharedDataDecorator.h"

namespace Mmp
{

SharedDataDecorator::SharedDataDecorator(AbstractSharedData::ptr sharedData)
{
    _sharedData = sharedData;
}

void SharedDataDecorator::SetCapacity(size_t size)
{
    _sharedData->SetCapacity(size);
}

size_t SharedDataDecorator::GetCapcaity()
{
    return _sharedData->GetCapcaity();
}

void SharedDataDecorator::SetSize(size_t size)
{
    _sharedData->SetSize(size);
}

size_t SharedDataDecorator::GetSize()
{
    return _sharedData->GetSize();
}

void* SharedDataDecorator::GetData(uint64_t offset)
{
    return _sharedData->GetData(offset);
}

AbstractAllocateMethod::ptr SharedDataDecorator::GetAllocateMethod()
{
    return _sharedData->GetAllocateMethod();
}

} // namespace Mmp