#include "PixelsInfo.h"

namespace Mmp
{

PixelsInfo::PixelsInfo()
{
    width    = -1;
    height   = -1;
    bitdepth = 0;
    format   = PixelFormat::RGB888;
}

PixelsInfo::PixelsInfo(int32_t width, int32_t height, int32_t bitdepth, PixelFormat format)
{
    this->width    = width;
    this->height   = height;
    this->bitdepth = bitdepth;;
    this->format   = format;
}

bool operator==(const PixelsInfo& left, const PixelsInfo& right)
{
    if (left.width == right.width &&
        left.height == right.height &&
        left.bitdepth == right.bitdepth &&
        left.format == right.format
    )
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool operator!=(const PixelsInfo& left, const PixelsInfo& right)
{
    return left == right ? false : true;
}

} // namespace Mmp