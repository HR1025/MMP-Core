//
// VulkanFrameBuffer.h
//
// Library: GPU
// Package: GL
// Module:  Vulkan
// 

#pragma once

#include "VulkanCommon.h"

namespace Mmp
{

class VulkanFrameBuffer
{
public:
    using ptr = std::shared_ptr<VulkanFrameBuffer>;
public:
    explicit VulkanFrameBuffer(std::shared_ptr<VulkanDrawContex> draw);
    ~VulkanFrameBuffer();
public:
    void Init(VkCommandBuffer initCmd, std::shared_ptr<VulkanRenderPass> compatibleRenderPass, uint32_t width, uint32_t height, uint32_t numLayers, VkSampleCountFlagBits sampleCount, bool createDepthStencilBuffer);
    VkFramebuffer Get(VulkanRenderPassType rpType);
public:
    uint32_t                                      numLayers;
    uint32_t                                      width;
    uint32_t                                      height;
    std::shared_ptr<VulkanRenderImage>            color;
    std::shared_ptr<VulkanRenderImage>            depth;
    std::shared_ptr<VulkanRenderImage>            msaaColor;
    std::shared_ptr<VulkanRenderImage>            msaaDepth;
    VkSampleCountFlagBits                         sampleCount;
private:
    void CreateImage(std::shared_ptr<VulkanDrawContex> draw, VkCommandBuffer cmd,  std::shared_ptr<VulkanRenderImage> img, uint32_t width, uint32_t height, uint32_t numLayers, VkSampleCountFlagBits sampleCount, VkFormat format, VkImageLayout initialLayout, bool color);
private:
    std::map<VulkanRenderPassType, VkFramebuffer>  _frameBuffers;
    std::shared_ptr<VulkanDrawContex>              _draw;
    std::shared_ptr<VulkanRenderPass>              _compatibleRenderPass;
};

} // namespace Mmp