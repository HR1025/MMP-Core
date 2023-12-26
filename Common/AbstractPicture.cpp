#include "AbstractPicture.h"

namespace Mmp
{

AbstractPicture::AbstractPicture(PixelsInfo info)
{
    this->info = info;
    this->sideData = info;
}

} // namespace Mmp