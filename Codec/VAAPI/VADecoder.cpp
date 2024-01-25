#include "VADecoder.h"

#include <string>

#include "Any.h"
#include "VAUtil.h"
#include "VADevice.h"
#include "VATranslator.h"

namespace Mmp
{
namespace Codec
{

VADecoder::VADecoder()
{
    _context = VA_INVALID_ID;
    _config = VA_INVALID_ID;
    _display = nullptr;
    _isInited = false;
    _reference = 0;
    _needUninit = false;
}

bool VADecoder::Init()
{
    assert(_needUninit == false);
    // Hint : nothing to do
    //        LIBVA 的初始化需要知道一些配置参数, 这些参数从码流中获取, 故在解析码流获取这些参数时在进行初始化
    // See also : VADecoder::VaInit
    return false;
}

void VADecoder::Uninit()
{
    // Hint : VADecoder 创建出来的一些资源会直接导出给外部使用, 通过 shared_ptr 的形式,
    //        故 VADecoder 并不持有这些资源文件, 但这些资源文件释放时需要使用到 VADecoder 相关的上下文,
    //        所以 VADecoder 需要延迟释放, 保证其他资源已完成释放, 再 Uninit 自己
    _needUninit = true;
    if (_reference == 0)
    {
        VaInit();
        _needUninit = false;
    }
}

void VADecoder::SetParameter(Any parameter)
{

}

Any VADecoder::GetParamter() 
{
    return Any();
}

bool VADecoder::Push(AbstractPack::ptr pack)
{
    return false;
}

bool VADecoder::Pop(AbstractFrame::ptr& frame)
{
    return false;
}

bool VADecoder::CanPush()
{
    return false;
}

bool VADecoder::CanPop() 
{
    return false;
}

const std::string& VADecoder::Description()
{
    static std::string description = "VADecoder";
    return description;
}

bool VADecoder::VaInit()
{
    if (_isInited)
    {
        return true;
    }
    if (!VADevice::Instance()->Create())
    {
        goto END;
    }
    _display = VADevice::Instance()->GetDisplay();
    if (!CreateVaConfig())
    {
        goto END1;
    }
    if (!CreateContext())
    {
        goto END2;
    }
    _isInited = true;
    return true;
END2:
    DestroyVaConfig();
END1:
    _display = nullptr;
    VADevice::Instance()->Destroy();
END:
    return false;
}

void VADecoder::VaUninit()
{
    if (!_isInited)
    {
        return;
    }
    VAAPI_LOG_INFO << "VADecoder uninit";
    DestroyVaConfig();
    DestroyContext();
    VADevice::Instance()->Destroy();
    _display = nullptr;
    _isInited = false;
}

void VADecoder::AddReference()
{
    _reference++;
}

void VADecoder::DelReference()
{
    _reference--;
    if (_reference == 0 && _needUninit)
    {
        Uninit();
    }
}

bool VADecoder::CreateContext()
{
    if (_context != VA_INVALID_ID)
    {
        return true;
    }
    VAAPI_LOG_INFO << "-- CreateContext";
    if (vaCreateContext(_display, _config, _params.width, _params.height, VA_PROGRESSIVE, nullptr, 0, &_context) != VA_STATUS_SUCCESS)
    {  
        VAAPI_LOG_ERROR << "vaCreateContext fail";
        assert(false);
        goto END;
    }
    return true;
END:
    return false;
}

void VADecoder::DestroyContext()
{
    if (_context == VA_INVALID_ID)
    {
        return;
    }
    VAAPI_LOG_INFO << "-- DestroyContext";
    vaDestroyContext(_display, _context);
    _context = VA_INVALID_ID;
}

bool VADecoder::CreateVaConfig()
{
    if (_config != VA_INVALID_ID)
    {
        return true;
    }
    VAAPI_LOG_INFO << "-- CreateVaConfig";
    VAProfile vaProfile = _params.profile;
    if (VADevice::Instance()->GetSupportProfiles().count(vaProfile) == 0)
    {
        VAAPI_LOG_ERROR << "Unsupport profile, profile is: " << VAProfileToStr(vaProfile);
        goto END;
    }
    if (vaCreateConfig(_display, vaProfile, VAEntrypointVLD, nullptr, 0, &_config) != VA_STATUS_SUCCESS)
    {
        VAAPI_LOG_ERROR << "vaCreateConfig fail";
        assert(false);
        goto END;
    }
    return true;
END:
    return false;
}

void VADecoder::DestroyVaConfig()
{
    if (_config == VA_INVALID_ID)
    {
        return;
    }
    VAAPI_LOG_INFO << "-- DestroyVaConfig";
    vaDestroyConfig(_display, _config);
    _config = VA_INVALID_ID;
}

VaDecoderParams VADecoder::GetDecoderParams()
{
    return _params;
}

void VADecoder::SetDecoderParams(const VaDecoderParams& param)
{
    if (_params != param)
    {
        VaDecoderParams oldParams = _params;
        _params = param;
        VaUninit();
        VaInit();
        OnVaDecoderParamsChange(oldParams, param);
    }
}

VASurfaceID VADecoder::CreateVaSurface(const std::vector<VASurfaceAttrib>& attributes)
{
    uint32_t rtFormat = PixelFormatToVaRTFormat(_params.format);
    VASurfaceID surfaceId = VA_INVALID_ID;
    if (rtFormat == 0)
    {
        VAAPI_LOG_INFO << "Unsupport pix format, pix format is: " << _params.format;
        assert(false);
        goto END;
    }
    if (vaCreateSurfaces(_display, rtFormat, _params.width, _params.height, &surfaceId, 1, const_cast<VASurfaceAttrib*>(attributes.data()), attributes.size()) != VA_STATUS_SUCCESS)
    {
        VAAPI_LOG_ERROR << "vaCreateSurfaces fail";
        assert(false);
        goto END;
    }
    return surfaceId;
END:
    return VA_INVALID_ID;
}

VAImage VADecoder::CreateVaImage(VASurfaceID surfaceId)
{
    VAImage image = {};
    if (vaDeriveImage(_display, surfaceId, &image) != VA_STATUS_SUCCESS)
    {
        VAAPI_LOG_ERROR << "vaDeriveImage fail";
        assert(false);
    }
    return image;
}

void VADecoder::DestroyVaImage(VAImage image)
{
    if (image.image_id != VA_INVALID_ID)
    {
        vaDestroyImage(_display, image.image_id);
    }
}

void VADecoder::DestroyVaSurface(VASurfaceID surfaceId)
{
    if (surfaceId != VA_INVALID_ID)
    {
        vaDestroySurfaces(_display, &surfaceId, 1);
    }
}

VABufferID VADecoder::CreateVaParamBuffer(VABufferType type, void* data, size_t size)
{
    VABufferID buffer = VA_INVALID_ID;
    if (vaCreateBuffer(_display, _context, type, size, 1, (void*)data, &buffer) != VA_STATUS_SUCCESS)
    {
        VAAPI_LOG_ERROR << "vaCreateBuffer fail";
        assert(false);
    }
    return buffer;
}

void VADecoder::DestroyVaParamBuffer(VABufferID buffer)
{
    vaDestroyBuffer(_display, buffer);
}

VABufferID VADecoder::CreateVaSliceParamBuffer(VABufferType type, void* data, size_t size)
{
    VABufferID buffer = VA_INVALID_ID;
    if (vaCreateBuffer(_display, _context, type, size, 1, (void*)data, &buffer) != VA_STATUS_SUCCESS)
    {
        VAAPI_LOG_ERROR << "vaCreateBuffer fail, type is: VASliceParameterBufferType";
        assert(false);
    }
    return buffer;
}

void VADecoder::DestroyVaSliceParamBuffer(VABufferID buffer)
{
    vaDestroyBuffer(_display, buffer);
}

VABufferID VADecoder::CreateVaSliceDataBuffer(void* data, size_t size)
{
    VABufferID buffer = VA_INVALID_ID;
    if (vaCreateBuffer(_display, _context, VASliceDataBufferType, size, 1, (void*)data, &buffer) != VA_STATUS_SUCCESS)
    {
        VAAPI_LOG_ERROR << "vaCreateBuffer fail, type is: VASliceDataBufferType";
        assert(false);
    }
    return buffer;
}

void VADecoder::DestroyVaSliceDataBuffer(VABufferID buffer)
{
    vaDestroyBuffer(_display, buffer);
}

bool VADecoder::CommitVaDecodeCommand(VADecodePictureContext::ptr picContext)
{
    if (vaBeginPicture(_display, _context, picContext->surface) != VA_STATUS_SUCCESS)
    {
        VAAPI_LOG_ERROR << "vaBeginPicture fail";
        assert(false);
        goto END;
    }
    if (vaRenderPicture(_display, _context, const_cast<VABufferID *>(picContext->paramBuffers.data()), picContext->paramBuffers.size()) != VA_STATUS_SUCCESS)
    {
        VAAPI_LOG_ERROR << "vaRenderPicture fail";
        assert(false);
        goto END1;
    }
    if (vaRenderPicture(_display, _context, const_cast<VABufferID *>(picContext->sliceBuffers.data()), picContext->sliceBuffers.size()) != VA_STATUS_SUCCESS)
    {
        VAAPI_LOG_ERROR << "vaRenderPicture fail";
        assert(false);
        goto END1;
    }
    vaEndPicture(_display, _context);
    return true;
END1:
    vaEndPicture(_display, _context);
END:
    return false;
}

void VADecoder::OnVaDecoderParamsChange(const VaDecoderParams& oldValue, const VaDecoderParams& newValue)
{

}

} // namespace Codec
} // namespace Mmp