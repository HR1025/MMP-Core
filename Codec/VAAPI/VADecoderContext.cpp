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
    if (!VADevice::Instance()->GetImageFormat(_params.format, _imageFormat))
    {
        VAAPI_LOG_ERROR << "VADevice::Instance GetImageFormat fail";
        goto END3;
    }
    _isInited = true;
    return true;
END3:
    DestroyVaConfig();
END2:
    DestroyContext();
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
    VAStatus vaStatus = VA_STATUS_SUCCESS;
    uint32_t rtFormat = PixelFormatToVaRTFormat(_params.format);
    VASurfaceID surfaceId = VA_INVALID_ID;
    if (rtFormat == 0)
    {
        VAAPI_LOG_INFO << "Unsupport pix format, pix format is: " << _params.format;
        assert(false);
        goto END;
    }
    if (VA_OP_FAIL(vaCreateSurfaces(_display, rtFormat, _params.width, _params.height, &surfaceId, 1, const_cast<VASurfaceAttrib*>(attributes.data()), attributes.size())))
    {
        VAAPI_LOG_ERROR << "vaCreateSurfaces fail, status is: " << VAStatusToStr(vaStatus);
        assert(false);
        goto END;
    }
    return surfaceId;
END:
    return VA_INVALID_ID;
}

void VADecoderContext::DestroyVaSurface(VASurfaceID surfaceId)
{
    VAStatus vaStatus = VA_STATUS_SUCCESS;
    if (surfaceId != VA_INVALID_ID)
    {
        if (VA_OP_FAIL(vaDestroySurfaces(_display, &surfaceId, 1)))
        {
            VAAPI_LOG_ERROR << "vaDestroySurfaces fail, status is: " << VAStatusToStr(vaStatus);
            assert(false);
        }
    }
}

VABufferID VADecoderContext::CreateVaParamBuffer(VABufferType type, void* data, size_t size)
{
    VAStatus vaStatus = VA_STATUS_SUCCESS;
    VABufferID buffer = VA_INVALID_ID;
    if (VA_OP_FAIL(vaCreateBuffer(_display, _context, type, size, 1, (void*)data, &buffer)))
    {
        VAAPI_LOG_ERROR << "vaCreateBuffer fail, status is: " << VAStatusToStr(vaStatus);
        assert(false);
    }
    return buffer;
}

void VADecoderContext::DestroyVaParamBuffer(VABufferID buffer)
{
    VAStatus vaStatus = VA_STATUS_SUCCESS;
    if (VA_OP_FAIL(vaDestroyBuffer(_display, buffer)))
    {
        VAAPI_LOG_ERROR << "vaDestroyBuffer fail, status is: " << VAStatusToStr(vaStatus);
        assert(false);
    }
}

VABufferID VADecoderContext::CreateVaSliceParamBuffer(void* data, size_t size)
{
    VAStatus vaStatus = VA_STATUS_SUCCESS;
    VABufferID buffer = VA_INVALID_ID;
    if (VA_OP_FAIL(vaCreateBuffer(_display, _context, VASliceParameterBufferType, size, 1, (void*)data, &buffer)))
    {
        VAAPI_LOG_ERROR << "vaCreateBuffer fail, type is: VASliceParameterBufferType, status is: " << VAStatusToStr(vaStatus);
        assert(false);
    }
    return buffer;
}

void VADecoderContext::DestroyVaSliceParamBuffer(VABufferID buffer)
{
    VAStatus vaStatus = VA_STATUS_SUCCESS;
    if (VA_OP_FAIL(vaDestroyBuffer(_display, buffer)))
    {
        VAAPI_LOG_ERROR << "vaDestroyBuffer fail, type is: VASliceParameterBufferType, status is: " << VAStatusToStr(vaStatus);
        assert(false);
    }
}

VABufferID VADecoderContext::CreateVaSliceDataBuffer(void* data, size_t size)
{
    VAStatus vaStatus = VA_STATUS_SUCCESS;
    VABufferID buffer = VA_INVALID_ID;
    if (VA_OP_FAIL(vaCreateBuffer(_display, _context, VASliceDataBufferType, size, 1, (void*)data, &buffer)))
    {
        VAAPI_LOG_ERROR << "vaCreateBuffer fail, type is: VASliceDataBufferType, status is: " << VAStatusToStr(vaStatus);
        assert(false);
    }
    return buffer;
}

void VADecoderContext::DestroyVaSliceDataBuffer(VABufferID buffer)
{
    VAStatus vaStatus = VA_STATUS_SUCCESS;
    if (VA_OP_FAIL(vaDestroyBuffer(_display, buffer)))
    {
        VAAPI_LOG_ERROR << "vaDestroyBuffer fail, status is: " << VAStatusToStr(vaStatus);
        assert(false);
    }
}

bool VADecoderContext::CommitVaDecodeCommand(VADecodePictureContext::ptr picContext)
{
    VAStatus vaStatus = VA_STATUS_SUCCESS;
    if (VA_OP_FAIL(vaBeginPicture(_display, _context, picContext->surface)))
    {
        VAAPI_LOG_ERROR << "vaBeginPicture fail, status is: " << VAStatusToStr(vaStatus);
        assert(false);
        goto END;
    }
    if (VA_OP_FAIL(vaRenderPicture(_display, _context, const_cast<VABufferID *>(picContext->paramBuffers.data()), picContext->paramBuffers.size())))
    {
        VAAPI_LOG_ERROR << "vaRenderPicture fail, status is: " << VAStatusToStr(vaStatus);
        assert(false);
        goto END1;
    }
    if (VA_OP_FAIL(vaRenderPicture(_display, _context, const_cast<VABufferID *>(picContext->sliceBuffers.data()), picContext->sliceBuffers.size())))
    {
        VAAPI_LOG_ERROR << "vaRenderPicture fail, status is: " << VAStatusToStr(vaStatus);
        assert(false);
        goto END1;
    }
    if (VA_OP_FAIL(vaEndPicture(_display, _context)))
    {
        VAAPI_LOG_ERROR << "vaEndPicture fail, status is: " << VAStatusToStr(vaStatus);
        assert(false);
    }
    VAAPI_LOG_TRACE << "CommitVaDecodeCommand , surface id is: " << picContext->surface;
    return true;
END1:
    if (VA_OP_FAIL(vaEndPicture(_display, _context)))
    {
        VAAPI_LOG_ERROR << "vaEndPicture fail, status is: " << VAStatusToStr(vaStatus);
        assert(false);
    }
END:
    return false;
}

bool VADecoderContext::SyncVaSurface(VASurfaceID surfaceId)
{
    VAStatus vaStatus = VA_STATUS_SUCCESS;
    if (VA_INVALID_ID != surfaceId)
    {
        if (VA_OP_FAIL(vaSyncSurface(_display, surfaceId)))
        {
            VAAPI_LOG_WARN << "vaSyncSurface fail, status is: " << VAStatusToStr(vaStatus);
            assert(false);
            return false;
        }
        else
        {
            return true;
        }
    }
    else
    {
        assert(false);
        return false;
    }
}

bool VADecoderContext::MapVaSurface(VASurfaceID surfaceId, VAImage& image, void*& address)
{
    VAStatus vaStatus = VA_STATUS_SUCCESS;
    VAAPI_LOG_TRACE << "MapVaSurface, surface id is: " << surfaceId << " " << _params.width << "x" << _params.height;
    if (VA_OP_FAIL(vaCreateImage(_display,  &_imageFormat, _params.width, _params.height, &image)))
    {
        VAAPI_LOG_WARN << "vaCreateImage fail, status is: " << VAStatusToStr(vaStatus);
        assert(false);
        goto END;
    }
    if (VA_OP_FAIL(vaGetImage(_display, surfaceId, 0, 0, _params.width, _params.height, image.image_id)))
    {
        VAAPI_LOG_WARN << "vaGetImage fail, status is: " << VAStatusToStr(vaStatus);
        assert(false);
        goto END1;
    }
    if (VA_OP_FAIL(vaMapBuffer(_display, image.buf, &address)))
    {
        VAAPI_LOG_WARN << "vaMapBuffer fail, status is: " << VAStatusToStr(vaStatus);
        assert(false);
        goto END1;
    }
    return true;
END1:
    if (VA_OP_FAIL(vaDestroyImage(_display, image.image_id)))
    {
        VAAPI_LOG_WARN << "vaDestroyImage fail, status is: " << VAStatusToStr(vaStatus);
        assert(false);
    }
END:
    image.image_id = VA_INVALID_ID;
    address = nullptr;
    return false;
}

void VADecoderContext::UnMapVaSurface(const VAImage& image)
{
    VAStatus vaStatus = VA_STATUS_SUCCESS;
    if (VA_OP_FAIL(vaUnmapBuffer(_display, image.buf)))
    {
        VAAPI_LOG_WARN << "vaUnmapBuffer fail, status is: " << VAStatusToStr(vaStatus);
        assert(false);
    }
    if (VA_OP_FAIL(vaDestroyImage(_display, image.image_id)))
    {
        VAAPI_LOG_WARN << "vaDestroyImage fail, status is: " << VAStatusToStr(vaStatus);
        assert(false);
    }
}

bool VADecoderContext::CreateContext()
{
    VAStatus vaStatus = VA_STATUS_SUCCESS;
    if (_context != VA_INVALID_ID)
    {
        return true;
    }
    VAAPI_LOG_INFO << "-- CreateContext";
    if (VA_OP_FAIL(vaCreateContext(_display, _config, _params.width, _params.height, VA_PROGRESSIVE, nullptr, 0, &_context)))
    {  
        VAAPI_LOG_ERROR << "vaCreateContext fail, status is: " << VAStatusToStr(vaStatus);
        assert(false);
        goto END;
    }
    return true;
END:
    return false;
}

void VADecoderContext::DestroyContext()
{
    VAStatus vaStatus = VA_STATUS_SUCCESS;
    if (_context == VA_INVALID_ID)
    {
        return;
    }
    VAAPI_LOG_INFO << "-- DestroyContext";
    if (VA_OP_FAIL(vaDestroyContext(_display, _context)))
    {
        VAAPI_LOG_ERROR << "vaDestroyContext fail, status is: " << VAStatusToStr(vaStatus);
        assert(false);
    }
    _context = VA_INVALID_ID;
}

bool VADecoderContext::CreateVaConfig()
{
    VAStatus vaStatus = VA_STATUS_SUCCESS;
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
    if (VA_OP_FAIL(vaCreateConfig(_display, vaProfile, VAEntrypointVLD, nullptr, 0, &_config)))
    {
        VAAPI_LOG_ERROR << "vaCreateConfig fail, status is: " << VAStatusToStr(vaStatus);
        assert(false);
        goto END;
    }
    return true;
END:
    return false;
}

void VADecoderContext::DestroyVaConfig()
{
    VAStatus vaStatus = VA_STATUS_SUCCESS;
    if (_config == VA_INVALID_ID)
    {
        return;
    }
    VAAPI_LOG_INFO << "-- DestroyVaConfig";
    if (VA_OP_FAIL(vaDestroyConfig(_display, _config)))
    {
        VAAPI_LOG_ERROR << "vaDestroyConfig fail, status is: " << VAStatusToStr(vaStatus);
        assert(false);
    }
    _config = VA_INVALID_ID;
}

} // namespace Codec
} // namespace Mmp
