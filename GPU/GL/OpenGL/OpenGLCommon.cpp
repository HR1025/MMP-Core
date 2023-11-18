#include "OpenGLCommon.h"

#include <cassert>

namespace Mmp
{

const std::string OpenGLInitStepTypeToStr(OpenGLInitStepType step)
{
    switch (step)
    {
        case OpenGLInitStepType::CREATE_TEXTURE: return "CREATE_TEXTURE";
        case OpenGLInitStepType::CREATE_SHADER: return "CREATE_SHADER";
        case OpenGLInitStepType::CREATE_PROGRAM: return "CREATE_PROGRAM";
        case OpenGLInitStepType::CREATE_BUFFER: return "CREATE_BUFFER";
        case OpenGLInitStepType::CREATE_INPUT_LAYOUT: return "CREATE_INPUT_LAYOUT";
        case OpenGLInitStepType::CREATE_FRAMEBUFFER: return "CREATE_FRAMEBUFFER";
        case OpenGLInitStepType::TEXTURE_FINALIZE: return "TEXTURE_FINALIZE";
        case OpenGLInitStepType::BUFFER_SUBDATA: return "BUFFER_SUBDATA";
        default: assert(false); return "UNKNOWN";
    }
}

std::ostream& operator<<(std::ostream& os, OpenGLInitStepType step)
{
    os << OpenGLInitStepTypeToStr(step);
    return os;
}

const std::string OpenGLRenderCommondToStr(OpenGLRenderCommond commond)
{
    switch (commond)
    {
        case OpenGLRenderCommond::DEPTH: return "DEPTH";
        case OpenGLRenderCommond::STENCILFUNC: return "STENCILFUNC";
        case OpenGLRenderCommond::STENCILOP: return "STENCILOP";
        case OpenGLRenderCommond::BLEND: return "BLEND";
        case OpenGLRenderCommond::BLENDCOLOR: return "BLENDCOLOR";
        case OpenGLRenderCommond::LOGICOP: return "LOGICOP";
        case OpenGLRenderCommond::UNIFORM4I: return "UNIFORM4I";
        case OpenGLRenderCommond::UNIFORM4UI: return "UNIFORM4UI";
        case OpenGLRenderCommond::UNIFORM4F: return "UNIFORM4F";
        case OpenGLRenderCommond::UNIFORMMATRIX: return "UNIFORMMATRIX";
        case OpenGLRenderCommond::UNIFORMSTEREOMATRIX: return "UNIFORMSTEREOMATRIX";
        case OpenGLRenderCommond::TEXTURESAMPLER: return "TEXTURESAMPLER";
        case OpenGLRenderCommond::TEXTURELOD: return "TEXTURELOD";
        case OpenGLRenderCommond::VIEWPORT: return "VIEWPORT";
        case OpenGLRenderCommond::SCISSOR: return "SCISSOR";
        case OpenGLRenderCommond::RASTER: return "RASTER";
        case OpenGLRenderCommond::CLEAR: return "CLEAR";
        case OpenGLRenderCommond::INVALIDATE: return "INVALIDATE";
        case OpenGLRenderCommond::BINDPROGRAM: return "BINDPROGRAM";
        case OpenGLRenderCommond::BINDTEXTURE: return "BINDTEXTURE";
        case OpenGLRenderCommond::BIND_FB_TEXTURE: return "BIND_FB_TEXTURE";
        case OpenGLRenderCommond::BIND_VERTEX_BUFFER: return "BIND_VERTEX_BUFFER";
        case OpenGLRenderCommond::BIND_BUFFER: return "BIND_BUFFER";
        case OpenGLRenderCommond::GENMIPS: return "GENMIPS";
        case OpenGLRenderCommond::DRAW: return "DRAW";
        case OpenGLRenderCommond::DRAW_INDEXED: return "DRAW_INDEXED";
        case OpenGLRenderCommond::TEXTURE_SUBIMAGE: return "TEXTURE_SUBIMAGE";
        default: assert(false); return "UNKNOWN";
    }
}

std::ostream& operator<<(std::ostream& os, OpenGLRenderCommond commond)
{
    os << OpenGLRenderCommondToStr(commond);
    return os;
}

const std::string OpenGLRenderStepTypeToStr(OpenGLRenderStepType type)
{
    switch (type)
    {
        case OpenGLRenderStepType::RENDER: return "RENDER";
        case OpenGLRenderStepType::COPY: return "COPY";
        case OpenGLRenderStepType::BLIT: return "BLIT";
        case OpenGLRenderStepType::READBACK: return "READBACK";
        case OpenGLRenderStepType::READBACK_IMAGE: return "READBACK_IMAGE";
        case OpenGLRenderStepType::RENDER_SKIP: return "RENDER_SKIP";
        default: assert(false); return "UNKNOWN";
    }
}

extern std::ostream& operator<<(std::ostream& os, OpenGLRenderStepType type)
{
    os << OpenGLRenderStepTypeToStr(type);
    return os;
}

const std::string OpenGLRenderRunTypeToStr(OpenGLRenderRunType type)
{
    switch (type)
    {
        case OpenGLRenderRunType::NORMAL:  return "NORMAL";
        case OpenGLRenderRunType::PRESENT: return "PRESENT";
        case OpenGLRenderRunType::SYNC:    return "SYNC";
        case OpenGLRenderRunType::EXIT:    return "EXIT";
        default: assert(false); return "UNKNOWN";
    }
}

extern std::ostream& operator<<(std::ostream& os, OpenGLRenderRunType type)
{
    os << OpenGLRenderRunTypeToStr(type);
    return os;
}

int operator& (const OpenGLBufferStrategy& lhs, const OpenGLBufferStrategy& rhs)
{
    return (int)lhs & (int)rhs;
}

} // namespace Mmp