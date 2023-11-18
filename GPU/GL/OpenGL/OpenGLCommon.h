//
// OpenGLCommon.h
//
// Library: GPU
// Package: GL
// Module:  OpenGL
//

#pragma once

#include <cstdint>
#include <string>
#include <iostream>
#include <vector>

#include <GL/glew.h>

#include "Common/Any.h"
#include "GLCommon.h"
#include "GLContexDesc.h"
#include "GLContex.h"
#include "OpenGLFeature.h"

namespace Mmp
{

class OpenGLRenderBuffer;
class OpenGLRenderShader;
class OpenGLRenderProgram;
class OpenGLRenderTexture;
class OpenGLRenderInputLayout;
class OpenGLRenderFrameBuffer;

#define MAX_TEXTURE_SLOTS 3

enum class OpenGLInitStepType
{
    CREATE_TEXTURE,
    CREATE_SHADER,
    CREATE_PROGRAM,
    CREATE_BUFFER,
    CREATE_INPUT_LAYOUT,
    CREATE_FRAMEBUFFER,

    TEXTURE_IMAGE,
    TEXTURE_FINALIZE,
    BUFFER_SUBDATA,
};
const std::string OpenGLInitStepTypeToStr(OpenGLInitStepType step);
extern std::ostream& operator<<(std::ostream& os, OpenGLInitStepType step);

enum class OpenGLRenderCommond
{
    DEPTH,
    STENCILFUNC,
    STENCILOP,
    BLEND,
    BLENDCOLOR,
    LOGICOP,
    UNIFORM4I,
    UNIFORM4UI,
    UNIFORM4F,
    UNIFORMMATRIX,
    UNIFORMSTEREOMATRIX,
    TEXTURESAMPLER,
    TEXTURELOD,
    VIEWPORT,
    SCISSOR,
    RASTER,
    CLEAR,
    INVALIDATE,
    BINDPROGRAM,
    BINDTEXTURE,
    BIND_FB_TEXTURE,
    BIND_VERTEX_BUFFER,
    BIND_BUFFER, // not used
    GENMIPS,
    DRAW,
    DRAW_INDEXED,
    TEXTURE_SUBIMAGE,
};
const std::string OpenGLRenderCommondToStr(OpenGLRenderCommond commond);
extern std::ostream& operator<<(std::ostream& os, OpenGLRenderCommond commond);

enum class OpenGLRenderStepType
{
    RENDER,
    COPY,
    BLIT,
    READBACK,
    READBACK_IMAGE,
    RENDER_SKIP,
};
const std::string OpenGLRenderStepTypeToStr(OpenGLRenderStepType type);
extern std::ostream& operator<<(std::ostream& os, OpenGLRenderStepType type);

enum class OpenGLRenderRunType
{
    NORMAL,
    PRESENT,
    SYNC,
    EXIT,
};
const std::string OpenGLRenderRunTypeToStr(OpenGLRenderRunType type);
extern std::ostream& operator<<(std::ostream& os, OpenGLRenderRunType type);

enum class OpenGLBufferStrategy
{
    SUBDATA = 0,

    MASK_FLUSH = 0x10,
    MASK_INVALIDATE = 0x20,

    // Map/unmap the buffer each frame.
    FRAME_UNMAP = 1,
    // Map/unmap and also invalidate the buffer on map.
    INVALIDATE_UNMAP = MASK_INVALIDATE,
    // Map/unmap and explicitly flushed changed ranges.
    FLUSH_UNMAP = MASK_FLUSH,
    // Map/unmap, invalidate on map, and explicit flush.
    FLUSH_INVALIDATE_UNMAP = MASK_FLUSH | MASK_INVALIDATE,
};
int operator& (const OpenGLBufferStrategy& lhs, const OpenGLBufferStrategy& rhs);

enum OpenGLAspectType
{
    COLOR,
    DEPTH,
    STENCIL
};

struct OpenGLViewport
{
    float     x;
    float     y;
    float     w;
    float     h;
    float     minZ;
    float     maxZ;
};


struct OpenGLOffset2D
{
    int32_t    x;
    int32_t    y;
};

struct OpenGLInitStep
{
    OpenGLInitStepType type;
    Any                data;
};

struct OpenGLRender
{
    OpenGLRenderCommond cmd;
    Any                 data;
};

struct OpenGLRenderStep
{
    OpenGLRenderStepType         stepType;
    std::vector<OpenGLRender>    commands;
    std::string                  tag;
    Any                          data;
};

struct OpenGLThreadTask
{
    std::vector<OpenGLRenderStep> steps;
    std::vector<OpenGLInitStep>   initSteps;
    int                           frame = 0;
    OpenGLRenderRunType           runType;
};

} // namespace Mmp