#include "NormalPack.h"

#include "NormalSharedData.h"

namespace Mmp
{

NormalPack::NormalPack(size_t size, AbstractAllocateMethod::ptr allocateMethod)
{
    _data = std::make_shared<NormalSharedData>(size, allocateMethod);
}

NormalPack::~NormalPack()
{
}

void  NormalPack::SetCapacity(size_t size)
{
    _data->SetCapacity(size);
}

size_t NormalPack::GetCapcaity()
{
    return _data->GetCapcaity();
}

void NormalPack::SetSize(size_t size)
{
    _data->SetSize(size);
}

size_t NormalPack::GetSize()
{
    return _data->GetSize();
}

void* NormalPack::GetData(uint64_t offset)
{
    return _data->GetData();
}

AbstractAllocateMethod::ptr NormalPack::GetAllocateMethod()
{
    return _data->GetAllocateMethod();
}

} // namespace Mmp