#include "PNGDecorder.h"

#include <cassert>
#include <lodepng.h>

#include "Common/AllocateMethodFactory.h"

#include "PNGUtil.h"

namespace Mmp
{
namespace Codec
{

PngDecoder::PngDecoder()
{
    _inDecoding = false;
}

PngDecoder::~PngDecoder()
{
    
}

void PngDecoder::SetParameter(Any parameter) 
{
    std::lock_guard<std::recursive_mutex> lock(_mutex);
    if (parameter.type() == typeid(PngDecoderParameter))
    {
        _setting = AnyCast<PngDecoderParameter>(parameter);
    }
    else
    {
        CODEC_LOG_WARN << "Expect PngDecoderParameter type";
        assert(false);
        return;
    }
}

Any PngDecoder::GetParamter()
{
    std::lock_guard<std::recursive_mutex> lock(_mutex);
    return _setting;
}

bool PngDecoder::Push(AbstractPack::ptr pack)
{
    if (_inDecoding) return false;
    std::lock_guard<std::recursive_mutex> lock(_mutex);
    if (!IsAllowDecodePixelFormat(_setting))
    {
        return false;
    }
    if (_picture)
    {
        const std::string msg = "Push Pack but Frame not pop, discard frame";
        CODEC_LOG_WARN << msg;
    }
    _inDecoding = true;
    {
        unsigned char* pixel = nullptr;
        uint32_t       width = 0;
        uint32_t       height = 0;
        int error = lodepng_decode_memory(&pixel, &width, &height, 
                            reinterpret_cast<const unsigned char*>(pack->GetData()), pack->GetSize(),
                            _setting.format == PixelFormat::RGB888 ? LodePNGColorType::LCT_RGB : LodePNGColorType::LCT_RGBA, 8);
        if (pixel == nullptr || error != 0)
        {
            CODEC_LOG_WARN << "[lodepng] Unkown reason, decode fail";
            _inDecoding = false;
            return false;
        }
        std::shared_ptr<PngAllocateMethod> allocateMethod = std::dynamic_pointer_cast<PngAllocateMethod>(AllocateMethodFactory::DefaultFactory().CreateAllocateMethod("LodePngAllocateMethod"));
        allocateMethod->data = pixel;
        allocateMethod->size = width * height * (_setting.format == PixelFormat::RGB888 ? 3 : 4);
        NormalPicture::ptr picture = std::make_shared<NormalPicture>(PixelsInfo({(int32_t)width, (int32_t)height, 8, _setting.format}), allocateMethod);
        _picture = picture;
    }
    _inDecoding = false;
    return true;
}

bool PngDecoder::Pop(AbstractFrame::ptr& frame)
{
    std::lock_guard<std::recursive_mutex> lock(_mutex);
    frame = _picture;
    frame->sideData = _picture->info;
    _picture.reset();
    return frame ? true : false;
}

bool PngDecoder::CanPush()
{
    return _inDecoding ? false : true;
}

bool PngDecoder::CanPop()
{
    std::lock_guard<std::recursive_mutex> lock(_mutex);
    return _picture ? true : false;
}

const std::string& PngDecoder::Description()
{
    static const std::string desc = "png(lodepng) decoder";
    return desc;
}

bool PngDecoder::IsAllowDecodePixelFormat(const PngDecoderParameter& setting)
{
    switch (setting.format)
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
