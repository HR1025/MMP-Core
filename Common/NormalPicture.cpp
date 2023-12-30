#include "NormalPicture.h"

#include "NormalSharedData.h"

namespace Mmp
{

NormalPicture::NormalPicture(PixelsInfo info, AbstractAllocateMethod::ptr allocateMethod)
{
    size_t size = (size_t)(info.width*info.height*BytePerPixel(info.format));
    this->info = info;
    this->_sharedData = std::make_shared<NormalSharedData>(size, allocateMethod);
}

} // namespace Mmp