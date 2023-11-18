#include "EGLWindowGBM.h"

#include <cstddef>
#include <fcntl.h>
#include <dlfcn.h>
#include <unistd.h>
#include <memory.h>
#include <cassert>

// IDE WORKAROUND : may be add /usr/include/drm for C++ intelliSense (drm.h may in /usr/incldue/drm but not be included in system path)
#include <gbm.h>
#include <drm/drm.h>
#include <drm/drm_fourcc.h>
#include <xf86drmMode.h>
#include <xf86drm.h>
#include <EGL/eglext.h>

#include <Poco/Path.h>
#include <Poco/String.h>
#include <Poco/SortedDirectoryIterator.h>

#include "Common/EventTag.h"
#include "Common/NoticeCenter.h"

namespace Mmp
{

std::vector<std::string> GetRenderNodes()
{
    static std::string kFindPath = "/dev/dri/";
    std::vector<std::string> renderNodes;
    Poco::SortedDirectoryIterator dirIteratorCurrent(kFindPath);
	Poco::SortedDirectoryIterator dirIteratorEnd;

	while (dirIteratorCurrent != dirIteratorEnd)
	{
        if (Poco::startsWith(Poco::Path(dirIteratorCurrent->path()).getFileName(), std::string("renderD")))
        {
            renderNodes.push_back(dirIteratorCurrent->path());
        }
        ++dirIteratorCurrent;
	}
    dirIteratorCurrent = Poco::SortedDirectoryIterator(kFindPath);
	while (dirIteratorCurrent != dirIteratorEnd)
	{
        if (Poco::startsWith(Poco::Path(dirIteratorCurrent->path()).getFileName(), std::string("card")))
        {
            renderNodes.push_back(dirIteratorCurrent->path());
        }
        ++dirIteratorCurrent;
	}
    return renderNodes;
}

static bool IsDrmMaster(int drmFd)
{
    drm_magic_t magic;
    return drmGetMagic(drmFd, &magic) == 0 && drmAuthMagic(drmFd, magic) == 0;
}

static std::string GetConnectTypeStr(uint64_t type)
{
    switch (type)
    {
        case DRM_MODE_CONNECTOR_Unknown: return "DRM_MODE_CONNECTOR_Unknown";
        case DRM_MODE_CONNECTOR_VGA: return "DRM_MODE_CONNECTOR_VGA";
        case DRM_MODE_CONNECTOR_DVII: return "DRM_MODE_CONNECTOR_DVII";
        case DRM_MODE_CONNECTOR_DVID: return "DRM_MODE_CONNECTOR_DVID";
        case DRM_MODE_CONNECTOR_DVIA: return "DRM_MODE_CONNECTOR_DVIA";
        case DRM_MODE_CONNECTOR_Composite: return "DRM_MODE_CONNECTOR_Composite";
        case DRM_MODE_CONNECTOR_SVIDEO: return "DRM_MODE_CONNECTOR_SVIDEO";
        case DRM_MODE_CONNECTOR_LVDS: return "DRM_MODE_CONNECTOR_LVDS";
        case DRM_MODE_CONNECTOR_Component: return "DRM_MODE_CONNECTOR_Component";
        case DRM_MODE_CONNECTOR_9PinDIN: return "DRM_MODE_CONNECTOR_9PinDIN";
        case DRM_MODE_CONNECTOR_DisplayPort: return "DRM_MODE_CONNECTOR_DisplayPort";
        case DRM_MODE_CONNECTOR_HDMIA: return "DRM_MODE_CONNECTOR_HDMIA";
        case DRM_MODE_CONNECTOR_HDMIB: return "DRM_MODE_CONNECTOR_HDMIB";
        case DRM_MODE_CONNECTOR_TV: return "DRM_MODE_CONNECTOR_TV";
        case DRM_MODE_CONNECTOR_eDP: return "DRM_MODE_CONNECTOR_eDP";
        case DRM_MODE_CONNECTOR_VIRTUAL: return "DRM_MODE_CONNECTOR_VIRTUAL";
        case DRM_MODE_CONNECTOR_DSI: return "DRM_MODE_CONNECTOR_DSI";
        case DRM_MODE_CONNECTOR_DPI: return "DRM_MODE_CONNECTOR_DPI";
        default:
            return "DRM_MODE_CONNECTOR_Unknown";
    }
}

static std::string ConnecterStatusToStr(drmModeConnection status)
{
    switch (status)
    {
        case DRM_MODE_CONNECTED: return "DRM_MODE_CONNECTED";
        case DRM_MODE_DISCONNECTED: return "DRM_MODE_DISCONNECTED";
        default: return "DRM_MODE_UNKNOWNCONNECTION";
    }
}

} // namespace Mmp

namespace Mmp
{

constexpr uint32_t kDisplayWidth  = 1920;
constexpr uint32_t kDisplayHeight = 1080;
constexpr uint32_t kDisplayRate   = 60;

EGLWindowGBM::EGLWindowGBM()
{
    _drmOpen = false;
    _fd      = -1;
    _device  = nullptr;
    _surface = nullptr;
}

EGLWindowGBM::~EGLWindowGBM()
{

}

void EGLWindowGBM::Swap()
{
    if (_surfaceType == GLSurfaceType::WINDOWS)
    {
#if 0
        static PFNEGLSWAPBUFFERSWITHDAMAGEEXTPROC _eglSwapBuffersWithDamageKHR = (PFNEGLSWAPBUFFERSWITHDAMAGEEXTPROC)eglGetProcAddress("eglSwapBuffersWithDamageKHR");
        if (_eglSwapBuffersWithDamageKHR)
        {
            EGLint rect[4] = {0, 0, kDisplayWidth, kDisplayHeight};
            _eglSwapBuffersWithDamageKHR(_eglDisplay, _eglSurface, rect, 1);
        }
        else
        {
            eglSwapBuffers(_eglDisplay, _eglSurface);
        }
#else
        eglSwapBuffers(_eglDisplay, _eglSurface);
        {
            drmModeRes* resource = drmModeGetResources(_fd);
            uint32_t crtcId = 0;
            uint32_t connectId = 0;
            uint32_t encoderId = 0;
            drmModeConnector* connect = nullptr;
            drmModeModeInfo* mode = nullptr;
            struct gbm_bo* bo = gbm_surface_lock_front_buffer(_surface);
            assert(bo);
            uint32_t fbId = 0;
            uint32_t handles[4] = {};
            uint32_t strides[4] = {};            
            uint32_t offsets[4] = {};
            handles[0] = gbm_bo_get_handle(bo).u32;
            strides[0] = gbm_bo_get_stride(bo);            
            drmModeAddFB2(_fd, gbm_bo_get_width(bo), gbm_bo_get_height(bo), DRM_FORMAT_XRGB8888, handles, strides, offsets, &fbId, 0);
            // Bad Solution : cache resource message and use udev to detect display connection 
            //                really only need `drmModeSetCrtc` in every cycle
            for (int i=0; i<resource->count_connectors; i++) // Hint : find all connect CRTC
            {
                connect = nullptr;
                mode = nullptr;
                connect = drmModeGetConnector(_fd, resource->connectors[i]);
                if (!connect) continue;
                if (connect->connection == DRM_MODE_CONNECTED)
                {
                    connectId = resource->connectors[i];
                    encoderId = connect->encoder_id;
                }
                else
                {
                    continue;
                }
                for (int j=0; j<resource->count_encoders; j++)
                {
                    drmModeEncoderPtr encoder = drmModeGetEncoder(_fd, resource->encoders[j]);
                    if (!encoder) continue;
                    if (encoder->encoder_id == encoderId)
                    {
                        crtcId = encoder->crtc_id;
                    }
                    drmModeFreeEncoder(encoder);
                }
                for (int j=0; j<connect->count_modes; j++)
                {
                    mode = &connect->modes[j];
                    // TODO : negotiate with monitor
                    if ((mode->hdisplay == kDisplayWidth && mode->vdisplay == kDisplayHeight)
                    || (mode->hdisplay == kDisplayHeight && mode->vdisplay == kDisplayWidth))
                    {
                        break;
                    }
                    else
                    {
                        mode = nullptr;
                    }
                }
                drmModeCrtcPtr crtc = drmModeGetCrtc(_fd, crtcId);
                if (!crtc) continue;
                drmModeSetCrtc(_fd, crtcId, fbId, 0, 0, &connectId, 1, mode);
            }
        }        
#endif
    }
}

std::vector<EGLConfig> EGLWindowGBM::GetAllEGLConfigs()
{
    EGLint numConfigs;
    EGLBoolean result;
    std::vector<EGLConfig>  eglConfigs;

    WIN_LOG_INFO << "Try Get EGL Configs";

    eglConfigs.resize(1);
    if (_surfaceType == GLSurfaceType::WINDOWS)
    {
        EGLint attrs[] = {
            EGL_RENDERABLE_TYPE, EGL_OPENGL_ES3_BIT,
            EGL_RED_SIZE, 8,
            EGL_BLUE_SIZE, 8,
            EGL_GREEN_SIZE, 8,
            EGL_DEPTH_SIZE, 0,
            EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
            EGL_NONE
        };
        result = eglChooseConfig(_eglDisplay, attrs, &eglConfigs[0], 1, &numConfigs);
    }
    else if (_surfaceType == GLSurfaceType::PBUFFER)
    {
        EGLint attrs[] = {
            EGL_RENDERABLE_TYPE, EGL_OPENGL_ES3_BIT,
            EGL_RED_SIZE, 8,
            EGL_BLUE_SIZE, 8,
            EGL_GREEN_SIZE, 8,
            EGL_DEPTH_SIZE, 0,
            EGL_SURFACE_TYPE, EGL_PBUFFER_BIT,
            EGL_NONE
        };
        result = eglChooseConfig(_eglDisplay, attrs, &eglConfigs[0], 1, &numConfigs);
    }
    else
    {
        assert(false);
    }
    
    if (result != EGL_TRUE || numConfigs == 0)
    {
        WIN_LOG_ERROR << "eglGetConfigs fail, result is:" << EGLErrorToStr(result) << ", egl configs num is: " << numConfigs;
        assert(false);
        eglConfigs.clear();
        return eglConfigs;
    }
    else
    {
        return eglConfigs;
    }
}

bool EGLWindowGBM::Open()
{
    return EGLWindow::Open();
}

bool EGLWindowGBM::Close()
{
    EGLWindow::Close();
    NoticeCenter::Instance()->EmitEvent(Event::kCloseDrmDevice, _renderNode, _fd);
    _drmOpen = false;
    if (_surface)
    {
        gbm_surface_destroy(_surface);
        _surface = nullptr;
    }
    if (_device)
    {
        gbm_device_destroy(_device);
        _device = nullptr;
    }
    if (_fd)
    {
        close(_fd);
        _fd = -1;
    }
    return true;
}

EGLDisplay EGLWindowGBM::OnOpenDisplay()
{   
    if (!_drmOpen)
    {
        std::vector<std::string> renderNodes = GetRenderNodes();
        if (renderNodes.empty())
        {
            WIN_LOG_ERROR << "Can not find available render node";
            assert(false);
            goto END;
        }
        for (auto renderNode : renderNodes)
        {
            WIN_LOG_INFO << "Try to open render node, render node is: " << renderNode;
            _fd = open(renderNode.c_str(), O_RDWR | O_CLOEXEC);
            if (_fd < 0)
            {
                WIN_LOG_ERROR << "Open " << renderNode << " fail";
            }
            else
            {
                _renderNode = renderNode;
                NoticeCenter::Instance()->EmitEvent(Event::kOpenDrmDevice, _renderNode, _fd);
                break;
            }
        }
        if (_fd < 0)
        {
            assert(false);
            goto END;
        }
        drmSetClientCap(_fd, DRM_CLIENT_CAP_UNIVERSAL_PLANES, 1);
        drmSetClientCap(_fd, DRM_CLIENT_CAP_WRITEBACK_CONNECTORS, 1);
        drmSetClientCap(_fd, DRM_CLIENT_CAP_ATOMIC, 1);
        drmSetClientCap(_fd, DRM_CLIENT_CAP_UNIVERSAL_PLANES, 1);
        _device = gbm_create_device(_fd);
        if (!_device)
        {
            goto END1;
        }
        WIN_LOG_INFO << "Drm Master : " << (IsDrmMaster(_fd) ? "true" : "false");
        _surface = gbm_surface_create(_device, kDisplayWidth, kDisplayHeight, GBM_FORMAT_XRGB8888, GBM_BO_USE_SCANOUT | GBM_BO_USE_RENDERING);
        if (!_surface)
        {
            goto END2;
        }
        _drmOpen = true;
    }
#if 1 /* for debug */
    if (_surfaceType ==  GLSurfaceType::WINDOWS)
    {

        drmModeRes* resource = drmModeGetResources(_fd);
        if (!resource)
        {
            goto END3;
        }
        drmModeConnector* connect = nullptr;
        for (int i=0; i<resource->count_connectors; i++)
        {
            connect = drmModeGetConnector(_fd, resource->connectors[i]);
            if (!connect) continue;
            WIN_LOG_INFO << "Connecter Info:";
            WIN_LOG_INFO << "-- Encoder id is: " << connect->encoder_id; 
            WIN_LOG_INFO << "-- Status is: " << ConnecterStatusToStr(connect->connection);
            WIN_LOG_INFO << "-- Connect Type is: " << GetConnectTypeStr(connect->connector_type);
            WIN_LOG_INFO << "-- Size is: " << connect->mmWidth << "x" << connect->mmHeight;
            if (connect->count_modes) WIN_LOG_INFO << "-- Mode Info:";
            for (int j=0; j<connect->count_modes; j++)
            {
                drmModeModeInfoPtr mode = &connect->modes[j];
                WIN_LOG_INFO << "  Mode(" << j << ")";
                WIN_LOG_INFO << "  -- Clock is: " << mode->clock;
                WIN_LOG_INFO << "  -- HDisplay is: " << mode->hdisplay;
                WIN_LOG_INFO << "  -- VDisplay is: " << mode->vdisplay;
                WIN_LOG_INFO << "  -- HRefresh is: " << mode->vrefresh;
            }
        }
    }
#endif
    {
        EGLDisplay display = EGL_NO_DISPLAY;
        static PFNEGLGETPLATFORMDISPLAYEXTPROC _eglGetPlatformDisplayEXT = NULL;
        if (!_eglGetPlatformDisplayEXT) 
        {
            _eglGetPlatformDisplayEXT = (PFNEGLGETPLATFORMDISPLAYEXTPROC)eglGetProcAddress("eglGetPlatformDisplayEXT");
        }
        if (_eglGetPlatformDisplayEXT)
        {
            display = _eglGetPlatformDisplayEXT(EGL_PLATFORM_GBM_KHR, _device, NULL);
            if (display)
            {
                goto SUCCESS;
            }
        }
        {
            display = eglGetDisplay((EGLNativeDisplayType)_device);
            if (display)
            {
                goto SUCCESS;
            }
        }
        if (CheckExtension("EGL_MESA_platform_surfaceless") && _eglGetPlatformDisplayEXT)
        {
            display = _eglGetPlatformDisplayEXT(EGL_PLATFORM_SURFACELESS_MESA, (void *)EGL_DEFAULT_DISPLAY, nullptr);
            if (display)
            {
                goto SUCCESS;
            }
        }
        display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
        SUCCESS:
            return display;
    }
END3:
    gbm_surface_destroy(_surface);
    _surface = nullptr;
END2:
    gbm_device_destroy(_device);
    _device = nullptr;
END1:
    close(_fd);
    _fd = -1;
END:
    _drmOpen = false;
    return EGL_NO_DISPLAY;
}

EGLConfig  EGLWindowGBM::OnChooseEGLConfig()
{
    if (!CheckExtension("EGL_EXT_platform_base"))
    {
        return nullptr;
    }
    else
    {
        // Hint : 大多情况下选择第一个即可,默认情况下 EGLConfig 配置遵循一定规则的排序
        // eglspec.1.5.pdf - 3.4.1.2 Sorting of EGLConfigs 
        auto eglConfigs = GetAllEGLConfigs();
        return !eglConfigs.empty() ? eglConfigs[0] : nullptr;
    }
}

EGLSurface EGLWindowGBM::OnCreateSurface()
{
    static PFNEGLCREATEPLATFORMWINDOWSURFACEEXTPROC _eglCreatePlatformWindowSurfaceEXT = nullptr;
    if (_eglCreatePlatformWindowSurfaceEXT == nullptr)
    {
        _eglCreatePlatformWindowSurfaceEXT = (PFNEGLCREATEPLATFORMWINDOWSURFACEEXTPROC)eglGetProcAddress("eglCreatePlatformWindowSurfaceEXT");
    }

    if (!CheckExtension("EGL_EXT_platform_base"))
    {
        if (CheckExtension("EGL_KHR_surfaceless_context") || CheckExtension("EGL_MESA_platform_surfaceless"))
        {
            WIN_LOG_INFO << "No suuport EGL_EXT_platform_base EGL extension, try to run egl context in surfaceless mode";
        }
        else
        {
            // Hint : 某些 OpenGL (ES) 驱动即使没有 surfaceless 拓展, 仍然支持 surfaceless (驱动厂家的 bug ???)
            WIN_LOG_WARN << "No suuport EGL_EXT_platform_base EGL extension, try to run egl context in surfaceless mode";
            assert(false);
        }
        return EGL_NO_SURFACE;
    }

    EGLSurface surface = EGL_NO_SURFACE;
    switch (GetSurfaceType())
    {
        case GLSurfaceType::WINDOWS:
            if (_eglCreatePlatformWindowSurfaceEXT)
            {
                surface = _eglCreatePlatformWindowSurfaceEXT(_eglDisplay, _eglConfig, (void *)_surface, NULL);
            }
            else
            {
                surface = eglCreateWindowSurface(_eglDisplay, _eglConfig, (EGLNativeWindowType)_surface, nullptr);
            }
            if (surface == EGL_NO_SURFACE)
            {
                EGLint error = eglGetError();
                WIN_LOG_ERROR << "Create window surface fail, error is: " << EGLErrorToStr(error);
                assert(false);
            }
            break;
        case GLSurfaceType::PBUFFER:
            surface = eglCreatePbufferSurface(_eglDisplay, _eglConfig, nullptr);
            if (surface == EGL_NO_SURFACE)
            {
                EGLint error = eglGetError();
                WIN_LOG_ERROR << "Create pbuffer surface fail, error is: " << EGLErrorToStr(error);
            }
            break;
        case GLSurfaceType::PIXMAP:
            assert(false);
            WIN_LOG_ERROR << "Not support surface type, surface type is: " << GLSurfaceType::PIXMAP;
            break;
        case GLSurfaceType::TEXTURE:
            assert(false);
            WIN_LOG_ERROR << "Not support surface type, surface type is: " << GLSurfaceType::TEXTURE;
            break;
        default:
            assert(false);
            WIN_LOG_ERROR << "Unknown surface type, maybe need to update code";
            break;
    }
    return surface;
}


} // namespace Mmp