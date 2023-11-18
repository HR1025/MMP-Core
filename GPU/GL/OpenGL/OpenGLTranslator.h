//
// OpenGLTranslator.h
//
// Library: GPU
// Package: GL
// Module:  OpenGL
// 

#pragma once

#include "OpenGLCommon.h"

namespace Mmp
{

GLenum GLShaderStageToOpenGLType(SL::ShaderStage stage);

GLenum GLTextureTypeToOpenGLType(TextureType type);

GLenum GLComparisonTypeToOpenGLType(Comparison comparison);

GLenum GLLogicOpToOpenGLType(LogicOp op);

GLuint GLStencilOpToOpenGLType(StencilOp op);

GLuint GLBlendFactorToOpenGLType(BlendFactor bf);

GLuint GLBlendOpToOpenGLType(BlendOp op);

GLuint GLTextureAddressModeToOpenGLType(TextureAddressMode mode);

GLuint GLTextureFilterToOpenGLType(TextureFilter tf);

GLuint GLFacingToOpenGLType(Facing face);

GLuint GLCullModeToOpenGLType(CullMode mode);

GLuint GLPrimitiveToOpenGLType(Primitive primitive);

GLuint GenerateMipFilterOpenGLType(TextureFilter minTf, TextureFilter mipTf);

} // namespace Mmp