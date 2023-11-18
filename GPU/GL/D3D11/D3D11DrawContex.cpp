#include "D3D11DrawContex.h"

#include <winerror.h>

#include "D3D11Translator.h"

namespace Mmp
{

D3D11DrawContex::D3D11DrawContex()
{
    _window = nullptr;
    _device = nullptr;
    _context = nullptr;
    _D3DCompile = nullptr;

    memset(_nextVertexBuffers, 0, sizeof(ID3D11Buffer*) * D3D11_MAX_VERTEX_BUFFER_SIZE);
    memset(_nextVertexBufferOffsets, 0, sizeof(int) * D3D11_MAX_VERTEX_BUFFER_SIZE);

    memset(_blendFactor, 0, sizeof(float) * 4);
    _blendFactorDirty = false;
    _stencilRef = 0;
    _stencilWriteMask = 0xFF;
    _stencilCompareMask = 0xFF;
    _stencilDirty = true;
    memset(_nextVertexBuffers, 0, sizeof(ID3D11Buffer*) * D3D11_MAX_VERTEX_BUFFER_SIZE);
    memset(_nextVertexBuffers, 0, sizeof(int) * D3D11_MAX_VERTEX_BUFFER_SIZE);
    _curRenderTarget = nullptr;
    _nextIndexBuffer = nullptr;
    _nextIndexBufferOffset = 0;
    _indexBufferDirty = false;
    _curPipeline = nullptr;
    _curBlend = nullptr;
    _curDepthStencil = nullptr;
    _curRaster = nullptr;
    _curInputLayout = nullptr;
    _curVS = nullptr;
    _curPS = nullptr;
    _curGS = nullptr;
    _curTopology = D3D11_PRIMITIVE_TOPOLOGY_UNDEFINED;
    _curRenderTargetViews.clear();
    _curDepthStencilView = nullptr;
    _bbRenderTargetView = nullptr;
    _bbDepthStencilView = nullptr;
    _curRTWidth = 0;
    _curRTHeight = 0;   
    _bbWidth = 0;
    _bbHeight = 0;

    _fenceRef = 0;

    _syncDone = true;
}

D3D11DrawContex::~D3D11DrawContex()
{

}

void D3D11DrawContex::Set(const std::string& key, Any value)
{
    if (key == "DXGIWindow")
    {
        if (value.type() == typeid(AbstractWindows::ptr))
        {
            _window = AnyCast<AbstractWindows::ptr>(value);
            assert(_window);
            {
                _device = AnyCast<ID3D11Device*>(_window->Get("ID3D11Device"));
                assert(_device);
            }
            {
                _context = AnyCast<ID3D11DeviceContext*>(_window->Get("ID3D11DeviceContext"));
                assert(_context);
            }
            {
                _D3DCompile = AnyCast<pD3DCompile>(_window->Get("D3DCompile"));
                assert(_D3DCompile);
            }
            {
                _bbWidth = AnyCast<int>(_window->Get("Width"));
                _bbHeight = AnyCast<int>(_window->Get("Height"));
                _curRTWidth = _bbWidth;
                _curRTHeight = _bbHeight;
            }
            {
                _bbRenderTargetView = AnyCast<ID3D11RenderTargetView*>(_window->Get("BackRenderTargetView"));
                assert(_bbRenderTargetView);
                _curRenderTargetViews.resize(1);
                _curRenderTargetViews[0] = _bbRenderTargetView;
            }
            {
                _bbDepthStencilView = AnyCast<ID3D11DepthStencilView*>(_window->Get("BackDepthStencilView"));
                assert(_bbDepthStencilView);
                _curDepthStencilView = _bbDepthStencilView;
            }
        }
        else
        {
            assert(false);
        }
    }
    else
    {
        GLDrawContex::Set(key, value);
    }
}

void D3D11DrawContex::ThreadStart()
{
    std::lock_guard<std::mutex> lock(_taskMtx);
    GL_LOG_INFO << "Render Thread Start";
    _renderTID = std::this_thread::get_id();
    _running = true;
}

GpuTaskStatus D3D11DrawContex::ThreadFrame() 
{
    std::unique_lock<std::mutex> lock(_taskMtx);
    if (!_running)
    {
        return GpuTaskStatus::EXIT;
    }
    else
    {
        if (_tasksQueue.empty())
        {
            _taskCond.wait(lock);
        }
        if (!_tasksQueue.empty() /* && _running */)
        {
            auto task = _tasksQueue.front();
            _tasksQueue.pop_front();
            return task();
        }
        else
        {
            std::unique_lock<std::mutex> syncLock(_syncMtx);
            if (!_syncDone)
            {
                _syncCond.notify_all();
            }
            return GpuTaskStatus::EXIT;
        }
    }
}

void D3D11DrawContex::ThreadEnd() 
{
    GL_LOG_INFO << "Render Thread End";
}

void D3D11DrawContex::ThreadStop() 
{
    std::lock_guard<std::mutex> lock(_taskMtx);
    GL_LOG_INFO << "Render Thread Stop";
    _running = false;
    _taskCond.notify_all();
}

void D3D11DrawContex::FenceBegin() 
{
    _mtx.lock();
    _fenceRef++;
}

void D3D11DrawContex::FenceCommit() 
{
    // Hint : nothing to do
}

void D3D11DrawContex::FenceEnd() 
{
    _fenceRef--;
    _mtx.unlock();
}

DepthStencilState::ptr D3D11DrawContex::CreateDepthStencilState(const DepthStencilStateDesc& desc)
{
    D3D11DepthStencilState::ptr ds = std::make_shared<D3D11DepthStencilState>();
    ds->desc = desc;
    return ds;
}

BlendState::ptr D3D11DrawContex::CreateBlendState(const BlendStateDesc& desc)
{
    // See also : windows-win32-direct3d11.pdf - Configuring Blending Functionality
    D3D11BlendState::ptr bs = std::make_shared<D3D11BlendState>();
    D3D11_BLEND_DESC d3ddesc = {};
    d3ddesc.AlphaToCoverageEnable = FALSE;
    d3ddesc.IndependentBlendEnable = FALSE;
    for (int i=0; i<8; i++)
    {
        d3ddesc.RenderTarget[i].BlendEnable = desc.enabled ? TRUE : FALSE;
        d3ddesc.RenderTarget[i].RenderTargetWriteMask = desc.colorMask;
        d3ddesc.RenderTarget[i].BlendOp = GLBlendOpToOpenD3D11Type(desc.eqCol);
        d3ddesc.RenderTarget[i].BlendOpAlpha = GLBlendOpToOpenD3D11Type(desc.eqAlpha);
        d3ddesc.RenderTarget[i].SrcBlend = GLBlendFactorToOpenD3D11Type(desc.srcCol);
        d3ddesc.RenderTarget[i].SrcBlendAlpha = GLBlendFactorToOpenD3D11Type(desc.srcAlpha);
        d3ddesc.RenderTarget[i].DestBlend = GLBlendFactorToOpenD3D11Type(desc.dstCol);
        d3ddesc.RenderTarget[i].DestBlendAlpha = GLBlendFactorToOpenD3D11Type(desc.dstAlpha);
    }
    HRESULT hr = _device->CreateBlendState(&d3ddesc, &bs->bs);
    if (SUCCEEDED(hr))
    {
        return bs;
    }
    else
    {
        GL_LOG_ERROR << "ID3D11Device::CreateBlendState fail, error is:" << D3D11ReturnCodeToStr(hr);
        assert(false);
        return nullptr;
    }
}

SamplerState::ptr D3D11DrawContex::CreateSamplerState(const SamplerStateDesc& desc)
{
    D3D11SamplerState::ptr ss = std::make_shared<D3D11SamplerState>();
    D3D11_SAMPLER_DESC d3ddesc = {};
    d3ddesc.AddressU = GLTextureAddressModeToD3D11Type(desc.wrapU);
    d3ddesc.AddressV = GLTextureAddressModeToD3D11Type(desc.wrapV);
    d3ddesc.AddressW = GLTextureAddressModeToD3D11Type(desc.wrapW);
    // TODO : more detail
    d3ddesc.Filter = desc.magFilter == TextureFilter::LINEAR ? D3D11_FILTER_MIN_MAG_MIP_LINEAR : D3D11_FILTER_MIN_MAG_MIP_POINT;
    d3ddesc.MaxAnisotropy = 1;
    d3ddesc.MinLOD = -FLT_MAX;
    d3ddesc.MaxLOD = FLT_MAX;
    d3ddesc.ComparisonFunc = GLComparisonTypeToD3D11Type(desc.shadowCompareFunc);
    for (int i=0; i<4; i++)
    {
        d3ddesc.BorderColor[i] = 1.0f;
    }
    HRESULT hr = _device->CreateSamplerState(&d3ddesc, &ss->ss);
    if (SUCCEEDED(hr))
    {
        return ss;
    }
    else
    {
        GL_LOG_ERROR << "ID3D11Device::CreateSamplerState fail, error is:" << D3D11ReturnCodeToStr(hr);
        assert(false);
        return nullptr;
    }
}

RasterState::ptr D3D11DrawContex::CreateRasterState(const RasterStateDesc& desc)
{
    D3D11RasterState::ptr rs = std::make_shared<D3D11RasterState>();
    D3D11_RASTERIZER_DESC d3ddesc = {};
    d3ddesc.FillMode = D3D11_FILL_SOLID;
    d3ddesc.CullMode = GLCullModeToOpenD3D11Type(desc.cull);
    d3ddesc.FrontCounterClockwise = desc.frontFace == Facing::CCW ? TRUE : FALSE;
    d3ddesc.ScissorEnable = TRUE;
    d3ddesc.DepthClipEnable = TRUE;
    HRESULT hr = _device->CreateRasterizerState(&d3ddesc, &rs->rs);
    if (SUCCEEDED(hr))
    {
        return rs;
    }
    else
    {
        GL_LOG_ERROR << "ID3D11Device::CreateRasterizerState fail, error is:" << D3D11ReturnCodeToStr(hr);
        assert(false);
        return nullptr;
    }
}

InputLayout::ptr D3D11DrawContex::CreateInputLayout(const InputLayoutDesc& desc)
{
    D3D11InputLayout::ptr il = std::make_shared<D3D11InputLayout>();
    il->desc = desc;
    // See also : windows-win32-direct3d11.pdf - Create the Input-Layout Object
    for (size_t i=0; i < desc.attributes.size(); i++)
    {
        D3D11_INPUT_ELEMENT_DESC el = {};
        el.AlignedByteOffset = il->desc.attributes[i].offset;
        el.Format = DataFormatToD3D11(il->desc.attributes[i].format);
        el.InstanceDataStepRate = il->desc.bindings[desc.attributes[i].binding].instanceRate ? 1 : 0;
        el.InputSlot = il->desc.attributes[i].binding;
        el.SemanticName = il->desc.attributes[i].attribute.c_str(); // keep alive by D3D11InputLayout::desc
        el.InputSlotClass = il->desc.bindings[desc.attributes[i].binding].instanceRate ? D3D11_INPUT_PER_INSTANCE_DATA : D3D11_INPUT_PER_VERTEX_DATA;
        il->elements.push_back(el);
    }
    for (size_t i=0; i < desc.bindings.size(); i++)
    {
        il->strides.push_back(desc.bindings[i].stride);
    }
    return il;
}

ShaderModule::ptr D3D11DrawContex::CreateShaderModule(SL::ShaderStage stage, SL::ShaderLanguage lang, const std::string& code)
{
    if (lang != SL::ShaderLanguage::HLSL_D3D11)
    {
        return nullptr;
    }
    std::string _code;
    // Hint : 映射部分 HLSL 语法至 GLSL 语法, MMP 所有 SL 向 GLSL 实现靠拢
    // Todo : 此部分将来是否会存在膨胀问题 ???
    _code = 
R"(
/*********************************** Generate By MMP (Begin) **********************************/
#define vec2                       float2
#define vec3                       float3
#define vec4                       float4
#define mat3                       float3x3
#define mix                        lerp
#define clamp(value, minv, maxv)   (max(min(value, maxv), minv))
#define fract(x)                   ((x) - floor(x))
#define mod(x, y)                  ((x) - (y)*floor((x)/(y)))
#define lessThan(x, y)             ((x) < y)

float smmothstep(float edge0, float edge1, float x)
{
    float t = clamp((x - edge0) / (edge1 - edge0), 0.0, 1.0);
    return t * t * (3.0 - 2.0 * t);
}
/*********************************** Generate By MMP (END) **********************************/
)";
    _code += code;
    std::string compied;
    std::string error;
    std::string target;
    HRESULT     hr;
    switch (stage)
    {
        case SL::ShaderStage::VERTEX: target = "vs_4_0"; break;
        case SL::ShaderStage::FRAGMENT: target = "ps_4_0"; break;
        case SL::ShaderStage::GEOMETRY: target = "gs_4_0"; break;
        default: break;
    }
    if (target.empty())
    {
        assert(false);
        return nullptr;
    }

    ID3DBlob* compiledCode = nullptr;
    ID3DBlob* errorMsgs = nullptr;
    hr = _D3DCompile(_code.c_str(), _code.size(), nullptr, nullptr, nullptr, "main", target.c_str(),
                                D3DCOMPILE_ENABLE_BACKWARDS_COMPATIBILITY, 0, &compiledCode, &errorMsgs);
#ifdef ENABLE_D3D11_DEBUG
    GL_LOG_INFO << _code;
#endif /* ENABLE_D3D11_DEBUG */
    if (compiledCode)
    {
        compied = std::string((const char*)compiledCode->GetBufferPointer(), compiledCode->GetBufferSize());
        compiledCode->Release();
    }
    if (errorMsgs)
    {
        error = std::string((const char*)errorMsgs->GetBufferPointer(), errorMsgs->GetBufferSize());
        GL_LOG_ERROR << "Failed compiling \n" << _code << "\n" << error << "\n";
        errorMsgs->Release();
    }
    if (hr != S_OK)
    {
        return nullptr;
    }

    D3D11ShaderModule::ptr module = std::make_shared<D3D11ShaderModule>(stage, "");
    module->byteCode = std::vector<uint8_t>(compied.c_str(), compied.c_str() + compied.size());
    switch (stage)
    {
        case SL::ShaderStage::VERTEX:
        {
            hr = _device->CreateVertexShader(compied.c_str(), compied.size(), nullptr, &module->vs);
            break;
        }
        case SL::ShaderStage::FRAGMENT:
        {
            hr = _device->CreatePixelShader(compied.c_str(), compied.size(), nullptr, &module->ps);
            break;
        }
        case SL::ShaderStage::GEOMETRY:
        {
            hr = _device->CreateGeometryShader(compied.c_str(), compied.size(), nullptr, &module->gs); 
            break;
        }
        default:
        {
            hr = S_FALSE;
            break;
        }
    }
    if (SUCCEEDED(hr))
    {
        return module;
    }
    else
    {
        GL_LOG_ERROR << "ID3D11Device::Create*Shader fail, error is:" << D3D11ReturnCodeToStr(hr);
        assert(false);
        return nullptr;
    }
}

Pipeline::ptr D3D11DrawContex::CreateGraphicsPipeline(const PipelineDesc& desc, const std::string& tag)
{
    D3D11ShaderModule::ptr vs;
    HRESULT hr = S_OK;
    bool foundVertexShader = false;
    for (auto& shader : desc.shaders)
    {
        if (shader->GetStage() == SL::ShaderStage::VERTEX)
        {
            foundVertexShader = true;
            vs = std::dynamic_pointer_cast<D3D11ShaderModule>(shader);
            break;
        }
    }
    if (!foundVertexShader)
    {
        GL_LOG_ERROR << "Missing Vertex shader";
        assert(false);
        return nullptr;
    }

    D3D11Pipeline::ptr pipeline = std::make_shared<D3D11Pipeline>();
    pipeline->blend = std::dynamic_pointer_cast<D3D11BlendState>(desc.blend);
    pipeline->depthStencil = std::dynamic_pointer_cast<D3D11DepthStencilState>(desc.depthStencil);
    pipeline->input = std::dynamic_pointer_cast<D3D11InputLayout>(desc.inputLayout);
    pipeline->raster = std::dynamic_pointer_cast<D3D11RasterState>(desc.raster);
    pipeline->topology = GLPrimitiveToOpenD3D11Type(desc.prim);
    if (desc.uniformBufferDesc.uniformBufferSize)
    {
        pipeline->dynamicUniformsSize = desc.uniformBufferDesc.uniformBufferSize;
        D3D11_BUFFER_DESC bufdesc = {};
        bufdesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
        // See also : windows-win32-api-_direct3d11.pdf - ID3D11Device::CreateBuffer method
        // For a constant buffer (BindFlags of D3D11_BUFFER_DESC set to
        // D3D11_BIND_CONSTANT_BUFFER), you must set the ByteWidth value of
        // D3D11_BUFFER_DESC in multiples of 16, and less than or equal to
        // D3D11_REQ_CONSTANT_BUFFER_ELEMENT_COUNT.
        for (auto& uniform : desc.uniformBufferDesc.uniforms)
        {
            switch (uniform.type)
            {
                case UniformType::FLOAT1:
                {
                    bufdesc.ByteWidth += 1 * sizeof(float);
                    break;
                }
                case UniformType::FLOAT2:
                {
                    bufdesc.ByteWidth += 2 * sizeof(float);
                    break;
                }
                case UniformType::FLOAT3:
                {
                    bufdesc.ByteWidth += 3 * sizeof(float);
                    break;
                }
                case UniformType::FLOAT4:
                {
                    bufdesc.ByteWidth += 4 * sizeof(float);
                    break;
                }
                case UniformType::MATRIX4X4:
                {
                    bufdesc.ByteWidth += 16 * sizeof(float);
                    break;
                }
                default:
                    assert(false);
                    break;
            }
            
        }
        bufdesc.StructureByteStride = bufdesc.ByteWidth;
        bufdesc.ByteWidth = (UINT)((bufdesc.ByteWidth + 15) & ~15);
        bufdesc.Usage = D3D11_USAGE_DYNAMIC;
        bufdesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        hr = _device->CreateBuffer(&bufdesc, nullptr, &pipeline->dynamicUniforms);
        if (FAILED(hr))
        {
            GL_LOG_ERROR << "ID3D11Device::CreateBuffer fail, error is:" << D3D11ReturnCodeToStr(hr);
            assert(false);
            return nullptr;
        }
    }
    for (auto& shader : desc.shaders)
    {
        pipeline->shaderModules.push_back(std::dynamic_pointer_cast<D3D11ShaderModule>(shader));
        if (shader->GetStage() == SL::ShaderStage::VERTEX)
        {
            pipeline->vs = std::dynamic_pointer_cast<D3D11ShaderModule>(shader)->vs;
        }
        else if (shader->GetStage() == SL::ShaderStage::FRAGMENT)
        {
            pipeline->ps = std::dynamic_pointer_cast<D3D11ShaderModule>(shader)->ps;
        }
        else if (shader->GetStage() == SL::ShaderStage::GEOMETRY)
        {
            pipeline->gs = std::dynamic_pointer_cast<D3D11ShaderModule>(shader)->gs;
        }
    }
    if (pipeline->input != nullptr)
    {
        auto& elements = pipeline->input->elements;
        hr = _device->CreateInputLayout(elements.data(), (UINT)elements.size(), vs->byteCode.data(), vs->byteCode.size(), &pipeline->il);
        if (FAILED(hr))
        {
            GL_LOG_ERROR << "ID3D11Device::CreateInputLayout fail, error is:" << D3D11ReturnCodeToStr(hr);
            assert(false);
            return nullptr;
        }
    }
    else
    {
        pipeline->il = nullptr;
    }
    return pipeline;
}

GLBuffer::ptr D3D11DrawContex::CreateBuffer(size_t size, uint32_t usageFlags)
{
    // See also : windows-win32-direct3d11.pdf - Introduction to Buffers in Direct3D 11
    D3D11Buffer::ptr buf = std::make_shared<D3D11Buffer>();
    D3D11_BUFFER_DESC desc = {};
    desc.ByteWidth = (UINT)size;
    desc.BindFlags = 0;
    if (usageFlags & VERTEXDATA)
    {
        desc.BindFlags |= D3D11_BIND_VERTEX_BUFFER;
    }
    if (usageFlags & INDEXDATA)
    {
        desc.BindFlags |= D3D11_BIND_INDEX_BUFFER;
    }
    if (usageFlags & UNIFORM)
    {
        desc.BindFlags |= D3D11_BIND_CONSTANT_BUFFER;
    }
    desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    desc.Usage = D3D11_USAGE_DYNAMIC;
    buf->size = size;
    HRESULT hr = _device->CreateBuffer(&desc, nullptr, &buf->buf);
    if (SUCCEEDED(hr))
    {
        return buf;
    }
    else
    {
        GL_LOG_ERROR << "ID3D11Device::CreateBuffer fail, error is:" << D3D11ReturnCodeToStr(hr);
        assert(false);
        return nullptr;
    }
}

Texture::ptr D3D11DrawContex::CreateTexture(const TextureDesc& desc)
{
    bool generateMips = desc.generateMips;
    if (!(GetSupportDataFormat(desc.format) & SF_TEXTURE))
    {
        GL_LOG_ERROR << "Unsupport data format, data format is: " << desc.format;
        return nullptr;
    }
    D3D11Texture::ptr tex = std::make_shared<D3D11Texture>(desc);
    if ((!(GetSupportDataFormat(desc.format) && SF_AUTOGEN_MIPS) && desc.generateMips))
    {
        generateMips = false;
    }
    if (tex->Create(_context, _device, desc, generateMips))
    {
        return tex;
    }
    else
    {
        assert(false);
        return nullptr;
    }
}

FrameBuffer::ptr D3D11DrawContex::CreateFrameBuffer(const FrameBufferDesc& desc)
{
    HRESULT hr = S_OK;
    D3D11FrameBuffer::ptr fb;
    if (desc.colorTexs.empty())
    {
        fb = std::make_shared<D3D11FrameBuffer>(desc.width, desc.height);
    }
    else
    {
        std::vector<D3D11Texture::ptr> d3d11ColorTexs;
        for (auto colorTex :  desc.colorTexs)
        {
            d3d11ColorTexs.push_back(std::dynamic_pointer_cast<D3D11Texture>(colorTex));
        }
        fb = std::make_shared<D3D11FrameBuffer>(desc.width, desc.height, d3d11ColorTexs);
    }

    return fb->Create(_context, _device, desc.zStencil) ? fb : nullptr; 
}

void D3D11DrawContex::UpdateTexture(Texture::ptr tex, TextureDesc desc)
{
    std::lock_guard<std::mutex> lock(_taskMtx);
    _tasksQueue.push_back([this, tex, desc]() -> GpuTaskStatus
    {
        D3D11Texture::ptr texture = std::dynamic_pointer_cast<D3D11Texture>(tex);
        texture->Update(_context, _device, desc);
        return GpuTaskStatus::NORMAL;
    });
    _taskCond.notify_all();
}

void D3D11DrawContex::UpdateBuffer(GLBuffer::ptr buffer, RawData::ptr data, size_t offset, size_t size)
{
    std::lock_guard<std::mutex> lock(_taskMtx);
    _tasksQueue.push_back([this, buffer, data, offset, size]() -> GpuTaskStatus
    {
        D3D11Buffer::ptr buf = std::dynamic_pointer_cast<D3D11Buffer>(buffer);
        assert(buffer);
#if 0
        // TO FIX : properly to use `UpdateSubresource`
        if (offset == 0 && size == buf->size)
        {
            // Hint : useful for content that changes more than once per frame. In general, GPU reads will be slower, but CPU updates will be faster.
            D3D11_MAPPED_SUBRESOURCE map = {};
            _context->Map(buf->buf, 0, D3D11_MAP_WRITE_DISCARD, 0, &map);
            memcpy(map.pData, data->GetData(), size);
            _context->Unmap(buf->buf, 0);
        }
        else
        {
            // Hint :  useful for content that changes less than once per frame. In general, GPU reads will be fast and CPU updates will be slower.
            D3D11_BOX box = {};
            box.left = (UINT)offset;
            box.right = (UINT)(offset + size);
            box.bottom = 1;
            box.back = 1;
            _context->UpdateSubresource(buf->buf, 0, &box, data->GetData(), 0, 0);
        }
#else
        D3D11_MAPPED_SUBRESOURCE map = {};
        _context->Map(buf->buf, 0, D3D11_MAP_WRITE_DISCARD, 0, &map);
        memcpy(map.pData, data->GetData(), size);
        _context->Unmap(buf->buf, 0);
#endif
        return GpuTaskStatus::NORMAL;
    });
    _taskCond.notify_all();
}

void D3D11DrawContex::CopyFrameBufferImage(FrameBuffer::ptr src, int level, int x, int y, int z, 
                        FrameBuffer::ptr dst, int dstLevel, int dstX, int dstY, int dstZ,
                        int width, int height, int depth, int channelBits, const std::string& tag
                        )
{
    D3D11FrameBuffer::ptr srcFb = std::dynamic_pointer_cast<D3D11FrameBuffer>(src);
    D3D11FrameBuffer::ptr dstFb = std::dynamic_pointer_cast<D3D11FrameBuffer>(dst);

    ID3D11Texture2D* srcTex = nullptr;
    ID3D11Texture2D* dstTex = nullptr;
    if (channelBits & FBChannel::FB_COLOR_BIT)
    {
        // Hint : nou support MRT for now
        // Todo : support it if necessary
        srcTex = srcFb->colorTexs[0]->tex;
        dstTex = dstFb->colorTexs[0]->tex;
    }
    else if (channelBits & FBChannel::FB_DEPTH_BIT)
    {
        srcTex = srcFb->depthStencilTex;
        dstTex = dstFb->depthStencilTex;
    }
    else
    {
        // TO CHECK
        assert(false);
        return;
    }
    assert(srcTex && dstTex);

    /* Full Copy */
    if (width == srcFb->Width() && height == srcFb->Height() &&
        width == dstFb->Width() && height == dstFb->Height() &&
        x == 0 && y == 0 && z == 0 &&
        dstX == 0 && dstY == 0 && dstZ == 0
    )
    {
        _context->CopyResource(dstTex, srcTex);
    }
    /* Not full copy (only support to color channel) */
    else if (channelBits & FBChannel::FB_COLOR_BIT)
    {
        if (x < 0)
        {
            width += x;
            dstX -= x;
        }
        if (y < 0)
        {
            height += y;
            dstY -= y;
        }
        if (x + width > srcFb->Width())
        {
            width = srcFb->Width() - x;
        }
        if (y + height > src->Height())
        {
            height = srcFb->Height() - y;
        }
        D3D11_BOX srcBox = {};
        {
            srcBox.left = (UINT)x;
            srcBox.top = (UINT)y;
            srcBox.front = (UINT)z;
            srcBox.right = (UINT)(x + width);
            srcBox.bottom = (UINT)(y + height);
            srcBox.back = (UINT)(z + depth);
        }
        _context->CopySubresourceRegion(dstTex, dstLevel, dstX, dstY, dstZ, srcTex, level, &srcBox);
    }
    else
    {
        // TODO
    }

}

bool D3D11DrawContex::BlitFrameBuffer(FrameBuffer::ptr src, int srcX1, int srcY1, int srcX2, int srcY2,
                        FrameBuffer::ptr dst, int dstX1, int dstY1, int dstX2, int dstY2,
                        int channelBits, FBBlitFilter filter, const std::string& tag
                    )
{
    assert(false);
    return false;
}

bool D3D11DrawContex::CopyFramebufferToMemory(FrameBuffer::ptr src, int channelBits, int x, int y, int w, int h,  GLPixelData::ptr pixel)
{
    std::unique_lock<std::mutex> syncLock(_syncMtx);
    _syncDone = false;

    {
        std::lock_guard<std::mutex> lock(_taskMtx);
        _tasksQueue.push_back([this, src, channelBits, x, y, w, h, pixel]() -> GpuTaskStatus
        {
            D3D11FrameBuffer::ptr fb = std::dynamic_pointer_cast<D3D11FrameBuffer>(src);
            // TODO : 
            // 1 - support copy from default framebuffer
            // 2 - support copy stencil and depth channel
            assert(fb);
            int bx, by, bw, bh;
            {
                bx = x;
                by = y;
                bw = w;
                bh = h;
                if (bx + bw > fb->Width())
                {
                    bw -= (bx + bw) - fb->Width();
                }
                if (by + bh > fb->Height())
                {
                    bh -= (by + bh) - fb->Height();
                }
                if (bw<=0 || bh<=0)
                {
                    return GpuTaskStatus::NORMAL;
                }
            }

            switch (channelBits)
            {
                case FB_COLOR_BIT:
                    // Hint : not support MRT for now
                    // Todo : support it if necessary
                    fb->colorTexs[0]->ReadBack(this->_context, this->_device, bx, by, bw, bh, (uint8_t*)pixel->GetData());
                    break;
                case FB_DEPTH_BIT:
                case FB_STENCIL_BIT:
                    // Hint : not support for now
                    return GpuTaskStatus::NORMAL;
                default:
                    assert(false);
                    return GpuTaskStatus::NORMAL;
            }
            this->_syncDone = true;
            this->_syncCond.notify_all();
            return GpuTaskStatus::NORMAL;
        });
        _taskCond.notify_all();
    }

    _syncCond.wait(syncLock);
    return _syncDone;
}

bool D3D11DrawContex::CopyTextureToMemory(const std::vector<Texture::ptr>& srcs, const std::vector<GLRect2D>& rects, SharedData::ptr picture)
{
    std::unique_lock<std::mutex> syncLock(_syncMtx);
    _syncDone = false;
    {
        std::lock_guard<std::mutex> lock(_taskMtx);
        _tasksQueue.push_back([this, srcs, rects, picture]() -> GpuTaskStatus
        {
            size_t count = srcs.size();
            uint64_t offset = 0;
            for (size_t i=0; i< count; i++)
            {
                D3D11Texture::ptr tex = std::dynamic_pointer_cast<D3D11Texture>(srcs[i]);
                tex->ReadBack(this->_context, this->_device, rects[i].x, rects[i].y, rects[i].w, rects[i].h, (uint8_t*)picture->GetData(offset));
                offset += (rects[i].w - rects[i].x) * (rects[i].h - rects[i].y) * DxgiFormatSize(DataFormatToD3D11(tex->format));
            }
            this->_syncDone = true;
            this->_syncCond.notify_all();
            return GpuTaskStatus::NORMAL;
        });
        _taskCond.notify_all();
    }
    _syncCond.wait(syncLock);
    return _syncDone;
}

SL::ShaderLanguage D3D11DrawContex::GetShaderLanguage()
{
    return SL::ShaderLanguage::HLSL_D3D11;
}

DataFormat D3D11DrawContex::FrameBufferReadBackFormat(FBChannel channel)
{
    // TODO
    assert(false);
    return DataFormat::UNDEFINED;
}

void D3D11DrawContex::GetDefaultVorldViewProj(int width, int height, float worldViewProj[16], bool /* onScreen */)
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

void D3D11DrawContex::Draw(int vertexCount, int offset)
{
    std::lock_guard<std::mutex> lock(_taskMtx);
    _tasksQueue.push_back([this, vertexCount, offset]() -> GpuTaskStatus
    {
        ApplyCurrentState();
        _context->Draw(vertexCount, offset);
        if (_curRenderTargetViews[0] == _bbRenderTargetView)
        {
            return GpuTaskStatus::PRESENT;
        }
        else
        {
            return GpuTaskStatus::NORMAL;
        }
    });
    _taskCond.notify_all();
}

void D3D11DrawContex::DrawIndexed(int vertexCount, int offset)
{
    std::lock_guard<std::mutex> lock(_taskMtx);
    _tasksQueue.push_back([this, vertexCount, offset]() -> GpuTaskStatus
    {
        ApplyCurrentState();
        _context->DrawIndexed(vertexCount, offset, 0);
        if (_curRenderTargetViews[0] == _bbRenderTargetView)
        {
            return GpuTaskStatus::PRESENT;
        }
        else
        {
            return GpuTaskStatus::NORMAL;
        }
    });
    _taskCond.notify_all();
}

void D3D11DrawContex::SetScissorRect(int left, int top, int width, int height)
{
    std::lock_guard<std::mutex> lock(_taskMtx);
    _tasksQueue.push_back([this, left, top, width, height]() -> GpuTaskStatus
    {
        assert(width >= 0);
        assert(height >= 0);
        D3D11_RECT rc = {};
        rc.left = (UINT)left;
        rc.top = (UINT)top;
        rc.right = (UINT)(left + width);
        rc.bottom = (UINT)(top + height);
        _context->RSSetScissorRects(1, &rc);
        return GpuTaskStatus::NORMAL;
    });
    _taskCond.notify_all();
}

void D3D11DrawContex::SetFrameBufferSize(int width, int height)
{
    std::lock_guard<std::mutex> lock(_taskMtx);
    _tasksQueue.push_back([this, width, height]() -> GpuTaskStatus
    {
        _curRTWidth = width;
        _curRTHeight = height;
        return GpuTaskStatus::NORMAL;
    });
    _taskCond.notify_all();

}

void D3D11DrawContex::SetViewport(const Viewport& viewport)
{
    std::lock_guard<std::mutex> lock(_taskMtx);
    _tasksQueue.push_back([this, viewport]() -> GpuTaskStatus
    {
        D3D11_VIEWPORT vp = {};
        vp.TopLeftX = viewport.TopLeftX;
        vp.TopLeftY = viewport.TopLeftY;
        vp.Width = viewport.Width;
        vp.Height = viewport.Height;
        vp.MinDepth = viewport.MinDepth;
        vp.MaxDepth = viewport.MaxDepth;
        _context->RSSetViewports(1, &vp);
        return GpuTaskStatus::NORMAL;
    });
    _taskCond.notify_all();
}

void D3D11DrawContex::SetBlendFactor(float color[4])
{
    std::lock_guard<std::mutex> lock(_taskMtx);
    std::vector<float> _color;
    _color.resize(4);
    for (int i=0; i<4; i++)
    {
        _color[i] = color[i];
    }
    _tasksQueue.push_back([this, _color]() -> GpuTaskStatus
    {
        if (memcmp(_blendFactor, _color.data(), sizeof(float) * 4))
        {
            for (int i=0; i<4; i++)
            {
                _blendFactor[i] = _color[i];
            }
            _blendFactorDirty = true;
        }
        return GpuTaskStatus::NORMAL;
    });
    _taskCond.notify_all();


}

void D3D11DrawContex::SetStencilParams(uint8_t refValue, uint8_t writeMask, uint8_t compareMask)
{
    std::lock_guard<std::mutex> lock(_taskMtx);
    _tasksQueue.push_back([this, refValue, writeMask, compareMask]() -> GpuTaskStatus
    {
        _stencilRef = refValue;
        _stencilWriteMask = writeMask;
        _stencilCompareMask = compareMask;
        _stencilDirty = true;
        return GpuTaskStatus::NORMAL;
    });
    _taskCond.notify_all();

}

void D3D11DrawContex::BindSamplerStates(int start, std::vector<SamplerState::ptr> states)
{
    std::lock_guard<std::mutex> lock(_taskMtx);
    _tasksQueue.push_back([this, start, states]() -> GpuTaskStatus
    {
        ID3D11SamplerState* samplers[D3D11_MAX_BOUND_TEXTURES];
        size_t count = states.size();
        assert(start + count <= D3D11_MAX_BOUND_TEXTURES);
        for (size_t i=0; i<count; i++)
        {
            D3D11SamplerState::ptr ss = std::dynamic_pointer_cast<D3D11SamplerState>(states[i]);
            samplers[i] = ss ? ss->ss : nullptr;
        }
        _context->PSSetSamplers(start, (UINT)count, samplers);
        return GpuTaskStatus::NORMAL;
    });
    _taskCond.notify_all();
}

void D3D11DrawContex::BindTextures(int start, std::vector<Texture::ptr> textures)
{
    std::lock_guard<std::mutex> lock(_taskMtx);
    _tasksQueue.push_back([this, start, textures]() -> GpuTaskStatus
    {
        ID3D11ShaderResourceView* views[D3D11_MAX_BOUND_TEXTURES];
        size_t count = textures.size();
        assert(start + count <= D3D11_MAX_BOUND_TEXTURES);
        for (size_t i=0; i<count; i++)
        {
            D3D11Texture::ptr tex = std::dynamic_pointer_cast<D3D11Texture>(textures[i]);
            views[i] = tex ? tex->view : nullptr;
        }
        _context->PSSetShaderResources(start, (UINT)count, views);
        return GpuTaskStatus::NORMAL;
    });
    _taskCond.notify_all();
}

void D3D11DrawContex::BindVertexBuffers(int start, const std::vector<GLBuffer::ptr>& buffers, const std::vector<int>& offsets)
{
    std::lock_guard<std::mutex> lock(_taskMtx);
    _tasksQueue.push_back([this, start, buffers, offsets]() -> GpuTaskStatus
    {
        size_t count = buffers.size();
        assert(buffers.size() == offsets.size());
        assert(count <= D3D11_MAX_VERTEX_BUFFER_SIZE);
        for (size_t i=0; i<count; i++)
        {
            D3D11Buffer::ptr buf = std::dynamic_pointer_cast<D3D11Buffer>(buffers[i]);
            _nextVertexBuffers[start + i] = buf->buf;
            _nextVertexBufferOffsets[start + 1] = offsets[i];
        }
        return GpuTaskStatus::NORMAL;
    });
    _taskCond.notify_all();
}

void D3D11DrawContex::BindIndexBuffer(GLBuffer::ptr indexBuffer, int offset)
{
    std::lock_guard<std::mutex> lock(_taskMtx);
    _tasksQueue.push_back([this, indexBuffer, offset]() -> GpuTaskStatus
    {
        D3D11Buffer::ptr buf = std::dynamic_pointer_cast<D3D11Buffer>(indexBuffer);
        _nextIndexBuffer = buf->buf;
        _nextIndexBufferOffset = 0;
        _indexBufferDirty = true;
        return GpuTaskStatus::NORMAL;
    });
    _taskCond.notify_all();
}

void D3D11DrawContex::BindFramebufferAsRenderTarget(FrameBuffer::ptr fbo, const RenderPassInfo& rp)
{
    std::lock_guard<std::mutex> lock(_taskMtx);
    _tasksQueue.push_back([this, fbo, rp]() -> GpuTaskStatus
    {
        if (fbo)
        {
            D3D11FrameBuffer::ptr fb = std::dynamic_pointer_cast<D3D11FrameBuffer>(fbo);
            bool isSame = true;
            if (fb->colorTexs.size() != _curRenderTargetViews.size())
            {
                isSame = false;
            }
            else if (fb->depthStencilRTView && fb->depthStencilRTView != _curDepthStencilView)
            {
                isSame = false;
            }
            else
            {
                size_t viewNum = _curRenderTargetViews.size();
                for (size_t i=0; i<viewNum; i++)
                {
                    if (_curRenderTargetViews[i] != fb->colorTexs[i]->colorRTView)
                    {
                        isSame = false;
                    }
                }
            }

            if (isSame)
            {
                // Hint : nothing to do
            }
            else
            {
                ID3D11ShaderResourceView* empty[8] = {};
                _context->PSSetShaderResources(0, 8, empty);
                {
                    size_t viewNum = fb->colorTexs.size();
                    _curRenderTargetViews.resize(viewNum);
                    for (size_t i=0; i<viewNum; i++)
                    {
                        _curRenderTargetViews[i] = fb->colorTexs[i]->colorRTView;
                    }
                }
                _context->OMSetRenderTargets((UINT)_curRenderTargetViews.size(), _curRenderTargetViews.data(), fb->depthStencilRTView);
                _curDepthStencilView = fb->depthStencilRTView ? fb->depthStencilRTView : _bbDepthStencilView;
                _curRTWidth = fb->Width();
                _curRTHeight = fb->Height();
            }
            _curRenderTarget = fb;
        }
        else
        {
            if (_curRenderTargetViews[0] == _bbRenderTargetView && _curDepthStencilView == _bbDepthStencilView)
            {
                // Hint : nothing to do
            }
            else
            {
                _curRenderTargetViews.resize(1);
                _context->OMSetRenderTargets(1, &_bbRenderTargetView, _bbDepthStencilView);
                _curRenderTargetViews[0] = _bbRenderTargetView;
                _curDepthStencilView = _bbDepthStencilView;
                _curRTWidth = _bbWidth;
                _curRTHeight = _bbHeight;
            }
        }
        if (rp.color == RPAction::CLEAR && !_curRenderTargetViews.empty())
        {
            float cv[4] = {};
            if (rp.clearColor)
            {
                Uint8x4ToFloat4(rp.clearColor, cv);
            }
            for (auto& curRenderTargetView : _curRenderTargetViews)
            {
                _context->ClearRenderTargetView(curRenderTargetView, cv);
            }
        }
        int mask = 0;
        if (rp.depth == RPAction::CLEAR)
        {
            mask |= D3D11_CLEAR_DEPTH;
        }
        if (rp.stencil == RPAction::CLEAR)
        {
            mask |= D3D11_CLEAR_STENCIL;
        }
        if (mask && _curDepthStencilView)
        {
            _context->ClearDepthStencilView(_curDepthStencilView, mask, rp.clearDepth, rp.clearStencil);
        }
        return GpuTaskStatus::NORMAL;
    });
    _taskCond.notify_all();

}

void D3D11DrawContex::BindFramebufferAsTexture(FrameBuffer::ptr fbo, int binding, FBChannel channelBits, int layer)
{
    std::lock_guard<std::mutex> lock(_taskMtx);
    _tasksQueue.push_back([this, fbo, binding, channelBits, layer]() -> GpuTaskStatus
    {
        assert(binding < D3D11_MAX_BOUND_TEXTURES);
        assert(layer == 0);
        D3D11FrameBuffer::ptr fb = std::dynamic_pointer_cast<D3D11FrameBuffer>(fbo);
        switch (channelBits)
        {
            case FBChannel::FB_COLOR_BIT:
            {
                size_t numView = fb->colorTexs.size();
                std::vector<ID3D11ShaderResourceView*> colorSRViews;
                colorSRViews.resize(numView);
                for (size_t i=0; i<numView; i++)
                {
                    colorSRViews[i] = fb->colorTexs[i]->colorSRView;
                }
                _context->PSSetShaderResources(binding, (UINT)colorSRViews.size(), colorSRViews.data());
                break;
            }
            case FBChannel::FB_DEPTH_BIT:
            {
                _context->PSSetShaderResources(binding, 1, &fb->depthSRView);
            }
            default:
            {
                assert(false);
                break;
            }
        }
        return GpuTaskStatus::NORMAL;
    });
    _taskCond.notify_all();
}

void D3D11DrawContex::BindPipeline(Pipeline::ptr pipeline)
{
    std::lock_guard<std::mutex> lock(_taskMtx);
    _tasksQueue.push_back([this, pipeline]() -> GpuTaskStatus
    {
        D3D11Pipeline::ptr pp = std::dynamic_pointer_cast<D3D11Pipeline>(pipeline);
        if (_curPipeline == pp)
            return GpuTaskStatus::NORMAL;;
        _curPipeline = pp;
        return GpuTaskStatus::NORMAL;
    });
    _taskCond.notify_all();
}

void D3D11DrawContex::UpdataUniformBuffer(RawData::ptr uniformBuffer, size_t size)
{
    std::lock_guard<std::mutex> lock(_taskMtx);
    _tasksQueue.push_back([this, uniformBuffer, size]() -> GpuTaskStatus
    {
        if (_curPipeline->dynamicUniformsSize != size)
        {
            assert(false);
            return GpuTaskStatus::NORMAL;
        }
        D3D11_MAPPED_SUBRESOURCE map = {};
        _context->Map(_curPipeline->dynamicUniforms, 0, D3D11_MAP_WRITE_DISCARD, 0, &map);
        memcpy(map.pData, uniformBuffer->GetData(), uniformBuffer->GetSize());
        _context->Unmap(_curPipeline->dynamicUniforms, 0);
        return GpuTaskStatus::NORMAL;
    });
    _taskCond.notify_all();
}

uint16_t D3D11DrawContex::GetSupportDataFormat(DataFormat format)
{
    DXGI_FORMAT giFmt = DataFormatToD3D11(format);
    if (giFmt == DXGI_FORMAT_UNKNOWN)
    {
        return 0;
    }
    UINT giSupport = 0;
    if (FAILED(_device->CheckFormatSupport(giFmt, &giSupport)))
    {
        return 0;
    }
    uint32_t support = 0;
    if (giSupport & D3D11_FORMAT_SUPPORT_TEXTURE2D)
    {
        support |= SF_TEXTURE;
    }
    if (giSupport & D3D11_FORMAT_SUPPORT_RENDER_TARGET)
    {
        support |= SF_RENDERTARGET;
    }
    if (giSupport & D3D11_FORMAT_SUPPORT_IA_VERTEX_BUFFER)
    {
        support |= SF_INPUTLAYOUT;
    }
    if (giSupport & D3D11_FORMAT_SUPPORT_DEPTH_STENCIL)
    {
        support |= SF_DEPTHSTENCIL;
    }
    if (giSupport & D3D11_FORMAT_SUPPORT_MIP_AUTOGEN)
    {
        support |= SF_AUTOGEN_MIPS;
    }
    return support;
}

ID3D11DepthStencilState* D3D11DrawContex::GetCachedDepthStencilState(D3D11DepthStencilState::ptr state, uint8_t stencilWriteMask, uint8_t stencilCompareMask)
{
    D3D11DepthStencilKey key;
    key.desc = state->desc;
    key.writeMask = stencilWriteMask;
    key.compareMask = stencilCompareMask;
    if (_depthStencilCache.count(key) == 0)
    {
        D3D11_DEPTH_STENCIL_DESC d3ddesc{};
        d3ddesc.DepthEnable = state->desc.depthTestEnabled ? TRUE : FALSE;
        d3ddesc.DepthWriteMask = state->desc.depthWriteEnabled ? D3D11_DEPTH_WRITE_MASK_ALL : D3D11_DEPTH_WRITE_MASK_ZERO;
        d3ddesc.DepthFunc = GLComparisonTypeToD3D11Type(state->desc.depthCompare);
        d3ddesc.StencilEnable = state->desc.stencilEnabled ? TRUE : FALSE;
        d3ddesc.StencilReadMask = stencilCompareMask;
        d3ddesc.StencilWriteMask = stencilWriteMask;
        if (d3ddesc.StencilEnable)
        {
            d3ddesc.FrontFace.StencilFunc = GLComparisonTypeToD3D11Type(state->desc.stencil.compareOp);
            d3ddesc.FrontFace.StencilDepthFailOp = GLStencilOpToD3D11Type(state->desc.stencil.depthFailOp);
            d3ddesc.FrontFace.StencilFailOp = GLStencilOpToD3D11Type(state->desc.stencil.failOp);
            d3ddesc.FrontFace.StencilPassOp = GLStencilOpToD3D11Type(state->desc.stencil.passOp);
            d3ddesc.BackFace.StencilFunc = GLComparisonTypeToD3D11Type(state->desc.stencil.compareOp);
            d3ddesc.BackFace.StencilDepthFailOp = GLStencilOpToD3D11Type(state->desc.stencil.depthFailOp);
            d3ddesc.BackFace.StencilFailOp = GLStencilOpToD3D11Type(state->desc.stencil.failOp);
            d3ddesc.BackFace.StencilPassOp = GLStencilOpToD3D11Type(state->desc.stencil.passOp);
        }
        ID3D11DepthStencilState* dss = nullptr;
        if (SUCCEEDED(_device->CreateDepthStencilState(&d3ddesc, &dss)))
        {
            _depthStencilCache[key] = dss;
            return dss;
        }
        else
        {
            assert(false);
            return nullptr;
        }
    }
    else
    {
        return _depthStencilCache[key];
    }
}

void D3D11DrawContex::ApplyCurrentState()
{
    assert(!_curRenderTargetViews.empty());
    _context->OMSetRenderTargets((UINT)_curRenderTargetViews.size(), _curRenderTargetViews.data(), _curDepthStencilView);
    if (_curBlend != _curPipeline->blend || _blendFactorDirty)
    {
        _context->OMSetBlendState(_curPipeline->blend->bs, _blendFactor, 0xFFFFFFFF);
        _curBlend = _curPipeline->blend;
        _blendFactorDirty = false;
    }
    if (_curDepthStencil != _curPipeline->depthStencil || _stencilDirty)
    {
        ID3D11DepthStencilState* dss = GetCachedDepthStencilState(_curPipeline->depthStencil, _stencilWriteMask, _stencilCompareMask);
        _context->OMSetDepthStencilState(dss, _stencilRef);
        _curDepthStencil = _curPipeline->depthStencil;
        _stencilDirty = false;
    }
    if (_curRaster != _curPipeline->raster)
    {
        _context->RSSetState(_curPipeline->raster->rs);
        _curRaster = _curPipeline->raster;
    }
    if (_curInputLayout != _curPipeline->il)
    {
        _context->IASetInputLayout(_curPipeline->il);
        _curInputLayout = _curPipeline->il;
    }
    if (_curVS != _curPipeline->vs)
    {
        _context->VSSetShader(_curPipeline->vs, nullptr, 0);
        _curVS = _curPipeline->vs;
    }
    if (_curPS != _curPipeline->ps)
    {
        _context->PSSetShader(_curPipeline->ps, nullptr, 0);
        _curPS = _curPipeline->ps;
    }
    if (_curGS != _curPipeline->gs)
    {
        _context->GSSetShader(_curPipeline->gs, nullptr, 0);
        _curGS = _curPipeline->gs;
    }
    if (_curTopology != _curPipeline->topology)
    {
        _context->IASetPrimitiveTopology(_curPipeline->topology);
        _curTopology = _curPipeline->topology;
    }
    if (_curPipeline->input)
    {
        int numVBs = (int)_curPipeline->input->strides.size();
        _context->IASetVertexBuffers(0, numVBs, _nextVertexBuffers, (UINT *)_curPipeline->input->strides.data(), (UINT *)_nextVertexBufferOffsets);
    }
    if (_indexBufferDirty)
    {
        _context->IASetIndexBuffer(_nextIndexBuffer, DXGI_FORMAT_R16_UINT, _nextIndexBufferOffset);
        _indexBufferDirty = false;
    }
    if (_curPipeline->dynamicUniforms)
    {
        _context->VSSetConstantBuffers(0, 1, &_curPipeline->dynamicUniforms);
        _context->PSSetConstantBuffers(0, 1, &_curPipeline->dynamicUniforms);
    }
}

} // namespace Mmp