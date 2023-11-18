//
// OpenGLRenderData.h
//
// Library: GPU
// Package: GL
// Module:  OpenGL
//

#pragma once

#include "OpenGLCommon.h"

namespace Mmp
{

struct OpenGLBlendRenderData
{
    GLboolean enabled;
    GLenum    srcColor;
    GLenum    dstColor;
    GLenum    srcAlpha;
    GLenum    dstAlpha;
    GLenum    funcColor;
    GLenum    funcAlpha;
    int mask;
};

struct OpenGLBlendColorRenderData
{
    float color[4];
};


struct OpenGLDepthRenderData
{
    GLboolean enbaled;
    GLboolean write;
    GLenum    func;
};

struct OpenGLogicRenderData
{
    GLboolean enabled;
    GLenum    logicOp;
};

struct OpenGLStencilFuncRenderData
{
    GLboolean enabled;
    GLenum    func;
    uint8_t   ref;
    uint8_t   compareMask;
};

struct OpenGLStencilOpRenderData
{
    GLenum    sFail;
    GLenum    zFail;
    GLenum    pass;
    uint8_t   writeMask;
};

struct OpenGLDrawRenderData
{
    GLenum   mode;
    GLint    buffer;
    GLint    frist;
    GLint    count;
};

struct OpenGLDrawIndexedRenderData
{
    GLenum   mode;
    GLint    count;
    GLint    instances;
    GLint    indexType;
    void*    indices;
};

struct OpenGLUniform4RenderData
{
    const std::string  name;
    const GLint*       loc;
    GLint              count;
    float              v[4];
};

struct OpenGLUniformMatrix4RenderData
{
    const std::string  name;
    const GLint*        loc;
    float              m[32];
};

struct OpenGLClearRenderData
{
    uint32_t    clearColor;
    float       clearZ;
    uint8_t     clearStencil;
    uint8_t     colorMask;
    GLuint      clearMask;
    int16_t     scissorX;
    int16_t     scissorY;
    int16_t     scissorW;
    int16_t     scissorH;
};

struct OpenGLTextureRenderData
{
    int                       solt;
    OpenGLRenderTexture::ptr  texture;
};

struct OpenGLTextureSubImageRenderData
{
    OpenGLRenderTexture::ptr     texture;
    DataFormat                   format;
    uint8_t                      slot;
    uint8_t                      level;
    uint16_t                     width;
    uint16_t                     height;
    uint16_t                     x;
    uint16_t                     y;
    RawData::ptr                 data;
};

struct OpenGLBindFbTextureRenderData
{
    int                             slot;
    OpenGLRenderFrameBuffer::ptr    framebuffer;
    int                             aspect;
};

struct OpenGLBindBufferRenderData
{
    OpenGLRenderBuffer::ptr       buffer;
    GLuint                        target;
};

struct OpenGLProgramRenderData
{
    OpenGLRenderProgram::ptr program;
};

struct OpenGLBindVertexBufferRenderData
{
    OpenGLRenderInputLayout::ptr     inputLayout;
    OpenGLRenderBuffer::ptr          buffer;
    size_t                           offset;
};

struct OpenGLTextureSamplerRenderData
{
    int                    slot;
    GLenum                 wrapS;
    GLenum                 wrapT;
    GLenum                 magFilter;
    GLenum                 minFilter;
    float                  anisotropy;
};

struct OpenGLTextureLodRenderData
{
    int                    slot;
    float                  minLod;
    float                  maxLod;
    float                  lodBias;
};

struct OpenGLViewportRenderData
{
    OpenGLViewport         vp;
};

struct OpenGLScissorRenderData
{
    GLRect2D           rc;
};

struct OpenGLRasterRenderData
{
    GLboolean              cullEnable;
    GLenum                 frontFace;
    GLenum                 cullFace;
    GLboolean              ditherEnable;
    GLboolean              depthClampEnable;
};

} // namespace Mmp