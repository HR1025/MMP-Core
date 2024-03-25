#include "CodecFactory.h"

#include <Poco/SingletonHolder.h>

#include "PNG/PNGEncoder.h"
#include "PNG/PNGDecorder.h"

#if defined(USE_VAAPI)
    #include "VAAPI/VAH264Decoder.h"
#endif /* USE_VAAPI */

#if defined(USE_OPENH264)
    #include "openh264/OpenH264Decoder.h"
#endif /* USE_OPENH264 */

#if defined(USE_ROCKCHIP)
    #include "Rockchip/RKH264Decoder.h"
#endif /* USE_ROCKCHIP */

namespace Mmp
{
namespace Codec
{

namespace
{
    static Poco::SingletonHolder<EncoderFactory> esh;
}

EncoderFactory::EncoderFactory()
{
    RegisterBuiltins();
}

EncoderFactory::~EncoderFactory()
{

}

void EncoderFactory::RegisterEncoderClass(const std::string& className, EncoderInstantiator* instantiator)
{
    std::lock_guard<std::mutex> lock(_mutex);
    _encoderFactory.registerClass(className, instantiator);
}

AbstractEncoder::ptr EncoderFactory::CreateEncoder(const std::string& className)
{
    std::lock_guard<std::mutex> lock(_mutex);
    return _encoderFactory.CreateInstance(className);
}

void EncoderFactory::RegisterBuiltins()
{
    std::lock_guard<std::mutex> lock(_mutex);
    _encoderFactory.registerClass("PngEncoder", new Instantiator<PngEncoder, AbstractEncoder>);
}

EncoderFactory& EncoderFactory::DefaultFactory()
{
    return *esh.get();
}

} // namespace Codec
} // namespace Mmp

namespace Mmp
{
namespace Codec
{

namespace
{
    static Poco::SingletonHolder<DecoderFactory> dsh;
}

DecoderFactory::DecoderFactory()
{
    RegisterBuiltins();
}

DecoderFactory::~DecoderFactory()
{
    
}

void DecoderFactory::RegisterDecoderClass(const std::string& className, DecoderInstantiator* instantiator)
{
    std::lock_guard<std::mutex> lock(_mutex);
    _decoderFactory.registerClass(className, instantiator);
}

AbstractDecoder::ptr DecoderFactory::CreateDecoder(const std::string& className)
{
    std::lock_guard<std::mutex> lock(_mutex);
    return _decoderFactory.CreateInstance(className);
}

void DecoderFactory::RegisterBuiltins()
{
    std::lock_guard<std::mutex> lock(_mutex);
    _decoderFactory.registerClass("PngDecoder", new Instantiator<PngDecoder, AbstractDecoder>);
#if defined(USE_VAAPI)
    _decoderFactory.registerClass("VAH264Decoder", new Instantiator<VAH264Decoder, AbstractDecoder>);
#endif /* USE_VAAPI */
#if defined(USE_OPENH264)
    _decoderFactory.registerClass("OpenH264Decoder", new Instantiator<OpenH264Decoder, AbstractDecoder>);
#endif /* USE_OPENH264 */
#if defined(USE_ROCKCHIP)
    _decoderFactory.registerClass("RKH264Decoder", new Instantiator<RKH264Decoder, AbstractDecoder>);
#endif /* USE_ROCKCHIP */
}

DecoderFactory& DecoderFactory::DefaultFactory()
{
    return *dsh.get();
}

} // namespace Codec
} // namespace Mmp