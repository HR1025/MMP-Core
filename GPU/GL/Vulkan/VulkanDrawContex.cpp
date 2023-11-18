#include "VulkanDrawContex.h"

namespace Mmp
{

DepthStencilState::ptr VulkanDrawContex::CreateDepthStencilState(const DepthStencilStateDesc& desc)
{
    VulkanDepthStencilState::ptr ds = std::make_shared<VulkanDepthStencilState>();
    ds->info.depthCompareOp = GLComparisonTypeToVulkanType(desc.depthCompare);
    ds->info.depthTestEnable = desc.depthTestEnabled;
    ds->info.depthWriteEnable = desc.depthWriteEnabled;
    ds->info.stencilTestEnable = desc.stencilEnabled;
    ds->info.depthBoundsTestEnable = false;
    if (ds->info.stencilTestEnable)
    {
        ds->info.front.compareOp = GLComparisonTypeToVulkanType(desc.stencil.compareOp);
        ds->info.front.failOp = GLStencilOpToVulkanType(desc.stencil.failOp);
        ds->info.front.passOp = GLStencilOpToVulkanType(desc.stencil.passOp);
        ds->info.front.depthFailOp = GLStencilOpToVulkanType(desc.stencil.depthFailOp);
        ds->info.back.compareOp = GLComparisonTypeToVulkanType(desc.stencil.compareOp);
        ds->info.back.failOp = GLStencilOpToVulkanType(desc.stencil.failOp);
        ds->info.back.passOp = GLStencilOpToVulkanType(desc.stencil.passOp);
        ds->info.back.depthFailOp = GLStencilOpToVulkanType(desc.stencil.depthFailOp);
    }
    return ds;
}

BlendState::ptr VulkanDrawContex::CreateBlendState(const BlendStateDesc& desc)
{
    VulkanBlendState::ptr bs = std::make_shared<VulkanBlendState>();
    bs->info.attachmentCount = 1;
    bs->info.logicOp = GLLogicOpToOpenVulkanType(desc.logicOp);
    bs->info.logicOpEnable = desc.enabled;
    bs->attachments.resize(1);
    bs->attachments[0].blendEnable = desc.enabled;
    bs->attachments[0].colorBlendOp = GLBlendOpToVulkanType(desc.eqCol);
    bs->attachments[0].alphaBlendOp = GLBlendOpToVulkanType(desc.eqAlpha);
    bs->attachments[0].colorWriteMask = desc.colorMask;
    bs->attachments[0].dstAlphaBlendFactor = GLBlendFactorToVulkanType(desc.dstAlpha);
    bs->attachments[0].dstColorBlendFactor = GLBlendFactorToVulkanType(desc.dstCol);
    bs->attachments[0].srcAlphaBlendFactor = GLBlendFactorToVulkanType(desc.srcAlpha);
    bs->attachments[0].srcColorBlendFactor = GLBlendFactorToVulkanType(desc.srcCol);
    bs->info.pAttachments = bs->attachments.data();
    return bs;
}

SamplerState::ptr VulkanDrawContex::CreateSamplerState(const SamplerStateDesc& desc)
{
    VulkanSamplerState::ptr ss = std::make_shared<VulkanSamplerState>();
    VkSamplerCreateInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    info.addressModeU = GLTextureAddressModeToVulkanType(desc.wrapU);
    info.addressModeV = GLTextureAddressModeToVulkanType(desc.wrapV);
    info.addressModeW = GLTextureAddressModeToVulkanType(desc.wrapW);
    info.anisotropyEnable = desc.maxAniso > 1.0f;
    info.maxAnisotropy = desc.maxAniso;
    info.magFilter = desc.magFilter == TextureFilter::LINEAR ? VK_FILTER_LINEAR : VK_FILTER_NEAREST;
    info.minFilter = desc.minFilter == TextureFilter::LINEAR ? VK_FILTER_LINEAR : VK_FILTER_NEAREST;
    info.mipmapMode = desc.mipFilter == TextureFilter::LINEAR ? VK_SAMPLER_MIPMAP_MODE_LINEAR : VK_SAMPLER_MIPMAP_MODE_NEAREST;
    info.maxLod = VK_LOD_CLAMP_NONE;
    VkResult res = _vulkan->vkCreateSampler(_device, &info, nullptr, &(ss->sampler));
    if (res == VK_SUCCESS)
    {
        return ss;
    }
    else
    {
        assert(false);
        return nullptr;
    }
}

RasterState::ptr VulkanDrawContex::CreateRasterState(const RasterStateDesc& desc)
{
    VulkanRasterState::ptr rs = std::make_shared<VulkanRasterState>();
    rs->cullFace = desc.cull;
    rs->frontFace = desc.frontFace;
    return rs;
}

InputLayout::ptr VulkanDrawContex::CreateInputLayout(const InputLayoutDesc& desc)
{
    VulkanInputLayout::ptr il = std::make_shared<VulkanInputLayout>();
    il->visc.flags = 0;
    il->visc.vertexAttributeDescriptionCount = (uint32_t)desc.attributes.size();
    il->visc.vertexBindingDescriptionCount = (uint32_t)desc.bindings.size();
    il->bindings.resize(il->visc.vertexBindingDescriptionCount);
    il->attributes.resize(il->visc.vertexAttributeDescriptionCount);
    for (size_t i=0; i<desc.attributes.size(); i++)
    {
        il->attributes[i].binding = (uint32_t)desc.attributes[i].binding;
        il->attributes[i].format = DataFormatToVulkan(desc.attributes[i].format);
        il->attributes[i].location = desc.attributes[i].location;
        il->attributes[i].offset = desc.attributes[i].offset;
    }
    for (size_t i=0; i<desc.bindings.size(); i++)
    {
        il->bindings[i].inputRate = desc.bindings[i].instanceRate ? VK_VERTEX_INPUT_RATE_INSTANCE : VK_VERTEX_INPUT_RATE_VERTEX;
        il->bindings[i].binding = (uint32_t)i;
        il->bindings[i].stride = desc.bindings[i].stride;
    }
    return il;
}

ShaderModule::ptr VulkanDrawContex::CreateShaderModule(SL::ShaderStage stage, SL::ShaderLanguage lang, const std::string& code)
{
    VulkanShaderModule::ptr shader = std::make_shared<VulkanShaderModule>(stage, std::string());
    bool res = shader->Compile(_device, _vulkan, code);
    if (res)
    {
        return shader;
    }
    else
    {
        assert(false);
        return nullptr;
    }
}

Pipeline::ptr VulkanDrawContex::CreateGraphicsPipeline(const PipelineDesc& desc, const std::string& tag)
{
    VulkanInputLayout::ptr input = std::dynamic_pointer_cast<VulkanInputLayout>(desc.inputLayout);
    VulkanBlendState::ptr blend = std::dynamic_pointer_cast<VulkanBlendState>(desc.blend);
    VulkanDepthStencilState::ptr depth = std::dynamic_pointer_cast<VulkanDepthStencilState>(desc.depthStencil);
    VulkanRasterState::ptr raster = std::dynamic_pointer_cast<VulkanRasterState>(desc.raster);

    uint32_t pilelineFlags = VulkanPipelineFlags::VK_NONE;
    if (depth->info.depthBoundsTestEnable || depth->info.stencilTestEnable)
    {
        pilelineFlags |= VulkanPipelineFlags::VK_USES_DEPTH_STENCIL;
    }
    // TODO
    assert(false);
    return nullptr;
}

SL::ShaderLanguage VulkanDrawContex::GetShaderLanguage()
{
    return SL::ShaderLanguage::GLSL_VULKAN;
}

VulkanRenderPass::ptr VulkanDrawContex::GetRenderPass(VulkanRenderPassKey key)
{
    if (!_renderPasses.count(key))
    {
        assert(false); // TODO
        _renderPasses[key] = std::make_shared<VulkanRenderPass>(key, nullptr);
    }
    return _renderPasses[key];
}

VkDevice VulkanDrawContex::Device()
{
    return _device;
}

VmaAllocator VulkanDrawContex::Allocator()
{
    return _allocator;
}

void VulkanDrawContex::CreateDeviceObjects()
{
    VulkanRenderPassKey key = {};
    {
        key.colorLoadAction = RPAction::CLEAR;
        key.colorLoadAction = RPAction::CLEAR;
        key.colorLoadAction = RPAction::CLEAR;
        key.colorStoreAction = RPAction::STORE;
        key.colorStoreAction = RPAction::DONT_CARE;
        key.colorStoreAction = RPAction::DONT_CARE;
    }
    _compatibleRenderPass = GetRenderPass(key);
}

void VulkanDrawContex::DestoryDeviceObjects()
{
    _compatibleRenderPass.reset();
    _renderPasses.clear();
}

} // namespace Mmp