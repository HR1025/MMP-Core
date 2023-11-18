//
// OpenGLContex.h
//
// Library: GPU
// Package: GL
// Module:  OpenGL
// 

#pragma once

#include "OpenGLCommon.h"
#include "OpenGL.h"
#include "OpenGLRenderContex.h"

#include <vector>
#include <memory>
#include <functional>


namespace Mmp
{

class OpenGLShaderModule : public ShaderModule
{
public:
	using ptr = std::shared_ptr<OpenGLShaderModule>;
public:
	SL::ShaderStage GetStage() override;
public:
	OpenGLShaderModule(SL::ShaderStage stage, const std::string& tag);
	~OpenGLShaderModule();
	OpenGLRenderShader::ptr GetShader();
	const std::string GetSource();
	SL::ShaderLanguage GetShaderLanguage();
	bool Complie(OpenGL::ptr openGL, SL::ShaderLanguage language, const std::string& source);
private:
	OpenGL::ptr                _openGL;
	SL::ShaderStage            _stage;
	SL::ShaderLanguage         _lang;
	OpenGLRenderShader::ptr    _shader;
	GLenum                     _glStage;
	std::string                _source;
	std::string                _tag;
};

class OpenGLBlendState : public BlendState
{
public:
	using ptr = std::shared_ptr<OpenGLBlendState>;
public:
	bool    enabled;
	GLuint  eqCol, eqAlpha;
	GLuint  srcCol, srcAlpha, dstCol, dstAlpha;
	int colorMask;
public:
	void Apply(OpenGL::ptr openGL);
};

class OpenGLSamplerState : public SamplerState 
{
public:
	using ptr = std::shared_ptr<OpenGLSamplerState>;
public:
	GLint wrapU;
	GLint wrapV;
	GLint wrapW;
	GLint magFilt;
	GLint minFilt;
	GLint mipMinFilt;
};

class OpenGLRasterState : public RasterState
{
public:
	using ptr = std::shared_ptr<OpenGLRasterState>;
public:
	GLboolean cullEnable;
	GLenum cullMode;
	GLenum frontFace;
public:
	void Apply(OpenGL::ptr openGL);
};

class OpenGLInputLayout : public InputLayout
{
public:
	using ptr = std::shared_ptr<OpenGLInputLayout>;
public:
	void Complie(OpenGL::ptr openGL, const InputLayoutDesc& desc);
public:
	OpenGLRenderInputLayout::ptr inputLayout;
	int stride;
};

class PipeLineLocData : public OpenGLRenderProgramLocData
{
public:
	using ptr = std::shared_ptr<PipeLineLocData>;
public:
	GLint samplerLocs[MAX_TEXTURE_SLOTS] = {};
	std::vector<GLint> dynamicUniformLocs;
};

class OpenGLDepthStencilState : public DepthStencilState 
{
public:
	using ptr = std::shared_ptr<OpenGLDepthStencilState>;
public:
	bool depthTestEnabled;
	bool depthWriteEnabled;
	GLuint depthComp;
	// TODO: Two-sided. Although in practice, do we care?
	bool stencilEnabled;
	GLuint stencilFail;
	GLuint stencilZFail;
	GLuint stencilPass;
	GLuint stencilCompareOp;
public:
	void Apply(OpenGL::ptr openGL, uint8_t stencilRef, uint8_t stencilWriteMask, uint8_t stencilCompareMask);
};

class OpenGLPipeline : public Pipeline
{
public:
	using ptr = std::shared_ptr<OpenGLPipeline>;
public:
	GLuint prim = 0;
	std::vector<OpenGLShaderModule::ptr> shaders;
	OpenGLInputLayout::ptr               inputLayout;
	OpenGLDepthStencilState::ptr         depthStencil;
	OpenGLBlendState::ptr                blend;
	OpenGLRasterState::ptr               raster;
public:
	PipeLineLocData::ptr                 locs;
	UniformBufferDesc                    dynamicUniforms;
	OpenGLRenderProgram::ptr             program;
	std::vector<SamplerDef>              samplers;
public:
	~OpenGLPipeline();
	bool LinkShaders(OpenGL::ptr openGL, const PipelineDesc& desc);
};

class OpenGLBuffer : public GLBuffer
{
public:
	using ptr = std::shared_ptr<OpenGLBuffer>;
public:
	OpenGLBuffer(OpenGL::ptr openGL, size_t size, uint32_t flags);
	~OpenGLBuffer();
public:
	OpenGLRenderBuffer::ptr  buffer;
	GLuint                   target;
	GLuint				     usage;
	size_t                   totalSize;
};

class OpenGLTexture : public Texture
{
public:
	using ptr = std::shared_ptr<OpenGLTexture>;
public:
	OpenGLTexture(const OpenGL::ptr& openGL, const TextureDesc& desc);
	~OpenGLTexture();
public:
	bool HasMips();
	TextureType GetType();
	int NumMipmaps();
	OpenGLRenderTexture::ptr GetTex();
	void Bind(int stage);
	void SetImageData(int x, int y, int z, int width, int height, int depth, int level, int stride, GLPixelData::ptr data);
private:
	OpenGL::ptr               _openGL;
	bool                      _generatedMips;
	TextureType               _type;
	int                       _mipLevels;
	DataFormat                _format;
	OpenGLRenderTexture::ptr  _tex;
};

class OpenGLFrameBuffer : public FrameBuffer
{
public:
	using ptr = std::shared_ptr<OpenGLFrameBuffer>;
public:
	explicit OpenGLFrameBuffer(OpenGLRenderFrameBuffer::ptr frameBuffer);
	~OpenGLFrameBuffer();
public:
	OpenGLRenderFrameBuffer::ptr frameBuffer;
};

} // namespace Mmp