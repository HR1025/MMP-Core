#include "VaH264SliceDecodingProcess.h"

namespace Mmp
{
namespace Codec
{

VAH264DecodePictureContext::VAH264DecodePictureContext()
{
}

VAH264DecodePictureContext::~VAH264DecodePictureContext()
{

}

H264PictureContext::ptr VaH264SliceDecodingProcess::CreatePictureContext()
{
    return std::make_shared<VAH264DecodePictureContext>();
}

} // namespace Codec
} // namespace Mmp