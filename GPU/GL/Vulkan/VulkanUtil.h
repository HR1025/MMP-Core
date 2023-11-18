//
// VulkanUtil.h
//
// Library: GPU
// Package: GL
// Module:  Vulkan
//

#pragma once

#include "VulkanCommon.h"

namespace Mmp
{

void TransitionImageLayout2(VkCommandBuffer cmd, VkImage image, int baseMip, int numMipLevels, int numLayers, VkImageAspectFlags aspectMask,
	VkImageLayout oldImageLayout, VkImageLayout newImageLayout,
	VkPipelineStageFlags srcStageMask, VkPipelineStageFlags dstStageMask,
	VkAccessFlags srcAccessMask, VkAccessFlags dstAccessMask);

} // namespace Mmp