//
// OpenGLInitStepData.h
//
// Library: GPU
// Package: GL
// Module:  OpenGL
// 

#pragma once

#include "OpenGLCommon.h"

namespace Mmp
{

struct OpenGLCreateTextureInitData
{
    OpenGLRenderTexture::ptr   texture;
};

struct OpenGLCreateShaderInitData
{
    OpenGLRenderShader::ptr shader;
    std::string             code;
    GLuint                  stage;
};

/** 
 * @note
 *     shaders[0] -> vertex shader
 *     shaders[1] -> fragment shader
 */
struct OpenGLCreateProgramInitData
{
    OpenGLRenderProgram::ptr       program;
    OpenGLRenderShader::ptr        shaders[3];
    uint32_t                       shaderNum;
};

struct OpenGLCreateBufferInitData
{
    OpenGLRenderBuffer::ptr    buffer;
    int                        size;
    GLuint                     usage;
};

struct OpenGLCreateInputLayoutInitData
{
    OpenGLRenderInputLayout::ptr inputLayout;
};

struct OpenGLCreateFramebufferInitData
{
    OpenGLRenderFrameBuffer::ptr    frameBuffer;
};

struct OpenGLBufferSubdataInitData
{
    OpenGLRenderBuffer::ptr  buffer;
    int                      offset;
    int                      size;
    RawData::ptr             data;
};

struct OpenGLTextureImageInitData
{
    OpenGLRenderTexture::ptr   texture;
    DataFormat                 format;
    int                        level;
    uint16_t                   width;
    uint16_t                   height;
    uint16_t                   depth; // 1 mean 2D, otherwise 3D
    bool                       linearFilter;
    GLPixelData::ptr             data;
};

struct OpenGLTextureFinalizeInitData
{
    OpenGLRenderTexture::ptr  texture;
    int                       loadedLevels;
    bool                      genMips;
};

} // namespace Mmp