#include "VaH264SliceDecodingProcess.h"

namespace Mmp
{
namespace Codec
{

VaH264DecodePictureContext::VaH264DecodePictureContext()
{
    surface = VA_INVALID_ID;
}

VaH264DecodePictureContext::~VaH264DecodePictureContext()
{
    // TODO : 完善析构
}

H264PictureContext::ptr VaH264SliceDecodingProcess::CreatePictureContext()
{
    return std::make_shared<VaH264DecodePictureContext>();
}

} // namespace Codec
} // namespace Mmp