#include "VulkanRenderContex.h"

#include <cassert>

#include "VulkanDrawContex.h"
#include "VulkanInterface.h"

namespace Mmp
{

VulkanRenderImage::VulkanRenderImage(std::shared_ptr<VulkanDrawContex> draw)
{
    _draw = draw;
    image = VK_NULL_HANDLE;
    rtView = VK_NULL_HANDLE;
    rtexAllLayerView = VK_NULL_HANDLE;
    alloc = VK_NULL_HANDLE;
    format = VK_FORMAT_UNDEFINED;
    sampleCount = VK_SAMPLE_COUNT_1_BIT;
    layout = VK_IMAGE_LAYOUT_UNDEFINED;
    numLayers = 0;
}

VulkanRenderImage::~VulkanRenderImage()
{
    if (rtView != VK_NULL_HANDLE)
    {
        VulkanInterface::Instance()->vkDestroyImageView(_draw->Device(), rtView, nullptr);
    }
    if (rtexAllLayerView != VK_NULL_HANDLE)
    {
        VulkanInterface::Instance()->vkDestroyImageView(_draw->Device(), rtexAllLayerView, nullptr);
    }
    for (auto& rtexLayerView : rtexLayerViews)
    {
        if (rtexLayerView != VK_NULL_HANDLE)
        {
            VulkanInterface::Instance()->vkDestroyImageView(_draw->Device(), rtexLayerView, nullptr);
        }   
    }
    if (image != VK_NULL_HANDLE)
    {
        vmaDestroyImage(_draw->Allocator(), image, alloc);
    }
}

VulkanRenderGraphicsPipeline::VulkanRenderGraphicsPipeline(std::shared_ptr<VulkanDrawContex> draw, VulkanPipelineFlags flags)
{
    _draw = draw;
    _flags = flags;
    _sampleCount = VK_SAMPLE_COUNT_FLAG_BITS_MAX_ENUM;
}

VulkanRenderGraphicsPipeline::~VulkanRenderGraphicsPipeline()
{

}

bool VulkanRenderGraphicsPipeline::Create(VkRenderPass compatibleRenderPass, VulkanRenderPassType rpType, VkSampleCountFlagBits sampleCount)
{
    VkPipeline vkpipeline = VK_NULL_HANDLE;
    VkResult result = VK_SUCCESS;

    VkGraphicsPipelineCreateInfo pipe = {};
    VkPipelineShaderStageCreateInfo ss[3] = {};
    VkPipelineMultisampleStateCreateInfo ms = {};
    VkPipelineInputAssemblyStateCreateInfo inputAssembly = {};

    bool multisample = rpType == VulkanRenderPassType::MULTISAMPLE;
    if (multisample)
    {
        if (_sampleCount != VK_SAMPLE_COUNT_FLAG_BITS_MAX_ENUM)
        {
            assert(_sampleCount == sampleCount);
        }
        else
        {
            _sampleCount = sampleCount;
        }
    }
    if (!pipeline->vertexShader || !pipeline->fragementShader)
    {
        GL_LOG_INFO << "Can not create graphics pipeline, missing vs/fs!!!";
        assert(false);
        return false;
    }
    if (!compatibleRenderPass)
    {
        assert(false);
        return false;
    }
    VulkanShaderModule::ptr vs = pipeline->vertexShader;
    VulkanShaderModule::ptr fs = pipeline->fragementShader;
    VulkanShaderModule::ptr gs = pipeline->geometryShader;

    uint32_t stageCount = 2;
    {
        ss[0].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        ss[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
        ss[0].pSpecializationInfo = nullptr;
        ss[0].module = vs->smodule;
        ss[0].pName = "main";
    }
    {
        ss[1].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        ss[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
        ss[1].pSpecializationInfo = nullptr;
        ss[1].module = fs->smodule;
        ss[1].pName = "main";
    }
    {
        if (gs)
        {
            stageCount++;
            ss[2].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
            ss[2].stage = VK_SHADER_STAGE_GEOMETRY_BIT;
            ss[2].pSpecializationInfo = nullptr;
            ss[2].module = gs->smodule;
            ss[2].pName = "main";
        }
    }
    {
        ms.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
        if (multisample && _flags & VulkanPipelineFlags::VK_USES_DISCARD)
        {
            ms.sampleShadingEnable = true;
            ms.minSampleShading = 1.0f;
        }
    }
    {
        inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
        inputAssembly.topology = pipeline->topology;
    }
    {
        pipe.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
        pipe.pStages = ss;
        pipe.stageCount = stageCount;
        pipe.renderPass = compatibleRenderPass;
        pipe.basePipelineIndex = 0;
        pipe.pColorBlendState = &pipeline->cbs;
        pipe.pDepthStencilState = &pipeline->dss;
        pipe.pRasterizationState = &pipeline->rs;
        pipe.pVertexInputState = &pipeline->vis;
        pipe.pViewportState = &pipeline->views;
        pipe.pTessellationState = nullptr;
        pipe.pDynamicState = &pipeline->ds;
        pipe.pInputAssemblyState = &inputAssembly;
        pipe.pMultisampleState = &ms;
        pipe.layout = pipeline->pipelineLayout;
        pipe.basePipelineHandle = VK_NULL_HANDLE;
        pipe.basePipelineIndex = 0;
        pipe.subpass = 0;
    }
    result = VulkanInterface::Instance()->vkCreateGraphicsPipelines(_draw->Device(), pipeline->pipelineCache, 1, &pipe, nullptr, &vkpipeline);
    if (result != VK_SUCCESS)
    {
        // TODO : print more log
        GL_LOG_ERROR << "vkCreateGraphicsPipelines fail";
        assert(false);
        return false;
    }
    pipelines[rpType] = vkpipeline;
    return true;
}

void VulkanRenderGraphicsPipeline::Destroy(bool msaaOnly)
{
    std::map<VulkanRenderPassType, VkPipeline>  __pipelines;
    for (const auto& vkPipeline : pipelines)
    {
        if (msaaOnly && vkPipeline.first == VulkanRenderPassType::MULTISAMPLE)
        {
            VulkanInterface::Instance()->vkDestroyPipeline(_draw->Device(), vkPipeline.second, nullptr);
        }
        else
        {
            __pipelines[vkPipeline.first] = vkPipeline.second;
        }
    }
    pipelines.swap(__pipelines);
}

VkSampleCountFlagBits VulkanRenderGraphicsPipeline::SampleCount()
{
    return _sampleCount;
}

} // namespace Mmp