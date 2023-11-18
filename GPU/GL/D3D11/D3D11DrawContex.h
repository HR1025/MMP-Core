//
// D3D11DrawContex.h
//
// Library: GPU
// Package: GL
// Module:  D3D11
// 

#pragma once

#include <map>
#include <atomic>
#include <thread>
#include <mutex>
#include <queue>

#include "GLDrawContex.h"
#include "D3D11Contex.h"

#include "GPU/Windows/AbstractWindows.h"

namespace Mmp
{

class D3D11DrawContex : public GLDrawContex
{
public:
    D3D11DrawContex();
    ~D3D11DrawContex();
public:
    /**
     * @note
     *            key           :    value
     *            DXGIWindow    :    AbstractWindows::ptr
     */
    void Set(const std::string& key, Any value) override;
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
private:
    uint16_t GetSupportDataFormat(DataFormat format);
    ID3D11DepthStencilState* GetCachedDepthStencilState(D3D11DepthStencilState::ptr state, uint8_t stencilWriteMask, uint8_t stencilCompareMask);
    void ApplyCurrentState();
private:
    using D3D11Task = std::function<GpuTaskStatus()>;
    std::mutex              _taskMtx;
    std::deque<D3D11Task>   _tasksQueue;
    std::condition_variable _taskCond;
private:
    std::thread::id        _renderTID;
    std::atomic<bool>      _running;
    std::recursive_mutex   _mtx;
private:
    std::mutex                _syncMtx;
    std::condition_variable   _syncCond;
    bool                      _syncDone;
private:
    AbstractWindows::ptr   _window;
private:
    float    _blendFactor[4];
    bool     _blendFactorDirty;
private:
    uint8_t  _stencilRef;
    uint8_t  _stencilWriteMask;
    uint8_t  _stencilCompareMask;
    bool     _stencilDirty;
private:
    ID3D11Buffer* _nextIndexBuffer;
    int           _nextIndexBufferOffset;
    bool          _indexBufferDirty;
private:
    uint64_t      _fenceRef;
private:
    std::map<D3D11DepthStencilKey, ID3D11DepthStencilState*> _depthStencilCache;
private:
    D3D11Pipeline::ptr                      _curPipeline;
    D3D11BlendState::ptr                    _curBlend;
    D3D11DepthStencilState::ptr             _curDepthStencil;
    D3D11RasterState::ptr                   _curRaster;
    ID3D11InputLayout*                      _curInputLayout;
    ID3D11VertexShader*                     _curVS;
    ID3D11PixelShader*                      _curPS;
    ID3D11GeometryShader*                   _curGS;
    D3D11_PRIMITIVE_TOPOLOGY                _curTopology;
    D3D11FrameBuffer::ptr                   _curRenderTarget;
    int                                     _curRTWidth;
    int                                     _curRTHeight;
    std::vector<ID3D11RenderTargetView*>    _curRenderTargetViews;
    ID3D11DepthStencilView*                 _curDepthStencilView;
    int                                     _bbWidth;
    int                                     _bbHeight;
    ID3D11RenderTargetView*                 _bbRenderTargetView;  // owner by window
    ID3D11DepthStencilView*                 _bbDepthStencilView;  // owner by window
private:
    ID3D11Buffer* _nextVertexBuffers[D3D11_MAX_VERTEX_BUFFER_SIZE];
    int           _nextVertexBufferOffsets[D3D11_MAX_VERTEX_BUFFER_SIZE];
private:
    ID3D11Device*         _device;
    ID3D11DeviceContext*  _context;
    pD3DCompile           _D3DCompile;
};

} // namespace Mmp