#include "SceneItemImpl.h"

#include <mutex>
#include <cmath>
#include <memory>
#include <cstdint>
#include <cassert>

#include "Common/ImmutableVectorAllocateMethod.h"

#include "GPU/GL/GLCommon.h"
#include "GPU/GL/GLContex.h"
#include "GPU/GL/GLDrawContex.h"

#include "Eigen/Dense"

namespace Mmp 
{
namespace Gpu 
{

#define MPP_SCENE_ITEM_RGB_INTERNAL_PROGRAM 0
#define MPP_SCENE_ITEM_RGB_IMPORT_PROGRAM   1
#define MPP_SCENE_ITEM_YUV_IMPORT_PROGRAM   2
#define MPP_SCENE_ITEM_PROGRAM_NUM          3

class SceneItemVertex
{
public:
    SceneItemVertex(float x, float y, float u, float v);
public:
    float x, y;  // attribute vec2 Position
    float u, v;  // attribute vec2 iUv
};

SceneItemVertex::SceneItemVertex(float x, float y, float u, float v)
{
    this->x = x;
    this->y = y;
    this->u = u;
    this->v = v;
}

class SceneItemUniforms
{
public:
    SceneItemUniforms();
public:
    float worldViewProj[16] = {0.0f};
    float transparency;
};

SceneItemUniforms::SceneItemUniforms()
{
    transparency = 1.0f;
}

class SceneItemProgramParams
{
public:
    SceneItemProgramParams();
public:
    bool     rowFilp;         // 水平镜像
    bool     colFilp;         // 垂直镜像
    float    transparency;    // 透明度
    float    x, y;            // 平移
    uint64_t rotation;        // 旋转
    float    scaleX, scaleY;  // 缩放
};


SceneItemProgramParams::SceneItemProgramParams()
{
    rowFilp = false;
    colFilp = false;
    transparency = 1.0f;
    x = 0.0f;
    y = 0.0f;
    rotation = 0;
    scaleX = 0.0f;
    scaleY = 0.0f;
}


class SceneItemProgram
{
public:
    using ptr = std::shared_ptr<SceneItemProgram>;
public:
    static SceneItemProgram::ptr Create();
public:
    SceneItemProgram();
    ~SceneItemProgram();
public:
    void Begin();
    void End();
    void Draw(Texture::ptr image, Texture::ptr canvas);
    void UpdateContext(const SceneItemProgramParams& param);
private:
    static const std::string GetVertexShader();
    static const std::string GetFragmentShader(uint8_t slot);
    static std::vector<AttributeDesc> GetAttributeDescs();
    Pipeline::ptr CreatePipeline(uint8_t slot);
    void InitUniforms();
    void InitVertexData();
private:
    std::mutex                      _mtx;
    GLDrawContex::ptr               _draw;
    GLBuffer::ptr                   _vbo;
    Pipeline::ptr                   _pipeLines[MPP_SCENE_ITEM_PROGRAM_NUM];
    std::vector<SamplerState::ptr>  _samples;
    RawData::ptr                    _uniformsRawData;
    SceneItemUniforms*              _uniforms;
    RawData::ptr                    _vertexsRawData;
private:
    Eigen::Matrix4d                 _worldViewProj;
    SceneItemProgramParams          _programParams;
};

SceneItemProgram::ptr SceneItemProgram::Create()
{
    static std::mutex gMtx;
    static std::weak_ptr<SceneItemProgram> gCacheInstance;
    SceneItemProgram::ptr instace = nullptr;
    {
        std::lock_guard<std::mutex> lock(gMtx);
        instace = gCacheInstance.lock();
        if (!instace)
        {
            instace = std::make_shared<SceneItemProgram>();
            gCacheInstance = instace;
        }
    }
    return instace;
}

SceneItemProgram::SceneItemProgram()
{
    _draw = GLDrawContex::Instance();
    InitUniforms();
    InitVertexData();
    _vbo = _draw->CreateBuffer(sizeof(SceneItemVertex)*6, BufferUsageFlag::DYNAMIC | BufferUsageFlag::VERTEXDATA);

    for (uint8_t i=0; i<MPP_SCENE_ITEM_PROGRAM_NUM; i++)
    {
        _pipeLines[i] = CreatePipeline(i);
        if (!_pipeLines[i])
        {
            if (i != 0)
            {
                _pipeLines[i] = _pipeLines[i-1];
            }
        }
    }
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

SceneItemProgram::~SceneItemProgram()
{

}

const std::string SceneItemProgram::GetVertexShader()
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

const std::string SceneItemProgram::GetFragmentShader(uint8_t slot)
{
    std::string source;
    switch (GLDrawContex::Instance()->GetShaderLanguage())
    {
        case SL::ShaderLanguage::ELSL_3xx:
        case SL::ShaderLanguage::GLSL_4xx:
        {
            switch (slot) 
            {
                case MPP_SCENE_ITEM_RGB_INTERNAL_PROGRAM:
                {
                    source += "#define samplerItem sampler2D\n";
                    break;
                }
                case MPP_SCENE_ITEM_RGB_IMPORT_PROGRAM:
                {
                    source += "#define samplerItem samplerExternalOES\n";
                }
                case MPP_SCENE_ITEM_YUV_IMPORT_PROGRAM:
                {
                    source += "#define samplerItem __samplerExternal2DY2YEXT\n";
                }
                default:
                {
                    assert(false);
                    source += "#define samplerItem sampler2D\n";
                    break;
                }
            }
            source += 
R"(
varying vec2 oUV;

uniform samplerItem SceneItem;

uniform float Transparency;

vec4 getSceneItemColor(vec2 uv)
{
    vec4 color = texture(SceneItem, oUV);
    color.a = color.a * Transparency;
    return color;
}

vec2 getSceneItemPos(vec2 uv)
{
    return uv;
}

bool InRenderArea(vec2 pos)
{
    return (clamp(pos.x, 0.0f, 1.0f) == pos.x) && (clamp(pos.y, 0.0f, 1.0f) == pos.y); 
}

void main(void)
{
    vec2 sceneItemPos = getSceneItemPos(oUV);
    if (InRenderArea(sceneItemPos))
    {
        gl_FragColor = getSceneItemColor(sceneItemPos);
    }
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
};

std::vector<AttributeDesc> SceneItemProgram::GetAttributeDescs()
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

void SceneItemProgram::InitUniforms()
{
    _uniformsRawData = std::make_shared<RawData>(sizeof(SceneItemUniforms));
    _uniforms = reinterpret_cast<SceneItemUniforms*>(_uniformsRawData->GetData());
    {
        _draw->GetDefaultVorldViewProj(1, 1, _uniforms->worldViewProj, false);
        _uniforms->transparency = 1.0f;
    }
    {
        for (uint64_t i=0; i<4; i++)
        {
            for (uint64_t j=0; j<4; j++)
            {
                _worldViewProj(j, i) = _uniforms->worldViewProj[j*4 + i];
            }
        }
    }
}

void SceneItemProgram::InitVertexData()
{
    std::vector<SceneItemVertex> verts;
    {
        // one rectangle
        verts.push_back(SceneItemVertex({0.0, 0.0, 0.0, 0.0}));
        verts.push_back(SceneItemVertex({1.0, 0.0, 1.0, 0.0}));
        verts.push_back(SceneItemVertex({1.0, 1.0, 1.0, 1.0}));
        // another rectangle
        verts.push_back(SceneItemVertex({0.0, 0.0, 0.0, 0.0}));
        verts.push_back(SceneItemVertex({1.0, 1.0, 1.0, 1.0}));
        verts.push_back(SceneItemVertex({0.0, 1.0, 0.0, 1.0}));
    }
    std::shared_ptr<ImmutableVectorAllocateMethod<SceneItemVertex>> allocate = std::make_shared<ImmutableVectorAllocateMethod<SceneItemVertex>>();
    allocate->container.swap(verts);
    _vertexsRawData = std::make_shared<RawData>(allocate->container.size() * sizeof(SceneItemVertex), allocate);
}

Pipeline::ptr SceneItemProgram::CreatePipeline(uint8_t slot)
{
    ShaderModule::ptr vertexShader   = _draw->CreateShaderModule(SL::ShaderStage::VERTEX, _draw->GetShaderLanguage(), GetVertexShader());
    ShaderModule::ptr fragmentShader = _draw->CreateShaderModule(SL::ShaderStage::FRAGMENT, _draw->GetShaderLanguage(), GetFragmentShader(slot));
    InputLayout::ptr  inputLayout;
    BlendState::ptr   blendState;
    DepthStencilState::ptr depthStencilState;
    RasterState::ptr rasterState;
    Pipeline::ptr pipeLine;
    {
        InputLayoutDesc desc;
        {
            BindingDesc binding;
            binding.stride       = sizeof(SceneItemVertex);
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
            uniform.uniforms.push_back(UniformDesc("WorldViewProj", UniformType::MATRIX4X4, 0));
            uniform.uniforms.push_back(UniformDesc("Transparency", UniformType::FLOAT1, 64));
            uniform.uniformBufferSize = uniform.uniforms.size();
            desc.uniformBufferDesc = uniform;
        }
        {
            desc.samplers.push_back(SamplerDef("SceneItem"));
        }
        pipeLine = _draw->CreateGraphicsPipeline(desc, "SceneItem");
    }
    return pipeLine;
}

/**
 * @todo 目前来看 Scene Compositor 里的 Scene Item 数量不会很多, 应该不会是调度瓶颈,
 *       根据实际情况考虑是否缓存矩阵运算结果, 做 diff 处理
 *       (以及可以考虑做矩阵运算合并, Eigen 对多矩阵相乘做有优化处理)
 */
void SceneItemProgram::UpdateContext(const SceneItemProgramParams& param)
{
    Eigen::Matrix4d  worldViewProj = _worldViewProj;
    _programParams = param;
    _uniforms->transparency = _programParams.transparency;
    // 步骤
    // 镜像 -> 缩放 -> 平移 -> 旋转

    // Todo : 是否需要缓存并合并矩阵运算, 提升效率
    // Hint : 目前来看 Scene Compositor 里的 Scene Item 数量不会很多, 应该不会是调度瓶颈
    // 缩放 (锚点左上角 : 居中缩放 + 左上平移)
    if (_programParams.rowFilp || _programParams.colFilp)
    {
        Eigen::Matrix4d reflectMatrix = Eigen::Matrix4d::Identity();
        if (_programParams.rowFilp)
        {
            reflectMatrix(0, 0) = -1.0f;
        }
        if (_programParams.colFilp)
        {
            reflectMatrix(1, 1) = -1.0f;
        }
        worldViewProj = worldViewProj * reflectMatrix;
    }
    if (_programParams.scaleX != 1.0f || _programParams.scaleY != 1.0f)
    {
        // 居中缩放
        {        
            Eigen::Matrix4d scalingMatrix = Eigen::Matrix4d::Identity();
            scalingMatrix(0, 0) = _programParams.scaleX;
            scalingMatrix(1, 1) = _programParams.scaleY;
            worldViewProj = worldViewProj * scalingMatrix;
        }
        // 左上平移
        {
            Eigen::Matrix4d translationMatrix = Eigen::Matrix4d::Zero();
            translationMatrix(3, 0) = -(1.0f - _programParams.scaleX);
            translationMatrix(3, 1) = -(1.0f - _programParams.scaleY);
            worldViewProj = worldViewProj + translationMatrix;
        }
    }
    // 平移
    if (_programParams.x != 0.0f || _programParams.y != 0.0f)
    {
        Eigen::Matrix4d translationMatrix = Eigen::Matrix4d::Zero();
        translationMatrix(3, 0) = _programParams.x * 2;
        translationMatrix(3, 1) = _programParams.y * 2;
        worldViewProj = worldViewProj + translationMatrix;
    }
    // 旋转
    if (_programParams.rotation != 0)
    {
        double angleInRadians = (float)_programParams.rotation * (3.14159265358979323846 /* M_PI */) / 180.0;
        Eigen::Matrix4d rotationMatrix = Eigen::Matrix4d::Identity();
        rotationMatrix(0, 0) = std::cos(angleInRadians);
        rotationMatrix(0, 1) = -std::sin(angleInRadians);
        rotationMatrix(1, 0) = std::sin(angleInRadians);
        rotationMatrix(1, 1) = std::cos(angleInRadians);
        worldViewProj = worldViewProj * rotationMatrix;
    }
    for (uint32_t i=0; i<4; i++)
    {
        for (uint32_t j=0; j<4; j++)
        {
            _uniforms->worldViewProj[i + j*4] = (float)worldViewProj(j, i);
        }
    }
}

void SceneItemProgram::Begin()
{
    _mtx.lock();
}

void SceneItemProgram::End()
{
    _mtx.unlock();
}

void SceneItemProgram::Draw(Texture::ptr image, Texture::ptr canvas)
{
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
        desc.tag = "SceneItemProgram"; 
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
    _draw->SetScissorRect(0, 0, canvas->Width(), canvas->Height());
    {
        Viewport vp;
        vp.TopLeftX  = 0;
        vp.TopLeftY  = 0;
        vp.Width     = (float)canvas->Width();
        vp.Height    = (float)canvas->Height();
        vp.MaxDepth  = 1.0;
        vp.MinDepth  = 0.0;
        _draw->SetViewport(vp);
    }
    _draw->BindSamplerStates(0, _samples);
    {
        std::vector<Texture::ptr> textures;
        textures.push_back(image);
        _draw->BindTextures(0, textures);
    }
    {
        Pipeline::ptr pipeLine;
        if (image->Flags() & (GlTextureFlags::TEXTURE_EXTERNAL | GlTextureFlags::TEXTURE_YUV))
        {
            pipeLine = _pipeLines[MPP_SCENE_ITEM_YUV_IMPORT_PROGRAM];
        }
        else if (image->Flags() & GlTextureFlags::TEXTURE_EXTERNAL)
        {
            pipeLine = _pipeLines[MPP_SCENE_ITEM_RGB_IMPORT_PROGRAM];
        }
        else
        {
            pipeLine = _pipeLines[MPP_SCENE_ITEM_RGB_INTERNAL_PROGRAM];
        }
        _draw->BindPipeline(pipeLine);
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
    _draw->FenceEnd();
}

} // namespace Gpu 
} // namespace Mmp

namespace Mmp 
{
namespace Gpu 
{

SceneItemImpl::SceneItemImpl()
{
    _program = SceneItemProgram::Create();
}

bool SceneItemImpl::SetParam(const SceneItemParam& params)
{
    std::lock_guard<std::mutex> lock(_mtx);
    _params = params;
    return true;
}

SceneItemParam SceneItemImpl::GetParam()
{
    std::lock_guard<std::mutex> lock(_mtx);
    return _params;
}

void SceneItemImpl::UpdateImage(Texture::ptr image)
{
    std::lock_guard<std::mutex> lock(_mtx);
    _image = image;
}

void SceneItemImpl::Draw(Texture::ptr canvas)
{
    std::lock_guard<std::mutex> lock(_mtx);
    if (!_image || !canvas || _params.transparency == 0)
    {
        return;
    }
    _program->Begin();
    SceneItemProgramParams programParams;
    {
        programParams.rowFilp = _params.rowFilp;
        programParams.colFilp = _params.colFilp;
        programParams.transparency = _params.transparency;
        programParams.rotation = _params.rotation;
        programParams.x = _params.location.x;
        programParams.y = _params.location.y;
        programParams.scaleX = _params.area.w;
        programParams.scaleY = _params.area.h;
    }
    _program->UpdateContext(programParams);
    _program->Draw(_image, canvas);
    _program->End();
}

} // namespace Gpu 
} // namespace Mmp