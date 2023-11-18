#include "AbstractFrame.h"

#include <cassert>

namespace Mmp
{

AbstractFrame::AbstractFrame(size_t size, AbstractAllocateMethod::ptr allocateMethod)
    : SharedData(size, allocateMethod)
{

}

PictureFrame::PictureFrame(PixelsInfo info, AbstractAllocateMethod::ptr allocateMethod)
    : AbstractFrame(0, allocateMethod)
{
    this->info = info;
    this->sideData = info;
    size_t size = (size_t)(info.width*info.height*BytePerPixel(info.format));
    SetCapacity(size);
    SetSize(size);
}

} // namespace Mmp