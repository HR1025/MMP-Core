//
// VulkanCommon.h
//
// Library: GPU
// Package: GL
// Module:  Vulkan
//

#pragma once

#include "VulkanCommon.h"

#include <vector>

namespace Mmp
{

class VulkanBarrier
{
public:
    VulkanBarrier();
    ~VulkanBarrier();
public:
    void TransitionImage(
		VkImage image, int baseMip, int numMipLevels, int numLayers, VkImageAspectFlags aspectMask,
		VkImageLayout oldImageLayout, VkImageLayout newImageLayout,
		VkAccessFlags srcAccessMask, VkAccessFlags dstAccessMask,
		VkPipelineStageFlags srcStageMask, VkPipelineStageFlags dstStageMask
	);
    void TransitionImageAuto(
        VkImage image, int baseMip, int numMipLevels, int numLayers, 
        VkImageAspectFlags aspectMask, VkImageLayout oldImageLayout, VkImageLayout newImageLayout
	);
    void Flush(VkCommandBuffer cmd);
private:
    VkPipelineStageFlags               _srcStageMask;
    VkPipelineStageFlags               _dstStageMask;
    std::vector<VkImageMemoryBarrier>  _imageBarriers;
    VkDependencyFlags                  _dependencyFlags;
};

} // namespace Mmp