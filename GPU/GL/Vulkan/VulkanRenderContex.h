//
// VulkanDrawContex.h
//
// Library: GPU
// Package: GL
// Module:  Vulkan
// 

#pragma once

#include "VulkanCommon.h"

namespace Mmp
{

class VulkanRenderImage
{
public:
    using ptr = std::shared_ptr<VulkanRenderImage>;
public:
    explicit VulkanRenderImage(std::shared_ptr<VulkanDrawContex> draw);
    ~VulkanRenderImage();
public:
    VkImage                   image;
    VkImageView               rtView;
    VkImageView               rtexAllLayerView;
    std::vector<VkImageView>  rtexLayerViews;
    VmaAllocation             alloc;
    VkFormat                  format;
    VkSampleCountFlagBits     sampleCount;
    VkImageLayout             layout;
    uint32_t                  numLayers;
private:
    std::shared_ptr<VulkanDrawContex> _draw;
};

class VulkanRenderGraphicsPipeline
{
public:
    using ptr = std::shared_ptr<VulkanRenderGraphicsPipeline>;
public:
    VulkanRenderGraphicsPipeline(std::shared_ptr<VulkanDrawContex> draw, VulkanPipelineFlags flags);
    ~VulkanRenderGraphicsPipeline();
public:
    bool Create(VkRenderPass compatibleRenderPass, VulkanRenderPassType rpType, VkSampleCountFlagBits sampleCount);
    void Destroy(bool msaaOnly);
    VkSampleCountFlagBits SampleCount();
public:
    std::shared_ptr<VulkanGraphicsPileline>     pipeline;
    std::map<VulkanRenderPassType, VkPipeline>  pipelines;
private:
    std::shared_ptr<VulkanDrawContex>      _draw;
    VulkanPipelineFlags                    _flags;
    VkSampleCountFlagBits                  _sampleCount;
};

} // namespace Mmp