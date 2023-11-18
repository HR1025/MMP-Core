//
// VulkanRenderData.h
//
// Library: GPU
// Package: GL
// Module:  Vulkan
// 

#pragma once

#include "VulkanCommon.h"

namespace Mmp
{

struct VulkanPipelineRenderData
{
    VkPipeline           pipeline;
    VkPipelineLayout     pipelineLayout;
};

struct VulkanGraphicsPipelineRenderData
{
    std::shared_ptr<VulkanGraphicsPileline>  pipeline;
    VkPipelineLayout                         pipelineLayout;
};

struct VulkanComputePipelineRenderData
{
    // TODO
};

struct VulkanDrawRenderData
{
    VkDescriptorSet    ds;
    int64_t            numUboOffsets;
    uint32_t           uboOffsets[3];
    VkBuffer           vbuffer;
    VkDeviceSize       voffset;
    uint32_t           count;
    uint32_t           offset;
};

struct VulkanDrawIndexedRenderData
{
    VkDescriptorSet     ds;
    uint32_t            uboOffsets[3];
    uint16_t            numUboOffsets;
    uint16_t            instance;
    VkBuffer            vbuffer;
    VkBuffer            ibuffer;
    uint32_t            voffset;
    uint32_t            ioffset;
    uint32_t            count;
};

struct VulkanClearRenderData
{
    uint32_t                     clearData;
    float                        clearZ;
    int64_t                      clearStencil;
    int64_t                      clearMask;
};

struct VulkanViewportRenderData
{
    VkViewport                    vp;
};

struct VulkanScissorRenderData
{
    GLRect2D                      scissor;
};

struct VulkanStencilRenderData
{
    uint8_t                       stencilWriteMask;
    uint8_t                       stencilCompareMask;
    uint8_t                       stencilRef;
};

struct VulkanBlendColorRenderData
{
    uint32_t                      blendColor;
};

struct VulkanPushRenderData
{
    VkShaderStageFlags         stages;
    uint8_t                    offset;
    uint8_t                    size;
    uint8_t                    data[40];
};

struct VulkanBindDescSetRenderData
{
    int64_t                    setNumber;
    VkDescriptorSet            set;
    VkPipelineLayout           pipelineLayout;
};

} // namespace Mmp