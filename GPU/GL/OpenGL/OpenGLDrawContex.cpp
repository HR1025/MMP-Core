#include "OpenGLDrawContex.h"

#include <memory.h>
#include <cassert>

#include "OpenGLContex.h"
#include "OpenGLStepData.h"
#include "OpenGLRenderData.h"
#include "OpenGLInitStepData.h"
#include "OpenGLTranslator.h"
#include "OpenGLUtil.h"

namespace Mmp
{

constexpr uint32_t kVertexBufferSize = 4;

OpenGLDrawContex::OpenGLDrawContex()
{
    _boundSamplers.resize(MAX_TEXTURE_SLOTS);
    _boundTextures.resize(MAX_TEXTURE_SLOTS);
    _currentVBuffers.resize(kVertexBufferSize);
    _currentVBuffersOffsets.resize(kVertexBufferSize);
    _currentIndexBufferOffset = -1;

    _openGL = std::make_shared<OpenGL>();
}

OpenGLDrawContex::~OpenGLDrawContex()
{
    // Hint : 一定要确保所有与 _openGL 有关的资源在析构之前已经正确释放,
    //        否则要带来循环引用
    // See also : static void GarbageCollection(Any any)
    for (auto& sampler : _boundSamplers)
    {
        assert(!sampler);
    }
    for (auto& texture : _boundTextures)
    {
        assert(!texture);
    }
    for (auto& buffer : _currentVBuffers)
    {
        assert(!buffer);
    }
    assert(!_currentIndexBuffer);
    assert(!_currentPipeline);
}

void OpenGLDrawContex::ThreadStart() 
{
    _openGL->ThreadStart();
}

GpuTaskStatus OpenGLDrawContex::ThreadFrame()
{
    return _openGL->ThreadFrame();
}

void OpenGLDrawContex::ThreadEnd()
{
    _openGL->ThreadEnd();
}

void OpenGLDrawContex::ThreadStop() 
{
    // Hint : 提前回收好资源,在渲染线程退出前
    {
        for (auto& sampler : _boundSamplers)
        {
            sampler.reset();
        }
        for (auto& texture : _boundTextures)
        {
            texture.reset();
        }
        for (auto& buffer : _currentVBuffers)
        {
            buffer.reset();
        }
        for (auto& offset : _currentVBuffersOffsets)
        {
            offset = 0;
        }
        _currentIndexBuffer.reset();
        _currentPipeline.reset();
    }
    _openGL->ThreadStop();
}

void OpenGLDrawContex::FenceBegin()
{
    // GL_LOG_INFO << "Mtx Lock";
    _mtx.lock();
}

void OpenGLDrawContex::FenceEnd()
{
    // GL_LOG_INFO << "Mtx Unlock";
    _mtx.unlock();
}

void OpenGLDrawContex::FenceCommit()
{
    std::unique_lock<std::mutex> lock(_openGL->_renderMtx);
    if (!_openGL->_initSteps.empty() || !_openGL->_steps.empty())
    {
        OpenGLRenderRunType runType = OpenGLRenderRunType::NORMAL;
        // Hint : 最后一个任务是渲染,则需要刷新到屏幕上 (if needed)
        if (_openGL->_steps.back().stepType == OpenGLRenderStepType::RENDER)
        {
            runType = OpenGLRenderRunType::PRESENT;
        }
        OpenGLThreadTask task;
        task.initSteps.swap(_openGL->_initSteps);
        task.steps.swap(_openGL->_steps);
        _openGL->_curStep = nullptr;
        task.runType = runType;
        _openGL->_tasks.push(task);
        _openGL->_renderCond.notify_one();
    }
}

DepthStencilState::ptr OpenGLDrawContex::CreateDepthStencilState(const DepthStencilStateDesc& desc)
{
    OpenGLDepthStencilState::ptr ds = std::make_shared<OpenGLDepthStencilState>();
    ds->depthTestEnabled = desc.depthTestEnabled;
    ds->depthWriteEnabled = desc.depthWriteEnabled;
    ds->depthComp = GLComparisonTypeToOpenGLType(desc.depthCompare);
    ds->stencilEnabled = desc.stencilEnabled;
    ds->stencilCompareOp = GLComparisonTypeToOpenGLType(desc.stencil.compareOp);
    ds->stencilPass = GLStencilOpToOpenGLType(desc.stencil.passOp);
    ds->stencilFail = GLStencilOpToOpenGLType(desc.stencil.failOp);
    ds->stencilZFail = GLStencilOpToOpenGLType(desc.stencil.depthFailOp);
    return ds;
}

BlendState::ptr OpenGLDrawContex::CreateBlendState(const BlendStateDesc& desc)
{
    OpenGLBlendState::ptr bs = std::make_shared<OpenGLBlendState>();
    bs->enabled   = desc.enabled;
    bs->eqCol     = GLBlendOpToOpenGLType(desc.eqCol);
    bs->srcCol    = GLBlendFactorToOpenGLType(desc.srcCol);
    bs->dstCol    = GLBlendFactorToOpenGLType(desc.dstCol);
    bs->eqAlpha   = GLBlendOpToOpenGLType(desc.eqAlpha);
    bs->srcAlpha  = GLBlendFactorToOpenGLType(desc.srcAlpha);
    bs->dstAlpha  = GLBlendFactorToOpenGLType(desc.dstAlpha);
    bs->colorMask = desc.colorMask;
    return bs;
}

SamplerState::ptr OpenGLDrawContex::CreateSamplerState(const SamplerStateDesc& desc)
{
    OpenGLSamplerState::ptr samps = std::make_shared<OpenGLSamplerState>();
    samps->wrapU       = GLTextureAddressModeToOpenGLType(desc.wrapU);
    samps->wrapV       = GLTextureAddressModeToOpenGLType(desc.wrapV);
    samps->wrapW       = GLTextureAddressModeToOpenGLType(desc.wrapW);
    samps->magFilt     = GLTextureFilterToOpenGLType(desc.magFilter);
    samps->minFilt     = GLTextureFilterToOpenGLType(desc.minFilter);
    samps->mipMinFilt  = GenerateMipFilterOpenGLType(desc.minFilter, desc.mipFilter);
    return samps;
}

RasterState::ptr OpenGLDrawContex::CreateRasterState(const RasterStateDesc& desc)
{
    OpenGLRasterState::ptr rs = std::make_shared<OpenGLRasterState>();
    if (desc.cull == CullMode::NONE)
    {
        rs->cullEnable = GL_FALSE;
        return rs;
    }
    rs->cullEnable = GL_TRUE;
    rs->frontFace = GLFacingToOpenGLType(desc.frontFace);
    rs->cullMode = GLCullModeToOpenGLType(desc.cull);
    return rs;
}

InputLayout::ptr OpenGLDrawContex::CreateInputLayout(const InputLayoutDesc& desc)
{
    OpenGLInputLayout::ptr fmt = std::make_shared<OpenGLInputLayout>();
    fmt->Complie(_openGL, desc);
    return fmt;
}

ShaderModule::ptr OpenGLDrawContex::CreateShaderModule(SL::ShaderStage stage, SL::ShaderLanguage lang, const std::string& code)
{
    OpenGLShaderModule::ptr shader = std::make_shared<OpenGLShaderModule>(stage, "OpenGL");

    auto openGLFeature = GetOpenGLFeature();

    std::string _code;

    // Hint : 
    // (1) SL 分为 GLSL 和 ELSL 两大类, 各自有存在一些版本间的差异
    // (2) MMP 规定了一些 vs 和 fs 可用 attribute (如 Position, Color0 等)
    // TODO: 是否要开放 attribute 自定义的权限

    // add version information
    if (lang == SL::ShaderLanguage::GLSL_4xx)
    {
        _code += "#version 410 core\n";
    }
    else if (lang == SL::ShaderLanguage::ELSL_3xx)
    {
        _code += "#version 300 es\n";
    }

    // easy version compatibility
    if (stage == SL::ShaderStage::VERTEX)
    {
        _code += 
R"(
#if __VERSION__ >= 130
    #define attribute in
    #define varying out
#endif
)";
    }
    else if (stage == SL::ShaderStage::FRAGMENT)
    {
        _code += 
R"(
#ifdef GL_ES
    precision lowp float;
#endif
#if __VERSION__ < 130
    #define texture texture2D
#endif
#if __VERSION__ >= 130
    #define varying in
    #define gl_FragColor fragColor0
#ifdef GL_ES
    layout(location = 0) out vec4 fragColor0;
    layout(location = 1) out vec4 fragColor1;
    layout(location = 2) out vec4 fragColor2;
#else
    out vec4 fragColor0;
    out vec4 fragColor1;
    out vec4 fragColor2;
#endif
#endif
)";
    }

    _code += code;

    if (shader->Complie(_openGL, lang, _code))
    {
        return shader;
    }
    else
    {
        return nullptr;
    }
}

Pipeline::ptr OpenGLDrawContex::CreateGraphicsPipeline(const PipelineDesc& desc, const std::string& tag)
{
    if (desc.shaders.empty())
    {
        GL_LOG_ERROR << "Pipeline requires at least one shader";
        return nullptr;
    }
    if (!desc.depthStencil || !desc.blend || !desc.raster)
    {
        GL_LOG_ERROR << "Incomplete prim description";
        return nullptr;
    }

    OpenGLPipeline::ptr pipeline = std::make_shared<OpenGLPipeline>();
    for (auto shader : desc.shaders)
    {
        if (shader)
        {
            pipeline->shaders.push_back(std::dynamic_pointer_cast<OpenGLShaderModule>(shader));
        }
        else
        {
            GL_LOG_ERROR << "Try to create graphics pipeline " << tag << " with a null shader module";
            assert(false);
            return nullptr;
        }
    }
    pipeline->dynamicUniforms = desc.uniformBufferDesc;
    pipeline->samplers = desc.samplers;
    if (pipeline->LinkShaders(_openGL ,desc))
    {
        pipeline->prim = GLPrimitiveToOpenGLType(desc.prim);
        pipeline->depthStencil = std::dynamic_pointer_cast<OpenGLDepthStencilState>(desc.depthStencil);
        pipeline->blend = std::dynamic_pointer_cast<OpenGLBlendState>(desc.blend);
        pipeline->raster = std::dynamic_pointer_cast<OpenGLRasterState>(desc.raster);
        pipeline->inputLayout = std::dynamic_pointer_cast<OpenGLInputLayout>(desc.inputLayout);
        return pipeline;
    }
    else
    {
        GL_LOG_ERROR << "Fail to create graphics pipeline " << tag << " - shaders failed to link";
        return nullptr;
    }

    return pipeline;
}

GLBuffer::ptr OpenGLDrawContex::CreateBuffer(size_t size, uint32_t usageFlags)
{
    return std::make_shared<OpenGLBuffer>(_openGL, size, usageFlags);
}

void OpenGLDrawContex::SetFrameBufferSize(int width, int height)
{
    _openGL->Resize(width, height);
}

void OpenGLDrawContex::SetScissorRect(int left, int top, int width, int height)
{
    assert(_openGL->_curStep && _openGL->_curStep->stepType == OpenGLRenderStepType::RENDER);
    OpenGLRender cmd;
    OpenGLScissorRenderData data;
    cmd.cmd = OpenGLRenderCommond::SCISSOR;
    data.rc.x = left;
    data.rc.y = top;
    data.rc.w = width;
    data.rc.h = height;
    cmd.data = data;
    _openGL->_curStep->commands.push_back(cmd);
}

Texture::ptr OpenGLDrawContex::CreateTexture(const TextureDesc& desc)
{
    return std::make_shared<OpenGLTexture>(_openGL, desc);
}

FrameBuffer::ptr OpenGLDrawContex::CreateFrameBuffer(const FrameBufferDesc& desc)
{
    OpenGLRenderFrameBuffer::ptr frameBuffer = std::make_shared<OpenGLRenderFrameBuffer>(desc.width, desc.height, desc.zStencil);
    // Create Frame Buffer
    {
        OpenGLInitStep step;
        OpenGLCreateFramebufferInitData data;
        step.type = OpenGLInitStepType::CREATE_FRAMEBUFFER;
        data.frameBuffer = frameBuffer;
        // Hint : 支持将自定义 color texture 绑定至 framebuffer 上
        if (!desc.colorTexs.empty())
        {
            std::vector<OpenGLRenderTexture::ptr> textures;
            for (auto& colorTex : desc.colorTexs)
            {
                textures.push_back(std::dynamic_pointer_cast<OpenGLTexture>(colorTex)->GetTex());
            }
            data.frameBuffer->SetColorTextures(textures);
        }
        step.data = data;
        _openGL->_initSteps.push_back(step);
    }
    return std::make_shared<OpenGLFrameBuffer>(frameBuffer);
}

void OpenGLDrawContex::UpdateTexture(Texture::ptr tex, TextureDesc desc)
{
    OpenGLTexture::ptr _tex = std::dynamic_pointer_cast<OpenGLTexture>(tex);
    int level = 0;
    int width = desc.width;
    int height = desc.height;
    int depth = desc.depth;
    for (const auto& data : desc.initData)
    {
        assert((int)data->width == width);
        assert((int)data->height == height);
        assert((int)data->depth == depth);
        _tex->SetImageData(0, 0, 0, width, height, depth, level, 0, data);
        width  = (width + 1) / 2;
        height = (height + 1) / 2;
        depth  = (depth + 1) / 2; 
        level++;
    }
}

void OpenGLDrawContex::UpdateBuffer(GLBuffer::ptr buffer, RawData::ptr data, size_t offset, size_t size)
{
    OpenGLBuffer::ptr buf = std::dynamic_pointer_cast<OpenGLBuffer>(buffer);
    
    if (data->GetSize() < size + offset)
    {
        assert(false);
    }

    // Buffer Sub data
    {
        OpenGLInitStep step;
        OpenGLBufferSubdataInitData _data;
        step.type    = OpenGLInitStepType::BUFFER_SUBDATA;
        _data.buffer = buf->buffer;
        _data.data   = data;
        _data.size   = (int)size;
        _data.offset = (int)offset;
        step.data    = _data;
        _openGL->_initSteps.push_back(step);
    }
}

void OpenGLDrawContex::CopyFrameBufferImage(FrameBuffer::ptr src, int level, int x, int y, int z, 
                                            FrameBuffer::ptr dst, int dstLevel, int dstX, int dstY, int dstZ,
                                            int width, int height, int depth, int channelBits, const std::string& tag
                                            )
{
    OpenGLFrameBuffer::ptr fbsrc = std::dynamic_pointer_cast<OpenGLFrameBuffer>(src);
    OpenGLFrameBuffer::ptr fbdst = std::dynamic_pointer_cast<OpenGLFrameBuffer>(dst);

    int aspect = 0;
    if (channelBits & FBChannel::FB_COLOR_BIT)
    {
        aspect |= GL_COLOR_BUFFER_BIT;
    }
    else if (channelBits & (FBChannel::FB_DEPTH_BIT | FBChannel::FB_STENCIL_BIT))
    {
        if (channelBits & FBChannel::FB_DEPTH_BIT)
        {
            aspect |= GL_DEPTH_BUFFER_BIT;
        }
        else if (channelBits & FBChannel::FB_STENCIL_BIT)
        {
            aspect |= GL_STENCIL_BUFFER_BIT;
        }
    }

    // Copy Frame Buffer
    {
        OpenGLRenderStep step;
        OpenGLCopyData   data;
        step.stepType = OpenGLRenderStepType::COPY;
        {
            data.srcRect.x = x;
            data.srcRect.y = y;
            data.srcRect.w = width;
            data.srcRect.h = height;
        }
        {
            data.dstPos.x = dstX;
            data.dstPos.y = dstY;
        }
        data.aspectMask = aspect;
        data.src = fbsrc->frameBuffer;
        data.dst = fbdst->frameBuffer;
        step.data = data;
        _openGL->_steps.push_back(step);
    }

}

bool OpenGLDrawContex::BlitFrameBuffer(FrameBuffer::ptr src, int srcX1, int srcY1, int srcX2, int srcY2,
                                       FrameBuffer::ptr dst, int dstX1, int dstY1, int dstX2, int dstY2,
                                       int channelBits, FBBlitFilter filter, const std::string& tag
                                      )
{
    OpenGLFrameBuffer::ptr fbsrc = std::dynamic_pointer_cast<OpenGLFrameBuffer>(src);
    OpenGLFrameBuffer::ptr fbdst = std::dynamic_pointer_cast<OpenGLFrameBuffer>(dst);

    int aspect = 0;
    if (channelBits & FBChannel::FB_COLOR_BIT)
    {
        aspect |= GL_COLOR_BUFFER_BIT;
    }
    else if (channelBits & FBChannel::FB_DEPTH_BIT)
    {
        aspect |= GL_DEPTH_BUFFER_BIT;
    }
    else if (channelBits & FBChannel::FB_STENCIL_BIT)
    {
        aspect |= GL_STENCIL_BUFFER_BIT;
    }

    // Blit Frame Buffer
    {
        OpenGLRenderStep step;
        OpenGLBlitData   data;
        step.stepType = OpenGLRenderStepType::BLIT;
        {
            data.srcRect.x = srcX1;
            data.srcRect.y = srcY1;
            data.srcRect.w = srcX2 - srcX1;
            data.srcRect.h = srcY2 - srcY1;
        }
        {
            data.dstRect.x = dstX1;
            data.dstRect.y = dstY1;
            data.dstRect.w = dstX2 - dstX1;
            data.dstRect.h = dstY2 - dstY1;
        }
        data.src = fbsrc->frameBuffer;
        data.dst = fbdst->frameBuffer;
        data.aspectMask = aspect;
        step.data = data;
        _openGL->_steps.push_back(step);
    }
    return true;
}

bool OpenGLDrawContex::CopyFramebufferToMemory(FrameBuffer::ptr src, int channelBits, int x, int y, int w, int h,  GLPixelData::ptr pixel)
{
    auto openGLFeature = GetOpenGLFeature();

    if (openGLFeature.IsGLES && (channelBits & FBChannel::FB_COLOR_BIT) == 0)
    {
        // Hint : can not readback depth or stencil on OpenGL ES
        return false;
    }

    OpenGLFrameBuffer::ptr buffer = std::dynamic_pointer_cast<OpenGLFrameBuffer>(src);

    GLuint aspect = 0;
    if (channelBits & FBChannel::FB_COLOR_BIT)
    {
        aspect |= GL_COLOR_BUFFER_BIT;
        if (pixel->format != DataFormat::R8G8B8A8_UNORM)
        {
            GL_LOG_ERROR << "Only support readback data format " << DataFormat::R8G8B8A8_UNORM << " for color channel";
            assert(false);
            return false;
        }
    }
    else if (channelBits & FBChannel::FB_DEPTH_BIT)
    {
        aspect |= GL_DEPTH_BUFFER_BIT;
        assert(pixel->format == DataFormat::D24_S8);
    }
    else if (channelBits & FBChannel::FB_STENCIL_BIT)
    {
        aspect |= GL_STENCIL_BUFFER_BIT;
        assert(pixel->format == DataFormat::S8);
    }

    {
        OpenGLRenderStep     step;
        OpenGLReadbackData   data;
        step.stepType = OpenGLRenderStepType::READBACK;
        {
            data.srcRect.x = x;
            data.srcRect.y = y;
            data.srcRect.w = w;
            data.srcRect.h = h;
        }
        data.aspectMask = aspect;
        data.dstFormat  = pixel->format;
        data.src        = buffer ? buffer->frameBuffer : nullptr;
        data.pixel      = pixel;
        step.data       = data;
        _openGL->_steps.push_back(step);
    }

    // Sync wait operation complete
    {
        _openGL->_curStep = nullptr;
        _openGL->FlushSync();
    }

    return true;
}

bool OpenGLDrawContex::CopyTextureToMemory(const std::vector<Texture::ptr>& srcs, const std::vector<GLRect2D>& rects, AbstractSharedData::ptr picture)
{
    OpenGLRenderStep          step;
    OpenGLReadbackImageData   data;
    step.stepType = OpenGLRenderStepType::READBACK_IMAGE;
    {
        for (auto& src : srcs)
        {
            data.textures.push_back(std::dynamic_pointer_cast<OpenGLTexture>(src)->GetTex());
        }
        data.srcRects = rects;
        data.mipLevel = 1;
        data.picture    = picture;
        step.data     = data;
        _openGL->_steps.push_back(step);
    }
    // Sync wait operation complete
    {
        _openGL->_curStep = nullptr;
        _openGL->FlushSync();
    }
    return true;
}

DataFormat OpenGLDrawContex::FrameBufferReadBackFormat(FBChannel channel)
{
    if (channel & FBChannel::FB_COLOR_BIT)
    {
        return DataFormat::R8G8B8A8_UNORM;
    }
    else if (channel & FBChannel::FB_DEPTH_BIT)
    {
        return DataFormat::D24_S8;
    }
    else if (channel & FBChannel::FB_STENCIL_BIT)
    {
        return DataFormat::S8;
    }
    else
    {
        assert(false);
        return DataFormat::R8G8B8A8_UNORM;
    }
}

void OpenGLDrawContex::GetDefaultVorldViewProj(int width, int height, float worldViewProj[16], bool onScreen)
{
    if (onScreen)
    {
        // frist column
        worldViewProj[0] = 2.0f / (width - 0.0f);
        worldViewProj[1] = 0.0f;
        worldViewProj[2] = 0.0f;
        worldViewProj[3] = 0.0f;
        // second column
        worldViewProj[4] = 0.0f;
        worldViewProj[5] = (2.0f / (0.0f - height));
        worldViewProj[6] = 0.0;
        worldViewProj[7] = 0.0;
        // third column
        worldViewProj[8] = 0.0f;
        worldViewProj[9] = 0.0f;
        worldViewProj[10] = 1.0f;
        worldViewProj[11] = 0.0f;
        // last column
        worldViewProj[12] = -(width + 0.0f) / (width - 0.0f);
        worldViewProj[13] = -(height + 0.0f) / (0.0f - height);
        worldViewProj[14] = 0.0f;
        worldViewProj[15] = 1.0f;
    }
    else
    {
        // frist column
        worldViewProj[0] = 2.0f / (width - 0.0f);
        worldViewProj[1] = 0.0f;
        worldViewProj[2] = 0.0f;
        worldViewProj[3] = 0.0f;
        // second column
        worldViewProj[4] = 0.0f;
        worldViewProj[5] = -(2.0f / (0.0f - height));
        worldViewProj[6] = 0.0;
        worldViewProj[7] = 0.0;
        // third column
        worldViewProj[8] = 0.0f;
        worldViewProj[9] = 0.0f;
        worldViewProj[10] = 1.0f;
        worldViewProj[11] = 0.0f;
        // last column
        worldViewProj[12] = -(width + 0.0f) / (width - 0.0f);
        worldViewProj[13] = (height + 0.0f) / (0.0f - height);
        worldViewProj[14] = 0.0f;
        worldViewProj[15] = 1.0f;
    }
}

void OpenGLDrawContex::SetViewport(const Viewport& viewport)
{
    assert(_openGL->_curStep && _openGL->_curStep->stepType == OpenGLRenderStepType::RENDER);
    OpenGLRender cmd;
    OpenGLViewportRenderData data;
    cmd.cmd = OpenGLRenderCommond::VIEWPORT;
    data.vp.maxZ = viewport.MaxDepth;
    data.vp.minZ = viewport.MinDepth;
    data.vp.h    = viewport.Height;
    data.vp.w    = viewport.Width;
    data.vp.x    = viewport.TopLeftX;
    data.vp.y    = viewport.TopLeftY;
    cmd.data     = data;
    _openGL->_curStep->commands.push_back(cmd);
}

void OpenGLDrawContex::SetBlendFactor(float color[4])
{
    assert(_openGL->_curStep && _openGL->_curStep->stepType == OpenGLRenderStepType::RENDER);
    OpenGLRender cmd;
    OpenGLBlendColorRenderData data;
    cmd.cmd = OpenGLRenderCommond::BLENDCOLOR;
    data.color[0] = color[0];
    data.color[1] = color[1];
    data.color[2] = color[2];
    data.color[3] = color[3];
    cmd.data = data;
    _openGL->_curStep->commands.push_back(cmd);
}

void OpenGLDrawContex::SetStencilParams(uint8_t refValue, uint8_t writeMask, uint8_t compareMask)
{
    _stencilRef         = refValue;
    _stencilWriteMask   = writeMask;
    _stencilCompareMask = compareMask;

    // Set Stencil Func
    {
        assert(_openGL->_curStep && _openGL->_curStep->stepType == OpenGLRenderStepType::RENDER);
        OpenGLRender cmd;
        OpenGLStencilFuncRenderData data;
        cmd.cmd = OpenGLRenderCommond::STENCILFUNC;
        data.enabled      = _currentPipeline->depthStencil->stencilEnabled;
        data.func         = _currentPipeline->depthStencil->stencilCompareOp;
        data.ref          = refValue;
        data.compareMask  = compareMask;
        cmd.data = data;
        _openGL->_curStep->commands.push_back(cmd);
    }

    // Set Stencil Op
    {
        assert(_openGL->_curStep && _openGL->_curStep->stepType == OpenGLRenderStepType::RENDER);
        OpenGLRender cmd;
        OpenGLStencilOpRenderData data;
        cmd.cmd = OpenGLRenderCommond::STENCILOP;
        data.writeMask   = writeMask;
        data.sFail       = _currentPipeline->depthStencil->stencilFail;
        data.zFail       = _currentPipeline->depthStencil->stencilZFail;
        data.pass        = _currentPipeline->depthStencil->stencilPass;
        cmd.data = data;
        _openGL->_curStep->commands.push_back(cmd);
    }
}

void OpenGLDrawContex::BindSamplerStates(int start, std::vector<SamplerState::ptr> states)
{
    size_t count = states.size();
    assert(start + count <= MAX_TEXTURE_SLOTS);
    for (size_t i=0; i<count; i++)
    {
        _boundSamplers[i+start] = std::dynamic_pointer_cast<OpenGLSamplerState>(states[i]);
    }
}

void OpenGLDrawContex::BindTextures(int start, std::vector<Texture::ptr> textures)
{
    size_t count = textures.size();
    assert(start + count <= MAX_TEXTURE_SLOTS);
    for (size_t i=0; i<count; i++)
    {
        OpenGLTexture::ptr texture = std::dynamic_pointer_cast<OpenGLTexture>(textures[i]);
        if (!texture)
        {
            // Unbind texture
            _boundTextures[i+start] = nullptr;
            {
                assert(_openGL->_curStep && _openGL->_curStep->stepType == OpenGLRenderStepType::RENDER);
                OpenGLRender command;
                OpenGLTextureRenderData data;
                data.texture = nullptr;
                data.solt = (int)(i+start);
                command.cmd = OpenGLRenderCommond::BINDTEXTURE;
                command.data = data;
                _openGL->_curStep->commands.push_back(command);
            }
        }
        else
        {
            // bind texture
            texture->Bind((int)(i+start));
            _boundTextures[i+start] = texture->GetTex();
        }
    }
}

void OpenGLDrawContex::BindVertexBuffers(int start, const std::vector<GLBuffer::ptr>& buffers, const std::vector<int>& offsets)
{
    int count = (int)buffers.size();
    assert(buffers.size() == offsets.size());
    assert(start+count <= (int)kVertexBufferSize);
    for (int i=0; i<count; i++)
    {
        _currentVBuffers[i+start]        = std::dynamic_pointer_cast<OpenGLBuffer>(buffers[i]);
        _currentVBuffersOffsets[i+start]  = offsets[i];
    }

}

void OpenGLDrawContex::BindIndexBuffer(GLBuffer::ptr indexBuffer, int offset)
{
    _currentIndexBuffer = std::dynamic_pointer_cast<OpenGLBuffer>(indexBuffer);
    _currentIndexBufferOffset = offset;
}

void OpenGLDrawContex::BindFramebufferAsRenderTarget(FrameBuffer::ptr fbo, const RenderPassInfo& rp)
{
    // TODO : redundant filtering and precondition judgment

    // Create Render Step
    OpenGLRenderStep step;
    {
        OpenGLRenderData data;
        step.stepType = OpenGLRenderStepType::RENDER;
        if (fbo)
        {
            data.framebuffer = std::dynamic_pointer_cast<OpenGLFrameBuffer>(fbo)->frameBuffer;
            assert(data.framebuffer);
        }
        data.color = rp.color;
        data.depth = rp.depth;
        data.stencil = rp.stencil;
        data.drawNums = 0;
        step.data = data;
    }

    // Hint : 1 - 切换 FB 时有需要时清空相关配置
    //        2 - 绑定 FB 需要在一个新的 Render Pass 进行
    {
        GLuint clearMask = 0;
        OpenGLRender command;
        OpenGLClearRenderData data;
        command.cmd = OpenGLRenderCommond::CLEAR;
        if (rp.color == RPAction::CLEAR)
        {
            clearMask |= GL_COLOR_BUFFER_BIT;
            data.clearColor = rp.clearColor;
        }
        if (rp.depth == RPAction::CLEAR)
        {
            clearMask |= GL_DEPTH_BUFFER_BIT;
            data.clearZ = rp.clearDepth;
        }
        if (rp.stencil == RPAction::CLEAR)
        {
            clearMask |= GL_STENCIL_BUFFER_BIT;
            data.clearStencil = rp.clearStencil;
        }
        if (clearMask)
        {
            data.scissorX = 0;
            data.scissorY = 0;
            data.scissorH = 0;
            data.scissorW = 0;
            data.clearMask = clearMask;
            data.colorMask = 0xF;
            command.data = data;
            step.commands.push_back(command);
        }
    }
    _openGL->_steps.push_back(step);
    _openGL->_curStep = &(_openGL->_steps.back());
}

void OpenGLDrawContex::BindFramebufferAsTexture(FrameBuffer::ptr fbo, int binding, FBChannel channelBits, int layer)
{
    assert(binding <= MAX_TEXTURE_SLOTS);
    OpenGLFrameBuffer::ptr fb = std::dynamic_pointer_cast<OpenGLFrameBuffer>(fbo);

    GLuint aspect = 0;
    if (channelBits & FBChannel::FB_COLOR_BIT)
    {
        aspect |= GL_COLOR_BUFFER_BIT;
        _boundTextures[binding] = fb->frameBuffer->colorTextures[0];
    }
    else if (channelBits & FBChannel::FB_DEPTH_BIT)
    {
        aspect |= GL_DEPTH_BUFFER_BIT;
        _boundTextures[binding] = fb->frameBuffer->zStencilTexture;
    }
    else if (channelBits & FBChannel::FB_STENCIL_BIT)
    {
        aspect |= GL_DEPTH_BUFFER_BIT;
        _boundTextures[binding] = fb->frameBuffer->zStencilTexture;
    }

    // Bind Framebuffer As Targer
    {
        assert(_openGL->_curStep && _openGL->_curStep->stepType == OpenGLRenderStepType::RENDER);
        OpenGLRender command;
        OpenGLBindFbTextureRenderData data;
        command.cmd = OpenGLRenderCommond::BIND_FB_TEXTURE;
        data.slot = binding;
        data.aspect = aspect;
        data.framebuffer = fb->frameBuffer;
        command.data = data;
        _openGL->_curStep->commands.push_back(command);
    }
}

void OpenGLDrawContex::BindPipeline(Pipeline::ptr pipeline)
{
    _currentPipeline = std::dynamic_pointer_cast<OpenGLPipeline>(pipeline);
    if (!_currentPipeline)
    {
        return;
    }
    _currentPipeline->blend->Apply(_openGL);
    _currentPipeline->depthStencil->Apply(_openGL, _stencilRef, _stencilWriteMask, _stencilCompareMask);
    _currentPipeline->raster->Apply(_openGL);
    // Bind Program
    {
        assert(_openGL->_curStep && _openGL->_curStep->stepType == OpenGLRenderStepType::RENDER);
        OpenGLRender command;
        OpenGLProgramRenderData data;
        command.cmd = OpenGLRenderCommond::BINDPROGRAM;
        data.program = _currentPipeline->program;
        command.data = data;
        _openGL->_curStep->commands.push_back(command);
    }
}

void OpenGLDrawContex::UpdataUniformBuffer(RawData::ptr uniformBuffer, size_t size)
{
    assert(_currentPipeline->dynamicUniforms.uniformBufferSize == size);

    for (size_t i=0; i<_currentPipeline->dynamicUniforms.uniforms.size(); i++)
    {
        const auto& uniform = _currentPipeline->dynamicUniforms.uniforms[i];
        const GLint& loc = _currentPipeline->locs->dynamicUniformLocs[i];
        const float* data = (const float*)((uint8_t*)uniformBuffer->GetData() + uniform.offset);
        int count = 0;
        switch (uniform.type)
        {
            case UniformType::FLOAT1: count = 1; break;
            case UniformType::FLOAT2: count = 2; break;
            case UniformType::FLOAT3: count = 3; break;
            case UniformType::FLOAT4: count = 4; break;
            default: break;
        }
        switch (uniform.type)
        {
            case UniformType::FLOAT1:
            case UniformType::FLOAT2:
            case UniformType::FLOAT3:
            case UniformType::FLOAT4:
            // Set Uniform F
            {
                OpenGLRender command;
                OpenGLUniform4RenderData _data;
                command.cmd = OpenGLRenderCommond::UNIFORM4F;
                _data.loc = &loc;
                _data.count = count;
                memcpy(_data.v, data, sizeof(float) * count);
                command.data = _data;
                _openGL->_curStep->commands.push_back(command);
                break;
            }
            case UniformType::MATRIX4X4:
            // Set Uniform M4x4
            {
                OpenGLRender command;
                OpenGLUniformMatrix4RenderData _data;
                command.cmd = OpenGLRenderCommond::UNIFORMMATRIX;
                _data.loc = &loc;
                memcpy(_data.m, data, sizeof(float) * 16);
                command.data = _data;
                _openGL->_curStep->commands.push_back(command);
                break;
            }
            default: break;
        }
    }
}

SL::ShaderLanguage OpenGLDrawContex::GetShaderLanguage()
{
    if (GetOpenGLFeature().IsGLES)
    {
        return SL::ShaderLanguage::ELSL_3xx;
    }
    else
    {
        return SL::ShaderLanguage::GLSL_4xx;
    }
}

void OpenGLDrawContex::Draw(int vertexCount, int offset)
{
    if (!_currentVBuffers[0])
    {
        GL_LOG_ERROR << "Can not Draw without a vertex buffer";
        return;
    }
    ApplySamplers();
    // Bind Vertex buffer
    if (_currentPipeline->inputLayout)
    {
        assert(_openGL->_curStep && _openGL->_curStep->stepType == OpenGLRenderStepType::RENDER);
        OpenGLRender command;
        OpenGLBindVertexBufferRenderData data;
        command.cmd = OpenGLRenderCommond::BIND_VERTEX_BUFFER;
        data.inputLayout = _currentPipeline->inputLayout->inputLayout;
        data.buffer      = _currentVBuffers[0]->buffer;
        data.offset      = _currentVBuffersOffsets[0];
        command.data     = data;
        _openGL->_curStep->commands.push_back(command);
    }
    // Draw
    {
        assert(_openGL->_curStep && _openGL->_curStep->stepType == OpenGLRenderStepType::RENDER);
        OpenGLRender command;
        OpenGLDrawRenderData data;
        command.cmd   = OpenGLRenderCommond::DRAW;
        data.mode     = _currentPipeline->prim;
        data.frist    = offset;
        data.count    = vertexCount;
        data.buffer   = 0;
        command.data  = data;
        RefAnyCast<OpenGLRenderData>(_openGL->_curStep->data).drawNums++;
        _openGL->_curStep->commands.push_back(command);
    }
}

void OpenGLDrawContex::DrawIndexed(int vertexCount, int offset)
{
    if (!_currentVBuffers[0])
    {
        GL_LOG_ERROR << "Can not call DrawIndexed without a vertex buffer";
        assert(false);
        return;
    }
    if (!_currentIndexBuffer)
    {
        GL_LOG_ERROR << "Can not call DrawIndexed without an index buffer";
        assert(false);
        return;
    }
    ApplySamplers();
    // Bind Vertex buffer
    if (_currentPipeline->inputLayout)
    {
        assert(_openGL->_curStep && _openGL->_curStep->stepType == OpenGLRenderStepType::RENDER);
        OpenGLRender command;
        OpenGLBindVertexBufferRenderData data;
        command.cmd = OpenGLRenderCommond::BIND_VERTEX_BUFFER;
        data.inputLayout = _currentPipeline->inputLayout->inputLayout;
        data.buffer      = _currentVBuffers[0]->buffer;
        data.offset      = _currentVBuffersOffsets[0];
        command.data     = data;
        _openGL->_curStep->commands.push_back(command);
    }
    // Draw Indexed
    {
        assert(_openGL->_curStep && _openGL->_curStep->stepType == OpenGLRenderStepType::RENDER);
        OpenGLRender command;
        OpenGLDrawIndexedRenderData data;
        command.cmd = OpenGLRenderCommond::DRAW_INDEXED;
        data.mode   = _currentPipeline->prim;
        data.count  = vertexCount;
        data.indexType = GL_UNSIGNED_SHORT;
        data.indices   = (void *)((intptr_t)_currentIndexBufferOffset + offset * sizeof(uint32_t));
        command.data   = data;
        _openGL->_curStep->commands.push_back(command);
    }
}

void OpenGLDrawContex::ApplySamplers()
{
    for (int i=0; i<MAX_TEXTURE_SLOTS; i++)
    {
        OpenGLSamplerState::ptr  samp = _boundSamplers[i];
        OpenGLRenderTexture::ptr tex  = _boundTextures[i];
        if (!tex)
        {
            continue;
        }
        else if (/* tex && */ !samp)
        {
            GL_LOG_ERROR << "Sampler Missing";
            assert(false);
            continue;
        }
        GLenum wrapS;
        GLenum wrapT;
        if (tex->canWrap)
        {
            wrapS = samp->wrapU;
            wrapT = samp->wrapV;
        }
        else
        {
			wrapS = GL_CLAMP_TO_EDGE;
			wrapT = GL_CLAMP_TO_EDGE;
        }
        GLenum magFilt = samp->magFilt;
        GLenum minFilt = tex->numMips > 1 ? samp->mipMinFilt : samp->minFilt;
        // Set Texture Sampler
        {
            assert(_openGL->_curStep && _openGL->_curStep->stepType == OpenGLRenderStepType::RENDER);
            OpenGLRender commamd;
            OpenGLTextureSamplerRenderData data;
            commamd.cmd = OpenGLRenderCommond::TEXTURESAMPLER;
            data.slot       = i;
            data.wrapS      = wrapS;
            data.wrapT      = wrapT;
            data.magFilter  = magFilt;
            data.minFilter  = minFilt;
            data.anisotropy = 0.0f;
            commamd.data = data;
            _openGL->_curStep->commands.push_back(commamd);
        }
        // Set Texture Lods
        {
            assert(_openGL->_curStep && _openGL->_curStep->stepType == OpenGLRenderStepType::RENDER);
            OpenGLRender commamd;
            OpenGLTextureLodRenderData data;
            commamd.cmd = OpenGLRenderCommond::TEXTURELOD;
            data.slot     = i;
            data.minLod   = 0;
            data.maxLod   = (float)tex->numMips - 1;
            data.lodBias  = 0.0f; 
            commamd.data  = data;
            _openGL->_curStep->commands.push_back(commamd);
        }
    }
}

} // namespace Mmp