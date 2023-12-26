#include "NormalFrame.h"

#include "NormalSharedData.h"

namespace Mmp
{

NormalFrame::NormalFrame(size_t size, AbstractAllocateMethod::ptr allocateMethod)
{
    _data = std::make_shared<NormalFrame>(size, allocateMethod);
}

NormalFrame::~NormalFrame()
{
}

void  NormalFrame::SetCapacity(size_t size)
{
    _data->SetCapacity(size);
}

size_t NormalFrame::GetCapcaity()
{
    return _data->GetCapcaity();
}

void NormalFrame::SetSize(size_t size)
{
    _data->SetSize(size);
}

size_t NormalFrame::GetSize()
{
    return _data->GetSize();
}

void* NormalFrame::GetData(uint64_t offset)
{
    return _data->GetData(offset);
}

AbstractAllocateMethod::ptr NormalFrame::GetAllocateMethod()
{
    return _data->GetAllocateMethod();
}

} // namespace Mmp