#include "VulkanImage.h"

#include <cassert>
#include <functional>

#include "VulkanInterface.h"
#include "VulkanUtil.h"

namespace Mmp
{

void TextureCopyBatch::reserve(size_t mips)
{
    copies.reserve(mips);
}

bool TextureCopyBatch::empty()
{
    return copies.empty();
}

VulkanImage::VulkanImage(VulkanDrawContex::ptr draw)
{
    _draw = draw;
    _image = VK_NULL_HANDLE;
    _view = VK_NULL_HANDLE;
    _arrayView = VK_NULL_HANDLE;
    _allocation = VK_NULL_HANDLE;
    _width = 0;
    _height = 0;
    _numMips = 0;
    _depth = 1;
    _format = VK_FORMAT_UNDEFINED;
}

VulkanImage::~VulkanImage()
{
    assert(false);
    // TODO
}

bool VulkanImage::CreateDirect(VkCommandBuffer cmd, uint32_t w, uint32_t h, uint32_t depth, uint32_t numMips, VkFormat format, VkImageLayout initialLayout, VkImageUsageFlags usage, const VkComponentMapping *mapping)
{
    VkResult res = VK_SUCCESS;
    VkImageCreateInfo imageCreateInfo = {};
    VkImageViewCreateInfo viewInfo = {};
    if (w == 0 || h == 0 || numMips == 0)
    {
        assert(false);
        return false;
    }
    if (w > 4096 || h > 4096)
    {
        assert(false);
        return false;
    }
    {
        _width = w;
        _height = h;
        _depth = depth;
        _numMips = numMips;
        _format = format;
    }
    {
        imageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        imageCreateInfo.imageType = depth > 1 ? VK_IMAGE_TYPE_3D : VK_IMAGE_TYPE_2D;
        imageCreateInfo.format = _format;
        imageCreateInfo.extent.width = _width;
        imageCreateInfo.extent.height = _height;
        imageCreateInfo.extent.depth = _depth;
        imageCreateInfo.mipLevels = numMips;
        imageCreateInfo.samples = VK_SAMPLE_COUNT_1_BIT;
        imageCreateInfo.flags = 0;
        imageCreateInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
        imageCreateInfo.usage = usage;
        if (initialLayout == VK_IMAGE_LAYOUT_PREINITIALIZED)
        {
            imageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_PREINITIALIZED;
        }
        else
        {
            imageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        }
        imageCreateInfo.usage |= VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
    }
    {
        VmaAllocationCreateInfo allocCreateInfo = {};
        VmaAllocationInfo allocInfo  = {};
        allocCreateInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;
        res = vmaCreateImage(_draw->Allocator(), &imageCreateInfo, &allocCreateInfo, &_image, &_allocation, &allocInfo);
        if (res != VK_SUCCESS)
        {
            GL_LOG_WARN << "vmaCreateImage fail";
            assert(false);
            return false;
        }
    }
    {
        if (initialLayout != VK_IMAGE_LAYOUT_UNDEFINED && initialLayout != VK_IMAGE_LAYOUT_PREINITIALIZED)
        {
            switch (initialLayout)
            {
                case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
                {
                    TransitionImageLayout2(cmd, _image, 0, numMips, 1, VK_IMAGE_ASPECT_COLOR_BIT,
                        VK_IMAGE_LAYOUT_UNDEFINED, initialLayout,
                        VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT,
                        0, VK_ACCESS_TRANSFER_WRITE_BIT);
                    break;
                }
		        case VK_IMAGE_LAYOUT_GENERAL:
                {
                    TransitionImageLayout2(cmd, _image, 0, numMips, 1, VK_IMAGE_ASPECT_COLOR_BIT,
                        VK_IMAGE_LAYOUT_UNDEFINED, initialLayout,
                        VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
                        0, VK_ACCESS_SHADER_WRITE_BIT);
                    break;
                }
                default:
                {
                    assert(false);
                    break;
                }
            }
        }
    }
    {
        viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        viewInfo.image = _image;
        viewInfo.viewType = _depth > 1 ? VK_IMAGE_VIEW_TYPE_3D : VK_IMAGE_VIEW_TYPE_2D;
        viewInfo.format = _format;
        if (mapping)
        {
            viewInfo.components = *mapping;
        }
        else
        {
            viewInfo.components = {VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY};
        }
        viewInfo.subresourceRange.aspectMask = IsDepthStencilFormat(_format) ? VK_IMAGE_ASPECT_DEPTH_BIT : VK_IMAGE_ASPECT_COLOR_BIT;
        viewInfo.subresourceRange.baseMipLevel = 0;
        viewInfo.subresourceRange.levelCount = numMips;
        viewInfo.subresourceRange.baseArrayLayer = 0;
        viewInfo.subresourceRange.layerCount = 1;
        res = VulkanInterface::Instance()->vkCreateImageView(_draw->Device(), &viewInfo, nullptr, &_view);
        if (res != VK_SUCCESS)
        {
            GL_LOG_WARN << "vkCreateImageView fail";
            assert(false);
            return false;
        }
        if (viewInfo.viewType == VK_IMAGE_VIEW_TYPE_2D)
        {
            viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D_ARRAY;
            res = VulkanInterface::Instance()->vkCreateImageView(_draw->Device(), &viewInfo, nullptr, &_arrayView);
            if (res != VK_SUCCESS)
            {
                GL_LOG_WARN << "vkCreateImageView fail";
                assert(false);
            }
        }
    }
    return true;
}

void VulkanImage::ClearMip(VkCommandBuffer cmd, int mip, uint32_t value)
{
    VkImageSubresourceRange range = {};
    VkClearColorValue clearVal = {};
    for (int i=0; i<4; i++)
    {
        clearVal.float32[i] = ((value >> (i*8) & 0XFF)) / 255.0f;
    }
    {
        range.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        range.layerCount = 1;
        range.baseMipLevel = mip;
        range.levelCount = 1;
    }
    VulkanInterface::Instance()->vkCmdClearColorImage(cmd, _image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, &clearVal, 1, &range);
}

void VulkanImage::CopyBufferToMipLevel(VkCommandBuffer cmd, TextureCopyBatch& copyBatch, int mip, int mipWidth, int mipHeight, int depthLayer, VkBuffer buffer, uint32_t offset, size_t rowLength)
{
    assert(mip < (int)_numMips);
    VkBufferImageCopy copyRegion = {};
    {
        copyRegion.bufferOffset = offset;
        copyRegion.bufferRowLength = (uint32_t)rowLength;
        copyRegion.bufferImageHeight = 0; // 2D
        copyRegion.imageOffset.z = depthLayer;
        copyRegion.imageExtent.width = mipWidth;
        copyRegion.imageExtent.height = mipHeight;
        copyRegion.imageExtent.depth = 1;
        copyRegion.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        copyRegion.imageSubresource.mipLevel = mip;
        copyRegion.imageSubresource.baseArrayLayer = 0;
        copyRegion.imageSubresource.layerCount = 1;
    }
    if (copyBatch.buffer == VK_NULL_HANDLE)
    {
        copyBatch.buffer = buffer;
    }
    else if (copyBatch.buffer != buffer)
    {
        FinishCopyBatch(cmd, copyBatch);
        copyBatch.buffer = buffer;
    }
    copyBatch.copies.push_back(copyRegion);
}

void VulkanImage::FinishCopyBatch(VkCommandBuffer cmd, TextureCopyBatch& copyBatch)
{
    if (!copyBatch.copies.empty())
    {
        VulkanInterface::Instance()->vkCmdCopyBufferToImage(cmd, copyBatch.buffer, _image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, (uint32_t)copyBatch.copies.size(), copyBatch.copies.data());
        copyBatch.copies.clear();
    }
}

void VulkanImage::GenerateMips(VkCommandBuffer cmd, int firstMipToGenerate, bool fromCompute)
{
    assert(firstMipToGenerate > 0);
    assert(firstMipToGenerate < (int)_numMips);

	TransitionImageLayout2(cmd, _image, 0, firstMipToGenerate, 1, VK_IMAGE_ASPECT_COLOR_BIT,
		fromCompute ? VK_IMAGE_LAYOUT_GENERAL : VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
		VK_IMAGE_LAYOUT_GENERAL,
		fromCompute ? VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT : VK_PIPELINE_STAGE_TRANSFER_BIT, 
		VK_PIPELINE_STAGE_TRANSFER_BIT,
		fromCompute ? VK_ACCESS_SHADER_WRITE_BIT : VK_ACCESS_TRANSFER_WRITE_BIT,
		VK_ACCESS_TRANSFER_READ_BIT);

	TransitionImageLayout2(cmd, _image, firstMipToGenerate, _numMips - firstMipToGenerate, 1,
		VK_IMAGE_ASPECT_COLOR_BIT,
		VK_IMAGE_LAYOUT_UNDEFINED,
		VK_IMAGE_LAYOUT_GENERAL,
		VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
		VK_PIPELINE_STAGE_TRANSFER_BIT,
		0,
		VK_ACCESS_TRANSFER_WRITE_BIT);

    for (int mip = firstMipToGenerate; mip < (int)_numMips; mip++)
    {
        VkImageBlit blit = {};
        blit.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        blit.srcSubresource.layerCount = 1;
        blit.srcSubresource.mipLevel = _numMips - 1;
        blit.srcOffsets[1].x = _width >> (mip-1) ? _width >> (mip-1) : 1;
        blit.srcOffsets[1].y = _height >> (mip-1) ? _height >> (mip-1) : 1;
        blit.srcOffsets[1].z = 1;

        blit.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        blit.dstSubresource.layerCount = 1;
        blit.dstSubresource.mipLevel = mip;
        blit.dstOffsets[1].x = _width >> mip ? _width >> mip : 1;
        blit.dstOffsets[1].y = _height >> mip ? _height >> mip : 1;
        blit.dstOffsets[1].z = 1;

        VulkanInterface::Instance()->vkCmdBlitImage(cmd, _image, VK_IMAGE_LAYOUT_GENERAL, _image, VK_IMAGE_LAYOUT_GENERAL, 1, &blit, VK_FILTER_LINEAR);
		
        TransitionImageLayout2(cmd, _image, mip, 1, 1, VK_IMAGE_ASPECT_COLOR_BIT,
			VK_IMAGE_LAYOUT_GENERAL, VK_IMAGE_LAYOUT_GENERAL,
			VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT,
			VK_ACCESS_TRANSFER_WRITE_BIT, VK_ACCESS_TRANSFER_READ_BIT);
    }
}

void VulkanImage::EndCreate(VkCommandBuffer cmd, bool vertexTexture, VkPipelineStageFlags prevStage, VkImageLayout layout)
{
    TransitionImageLayout2(cmd, _image, 0, _numMips, 1, 
        VK_IMAGE_ASPECT_COLOR_BIT,
        layout, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
		prevStage, vertexTexture ? VK_PIPELINE_STAGE_VERTEX_SHADER_BIT : VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
		prevStage == VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT ? VK_ACCESS_SHADER_WRITE_BIT : VK_ACCESS_TRANSFER_WRITE_BIT, VK_ACCESS_SHADER_READ_BIT
    );
}

void VulkanImage::PrepareForTransferDst(VkCommandBuffer cmd, int levels) 
{
	TransitionImageLayout2(cmd, _image, 0, levels, 1,
		VK_IMAGE_ASPECT_COLOR_BIT,
		VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
		VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT,
		VK_ACCESS_SHADER_READ_BIT, VK_ACCESS_TRANSFER_WRITE_BIT);
}

void VulkanImage::RestoreAfterTransferDst(VkCommandBuffer cmd, int levels) 
{
	TransitionImageLayout2(cmd, _image, 0, levels, 1,
		VK_IMAGE_ASPECT_COLOR_BIT,
		VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
		VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
		VK_ACCESS_TRANSFER_WRITE_BIT, VK_ACCESS_SHADER_READ_BIT);
}

VkImageView VulkanImage::CreateViewForMip(int mip)
{
    VkImageView view = VK_NULL_HANDLE;
    VkResult res = VK_SUCCESS;
    VkImageViewCreateInfo viewInfo = {};
    {
        viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        viewInfo.image = _image;
        viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        viewInfo.format = _format;
        viewInfo.components.r = VK_COMPONENT_SWIZZLE_R;
        viewInfo.components.g = VK_COMPONENT_SWIZZLE_G;
        viewInfo.components.b = VK_COMPONENT_SWIZZLE_B;
        viewInfo.components.a = VK_COMPONENT_SWIZZLE_A;
        viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        viewInfo.subresourceRange.baseMipLevel = mip;
        viewInfo.subresourceRange.levelCount = 1;
        viewInfo.subresourceRange.baseArrayLayer = 0;
        viewInfo.subresourceRange.layerCount = 1;
    }
    res = VulkanInterface::Instance()->vkCreateImageView(_draw->Device(), &viewInfo, nullptr, &view);
    assert(res == VK_SUCCESS && view != VK_NULL_HANDLE);
    return view;
}

VkImage VulkanImage::GetImage()
{
    return _image;
}

VkImageView VulkanImage::GetImageView()
{
    return _view;
}

VkImageView VulkanImage::GetImageArrayView()
{
    return _arrayView;
}

uint32_t VulkanImage::GetWidth()
{
    return _width;
}

uint32_t VulkanImage::GetHeight()
{
    return _height;
}

uint32_t VulkanImage::GetNumMips()
{
    return _numMips;
}

VkFormat VulkanImage::GetFormat()
{
    return _format;
}

} // namespace Mmp