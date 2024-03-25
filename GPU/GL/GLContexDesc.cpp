#include "GLContexDesc.h"

namespace Mmp
{

StencilSetup::StencilSetup()
{
    failOp      = StencilOp::KEEP;
    passOp      = StencilOp::KEEP;
    depthFailOp = StencilOp::KEEP;
    compareOp   = Comparison::NEVER;
}

SamplerDef::SamplerDef()
{
    falgs = SamplerFalgs::ARRAY_ON_VULKAN;
}

SamplerDef::SamplerDef(const std::string& name, SamplerFalgs falgs)
{
    this->name = name;
    this->falgs = falgs;
}

AttributeDesc::AttributeDesc()
{
    binding  = -1;
    location = -1;
    format   = DataFormat::UNDEFINED;
    offset   = -1;
}

AttributeDesc::AttributeDesc(const std::string& attribute, int binding, int location, DataFormat format, int offset)
{
    this->attribute = attribute;
    this->binding   = binding;
    this->location  = location;
    this->format    = format;
    this->offset    = offset;
}

UniformDesc::UniformDesc()
{
    type   = UniformType::FLOAT1;
    offset = -1;
}

UniformDesc::UniformDesc(const std::string& name, UniformType type, int16_t offset)
{
    this->name   = name;
    this->type   = type;
    this->offset = offset;
}

UniformBufferDesc::UniformBufferDesc()
{
    uniformBufferSize = 0;
}

DepthStencilStateDesc::DepthStencilStateDesc()
{
    depthTestEnabled  = false;
    depthWriteEnabled = false;
    depthCompare      = Comparison::ALWAYS;
    stencilEnabled    = false;
}

bool operator<(const DepthStencilStateDesc& left, const DepthStencilStateDesc& right)
{
    if ((char)left.depthTestEnabled < (char)right.depthTestEnabled)
    {
        return true;
    }
    else if ((char)left.depthWriteEnabled < (char)right.depthWriteEnabled)
    {
        return true;
    }
    else if ((uint64_t)left.depthCompare < (uint64_t)right.depthCompare)
    {
        return true;
    }
    else if ((char)left.stencilEnabled < (char)right.stencilEnabled)
    {
        return true;
    }
    else
    {
        return false;
    }
}

TextureDesc::TextureDesc()
{
    type         = TextureType::LINEAR2D;
    format       = DataFormat::UNDEFINED;
    width        = 0;
    height       = 0;
    depth        = 0;
    mipLevels    = -1;
    generateMips = false;
    miscFlag     = GlTextureFlags::NONE;
}

FrameBufferDesc::FrameBufferDesc()
{
    width            = -1;
    height           = -1;
    depth            = -1;
    numLayers        = -1;
    multiSampleLevel = -1;
    zStencil         = -1;
}

BlendStateDesc::BlendStateDesc()
{
    enabled      = false;
    colorMask    = 0xFF;
    srcCol       = BlendFactor::ZERO;
    dstCol       = BlendFactor::ZERO;
    eqCol        = BlendOp::ADD;
    srcAlpha     = BlendFactor::ONE;
    dstAlpha     = BlendFactor::ONE;
    eqAlpha      = BlendOp::ADD;
    logicEnabled = false;
    logicOp      = LogicOp::AND;
}

SamplerStateDesc::SamplerStateDesc()
{
    magFilter             = TextureFilter::NEAREST;
    minFilter             = TextureFilter::NEAREST;
    mipFilter             = TextureFilter::NEAREST;
    maxAniso              = -1.0;
    wrapU                 = TextureAddressMode::CLAMP_TO_EDGE;
    wrapV                 = TextureAddressMode::CLAMP_TO_EDGE;
    wrapW                 = TextureAddressMode::CLAMP_TO_EDGE;
    shadowCompareEnabled  = false;
    shadowCompareFunc     = Comparison::ALWAYS;
    borderColor           = BorderColor::DONT_CARE;
}

RasterStateDesc::RasterStateDesc()
{
    cull      = CullMode::NONE;
    frontFace = Facing::CCW;
}

RenderPassInfo::RenderPassInfo()
{
    color        = RPAction::DONT_CARE;
    depth        = RPAction::DONT_CARE;
    stencil      = RPAction::DONT_CARE;
    clearColor   = 0xFFFFFFFF;
    clearDepth   = 0.0f;
    clearStencil = 0;
}

} // namespace Mmp