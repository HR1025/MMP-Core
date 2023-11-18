#include "EGLWindowDefault.h"

#include <cassert>

namespace Mmp
{

EGLDisplay EGLWindowDefault::OnOpenDisplay()
{   
    // eglspec.1.5.pdf - 3.2 Initialization
    return eglGetDisplay(EGL_DEFAULT_DISPLAY);
}

EGLConfig  EGLWindowDefault::OnChooseEGLConfig()
{
    // Hint : 大多情况下选择第一个即可,默认情况下 EGLConfig 配置遵循一定规则的排序
    // eglspec.1.5.pdf - 3.4.1.2 Sorting of EGLConfigs 
    auto eglConfigs = GetAllEGLConfigs();

    return !eglConfigs.empty() ? eglConfigs[0] : nullptr;
}

EGLSurface EGLWindowDefault::OnCreateSurface()
{
    EGLSurface surface = EGL_NO_SURFACE;
    switch (GetSurfaceType())
    {
        case GLSurfaceType::WINDOWS:
            surface = eglCreateWindowSurface(_eglDisplay, _eglConfig, (EGLNativeWindowType)nullptr, nullptr);
            if (surface == EGL_NO_SURFACE)
            {
                // Hint : Mesa Vendor 下报错 EGL_BAD_NATIVE_WINDOW, 是因为 Mesa 是纯软件实现的 ?
                // TODO : 可能需要引入 X11 或者 wayland 库去提供 EGLNativeWindowType , 但不是在这里
                //        进行, may be EGLWindowX11 or EGLWindowWayland
                EGLint error = eglGetError();
                WIN_LOG_ERROR << "Create window surface fail, error is: " << EGLErrorToStr(error);
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
            // Hint : render to texture 实际上不需要 EGL 提供 default framebuffer
            //        eglMakeCurrent 有可能不接受 EGL_NO_SURFACE 的入参
            //        目前测试的机器都没有出现这种情况,所以直接返回即可
            // See also : eglspec.1.5 - 3.7.3 Binding Contexts and Drawables
            //			If ctx does not support being bound without read and draw surfaces, and
            //		both draw and read are EGL_NO_SURFACE, an EGL_BAD_MATCH error is gen-
            //		erated.
            // TODO:
            //    是否要尝试创建 PBUFFER (消耗一定的资源),来进一步确保 OnCreateSurface 的泛用性
            break;
        default:
            assert(false);
            WIN_LOG_ERROR << "Unknown surface type, maybe need to update code";
            break;
    }
    return surface;
}

} // namespace Mmp