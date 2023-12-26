#include "NormalPicture.h"

#include "NormalSharedData.h"

namespace Mmp
{

NormalPicture::NormalPicture(PixelsInfo info, AbstractAllocateMethod::ptr allocateMethod)
    : AbstractPicture(info)
{
    size_t size = (size_t)(info.width*info.height*BytePerPixel(info.format));
    _data = std::make_shared<NormalSharedData>(size, allocateMethod);
}

NormalPicture::~NormalPicture()
{
}

void  NormalPicture::SetCapacity(size_t size)
{
    _data->SetCapacity(size);
}

size_t NormalPicture::GetCapcaity()
{
    return _data->GetCapcaity();
}

void NormalPicture::SetSize(size_t size)
{
    _data->SetSize(size);
}

size_t NormalPicture::GetSize()
{
    return _data->GetSize();
}

void* NormalPicture::GetData(uint64_t offset)
{
    return _data->GetData(offset);
}

AbstractAllocateMethod::ptr NormalPicture::GetAllocateMethod()
{
    return _data->GetAllocateMethod();
}

} // namespace Mmp