#include "VulkanBarrier.h"

#include "VulkanInterface.h"

#include <cassert>

namespace Mmp
{

VulkanBarrier::VulkanBarrier()
{
    _srcStageMask = 0;
    _dstStageMask = 0;
    _dependencyFlags = 0;
}

VulkanBarrier::~VulkanBarrier()
{

}

void VulkanBarrier::TransitionImage(
    VkImage image, int baseMip, int numMipLevels, int numLayers, VkImageAspectFlags aspectMask,
    VkImageLayout oldImageLayout, VkImageLayout newImageLayout,
    VkAccessFlags srcAccessMask, VkAccessFlags dstAccessMask,
    VkPipelineStageFlags srcStageMask, VkPipelineStageFlags dstStageMask
)
{
    _srcStageMask |= srcStageMask;
    _dstStageMask |= dstStageMask;
    _dependencyFlags |= VK_DEPENDENCY_BY_REGION_BIT;

    VkImageMemoryBarrier imageBarrier = {};
    {
        imageBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        imageBarrier.pNext = nullptr;
        imageBarrier.srcAccessMask = srcAccessMask;
        imageBarrier.dstAccessMask = dstAccessMask;
        imageBarrier.oldLayout = oldImageLayout;
        imageBarrier.newLayout = newImageLayout;
        imageBarrier.image = image;
        imageBarrier.subresourceRange.aspectMask = aspectMask;
        imageBarrier.subresourceRange.baseMipLevel = baseMip;
        imageBarrier.subresourceRange.levelCount = numMipLevels;
        imageBarrier.subresourceRange.layerCount = numLayers;
        imageBarrier.subresourceRange.baseArrayLayer = 0;
        imageBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        imageBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    }
    _imageBarriers.push_back(imageBarrier);
}

void VulkanBarrier::TransitionImageAuto(
    VkImage image, int baseMip, int numMipLevels, int numLayers, 
    VkImageAspectFlags aspectMask, VkImageLayout oldImageLayout, VkImageLayout newImageLayout
)
{
    VkAccessFlags srcAccessMask = 0;
    VkAccessFlags dstAccessMask = 0;

    switch (oldImageLayout) 
    {
        case VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL:
            srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_COLOR_ATTACHMENT_READ_BIT;
            _srcStageMask |= VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
            break;
        case VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL:
            srcAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT;
            _srcStageMask |= VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
            break;
        case VK_IMAGE_LAYOUT_UNDEFINED:
            if (aspectMask == VK_IMAGE_ASPECT_COLOR_BIT) 
            {
                srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_COLOR_ATTACHMENT_READ_BIT;
                _srcStageMask |= VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
            }
            break;
        case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
            srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
            _srcStageMask |= VK_PIPELINE_STAGE_TRANSFER_BIT;
            break;
        case VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL:
            srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
            _srcStageMask |= VK_PIPELINE_STAGE_TRANSFER_BIT;
            break;
        default:
            assert(false);
            break;
    }

    switch (newImageLayout) 
    {
        case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL:
            dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
            _dstStageMask |= VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
            break;
        default:
            assert(false);
            break;
    }

    VkImageMemoryBarrier imageBarrier = {};
    {
        imageBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        imageBarrier.pNext = nullptr;
        imageBarrier.srcAccessMask = srcAccessMask;
        imageBarrier.dstAccessMask = dstAccessMask;
        imageBarrier.oldLayout = oldImageLayout;
        imageBarrier.newLayout = newImageLayout;
        imageBarrier.image = image;
        imageBarrier.subresourceRange.aspectMask = aspectMask;
        imageBarrier.subresourceRange.baseMipLevel = baseMip;
        imageBarrier.subresourceRange.levelCount = numMipLevels;
        imageBarrier.subresourceRange.layerCount = numLayers;
        imageBarrier.subresourceRange.baseArrayLayer = 0;
        imageBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        imageBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    }
    _imageBarriers.push_back(imageBarrier);
}

void VulkanBarrier::Flush(VkCommandBuffer cmd)
{
    if (!_imageBarriers.empty())
    {
        VulkanInterface::Instance()->vkCmdPipelineBarrier(cmd, _srcStageMask, _dstStageMask, _dependencyFlags,
            0, nullptr, 0, nullptr, (uint32_t)_imageBarriers.size(), _imageBarriers.data()
        );
    }
    _imageBarriers.clear();
    _srcStageMask = 0;
    _dstStageMask = 0;
    _dependencyFlags = 0;
}

} // namespace Mmp