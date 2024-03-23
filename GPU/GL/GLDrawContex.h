//
// GLDrawContex.h
//
// Library: GPU
// Package: GL
// Module:  GL
//

#pragma once

#include <memory>
#include <string>

#include "Common/Any.h"

#include "GLCommon.h"
#include "GLContexDesc.h"
#include "GLContex.h"

#include "GPU/Windows/AbstractWindows.h"

namespace Mmp
{

class GLDrawContex
{
public:
    using ptr = std::shared_ptr<GLDrawContex>;
    /**
     * @note invoke before `Instance`
     */
    static void SetGPUBackendType(GPUBackend backend);
    static GPUBackend GetGpuBackendType();
    static GLDrawContex::ptr Instance();
public:
    GLDrawContex();
    virtual ~GLDrawContex() = default;
public: /* raw property */
    virtual Any Get(const std::string& key);
    virtual void Set(const std::string& key, Any value);
public:
    virtual void ThreadStart() = 0;
    virtual GpuTaskStatus ThreadFrame() = 0;
    virtual void ThreadEnd() = 0;
    virtual void ThreadStop() = 0;
public: /* render sync (transaction mode) */
    virtual void FenceBegin() = 0;
    virtual void FenceCommit() = 0;
    virtual void FenceEnd() = 0;
public: /* pipeline state */
    virtual DepthStencilState::ptr CreateDepthStencilState(const DepthStencilStateDesc& desc) = 0;
    virtual BlendState::ptr CreateBlendState(const BlendStateDesc& desc) = 0;
    virtual SamplerState::ptr CreateSamplerState(const SamplerStateDesc& desc) = 0;
    virtual RasterState::ptr CreateRasterState(const RasterStateDesc& desc) = 0;
    virtual InputLayout::ptr CreateInputLayout(const InputLayoutDesc& desc) = 0;
    virtual ShaderModule::ptr CreateShaderModule(SL::ShaderStage stage, SL::ShaderLanguage lang, const std::string& code) = 0;
    virtual Pipeline::ptr CreateGraphicsPipeline(const PipelineDesc& desc, const std::string& tag) = 0;
public: /* resource */
    virtual GLBuffer::ptr CreateBuffer(size_t size, uint32_t usageFlags) = 0;
    virtual Texture::ptr CreateTexture(const TextureDesc& desc) = 0;
    virtual FrameBuffer::ptr CreateFrameBuffer(const FrameBufferDesc& desc) = 0;
public:
    virtual void UpdateTexture(Texture::ptr tex, TextureDesc desc) = 0;
    virtual void UpdateBuffer(GLBuffer::ptr buffer, RawData::ptr data, size_t offset, size_t size) = 0;
    virtual void CopyFrameBufferImage(FrameBuffer::ptr src, int level, int x, int y, int z,
                                      FrameBuffer::ptr dst, int dstLevel, int dstX, int dstY, int dstZ,
                                      int width, int height, int depth, int channelBits, const std::string& tag
                                     ) = 0;
    virtual bool BlitFrameBuffer(FrameBuffer::ptr src, int srcX1, int srcY1, int srcX2, int srcY2,
                                 FrameBuffer::ptr dst, int dstX1, int dstY1, int dstX2, int dstY2,
                                 int channelBits, FBBlitFilter filter, const std::string& tag
                                ) = 0;
    virtual bool CopyFramebufferToMemory(FrameBuffer::ptr src, int channelBits, int x, int y, int w, int h,  GLPixelData::ptr pixel) = 0;
    virtual bool CopyTextureToMemory(const std::vector<Texture::ptr>& srcs, const std::vector<GLRect2D>& rects, AbstractSharedData::ptr picture) = 0;    
public: /* dynamic state */
    virtual void SetFrameBufferSize(int width, int height) = 0;
    virtual void SetScissorRect(int left, int top, int width, int height) = 0;
    virtual void SetViewport(const Viewport& viewport) = 0;
    virtual void SetBlendFactor(float color[4]) = 0;
    virtual void SetStencilParams(uint8_t refValue, uint8_t writeMask, uint8_t compareMask) = 0;
    virtual void BindSamplerStates(int start, std::vector<SamplerState::ptr> states) = 0;
    virtual void BindTextures(int start, std::vector<Texture::ptr> textures) = 0;
    virtual void BindVertexBuffers(int start, const std::vector<GLBuffer::ptr>& buffers, const std::vector<int>& offsets) = 0;
    virtual void BindIndexBuffer(GLBuffer::ptr indexBuffer, int offset) = 0;
    virtual void BindFramebufferAsRenderTarget(FrameBuffer::ptr fbo, const RenderPassInfo& rp) = 0;
    virtual void BindFramebufferAsTexture(FrameBuffer::ptr fbo, int binding, FBChannel channelBits, int layer) = 0;
    virtual void BindPipeline(Pipeline::ptr pipeline) = 0;
    virtual void UpdataUniformBuffer(RawData::ptr uniformBuffer, size_t size) = 0;
public: /* query */
    virtual SL::ShaderLanguage GetShaderLanguage() = 0;
    virtual DataFormat FrameBufferReadBackFormat(FBChannel channel) = 0;
    virtual void GetDefaultVorldViewProj(int width, int height, float worldViewProj[16], bool onScreen = false) = 0;
public:
    virtual void Draw(int vertexCount, int offset) = 0;
    virtual void DrawIndexed(int vertexCount, int offset) = 0;
public:
    AbstractWindows::ptr GetWindows();
    void SetWindows(AbstractWindows::ptr windows);
private:
    AbstractWindows::ptr _windows;
};

} // namespace Mmp