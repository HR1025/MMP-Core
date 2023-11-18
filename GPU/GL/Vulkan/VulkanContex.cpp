#include "VulkanContex.h"

#include <cassert>

namespace Mmp
{

VulkanBlendState::VulkanBlendState()
{
    info = {};
    info.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
}

VulkanBlendState::~VulkanBlendState()
{

}

VulkanDepthStencilState::VulkanDepthStencilState()
{
    info = {};
    info.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
}

VulkanDepthStencilState::~VulkanDepthStencilState()
{

}

VulkanRasterState::VulkanRasterState()
{

}

VulkanRasterState::~VulkanRasterState()
{

}

VulkanShaderModule::VulkanShaderModule(SL::ShaderStage stage, const std::string& tag)
{
    this->flag = SLShaderStageToVulkanType(stage);
    isOK = false;
    smodule = VK_NULL_HANDLE;
}

VulkanShaderModule::~VulkanShaderModule()
{
    if (smodule != VK_NULL_HANDLE)
    {
        // TODO
        assert(false);
    }
}

SL::ShaderStage VulkanShaderModule::GetStage()
{
    return stage;
}

bool VulkanShaderModule::Compile(VkDevice device, VulkanInterface::ptr vulkan, const std::string& code)
{
    source = code;
    VkShaderModuleCreateInfo info;
    {
        info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        info.pCode = (const uint32_t*)source.data();
        info.codeSize = source.size();
    }
    VkResult res = vulkan->vkCreateShaderModule(device, &info, nullptr, &smodule);
    assert(res == VK_SUCCESS);
    isOK = res == VK_SUCCESS;
    return res == VK_SUCCESS;
}

VulkanInputLayout::VulkanInputLayout()
{
    visc = {};
    visc.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
}

VulkanInputLayout::~VulkanInputLayout()
{

}

VulkanSamplerState::VulkanSamplerState()
{
    sampler = VK_NULL_HANDLE;
}

VulkanSamplerState::~VulkanSamplerState()
{
    if (sampler != VK_NULL_HANDLE)
    {
        // TODO
        assert(false);
    }
}

VulkanBuffer::VulkanBuffer(size_t size)
{
    this->size = size;
    if (size == 0)
    {
        data = nullptr;
    }
    else
    {
        data = new uint8_t[size];
    }
}

VulkanBuffer::~VulkanBuffer()
{
    if (data)
    {
        delete[] data;
    }
}

VulkanGraphicsPileline::VulkanGraphicsPileline()
{
    pipelineCache = VK_NULL_HANDLE;
    cbs = {};
    cbs.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    blend0 = {};
    dss = {};
    dss.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    ds = {};
    ds.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    rs = {};
    rs.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    ibd = {};
    vis = {};
    vis.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    views = {};
    views.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    pipelineLayout = VK_NULL_HANDLE;
    rpKey = {};
}

VulkanGraphicsPileline::~VulkanGraphicsPileline()
{

}

} // namespace Mmp

namespace Mmp
{

DescriptorSetKey::DescriptorSetKey()
{
    imageViews.resize(VULKAN_MAX_BOUND_TEXTURES);
    samplers.resize(VULKAN_MAX_BOUND_TEXTURES);
    for (int i=0; i<VULKAN_MAX_BOUND_TEXTURES; i++)
    {
        imageViews[i] = VK_NULL_HANDLE;
        samplers[i] = std::make_shared<VulkanSamplerState>();
    }
    buffer = std::make_shared<VulkanBuffer>();
}

bool DescriptorSetKey::operator<(const DescriptorSetKey &other) const
{
    for (int i=0; i<VULKAN_MAX_BOUND_TEXTURES; i++)
    {
        if (imageViews[i] > other.imageViews[i])
        {
            return true;
        }
        else if (imageViews[i] < other.imageViews[i])
        {
            return false;
        }
        if (samplers[i]->sampler > other.samplers[i]->sampler)
        {
            return true;
        }
        else if (samplers[i]->sampler < other.samplers[i]->sampler)
        {
            return false;
        }
    }
    if (buffer->data < other.buffer->data)
    {
        return true;
    }
    else if (buffer->data > other.buffer->data)
    {
        return false;
    }
    return false;
}

} // namespace Mmp