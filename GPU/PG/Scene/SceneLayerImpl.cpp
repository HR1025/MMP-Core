#include "SceneLayerImpl.h"

#include <cassert>
#include <mutex>
#include <memory>
#include <string>
#include <vector>
#include <algorithm>

#include "Common/AllocateMethodFactory.h"
#include "Common/ImmutableVectorAllocateMethod.h"
#include "GPU/GL/GLContex.h"
#include "GPU/GL/GLDrawContex.h"

#include "SceneItemImpl.h"

namespace Mmp 
{
namespace Gpu 
{

class SceneLayerVertex
{
public:
    SceneLayerVertex(float x, float y, float u, float v);
public:
    float x, y;  // attribute vec2 Position
    float u, v;  // attribute vec2 iUv
};

class SceneLayerUniforms
{
public:
    SceneLayerUniforms();
public:
    float worldViewProj[16] = {0.0f};
    float transparency;
};

SceneLayerVertex::SceneLayerVertex(float x, float y, float u, float v)
{
    this->x = x;
    this->y = y;
    this->u = u;
    this->v = v;
}

SceneLayerUniforms::SceneLayerUniforms()
{
    transparency = 1.0f;
}

class SceneLayerProgram
{
public:
    using ptr = std::shared_ptr<SceneLayerProgram>;
public:
    static SceneLayerProgram::ptr Create();
public:
    SceneLayerProgram();
    ~SceneLayerProgram();
public:
    void Begin();
    void End();
    void ClearCanvas(Texture::ptr canvas);
    void UpdateContext(float transparency);
    void Draw(Texture::ptr canvas, Texture::ptr framebuffer);
private:
    static const std::string GetVertexShader();
    static const std::string GetFragmentShader();
    static std::vector<AttributeDesc> GetAttributeDescs();
    Pipeline::ptr CreatePipeline();
    void InitUniforms();
    void InitVertexData();
private:
    std::mutex                      _mtx;
    GLDrawContex::ptr               _draw;
    GLBuffer::ptr                   _vbo;
    Pipeline::ptr                   _pipeLine;
    std::vector<SamplerState::ptr>  _samples;
    RawData::ptr                    _uniformsRawData;
    SceneLayerUniforms*              _uniforms;
    RawData::ptr                    _vertexsRawData;
};

SceneLayerProgram::ptr SceneLayerProgram::Create()
{
    static std::mutex gMtx;
    static std::weak_ptr<SceneLayerProgram> gCacheInstance;
    SceneLayerProgram::ptr instace = nullptr;
    {
        std::lock_guard<std::mutex> lock(gMtx);
        instace = gCacheInstance.lock();
        if (!instace)
        {
            instace = std::make_shared<SceneLayerProgram>();
            gCacheInstance = instace;
        }
    }
    return instace;
}

SceneLayerProgram::SceneLayerProgram()
{
    _draw = GLDrawContex::Instance();
    InitUniforms();
    InitVertexData();
    _vbo = _draw->CreateBuffer(sizeof(SceneLayerVertex)*6, BufferUsageFlag::DYNAMIC | BufferUsageFlag::VERTEXDATA);
    _pipeLine = CreatePipeline();
    // TODO : 是否需要暴露采样模式, 临近采样 or 双线性采样
    {
        SamplerStateDesc desc;
        desc.magFilter             = TextureFilter::LINEAR;
        desc.minFilter             = TextureFilter::LINEAR;
        desc.mipFilter             = TextureFilter::LINEAR;
        desc.maxAniso              = 0.0f;
        desc.wrapU                 = TextureAddressMode::CLAMP_TO_EDGE;
        desc.wrapV                 = TextureAddressMode::CLAMP_TO_EDGE;
        desc.wrapW                 = TextureAddressMode::CLAMP_TO_EDGE;
        desc.shadowCompareEnabled  = false;
        desc.shadowCompareFunc     = Comparison::ALWAYS;
        desc.borderColor           = BorderColor::DONT_CARE;
        SamplerState::ptr sample   = _draw->CreateSamplerState(desc);
        _samples.push_back(sample);
    }
}

SceneLayerProgram::~SceneLayerProgram()
{

}

void SceneLayerProgram::Begin()
{
    _mtx.lock();
}

void SceneLayerProgram::End()
{
    _mtx.unlock();
}

void SceneLayerProgram::ClearCanvas(Texture::ptr canvas)
{
    _draw->FenceBegin();
    FrameBuffer::ptr fbo;
    {
        FrameBufferDesc desc;
        desc.width  = canvas->Width();
        desc.height = canvas->Height();
        desc.depth  = 0;
        desc.multiSampleLevel = 0;
        desc.numLayers = 1;
        desc.zStencil = 0;
        desc.colorTexs.push_back(canvas);
        desc.tag = "SceneLayerCanvas"; 
        fbo = _draw->CreateFrameBuffer(desc);
    }
    {
        RenderPassInfo info;
        info.color      = RPAction::CLEAR;
        info.depth      = RPAction::CLEAR;
        info.stencil    = RPAction::CLEAR; 
        info.clearColor = 0x00000000;
        _draw->BindFramebufferAsRenderTarget(fbo, info);
    }
    _draw->FenceCommit();
    _draw->FenceEnd();
}

void SceneLayerProgram::UpdateContext(float transparency)
{
    _uniforms->transparency = transparency;
}

void SceneLayerProgram::Draw(Texture::ptr canvas, Texture::ptr framebuffer)
{
    FrameBuffer::ptr fbo;
    {
        FrameBufferDesc desc;
        desc.width  = framebuffer->Width();
        desc.height = framebuffer->Height();
        desc.depth  = 0;
        desc.multiSampleLevel = 0;
        desc.numLayers = 1;
        desc.zStencil = 0;
        desc.colorTexs.push_back(framebuffer);
        desc.tag = "SceneItemFrameBuffer"; 
        fbo = _draw->CreateFrameBuffer(desc);
    }
    _draw->FenceBegin();
    {
        RenderPassInfo info;
        info.color      = RPAction::KEEP;
        info.depth      = RPAction::KEEP;
        info.stencil    = RPAction::KEEP; 
        info.clearColor = 0x00000000;
        _draw->BindFramebufferAsRenderTarget(fbo, info);
    }
    _draw->SetScissorRect(0, 0, framebuffer->Width(), framebuffer->Height());
    {
        Viewport vp;
        vp.TopLeftX  = 0;
        vp.TopLeftY  = 0;
        vp.Width     = (float)framebuffer->Width();
        vp.Height    = (float)framebuffer->Height();
        vp.MaxDepth  = 1.0;
        vp.MinDepth  = 0.0;
        _draw->SetViewport(vp);
    }
    _draw->BindSamplerStates(0, _samples);
    {
        std::vector<Texture::ptr> textures;
        textures.push_back(canvas);
        _draw->BindTextures(0, textures);
    }
    {
        _draw->BindPipeline(_pipeLine);
        _draw->UpdataUniformBuffer(_uniformsRawData, 2);
        // Update Vertex Buffer Object
        {
            _draw->UpdateBuffer(_vbo, _vertexsRawData, 0, _vertexsRawData->GetSize()); 
        }
        // Bind Vertex Buffer Object
        {
            std::vector<GLBuffer::ptr> vbos;
            vbos.push_back(_vbo);
            std::vector<int> offsets;
            offsets.push_back(0);
            _draw->BindVertexBuffers(0, vbos, offsets);
        }
        _draw->Draw(6, 0);
    }
    _draw->FenceCommit();
    _draw->FenceEnd();
}

const std::string SceneLayerProgram::GetVertexShader()
{
    std::string source;

    switch (GLDrawContex::Instance()->GetShaderLanguage())
    {
        case SL::ShaderLanguage::ELSL_3xx:
        case SL::ShaderLanguage::GLSL_4xx:
        {
            source = 
R"(
attribute vec2 Position;
attribute vec2 iUV;
varying   vec2 oUV;
uniform mat4 WorldViewProj;

void main(void)
{
    oUV         =  iUV;
    gl_Position =  WorldViewProj * vec4(Position, 1.0, 1.0);
}
)";
            break;
        }
        case SL::ShaderLanguage::HLSL_D3D11:
        {
            source = 
R"(
struct VS_INPUT 
{
    vec2 Position : POSITION;
    vec2 iUV      : TEXCOORD0;
};

struct VS_OUTPUT
{
    vec2 oUV      : TEXCOORD0;
    vec4 Position : SV_Position;
};

cbuffer ConstantBuffer : register(b0) 
{
    matrix WorldViewProj;
    float Progress;
};

VS_OUTPUT main(VS_INPUT input)
{
    VS_OUTPUT output;
    output.Position = mul(WorldViewProj, vec4(input.Position, 1.0, 1.0));
    output.oUV      = input.iUV;
    return output;
}
)";
            break;
        }
        default:
            assert(false);
            break;
    }

    return source;
}

const std::string SceneLayerProgram::GetFragmentShader()
{
    std::string source;
    switch (GLDrawContex::Instance()->GetShaderLanguage())
    {
        case SL::ShaderLanguage::ELSL_3xx:
        case SL::ShaderLanguage::GLSL_4xx:
        {
            source = 
R"(
varying vec2 oUV;

uniform sampler2D SceneLayer;

uniform float Transparency;

vec4 getSceneLayerColor(vec2 uv)
{
    vec4 color = texture(SceneLayer, oUV);
    color.a = color.a * Transparency;
    return color;
}

void main(void)
{
    gl_FragColor = getSceneLayerColor(oUV);
}
)";
            break;
        }
        case SL::ShaderLanguage::HLSL_D3D11:
        {
            // TODO
            assert(false);
            break;
        }
        default:
            assert(false);
    }
    return source;
}

std::vector<AttributeDesc> SceneLayerProgram::GetAttributeDescs()
{
    std::vector<AttributeDesc> attributes;
    switch (GLDrawContex::Instance()->GetShaderLanguage())
    {
        case SL::ShaderLanguage::ELSL_3xx:
        case SL::ShaderLanguage::GLSL_4xx:
        {
            attributes.push_back(AttributeDesc("Position", 0, 0, DataFormat::R32G32_FLOAT, 0 /* ~ 7 */));
            attributes.push_back(AttributeDesc("iUV", 0, 1, DataFormat::R32G32_FLOAT, 8 /* ~ 15 */));
            break;
        }
        case SL::ShaderLanguage::HLSL_D3D11:
        {
            attributes.push_back(AttributeDesc("POSITION", 0, 0, DataFormat::R32G32_FLOAT, 0 /* ~ 7 */));
            attributes.push_back(AttributeDesc("TEXCOORD", 0, 1, DataFormat::R32G32_FLOAT, 8 /* ~ 15 */));
            break;
        }
        default:
            assert(false);
            break;
    }
    return attributes;
}

Pipeline::ptr SceneLayerProgram::CreatePipeline()
{
    ShaderModule::ptr vertexShader   = _draw->CreateShaderModule(SL::ShaderStage::VERTEX, _draw->GetShaderLanguage(), GetVertexShader());
    ShaderModule::ptr fragmentShader = _draw->CreateShaderModule(SL::ShaderStage::FRAGMENT, _draw->GetShaderLanguage(), GetFragmentShader());
    InputLayout::ptr  inputLayout;
    BlendState::ptr   blendState;
    DepthStencilState::ptr depthStencilState;
    RasterState::ptr rasterState;
    Pipeline::ptr pipeLine;
    {
        InputLayoutDesc desc;
        {
            BindingDesc binding;
            binding.stride       = sizeof(SceneLayerVertex);
            binding.instanceRate = false;
            desc.bindings.push_back(binding);
        }
        desc.attributes = GetAttributeDescs();
        inputLayout = _draw->CreateInputLayout(desc);
    }
    {
        BlendStateDesc desc;
        desc.enabled = true;
        desc.srcCol = BlendFactor::SRC_ALPHA;
        desc.dstCol = BlendFactor::ONE_MINUS_SRC_ALPHA;
        desc.colorMask = 0x0F;
        desc.srcAlpha  = BlendFactor::ONE;
        desc.dstAlpha  = BlendFactor::ONE_MINUS_SRC_ALPHA;
        blendState = _draw->CreateBlendState(desc);
    }
    {
        DepthStencilStateDesc desc;
        desc.depthTestEnabled = false;
        desc.depthWriteEnabled = false;
        desc.depthCompare = Comparison::LESS;
        desc.stencilEnabled = false;
        depthStencilState = _draw->CreateDepthStencilState(desc);
    }
    {
        RasterStateDesc desc;
        desc.cull = CullMode::NONE;
        rasterState = _draw->CreateRasterState(desc);
    }
    {
        PipelineDesc desc;
        desc.prim = Primitive::TRIANGLE_LIST;
        desc.shaders.push_back(vertexShader);
        desc.shaders.push_back(fragmentShader);
        desc.inputLayout = inputLayout;
        desc.depthStencil = depthStencilState;
        desc.blend = blendState;
        desc.raster = rasterState;
        {
            UniformBufferDesc uniform;
            uniform.uniformBufferSize = 2;
            uniform.uniforms.push_back(UniformDesc("WorldViewProj", UniformType::MATRIX4X4, 0));
            uniform.uniforms.push_back(UniformDesc("Transparency", UniformType::FLOAT1, 64));
            desc.uniformBufferDesc = uniform;
        }
        {
            desc.samplers.push_back(SamplerDef("SceneLayer"));
        }
        pipeLine = _draw->CreateGraphicsPipeline(desc, "SceneLayer");
    }
    return pipeLine;
}

void SceneLayerProgram::InitUniforms()
{
    _uniformsRawData = std::make_shared<RawData>(sizeof(SceneLayerUniforms));
    _uniforms = reinterpret_cast<SceneLayerUniforms*>(_uniformsRawData->GetData());
    {
        _draw->GetDefaultVorldViewProj(1, 1, _uniforms->worldViewProj, false);
        _uniforms->transparency = 1.0f;
    }
}

void SceneLayerProgram::InitVertexData()
{
    std::vector<SceneLayerVertex> verts;
    {
        // one rectangle
        verts.push_back(SceneLayerVertex({0.0, 0.0, 0.0, 0.0}));
        verts.push_back(SceneLayerVertex({1.0, 0.0, 1.0, 0.0}));
        verts.push_back(SceneLayerVertex({1.0, 1.0, 1.0, 1.0}));
        // another rectangle
        verts.push_back(SceneLayerVertex({0.0, 0.0, 0.0, 0.0}));
        verts.push_back(SceneLayerVertex({1.0, 1.0, 1.0, 1.0}));
        verts.push_back(SceneLayerVertex({0.0, 1.0, 0.0, 1.0}));
    }
    std::shared_ptr<ImmutableVectorAllocateMethod<SceneLayerVertex>> allocate = std::make_shared<ImmutableVectorAllocateMethod<SceneLayerVertex>>();
    allocate->container.swap(verts);
    _vertexsRawData = std::make_shared<RawData>(allocate->container.size() * sizeof(SceneLayerVertex), allocate);
}

} // namespace Gpu 
} // namespace Mmp

namespace Mmp 
{
namespace Gpu 
{

SceneLayerImpl::SceneLayerImpl()
{
    _program = SceneLayerProgram::Create();
}

bool SceneLayerImpl::SetParam(const SceneLayerParam& param)
{
    std::lock_guard<std::mutex> lock(_mtx);
    _param = param;
    return true;
}

SceneLayerParam SceneLayerImpl::GetParam()
{
    std::lock_guard<std::mutex> lock(_mtx);
    return _param;
}

bool SceneLayerImpl::AddSceneItem(const std::string& tag, AbstractSceneItem::ptr item)
{
    std::lock_guard<std::mutex> itemLock(_itemMtx);
    _items[tag] = item;
    return true;
}

void SceneLayerImpl::DelSceneItem(const std::string& tag)
{
    std::lock_guard<std::mutex> lock(_itemMtx);
    _items.erase(tag);
}

AbstractSceneItem::ptr SceneLayerImpl::GetSceneItem(const std::string& tag)
{
    std::lock_guard<std::mutex> lock(_itemMtx);
    if (_items.count(tag))
    {
        return _items[tag];
    }
    else
    {
        return nullptr;
    }
}

std::vector<std::string /* tag */> SceneLayerImpl::GetSceneItemsTag()
{
    std::vector<std::string> tags;
    {
        std::lock_guard<std::mutex> lock(_itemMtx);
        for (auto& val : _items)
        {
            tags.push_back(val.first /* tag */);
        }
    }
    return tags;
}

void SceneLayerImpl::UpdateCanvas(Texture::ptr canvas)
{
    std::lock_guard<std::mutex> lock(_mtx);
    _canvas = canvas;
}

void SceneLayerImpl::Draw(Texture::ptr framebuffer)
{
    std::lock_guard<std::mutex> lock(_mtx);
    if (!framebuffer || !_canvas || !_param.show)
    {
        return;
    }
    _program->Begin();
    // clear layer
    {
        _program->ClearCanvas(_canvas);
    }
    // draw items to layer
    {
        std::lock_guard<std::mutex> itemLock(_itemMtx);
        std::vector<AbstractSceneItem::ptr> items;
        for (const auto& val : _items)
        {
            AbstractSceneItem::ptr item = val.second;
            items.push_back(item);
        }
        // Hint : 从下层到上层, 从左上到右下
        std::sort(items.begin(), items.end(), [](const AbstractSceneItem::ptr& left, const AbstractSceneItem::ptr& right) -> bool
        {
            SceneItemParam leftParam = left->GetParam();
            SceneItemParam rightParam = right->GetParam();
            if ((uint32_t)leftParam.level < (uint32_t)rightParam.level)
            {
                return true;
            }
            else if (leftParam.location.x < rightParam.location.x && leftParam.location.y < rightParam.location.y)
            {
                return true;
            }
            else
            {
                return false;
            }
        });
        for (const auto& item : items)
        {
            item->Draw(_canvas);
        }
    }
    // draw layer to framebuffer
    {
        _program->UpdateContext(_param.transparency);
        _program->Draw(_canvas, framebuffer);
    }
    _program->End();
}

} // namespace Gpu 
} // namespace Mmp