#include "AbstractEGLWindow.h"

#include <cassert>

namespace Mmp
{

bool AbstractEGLWindow::SetAPIType(APIType type)
{
    // Hint : nothing to do
    return true;
}

bool AbstractEGLWindow::SetRenderMode(bool onScreen)
{
    if (onScreen)
    {
        SetSurfaceType(GLSurfaceType::WINDOWS);
    }
    else
    {
        // Hint : GLSurfaceType::Texture 会导致不存在 default framebuffer, 造成某些 GL 操作失败
        //        (本质原因是目前 GL 设计是默认存在 default framebuffer 的)
        // See also : EGL Extension - KHR_surfaceless_context - https://registry.khronos.org/EGL/extensions/KHR/EGL_KHR_surfaceless_context.txt
        // 2) Do we need to include all of the relevant "default framebuffer" language
        // from the OpenGL specification to properly specify OpenGL ES behavior
        // with no default framebuffer bound?
        // RESOLVED. Yes, the behaviour of the GLES contexts when no default
        // framebuffer is associated with the context has been moved to the OpenGL
        // ES extension OES_surfaceless_context.
        SetSurfaceType(GLSurfaceType::PBUFFER);
    }
    // todo : 支持设置确认判断
    return true;
}

WindowInfo AbstractEGLWindow::GetInfo()
{
    GLMode mode     = GetGLMode();
    Version version = GetVersion();

    WindowInfo info;
    switch (mode)
    {
        case GLMode::OPENGL: info.apiType = APIType::OPENGL; break;
        case GLMode::OPENGL_ES3: info.apiType = APIType::OPENGL_ES3; break;
        default: info.apiType = APIType::UNKNOWN; break;
    }
    info.vMajor = version.first;
    info.vMinor = version.second;
    return info;
}

} // namespace Mmp