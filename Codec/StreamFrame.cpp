#include "StreamFrame.h"

#include "Common/NormalSharedData.h"

namespace Mmp
{
namespace Codec
{

StreamFrame::StreamFrame(const PixelsInfo& info, AbstractAllocateMethod::ptr allocateMethod)
{
    size_t size = (size_t)(info.width*info.height*BytePerPixel(info.format));
    this->_sharedData = std::make_shared<NormalSharedData>(size, allocateMethod);
    this->info = info;
    this->sideData = info;
}

StreamFrame::StreamFrame(const PixelsInfo& info, AbstractSharedData::ptr data)
{
    this->_sharedData = data;
    this->info = info;
    this->sideData = info;
}

} // namespace Codec
} // namespace Mmp