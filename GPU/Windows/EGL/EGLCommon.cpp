#include "EGLCommon.h"

#include <cassert>

namespace Mmp
{

// TODO : Linux 可能可以尝试使用 DRM 创建 pBuffer
// EGL Extension Brief
// - EGL_EXT_image_dma_buf_import - https://registry.khronos.org/EGL/extensions/EXT/EGL_EXT_image_dma_buf_import.txt
//      This extension allows creating an EGLImage from a Linux dma_buf file
// descriptor or multiple file descriptors in the case of multi-plane YUV
// images.
// - EXT_image_dma_buf_import_modifiers - https://registry.khronos.org/EGL/extensions/EXT/EGL_EXT_image_dma_buf_import_modifiers.txt
//      This extension builds on EGL_EXT_image_dma_buf_import, in order to support
// format modifiers used for tiling, compression, and additional non-linear
// modes. It also adds support for a fourth auxiliary plane, and queries for
// the implementation-supported types.
// - KHR_no_config_context
//      Modern GPUs allow contexts to render to almost any combination of
// supported color and auxiliary buffer formats. Traditionally EGL context
// creation is done with respect to an EGLConfig specifying buffer formats,
// and constrains contexts to only work with surfaces created with a
// "compatible" config.

const std::string GLModeToStr(GLMode mode)
{
    switch (mode)
    {
        case GLMode::OPENGL:     return "OPENGL";
        case GLMode::OPENGL_ES3: return "OPENGL_ES3";
        case GLMode::OPENGL_ES2: return "OPENGL_ES2";
        case GLMode::OPENVG:     return "OPENVG";
        case GLMode::UNKNOWN:    return "UNKNOWN";
        default: assert(false); return "UNKNOWN";
    }
}

std::ostream& operator<<(std::ostream& os, GLMode mode)
{
    os <<  GLModeToStr(mode);
    return os;
}

const std::string GLSurfaceTypeToStr(GLSurfaceType type)
{
    switch (type)
    {
        case GLSurfaceType::WINDOWS: return "WINDOWS";
        case GLSurfaceType::PBUFFER: return "PBUFFER";
        case GLSurfaceType::PIXMAP: return "PIXMAP";
        case GLSurfaceType::TEXTURE: return "TEXTURE";
        default: assert(false); return "UNKNOWN";
    };
}

extern std::ostream& operator<<(std::ostream& os, GLSurfaceType type)
{
    os << GLSurfaceTypeToStr(type);
    return os;
}

std::string EGLErrorToStr(EGLint error)
{
    switch (error)
    {
        case EGL_SUCCESS: return "EGL_SUCCESS";
        case EGL_NOT_INITIALIZED: return "EGL_NOT_INITIALIZED";
        case EGL_BAD_ACCESS: return "EGL_BAD_ACCESS";
        case EGL_BAD_ALLOC: return "EGL_BAD_ALLOC";
        case EGL_BAD_ATTRIBUTE: return "EGL_BAD_ATTRIBUTE";
        case EGL_BAD_CONTEXT: return "EGL_BAD_CONTEXT";
        case EGL_BAD_CONFIG: return "EGL_BAD_CONFIG";
        case EGL_BAD_CURRENT_SURFACE: return "EGL_BAD_CURRENT_SURFACE";
        case EGL_BAD_DISPLAY: return "EGL_BAD_DISPLAY";
        case EGL_BAD_SURFACE: return "EGL_BAD_SURFACE";
        case EGL_BAD_MATCH: return "EGL_BAD_MATCH";
        case EGL_BAD_PARAMETER: return "EGL_BAD_PARAMETER";
        case EGL_BAD_NATIVE_PIXMAP: return "EGL_BAD_NATIVE_PIXMAP";
        case EGL_BAD_NATIVE_WINDOW: return "EGL_BAD_NATIVE_WINDOW";
        case EGL_CONTEXT_LOST: return "EGL_CONTEXT_LOST";
        default:  return "EGL_UNKONWN";
    }
}

std::string EGLAttributeToStr(EGLint attr)
{
    switch (attr)
    {
        case EGL_BUFFER_SIZE: return "EGL_BUFFER_SIZE";
        case EGL_RED_SIZE: return "EGL_RED_SIZE";
        case EGL_GREEN_SIZE: return "EGL_GREEN_SIZE";
        case EGL_BLUE_SIZE: return "EGL_BLUE_SIZE";
        case EGL_LUMINANCE_SIZE: return "EGL_LUMINANCE_SIZE";
        case EGL_ALPHA_SIZE: return "EGL_ALPHA_SIZE";
        case EGL_ALPHA_MASK_SIZE: return "EGL_ALPHA_MASK_SIZE";
        case EGL_BIND_TO_TEXTURE_RGB: return "EGL_BIND_TO_TEXTURE_RGB";
        case EGL_BIND_TO_TEXTURE_RGBA: return "EGL_BIND_TO_TEXTURE_RGBA";
        case EGL_COLOR_BUFFER_TYPE: return "EGL_COLOR_BUFFER_TYPE";
        case EGL_CONFIG_CAVEAT: return "EGL_CONFIG_CAVEAT";
        case EGL_CONFIG_ID: return "EGL_CONFIG_ID";
        case EGL_CONFORMANT: return "EGL_CONFORMANT";
        case EGL_DEPTH_SIZE: return "EGL_DEPTH_SIZE";
        case EGL_LEVEL: return "EGL_LEVEL";
        case EGL_MAX_PBUFFER_WIDTH: return "EGL_MAX_PBUFFER_WIDTH";
        case EGL_MAX_PBUFFER_HEIGHT: return "EGL_MAX_PBUFFER_HEIGHT";
        case EGL_MAX_PBUFFER_PIXELS: return "EGL_MAX_PBUFFER_PIXELS";
        case EGL_MAX_SWAP_INTERVAL: return "EGL_MAX_SWAP_INTERVAL";
        case EGL_MIN_SWAP_INTERVAL: return "EGL_MIN_SWAP_INTERVAL";
        case EGL_NATIVE_RENDERABLE: return "EGL_NATIVE_RENDERABLE";
        case EGL_NATIVE_VISUAL_ID: return "EGL_NATIVE_VISUAL_ID";
        case EGL_NATIVE_VISUAL_TYPE: return "EGL_NATIVE_VISUAL_TYPE";
        case EGL_RENDERABLE_TYPE: return "EGL_RENDERABLE_TYPE";
        case EGL_SAMPLE_BUFFERS: return "EGL_SAMPLE_BUFFERS";
        case EGL_SAMPLES: return "EGL_SAMPLES";
        case EGL_STENCIL_SIZE: return "EGL_STENCIL_SIZE";
        case EGL_SURFACE_TYPE: return "EGL_SURFACE_TYPE";
        case EGL_TRANSPARENT_TYPE: return "EGL_TRANSPARENT_TYPE";
        case EGL_TRANSPARENT_RED_VALUE: return "EGL_TRANSPARENT_RED_VALUE";
        case EGL_TRANSPARENT_GREEN_VALUE: return "EGL_TRANSPARENT_GREEN_VALUE";
        case EGL_TRANSPARENT_BLUE_VALUE: return "EGL_TRANSPARENT_BLUE_VALUE";
        default: assert(false); return "EGL_UNKONWN";
    }
}

std::string EGLRenderableType(EGLint type)
{
    switch (type)
    {
        case EGL_OPENGL_BIT: return "OpenGL";
        case EGL_OPENGL_ES_BIT: return "OpenGL ES 1.x";
        case EGL_OPENGL_ES2_BIT: return "OpenGL ES 2.x";
        case EGL_OPENGL_ES3_BIT: return "OpenGL ES 3.x";
        case EGL_OPENVG_BIT: return "OpenVG 1.x";
        default: assert(false); return "UNKNOWN";
    }
}

} // namespace Mmp