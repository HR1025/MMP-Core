//
// OpenGLStepData.h
//
// Library: GPU
// Package: GL
// Module:  OpenGL
//

#pragma once

#include "OpenGLCommon.h"

namespace Mmp
{

struct OpenGLRenderData
{
    OpenGLRenderFrameBuffer::ptr   framebuffer;
    RPAction                       color;
    RPAction                       depth;
    RPAction                       stencil;
    int32_t                        drawNums;
};

struct OpenGLCopyData
{
    OpenGLRenderFrameBuffer::ptr    src;
    OpenGLRenderFrameBuffer::ptr    dst;
    GLRect2D                        srcRect;
    OpenGLOffset2D                  dstPos;
    int32_t                         aspectMask;
};

struct OpenGLBlitData
{
    OpenGLRenderFrameBuffer::ptr    src;
    OpenGLRenderFrameBuffer::ptr    dst;
    GLRect2D                        srcRect;
    GLRect2D                        dstRect;
    int32_t                         aspectMask;
    GLboolean                       filter;
};

struct OpenGLReadbackData
{
    int32_t                        aspectMask;
    OpenGLRenderFrameBuffer::ptr   src;
    GLRect2D                       srcRect;
    DataFormat                     dstFormat;
    GLPixelData::ptr               pixel;
};

struct OpenGLReadbackImageData
{
    std::vector<OpenGLRenderTexture::ptr>  textures;
    std::vector<GLRect2D>                  srcRects;
    int32_t                                mipLevel;
    SharedData::ptr                        picture;
};

} // namespace Mmp