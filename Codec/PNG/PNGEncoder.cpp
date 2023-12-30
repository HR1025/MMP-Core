#include "PNGEncoder.h"

#include <cassert>
#include <lodepng.h>

#include "Common/AllocateMethodFactory.h"

#include "PNGUtil.h"

namespace Mmp
{
namespace Codec
{

PngEncoder::PngEncoder()
{
    _inEncoding = false;
}

PngEncoder::~PngEncoder()
{

}

void PngEncoder::SetParameter(Any parameter)
{
    // nothing to do in fact
}

Any PngEncoder::GetParamter()
{
    std::lock_guard<std::recursive_mutex> lock(_mutex);
    return _setting;
}

bool PngEncoder::Push(AbstractFrame::ptr frame)
{
    if (_inEncoding) return false;
    if (frame->sideData.type() != typeid(PixelsInfo))
    {
        return false;
    }
    PixelsInfo picInfo = AnyCast<PixelsInfo>(frame->sideData);
    if (!IsAllowEncodePixelFormat(picInfo.format))
    {
        return false;
    }
    std::lock_guard<std::recursive_mutex> lock(_mutex);
    if (_pack)
    {
        const std::string msg = "Push Frame but Pack not pop, discard pack";
        CODEC_LOG_WARN << msg;
    }
    _inEncoding = true;
    {
        unsigned char* compress = nullptr;
        size_t size = 0;
        int error = lodepng_encode_memory(&compress, &size,
            reinterpret_cast<const unsigned char*>(frame->GetData()), picInfo.width, picInfo.height,
            picInfo.format == PixelFormat::RGB888 ? LodePNGColorType::LCT_RGB : LodePNGColorType::LCT_RGBA, 8
        );
        if (compress == nullptr || error != 0)
        {
            CODEC_LOG_WARN << "[lodepng] Unkown reason, encode fail";
            _inEncoding = false;
            return false;
        }
        std::shared_ptr<PngAllocateMethod> allocateMethod = std::dynamic_pointer_cast<PngAllocateMethod>(AllocateMethodFactory::DefaultFactory().CreateAllocateMethod("LodePngAllocateMethod"));
        allocateMethod->data = compress;
        allocateMethod->size = size;
        NormalPack::ptr pack = std::make_shared<NormalPack>(allocateMethod->size, allocateMethod);
        _pack = pack;
    }
    return true;
}

bool PngEncoder::Pop(AbstractPack::ptr& pack)
{
    std::lock_guard<std::recursive_mutex> lock(_mutex);
    if (!_pack)
    {
        return false;
    }
    else
    {
        pack = _pack;
        _pack.reset();
        return true;
    }
}

bool PngEncoder::CanPush()
{
    return _inEncoding ? false : true;
}

bool PngEncoder::CanPop()
{
    std::lock_guard<std::recursive_mutex> lock(_mutex);
    return _pack ? true : false;
}

const std::string& PngEncoder::Description()
{
    static const std::string desc = "png(lodepng) encoder";
    return desc;
}

bool PngEncoder::IsAllowEncodePixelFormat(PixelFormat format)
{
    switch (format)
    {
        case PixelFormat::RGB888:
        case PixelFormat::RGBA8888:
            return true;
        default:
            return false;
    }
}

} // namespace Codec
} // namespace Mmp