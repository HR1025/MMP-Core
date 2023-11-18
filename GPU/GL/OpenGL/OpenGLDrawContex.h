//
// OpenGLDrawContex.h
//
// Library: GPU
// Package: GL
// Module:  OpenGL
// 

#pragma once

#include "GLDrawContex.h"
#include "OpenGL.h"
#include "OpenGLContex.h"

namespace Mmp
{

class OpenGLDrawContex : public GLDrawContex
{
public:
    OpenGLDrawContex();
    ~OpenGLDrawContex();
public:
    void ThreadStart() override;
    GpuTaskStatus ThreadFrame() override;
    void ThreadEnd() override;
    void ThreadStop() override;
    void FenceBegin() override;
    void FenceCommit() override;
    void FenceEnd() override;
public:
    DepthStencilState::ptr CreateDepthStencilState(const DepthStencilStateDesc& desc) override;
    BlendState::ptr CreateBlendState(const BlendStateDesc& desc) override;
    SamplerState::ptr CreateSamplerState(const SamplerStateDesc& desc) override;
    RasterState::ptr CreateRasterState(const RasterStateDesc& desc) override;
    InputLayout::ptr CreateInputLayout(const InputLayoutDesc& desc) override;
    ShaderModule::ptr CreateShaderModule(SL::ShaderStage stage, SL::ShaderLanguage lang, const std::string& code) override;
    Pipeline::ptr CreateGraphicsPipeline(const PipelineDesc& desc, const std::string& tag) override;
public:
    GLBuffer::ptr CreateBuffer(size_t size, uint32_t usageFlags) override;
    Texture::ptr CreateTexture(const TextureDesc& desc) override;
    FrameBuffer::ptr CreateFrameBuffer(const FrameBufferDesc& desc) override;
public:
    void UpdateTexture(Texture::ptr tex, TextureDesc desc) override;
    void UpdateBuffer(GLBuffer::ptr buffer, RawData::ptr data, size_t offset, size_t size) override;
    void CopyFrameBufferImage(FrameBuffer::ptr src, int level, int x, int y, int z, 
                              FrameBuffer::ptr dst, int dstLevel, int dstX, int dstY, int dstZ,
                              int width, int height, int depth, int channelBits, const std::string& tag
                              ) override;
    bool BlitFrameBuffer(FrameBuffer::ptr src, int srcX1, int srcY1, int srcX2, int srcY2,
                         FrameBuffer::ptr dst, int dstX1, int dstY1, int dstX2, int dstY2,
                         int channelBits, FBBlitFilter filter, const std::string& tag
                        ) override;
    bool CopyFramebufferToMemory(FrameBuffer::ptr src, int channelBits, int x, int y, int w, int h,  GLPixelData::ptr pixel) override;
    bool CopyTextureToMemory(const std::vector<Texture::ptr>& srcs, const std::vector<GLRect2D>& rects, SharedData::ptr picture) override;
public:
    void SetFrameBufferSize(int width, int height) override;
    void SetScissorRect(int left, int top, int width, int height) override;
    void SetViewport(const Viewport& viewport) override;
    void SetBlendFactor(float color[4]) override;
    void SetStencilParams(uint8_t refValue, uint8_t writeMask, uint8_t compareMask) override;
    void BindSamplerStates(int start, std::vector<SamplerState::ptr> states) override;
    /**
     * @note nullptr vector mean unbind operation
     */
    void BindTextures(int start, std::vector<Texture::ptr> textures) override;
    void BindVertexBuffers(int start, const std::vector<GLBuffer::ptr>& buffers, const std::vector<int>& offsets) override;
    void BindIndexBuffer(GLBuffer::ptr indexBuffer, int offset) override;
    void BindFramebufferAsRenderTarget(FrameBuffer::ptr fbo, const RenderPassInfo& rp) override;
    void BindFramebufferAsTexture(FrameBuffer::ptr fbo, int binding, FBChannel channelBits, int layer) override;
    void BindPipeline(Pipeline::ptr pipeline) override;
    void UpdataUniformBuffer(RawData::ptr uniformBuffer, size_t size) override;
public:
    SL::ShaderLanguage GetShaderLanguage() override;
    DataFormat FrameBufferReadBackFormat(FBChannel channel) override;
    void GetDefaultVorldViewProj(int width, int height, float worldViewProj[16], bool onScreen) override;
public:
    void Draw(int vertexCount, int offset) override;
    void DrawIndexed(int vertexCount, int offset) override;
public:
    OpenGL::ptr _openGL;
private:
    void ApplySamplers();
private:
    std::recursive_mutex  _mtx;
    OpenGLPipeline::ptr   _currentPipeline;
private:
	uint8_t _stencilRef = 0;
	uint8_t _stencilWriteMask = 0;
	uint8_t _stencilCompareMask = 0;
private:
    std::vector<OpenGLSamplerState::ptr>     _boundSamplers;
    std::vector<OpenGLRenderTexture::ptr>    _boundTextures;
    std::vector<OpenGLBuffer::ptr>           _currentVBuffers;
    std::vector<int>                         _currentVBuffersOffsets;
    OpenGLBuffer::ptr                        _currentIndexBuffer;
    int                                      _currentIndexBufferOffset;
};

} // namespace Mmp