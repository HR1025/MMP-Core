//
// VulkanImage.h
//
// Library: GPU
// Package: GL
// Module:  Vulkan
// 

#pragma once

#include "VulkanCommon.h"
#include "VulkanDrawContex.h"

namespace Mmp
{

class TextureCopyBatch
{
public:
    void reserve(size_t mips);
    bool empty();
public:
    VkBuffer buffer = VK_NULL_HANDLE;
    std::vector<VkBufferImageCopy> copies;
};

class VulkanImage
{
public:
    using ptr = std::shared_ptr<VulkanImage>;
public:
    explicit VulkanImage(VulkanDrawContex::ptr draw);
    ~VulkanImage();
public:
    bool CreateDirect(VkCommandBuffer cmd, uint32_t w, uint32_t h, uint32_t depth, uint32_t numMips, VkFormat format, VkImageLayout initialLayout, VkImageUsageFlags usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, const VkComponentMapping *mapping = nullptr);
    void ClearMip(VkCommandBuffer cmd, int mip, uint32_t value);
    void CopyBufferToMipLevel(VkCommandBuffer cmd, TextureCopyBatch& copyBatch, int mip, int mipWidth, int mipHeight, int depthLayer, VkBuffer buffer, uint32_t offset, size_t rowLength);
    void FinishCopyBatch(VkCommandBuffer cmd, TextureCopyBatch& copyBatch);
    void GenerateMips(VkCommandBuffer cmd, int firstMipToGenerate, bool fromCompute);
    void EndCreate(VkCommandBuffer cmd, bool vertexTexture, VkPipelineStageFlags prevStage, VkImageLayout layout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
    void PrepareForTransferDst(VkCommandBuffer cmd, int levels);
    void RestoreAfterTransferDst(VkCommandBuffer cmd, int levels);
    VkImageView CreateViewForMip(int mip);
public:
    VkImage GetImage();
    VkImageView GetImageView();
    VkImageView GetImageArrayView();
    uint32_t GetWidth();
    uint32_t GetHeight();
    uint32_t GetNumMips();
    VkFormat GetFormat();
private:
    VulkanDrawContex::ptr  _draw;
    VkImage                _image;
    VkImageView            _view;
    VkImageView            _arrayView;
    VmaAllocation          _allocation;
    uint32_t               _width;
    uint32_t               _height;
    uint32_t               _numMips;
    uint32_t               _depth;
    VkFormat               _format;
};

} // namespace Mmp