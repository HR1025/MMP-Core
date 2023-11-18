#include "CodecConfig.h"

#include "PNG/PNGUtil.h"

#ifdef USE_VAAPI
#include "VAAPI/VADevice.h"
#endif /* USE_VAAPI */

namespace Mmp
{
namespace Codec
{

CodecConfig::ptr CodecConfig::Instance()
{
    static CodecConfig::ptr gCodecConfig = std::make_shared<CodecConfig>();
    return gCodecConfig;
}

CodecConfig::CodecConfig()
{
    PngAllocateMethod::Register();
}

CodecConfig::~CodecConfig()
{

}

void CodecConfig::Init()
{
#ifdef USE_VAAPI
        VADevice::Instance()->RegisterNoticeCenter();
#endif /* USE_VAAPI */
}

void CodecConfig::Uninit()
{
#ifdef USE_VAAPI
        VADevice::Instance()->UnRegisterNoticeCenter();
#endif /* USE_VAAPI */
}

} // namespace Codec
} // namespace Mmp