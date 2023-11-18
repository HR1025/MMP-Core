#include "VulkanFrameBuffer.h"

#include <cassert>

#include "VulkanUtil.h"
#include "VulkanRenderPass.h"
#include "VulkanDrawContex.h"
#include "VulkanRenderContex.h"

namespace Mmp
{

VulkanFrameBuffer::VulkanFrameBuffer(std::shared_ptr<VulkanDrawContex> draw)
{
    numLayers = 0;
    width = 0;
    height = 0;
    _draw = draw;
    color = std::make_shared<VulkanRenderImage>(_draw);
    depth = std::make_shared<VulkanRenderImage>(_draw);
    msaaColor = std::make_shared<VulkanRenderImage>(_draw);
    msaaDepth = std::make_shared<VulkanRenderImage>(_draw);
}

VulkanFrameBuffer::~VulkanFrameBuffer()
{

}

void VulkanFrameBuffer::CreateImage(std::shared_ptr<VulkanDrawContex> draw, VkCommandBuffer cmd,  std::shared_ptr<VulkanRenderImage> img, uint32_t width, uint32_t height, uint32_t numLayers, VkSampleCountFlagBits sampleCount, VkFormat format, VkImageLayout initialLayout, bool color)
{
    VkImageAspectFlags aspects = color ? VK_IMAGE_ASPECT_COLOR_BIT : (VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT);
    VkImageCreateInfo ici = {};
    VkImageViewCreateInfo ivci = {};
    VmaAllocationCreateInfo allocCreateInfo = {};
    VmaAllocationInfo allocInfo = {};
    VkResult res = VK_SUCCESS;
    {
        ici.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        ici.arrayLayers = numLayers;
        ici.mipLevels = 1;
        ici.extent.width = width;
        ici.extent.height = height;
        ici.extent.depth = 1;
        ici.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        ici.imageType = VK_IMAGE_TYPE_2D;
        ici.samples = sampleCount;
        ici.tiling = VK_IMAGE_TILING_OPTIMAL;
        ici.format = format;
        ici.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
        if (sampleCount == VK_SAMPLE_COUNT_1_BIT)
        {
            ici.usage |= VK_IMAGE_USAGE_SAMPLED_BIT;
        }
        if (color)
        {
            ici.usage |= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
            if (sampleCount == VK_SAMPLE_COUNT_1_BIT)
            {
                ici.usage |= VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT;
            }
        }
        else
        {
            ici.usage |= VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
        }
    }
    {
        allocCreateInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;
    }
    res = vmaCreateImage(draw->Allocator(), &ici, &allocCreateInfo, &img->image, &img->alloc, &allocInfo);
    assert(res == VK_SUCCESS);

    {
        ivci.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        ivci.components = { VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY};
        ivci.format = ici.format;
        ivci.image = img->image;
        ivci.viewType = numLayers == 1 ? VK_IMAGE_VIEW_TYPE_2D : VK_IMAGE_VIEW_TYPE_2D_ARRAY;
        ivci.subresourceRange.aspectMask = aspects;
        ivci.subresourceRange.layerCount = numLayers;
        ivci.subresourceRange.levelCount = 1;
    }
    res = VulkanInterface::Instance()->vkCreateImageView(draw->Device(), &ivci, nullptr, &img->rtView);
    assert(res == VK_SUCCESS);

    {
        if (!color)
        {
            ivci.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
        }
        ivci.viewType = VK_IMAGE_VIEW_TYPE_2D_ARRAY;
    }
    res = VulkanInterface::Instance()->vkCreateImageView(draw->Device(), &ivci, nullptr, &img->rtexAllLayerView);

    {
        img->rtexLayerViews.resize(numLayers);
        for (uint32_t i=0; i<numLayers; i++)
        {
            ivci.viewType = VK_IMAGE_VIEW_TYPE_2D;
            ivci.subresourceRange.layerCount = 1;
            ivci.subresourceRange.baseArrayLayer = i;
            res = VulkanInterface::Instance()->vkCreateImageView(draw->Device(), &ivci, nullptr, &img->rtexLayerViews[i]);
            assert(res == VK_SUCCESS);
        }
    }

    {
        VkPipelineStageFlags dstStage;
        VkAccessFlagBits dstAccessMask;
        switch (initialLayout)
        {
            case VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL:
            {
                dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
                dstStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
                break;
            }
            case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
            {
                dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
                dstStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
            }
            case VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL:
            {
                dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
                dstStage = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
                break;
            }
            default:
                assert(false);
                break;
        }
        TransitionImageLayout2(cmd, img->image, 0, 1, numLayers, aspects,
            VK_IMAGE_LAYOUT_UNDEFINED, initialLayout,
            VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, dstStage,
            0, dstAccessMask);
        img->layout = initialLayout;
        img->format = format;
        img->sampleCount = sampleCount;
        img->numLayers = numLayers;
    }
}

void VulkanFrameBuffer::Init(VkCommandBuffer initCmd, std::shared_ptr<VulkanRenderPass> compatibleRenderPass, uint32_t width, uint32_t height, uint32_t numLayers, VkSampleCountFlagBits sampleCount, bool createDepthStencilBuffer)
{
    this->numLayers = numLayers;
    this->width = width;
    this->height = height;
    this->sampleCount = sampleCount;
    this->_compatibleRenderPass = compatibleRenderPass;
    CreateImage(_draw, initCmd, color, width, height, numLayers, VK_SAMPLE_COUNT_1_BIT, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, true);
    if (createDepthStencilBuffer)
    {
        assert(false);
        CreateImage(_draw, initCmd, depth, width, height, numLayers, VK_SAMPLE_COUNT_1_BIT, VK_FORMAT_UNDEFINED, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL, false);
    }
    if (sampleCount != VK_SAMPLE_COUNT_1_BIT)
    {
		CreateImage(_draw, initCmd, msaaColor, width, height, numLayers, sampleCount, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, true);
		if (createDepthStencilBuffer) 
        {
            assert(false);
			CreateImage(_draw, initCmd, msaaDepth, width, height, numLayers, sampleCount, VK_FORMAT_UNDEFINED, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL, false);
		}
    }
}

VkFramebuffer VulkanFrameBuffer::Get(VulkanRenderPassType rpType)
{
    if (!_frameBuffers.count(rpType))
    {
        bool multiview = rpType == VulkanRenderPassType::MULTIVIEW;
        bool hasDepth = rpType == VulkanRenderPassType::HAS_DEPTH || rpType == VulkanRenderPassType::BACKBUFFER;

        VkFramebufferCreateInfo fbci = {};
        fbci.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        int attachmentCount = 0;
        VkImageView views[4] = {};

        views[attachmentCount++] = color->rtView;
        if (hasDepth) 
        {
            if (!depth->rtView) 
            {
                assert(false);
            }
            views[attachmentCount++] = depth->rtView;
        }
        if (rpType == VulkanRenderPassType::MULTISAMPLE) {

            views[attachmentCount++] = msaaColor->rtView;
            if (hasDepth) 
            {
                views[attachmentCount++] = msaaDepth->rtView;
            }
        }

        fbci.renderPass = _compatibleRenderPass->Get(rpType, sampleCount);
        fbci.attachmentCount = attachmentCount;
        fbci.pAttachments = views;
        fbci.width = width;
        fbci.height = height;
        fbci.layers = 1;

        VkResult res = VulkanInterface::Instance()->vkCreateFramebuffer(_draw->Device(), &fbci, nullptr, &_frameBuffers[rpType]);
        assert(res == VK_SUCCESS);
    }
    return _frameBuffers[rpType];
}

} // namespace Mmp