#include "OpenGLContex.h"

#include <cassert>
#include <memory.h>
#include <algorithm>

#include "OpenGLUtil.h"
#include "OpenGLInitStepData.h"
#include "OpenGLRenderData.h"
#include "OpenGLTranslator.h"
#include "OpenGLDrawContex.h"

namespace Mmp
{

static void GarbageCollection(Any any)
{
    // Hint : may be a better solution to find OpenGL Instance
    std::shared_ptr<OpenGLDrawContex> draw = std::dynamic_pointer_cast<OpenGLDrawContex>(GLDrawContex::Instance());
    draw->_openGL->ReleaseResource(any);
}

OpenGLShaderModule::OpenGLShaderModule(SL::ShaderStage stage, const std::string& tag)
{
    _lang    = SL::ShaderLanguage::GLSL_4xx;
    _stage   = stage;
    _tag     = tag;
    _glStage = GLShaderStageToOpenGLType(stage);
}

OpenGLShaderModule::~OpenGLShaderModule()
{
    if (_shader)
    {
        GarbageCollection(_shader);
    }
}

SL::ShaderStage OpenGLShaderModule::GetStage()
{
    return _stage;
}

OpenGLRenderShader::ptr OpenGLShaderModule::GetShader()
{
    return _shader;
}

const std::string OpenGLShaderModule::GetSource()
{
    return _source;
}

SL::ShaderLanguage OpenGLShaderModule::GetShaderLanguage()
{
    return _lang;
}

bool OpenGLShaderModule::Complie(OpenGL::ptr openGL, SL::ShaderLanguage language, const std::string& source)
{
    _lang   = language;
    _source   = source;
    _shader = std::make_shared<OpenGLRenderShader>();
    OpenGLInitStep step;
    OpenGLCreateShaderInitData data;
    step.type     = OpenGLInitStepType::CREATE_SHADER;
    _shader->desc = _tag;
    data.code     = _source;
    data.stage    = _glStage;
    data.shader   = _shader;
    step.data     = data;
    openGL->_initSteps.push_back(step);
    return true;
}

void OpenGLBlendState::Apply(OpenGL::ptr openGL)
{
    assert(openGL->_curStep && openGL->_curStep->stepType == OpenGLRenderStepType::RENDER);
    OpenGLRender           command;
    OpenGLBlendRenderData  data;
    command.cmd = OpenGLRenderCommond::BLEND;
    data.mask = colorMask;
    data.enabled = enabled;
    data.srcColor = srcCol;
    data.dstColor = dstCol;
    data.srcAlpha = srcAlpha;
    data.dstAlpha = dstAlpha;
    data.funcColor = eqCol;
    data.funcAlpha = eqAlpha;
    command.data = data;
    openGL->_curStep->commands.push_back(command);
}

void OpenGLRasterState::Apply(OpenGL::ptr openGL)
{
    assert(openGL->_curStep && openGL->_curStep->stepType == OpenGLRenderStepType::RENDER);
    OpenGLRender           command;
    OpenGLRasterRenderData  data;
    command.cmd = OpenGLRenderCommond::RASTER;
    data.cullEnable = cullEnable;
    data.frontFace = frontFace;
    data.cullFace = cullMode;
    data.ditherEnable = GL_FALSE;
    data.depthClampEnable = GL_FALSE;
    command.data = data;
    openGL->_curStep->commands.push_back(command);
}

void OpenGLInputLayout::Complie(OpenGL::ptr openGL, const InputLayoutDesc& desc)
{
	// TODO: This is only accurate if there's only one stream. But whatever, for now we
	// never use multiple streams anyway.
    // comment for ppsspp
    stride = desc.bindings.empty() ? 0 : (GLsizei)desc.bindings[0].stride;

    std::vector<OpenGLRenderInputLayout::Entry> entries;
    for (auto& attr : desc.attributes)
    {
        OpenGLRenderInputLayout::Entry entry;
        entry.attribute = attr.attribute;
        entry.location  = attr.location;
        entry.stride    = (GLsizei)desc.bindings[attr.binding].stride;
        entry.offset    = attr.offset;
        switch (attr.format)
        {
            case DataFormat::R32G32_FLOAT:
            {
                entry.count      = 2;
                entry.type       = GL_FLOAT;
                entry.normalized = GL_FALSE;
                break;
            }
            case DataFormat::R32G32B32_FLOAT:
            {
                entry.count      = 3;
                entry.type       = GL_FLOAT;
                entry.normalized = GL_FALSE;
                break;
            }
            case DataFormat::R32G32B32A32_FLOAT:
            {
                entry.count      = 4;
                entry.type       = GL_FLOAT;
                entry.normalized = GL_FALSE;
                break;
            }
            case DataFormat::R8G8B8A8_UNORM:
            {
                entry.count      = 4;
                entry.type       = GL_UNSIGNED_BYTE;
                // Hint : 坐标值做归一化处理
                entry.normalized = GL_TRUE;
                break;
            }
            case DataFormat::UNDEFINED:
            default:
                assert(false);
                GL_LOG_ERROR << "Vertex Format : invalid or unknown component type applied.";
                break;
        }
        entries.push_back(std::move(entry));
    }

    if (!entries.empty())
    {
        inputLayout = std::make_shared<OpenGLRenderInputLayout>();

        OpenGLInitStep step;
        OpenGLCreateInputLayoutInitData data;
        step.type = OpenGLInitStepType::CREATE_INPUT_LAYOUT;
        data.inputLayout = inputLayout;
        data.inputLayout->entries = std::move(entries);
        for (auto& iter : data.inputLayout->entries)
        {
            data.inputLayout->semanticsMask |= 1 << iter.location;
        }
        step.data = data;
        openGL->_initSteps.push_back(step);
    }
    else
    {
        inputLayout = nullptr;
    }
}

OpenGLPipeline::~OpenGLPipeline()
{
    if (!shaders.empty())
    {
        GarbageCollection(shaders);
        GarbageCollection(program);
    }
}

bool OpenGLPipeline::LinkShaders(OpenGL::ptr openGL, const PipelineDesc& desc)
{
    // Get shaders already link 
    // Hint : 0 - vertex, 1 - fragment (if exist)
    std::vector<OpenGLRenderShader::ptr> linkShaders;
    {
        for (auto shaderMoudle : shaders)
        {
            if (shaderMoudle)
            {
                OpenGLRenderShader::ptr shader = shaderMoudle->GetShader();
                if (shader)
                {
                    linkShaders.push_back(shader);
                }
                else
                {
                    assert(false);
                    GL_LOG_ERROR << "Link Shaders : bad shader in moudle";
                    return false;
                }
            }
            else
            {
                assert(false);
                GL_LOG_ERROR << "Link Shaders : bad shader in moudle";
                return false;
            }
        }
    }

    // Set Attribute Location
    std::vector<OpenGLRenderProgram::Semantic> semantics;
    {
        OpenGLRenderInputLayout::ptr layout = std::dynamic_pointer_cast<OpenGLInputLayout>(desc.inputLayout)->inputLayout;
        for (const auto& semantic : layout->entries)
        {
           semantics.push_back({semantic.location, semantic.attribute});
        }
    }

    locs = std::make_shared<PipeLineLocData>();
    locs->dynamicUniformLocs.resize(desc.uniformBufferDesc.uniforms.size());

    std::vector<OpenGLRenderProgram::UniformLocQuery> queries;
    std::vector<OpenGLRenderProgram::Initializer> initialize;

    int samplersTocheck = 0;
    if (!samplers.empty())
    {
        for (int i = 0; i < (int)std::min((const uint32_t)samplers.size(), (const uint32_t)MAX_TEXTURE_SLOTS); i++)
        {
            queries.push_back({&locs->samplerLocs[i], samplers[i].name, true});
        }
        samplersTocheck = (int)std::min((const uint32_t)samplers.size(), (const uint32_t)MAX_TEXTURE_SLOTS);
    }
    assert(queries.size() <= MAX_TEXTURE_SLOTS);
    for (size_t i=0; i<dynamicUniforms.uniforms.size(); i++)
    {
        queries.push_back({&locs->dynamicUniformLocs[i], dynamicUniforms.uniforms[i].name, false});
    }
    for (int i=0; i<MAX_TEXTURE_SLOTS; i++)
    {
        if (i < samplersTocheck)
        {
            initialize.push_back({&locs->samplerLocs[i], 0, i});
        }
        else
        {
            locs->samplerLocs[i] = -1;
        }
    }

    // Create Porgram
    {
        OpenGLRenderProgramFlags flags = {};
        program = std::make_shared<OpenGLRenderProgram>();
        OpenGLInitStep step;
        OpenGLCreateProgramInitData data;
        step.type = OpenGLInitStepType::CREATE_PROGRAM;
        data.program = program;
        data.program->semantics            = semantics;
        data.program->queries              = queries;
        data.program->initialize           = initialize;
        data.program->locData              = locs;
        data.program->use_clip_distance[0] = flags.useClipDistance0;
        data.program->use_clip_distance[1] = flags.useClipDistance1;
        data.program->use_clip_distance[2] = flags.useClipDistance2;
        if (linkShaders.empty())
        {
            GL_LOG_ERROR << "Can't create a programs with zero shaders";
            assert(false);
        }
        for (size_t i=0; i<linkShaders.size(); i++)
        {
            data.shaders[i] = linkShaders[i];
        }
        data.shaderNum = (uint32_t)shaders.size();
        for (auto& iter : queries)
        {
            assert(!iter.name.empty());
        }
        for (auto& sem : semantics)
        {
            assert(!sem.attrib.empty());
        }
        step.data = data;
        openGL->_initSteps.push_back(step);
    }
    return true;
}

void OpenGLDepthStencilState::Apply(OpenGL::ptr openGL, uint8_t stencilRef, uint8_t stencilWriteMask, uint8_t stencilCompareMask)
{
    // Set Depth
    {
        assert(openGL->_curStep && openGL->_curStep->stepType == OpenGLRenderStepType::RENDER);
        OpenGLRender           command;
        OpenGLDepthRenderData  data;
        command.cmd  = OpenGLRenderCommond::DEPTH;
        data.enbaled = depthTestEnabled;
        data.write   = depthWriteEnabled;
        data.func    = depthComp;
        command.data = data;
        openGL->_curStep->commands.push_back(command);
    }

    // Set Stencil Func
    {
        assert(openGL->_curStep && openGL->_curStep->stepType == OpenGLRenderStepType::RENDER);
        OpenGLRender                 command;
        OpenGLStencilFuncRenderData  data;
        command.cmd      = OpenGLRenderCommond::STENCILFUNC;
        data.enabled     = stencilEnabled;
        data.func        = stencilCompareOp;
        data.ref         = stencilRef;
        data.compareMask = stencilCompareMask;
        command.data     = data;
        openGL->_curStep->commands.push_back(command);
    }

    // Set Stencil Op
    {
        assert(openGL->_curStep && openGL->_curStep->stepType == OpenGLRenderStepType::RENDER);
        OpenGLRender                 command;
        OpenGLStencilOpRenderData    data;
        command.cmd                =  OpenGLRenderCommond::STENCILOP;
        data.writeMask             =  stencilWriteMask;
        data.sFail                 =  stencilFail;
        data.zFail                 =  stencilZFail;
        data.pass                  =  stencilPass; 
        command.data               =  data;
        openGL->_curStep->commands.push_back(command);
    }
}

OpenGLBuffer::OpenGLBuffer(OpenGL::ptr openGL, size_t size, uint32_t flags)
{
    if (flags & BufferUsageFlag::INDEXDATA)
    {
        target = GL_ELEMENT_ARRAY_BUFFER;
    }
    else
    {
        target = GL_ARRAY_BUFFER;
    }
    if (flags & BufferUsageFlag::DYNAMIC)
    {
        usage = GL_DYNAMIC_DRAW;
    }
    else
    {
        usage = GL_STATIC_DRAW;
    }
    totalSize = size;
    // Create Buffer
    {
        buffer = std::make_shared<OpenGLRenderBuffer>(target, size);
        OpenGLInitStep step;
        OpenGLCreateBufferInitData data;
        step.type = OpenGLInitStepType::CREATE_BUFFER;
        data.size   = (int)size;
        data.usage  = usage;
        data.buffer = buffer;
        step.data   = data;
        openGL->_initSteps.push_back(step);
    }
}

OpenGLBuffer::~OpenGLBuffer()
{
    if (buffer)
    {
        GarbageCollection(buffer);
    }
}

OpenGLTexture::OpenGLTexture(const OpenGL::ptr& openGL, const TextureDesc& desc)
{
    _openGL = openGL;

    _generatedMips  = false;
    _width          = desc.width;
    _height         = desc.height;
    _depth          = desc.depth;
    _format         = desc.format;
    _type           = desc.type;
    _mipLevels      = desc.mipLevels;
    
    {
        _tex = std::make_shared<OpenGLRenderTexture>(_width, _height, 1, _mipLevels);
        _tex->format = desc.format; 
        _tex->target = GLTextureTypeToOpenGLType(desc.type);
        assert(_tex);
        OpenGLInitStep step;
        OpenGLCreateTextureInitData data;
        step.type = OpenGLInitStepType::CREATE_TEXTURE;
        data.texture = _tex;
        step.data = data;
        _openGL->_initSteps.push_back(step);
    }

    if (desc.initData.empty())
    {
        return;
    }

    {
        int level = 0;
        int width = _width;
        int height = _height;
        int depth = _depth;
        for (const auto& data : desc.initData)
        {
            assert((int)data->width == width);
            assert((int)data->height == height);
            assert((int)data->depth == depth);
            SetImageData(0, 0, 0, width, height, depth, level, 0, data);
            width  = (width + 1) / 2;
            height = (height + 1) / 2;
            depth  = (depth + 1) / 2; 
            level++;
        }
        _mipLevels = desc.generateMips ? desc.mipLevels : level;
    }

    {
        bool genMips = false;
        if ((int)desc.initData.size() < desc.mipLevels && desc.generateMips)
        {
            genMips = true;
            _generatedMips = true;
        }
        OpenGLInitStep step;
        OpenGLTextureFinalizeInitData data;
        step.type = OpenGLInitStepType::TEXTURE_FINALIZE;
        data.texture = _tex;
        data.loadedLevels = _mipLevels;
        data.genMips = genMips;
        step.data = data;
        _openGL->_initSteps.push_back(step);
    }
}

OpenGLTexture::~OpenGLTexture()
{
    if (_tex)
    {
        GarbageCollection(_tex);
    }
}

bool OpenGLTexture::HasMips()
{
    return _mipLevels > 1 || _generatedMips;
}

TextureType OpenGLTexture::GetType()
{
    return _type;
}

int OpenGLTexture::NumMipmaps()
{
    return _mipLevels;
}

OpenGLRenderTexture::ptr OpenGLTexture::GetTex()
{
    return _tex;
}

void OpenGLTexture::Bind(int stage)
{    
    assert(_openGL->_curStep && _openGL->_curStep->stepType == OpenGLRenderStepType::RENDER);
    OpenGLRender command;
    OpenGLTextureRenderData data;
    command.cmd = OpenGLRenderCommond::BINDTEXTURE;
    data.solt    = stage;
    data.texture = _tex;
    command.data = data;
    _openGL->_curStep->commands.push_back(command);
}

void OpenGLTexture::SetImageData(int x, int y, int z, int width, int height, int depth, int level, int stride, GLPixelData::ptr data)
{
    // Hint : dynamic change size
    // See ppsspp : When switching to texStorage we need to handle this correctly.
    if (level == 0 && (width != _width || height != _height || depth == _depth))
    {
        _width = width;
        _height = height;
        _depth = depth;
    }
    stride = stride != 0 ? stride : width;

    // todo : support  stride and alignment
    // size_t alignment = DataFormatSizeInBytes(_format);
    assert(stride == (int)data->width);

    {
        OpenGLInitStep step;
        OpenGLTextureImageInitData _data;
        step.type         = OpenGLInitStepType::TEXTURE_IMAGE;
        _data.data         = data;
        _data.texture      = _tex;
        _data.format       = _format;
        _data.level        = level;
        _data.width        = width;
        _data.height       = height;
        _data.depth        = depth;
        _data.linearFilter = false;
        step.data = _data;
        _openGL->_initSteps.push_back(step);
    }

}

OpenGLFrameBuffer::OpenGLFrameBuffer(OpenGLRenderFrameBuffer::ptr frameBuffer)
{
    _width = frameBuffer->width;
    _height = frameBuffer->height;
    this->frameBuffer = frameBuffer;
}

OpenGLFrameBuffer::~OpenGLFrameBuffer()
{
    if (frameBuffer)
    {
        GarbageCollection(frameBuffer);
    }
}

} // namespace Mmp