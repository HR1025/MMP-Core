//
// VulkanImpl.h
//
// Library: GPU
// Package: GL
// Module:  Vulkan
//

#pragma once

#include "VulkanCommon.h"
#include "VulkanStepData.h"
#include "VulkanRenderData.h"

namespace Mmp
{

class Vulkan
{
public:
    using ptr = std::shared_ptr<Vulkan>;
public:
    Vulkan();
    ~Vulkan();
public:
private: /* Render Step, see VulkanRenderStep */
    void RenderStepRenderPass(VulkanRenderStep& step, VkCommandBuffer cmd);
    void RenderStepCopy(VulkanRenderStep& step, VkCommandBuffer cmd);
    void RenderStepBlit(VulkanRenderStep& step, VkCommandBuffer cmd);
    void RenderStepReadback(VulkanRenderStep& step, VkCommandBuffer cmd);
    void RenderStepReadbackImage(VulkanRenderStep& step, VkCommandBuffer cmd);
private:
    void SetupTransitionToTransferSrc(std::shared_ptr<VulkanRenderImage> img, VkImageAspectFlags aspect);
    void SetupTransitionToTransferDst(std::shared_ptr<VulkanRenderImage> img, VkImageAspectFlags aspect);
    void SetupTransferDstWriteAfterWrite(std::shared_ptr<VulkanRenderImage> img, VkImageAspectFlags aspect);
private:
    std::shared_ptr<VulkanRenderPass> PerformBindFramebufferAsRenderTarget(VulkanRenderStep& step, VkCommandBuffer cmd);
private:
    std::shared_ptr<VulkanRenderPass> GetRenderPass(const VulkanRenderPassKey& key);
    void TransitionToOptimal(VkCommandBuffer cmd, VkImage colorImage, VkImageLayout colorLayout, VkImage depthStencilImage, VkImageLayout depthStencilLayout, uint32_t numLayers);
    void TransitionFromOptimal(VkCommandBuffer cmd, VkImage colorImage, VkImageLayout colorLayout, VkImage depthStenciImage, VkImageLayout depthStencilLayout, uint32_t numLayers);
public:
    VkDevice     Device();
public:
    VkImage _backbufferImage;
private:
    std::shared_ptr<VulkanBarrier> _recordBarrier;
    std::map<VulkanRenderPassKey, std::shared_ptr<VulkanRenderPass>> _renderPassCaches;
private:
    std::shared_ptr<VulkanFrameBuffer>  _backBuffer;
    uint32_t                            _backBufferWidth;
    uint32_t                            _backBufferHeight;
private:
    VkDevice              _device;
};

} // namespace Mmp