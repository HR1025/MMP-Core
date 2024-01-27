#include "VADecoderContext.h"

#include "VAUtil.h"
#include "VADevice.h"
#include "VATranslator.h"

namespace Mmp
{
namespace Codec
{

VADecoderContext::VADecoderContext(const VaDecoderParams& params)
{
    _params = params;
    _isInited = false;
    _context = VA_INVALID_ID;
    _config = VA_INVALID_ID;
    _display = nullptr;
}

VADecoderContext::~VADecoderContext()
{
    Uninit();
}

bool VADecoderContext::Init()
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

void VADecoderContext::Uninit()
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

VASurfaceID VADecoderContext::CreateVaSurface(const std::vector<VASurfaceAttrib>& attributes)
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

VAImage VADecoderContext::CreateVaImage(VASurfaceID surfaceId)
{
    VAImage image = {};
    if (vaDeriveImage(_display, surfaceId, &image) != VA_STATUS_SUCCESS)
    {
        VAAPI_LOG_ERROR << "vaDeriveImage fail";
        assert(false);
    }
    return image;
}

void VADecoderContext::DestroyVaImage(VAImage image)
{
    if (image.image_id != VA_INVALID_ID)
    {
        vaDestroyImage(_display, image.image_id);
    }
}

void VADecoderContext::DestroyVaSurface(VASurfaceID surfaceId)
{
    if (surfaceId != VA_INVALID_ID)
    {
        vaDestroySurfaces(_display, &surfaceId, 1);
    }
}

VABufferID VADecoderContext::CreateVaParamBuffer(VABufferType type, void* data, size_t size)
{
    VABufferID buffer = VA_INVALID_ID;
    if (vaCreateBuffer(_display, _context, type, size, 1, (void*)data, &buffer) != VA_STATUS_SUCCESS)
    {
        VAAPI_LOG_ERROR << "vaCreateBuffer fail";
        assert(false);
    }
    return buffer;
}

void VADecoderContext::DestroyVaParamBuffer(VABufferID buffer)
{
    vaDestroyBuffer(_display, buffer);
}

VABufferID VADecoderContext::CreateVaSliceParamBuffer(VABufferType type, void* data, size_t size)
{
    VABufferID buffer = VA_INVALID_ID;
    if (vaCreateBuffer(_display, _context, type, size, 1, (void*)data, &buffer) != VA_STATUS_SUCCESS)
    {
        VAAPI_LOG_ERROR << "vaCreateBuffer fail, type is: VASliceParameterBufferType";
        assert(false);
    }
    return buffer;
}

void VADecoderContext::DestroyVaSliceParamBuffer(VABufferID buffer)
{
    vaDestroyBuffer(_display, buffer);
}

VABufferID VADecoderContext::CreateVaSliceDataBuffer(void* data, size_t size)
{
    VABufferID buffer = VA_INVALID_ID;
    if (vaCreateBuffer(_display, _context, VASliceDataBufferType, size, 1, (void*)data, &buffer) != VA_STATUS_SUCCESS)
    {
        VAAPI_LOG_ERROR << "vaCreateBuffer fail, type is: VASliceDataBufferType";
        assert(false);
    }
    return buffer;
}

void VADecoderContext::DestroyVaSliceDataBuffer(VABufferID buffer)
{
    vaDestroyBuffer(_display, buffer);
}

bool VADecoderContext::CommitVaDecodeCommand(VADecodePictureContext::ptr picContext)
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

bool VADecoderContext::CreateContext()
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

void VADecoderContext::DestroyContext()
{
    if (_context == VA_INVALID_ID)
    {
        return;
    }
    VAAPI_LOG_INFO << "-- DestroyContext";
    vaDestroyContext(_display, _context);
    _context = VA_INVALID_ID;
}

bool VADecoderContext::CreateVaConfig()
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

void VADecoderContext::DestroyVaConfig()
{
    if (_config == VA_INVALID_ID)
    {
        return;
    }
    VAAPI_LOG_INFO << "-- DestroyVaConfig";
    vaDestroyConfig(_display, _config);
    _config = VA_INVALID_ID;
}

} // namespace Codec
} // namespace Mmp
