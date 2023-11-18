//
// VulkanInterface.h
//
// Library: GPU
// Package: Windows
// Module:  Vulkan
//

#pragma once

#include <Poco/SharedLibrary.h>

#include "VulkanWindowsCommon.h"

namespace Mmp
{
/**
 * @todo 补充其他 vulkan 接口的加载 (,如果有需要的话)
 */
class VulkanInterfaceImpl : public VulkanInterface
{
public:
    VulkanInterfaceImpl();
    ~VulkanInterfaceImpl();
public:
    void LoadBasicFunc();
    void LoadInstanceFunc(VkInstance instance);
    void LoadDeviceFunc(VkDevice device, const VulkanExtensions &enabledExtensions);
public: /* Initialization */
    PFN_vkVoidFunction vkGetInstanceProcAddr(
        VkInstance instance,
        const char* pName
    ) override;
    VkResult vkCreateInstance(
        const VkInstanceCreateInfo* pCreateInfo,
        const VkAllocationCallbacks* pAllocator,
        VkInstance* pInstance
    ) override;
    void vkDestroyInstance(
        VkInstance instance,
        const VkAllocationCallbacks* pAllocator
    ) override;
    VkResult vkEnumerateInstanceVersion(
        uint32_t* pApiVersion
    ) override;
public: /* Fundamentals */
    PFN_vkVoidFunction vkGetDeviceProcAddr(
        VkDevice device,
        const char* pName
    ) override;
public: /* Devices and Queues */
    VkResult vkEnumeratePhysicalDevices(
        VkInstance instance,
        uint32_t* pPhysicalDeviceCount,
        VkPhysicalDevice* pPhysicalDevices
    ) override;
    void vkGetPhysicalDeviceProperties(
        VkPhysicalDevice physicalDevice,
        VkPhysicalDeviceProperties* pProperties
    ) override;
    void vkGetPhysicalDeviceQueueFamilyProperties(
        VkPhysicalDevice physicalDevice,
        uint32_t* pQueueFamilyPropertyCount,
        VkQueueFamilyProperties* pQueueFamilyProperties
    ) override;
    VkResult vkCreateDevice(
        VkPhysicalDevice physicalDevice,
        const VkDeviceCreateInfo* pCreateInfo,
        const VkAllocationCallbacks* pAllocator,
        VkDevice* pDevice
    ) override;
    void vkDestroyDevice(
        VkDevice device,
        const VkAllocationCallbacks* pAllocator
    ) override;
    void vkGetDeviceQueue(
        VkDevice device,
        uint32_t queueFamilyIndex,
        uint32_t queueIndex,
        VkQueue* pQueue
    ) override;
public: /* Command Buffers */
    VkResult vkQueueSubmit(
        VkQueue queue,
        uint32_t submitCount,
        const VkSubmitInfo* pSubmits,
        VkFence fence
    ) override;
    VkResult vkCreateCommandPool(
        VkDevice device,
        const VkCommandPoolCreateInfo* pCreateInfo,
        const VkAllocationCallbacks* pAllocator,
        VkCommandPool* pCommandPool
    ) override;
    void vkDestroyCommandPool(
        VkDevice device,
        VkCommandPool commandPool,
        const VkAllocationCallbacks* pAllocator
    ) override;
    VkResult vkResetCommandPool(
        VkDevice device,
        VkCommandPool commandPool,
        VkCommandPoolResetFlags flags
    ) override;
    VkResult vkAllocateCommandBuffers(
        VkDevice device,
        const VkCommandBufferAllocateInfo* pAllocateInfo,
        VkCommandBuffer* pCommandBuffers
    ) override;
    void vkFreeCommandBuffers(
        VkDevice device,
        VkCommandPool commandPool,
        uint32_t commandBufferCount,
        const VkCommandBuffer* pCommandBuffers
    ) override;
    VkResult vkBeginCommandBuffer(
        VkCommandBuffer commandBuffer,
        const VkCommandBufferBeginInfo* pBeginInfo
    ) override;
    VkResult vkEndCommandBuffer(
        VkCommandBuffer commandBuffer
    ) override;
    VkResult vkResetCommandBuffer(
        VkCommandBuffer commandBuffer,
        VkCommandBufferResetFlags flags
    ) override;
    void vkCmdExecuteCommands(
        VkCommandBuffer commandBuffer,
        uint32_t commandBufferCount,
        const VkCommandBuffer* pCommandBuffers
    ) override;
public: /* Synchronization and Cache Control */
    VkResult vkQueueWaitIdle(
        VkQueue queue
    ) override;
    VkResult vkDeviceWaitIdle(
        VkDevice device
    ) override;
    VkResult vkCreateFence(
        VkDevice device,
        const VkFenceCreateInfo* pCreateInfo,
        const VkAllocationCallbacks* pAllocator,
        VkFence* pFence
    ) override;
    void vkDestroyFence(
        VkDevice device,
        VkFence fence,
        const VkAllocationCallbacks* pAllocator
    ) override;
    VkResult vkResetFences(
        VkDevice device,
        uint32_t fenceCount,
        const VkFence* pFences
    ) override;
    VkResult vkGetFenceStatus(
        VkDevice device,
        VkFence fence
    ) override;
    VkResult vkWaitForFences(
        VkDevice device,
        uint32_t fenceCount,
        const VkFence* pFences,
        VkBool32 waitAll,
        uint64_t timeout
    ) override;
    VkResult vkCreateSemaphore(
        VkDevice device,
        const VkSemaphoreCreateInfo* pCreateInfo,
        const VkAllocationCallbacks* pAllocator,
        VkSemaphore* pSemaphore
    ) override;
    void vkDestroySemaphore(
        VkDevice device,
        VkSemaphore semaphore,
        const VkAllocationCallbacks* pAllocator
    ) override;
    VkResult vkCreateEvent(
        VkDevice device,
        const VkEventCreateInfo* pCreateInfo,
        const VkAllocationCallbacks* pAllocator,
        VkEvent* pEvent
    ) override;
    void vkDestroyEvent(
        VkDevice device,
        VkEvent event,
        const VkAllocationCallbacks* pAllocator
    ) override;
    VkResult vkGetEventStatus(
        VkDevice device,
        VkEvent event
    ) override;
    VkResult vkSetEvent(
        VkDevice device,
        VkEvent event
    ) override;
    VkResult vkResetEvent(
        VkDevice device,
        VkEvent event
    ) override;
    void vkCmdSetEvent(
        VkCommandBuffer commandBuffer,
        VkEvent event,
        VkPipelineStageFlags stageMask
    ) override;
    void vkCmdResetEvent(
        VkCommandBuffer commandBuffer,
        VkEvent event,
        VkPipelineStageFlags stageMask
    ) override;
    void vkCmdWaitEvents(
        VkCommandBuffer commandBuffer,
        uint32_t eventCount,
        const VkEvent* pEvents,
        VkPipelineStageFlags srcStageMask,
        VkPipelineStageFlags dstStageMask,
        uint32_t memoryBarrierCount,
        const VkMemoryBarrier* pMemoryBarriers,
        uint32_t bufferMemoryBarrierCount,
        const VkBufferMemoryBarrier* pBufferMemoryBarriers,
        uint32_t imageMemoryBarrierCount,
        const VkImageMemoryBarrier* pImageMemoryBarriers
    ) override;
public: /* Render Pass Commands */
    void vkCmdBeginRenderPass(
        VkCommandBuffer commandBuffer,
        const VkRenderPassBeginInfo* pRenderPassBegin,
        VkSubpassContents contents
    ) override;
    void vkCmdNextSubpass(
        VkCommandBuffer commandBuffer,
        VkSubpassContents contents
    ) override;
    void vkCmdEndRenderPass(
        VkCommandBuffer commandBuffer
    ) override;
public: /* Shaders */
    VkResult vkCreateShaderModule(
        VkDevice device,
        const VkShaderModuleCreateInfo* pCreateInfo, 
        const VkAllocationCallbacks* pAllocator, 
        VkShaderModule* pShaderModule
    ) override;
    void vkDestroyShaderModule(
        VkDevice device,
        VkShaderModule shaderModule,
        const VkAllocationCallbacks* pAllocator
    ) override;
public: /* Render Pass */
    VkResult vkCreateFramebuffer(
        VkDevice device,
        const VkFramebufferCreateInfo* pCreateInfo,
        const VkAllocationCallbacks* pAllocator,
        VkFramebuffer* pFramebuffer
    ) override;
    void vkDestroyFramebuffer(
        VkDevice device,
        VkFramebuffer framebuffer,
        const VkAllocationCallbacks* pAllocator
    ) override;
    VkResult vkCreateRenderPass(
        VkDevice device,
        const VkRenderPassCreateInfo* pCreateInfo,
        const VkAllocationCallbacks* pAllocator,
        VkRenderPass* pRenderPass
    ) override;
    void vkDestroyRenderPass(
        VkDevice device,
        VkRenderPass renderPass,
        const VkAllocationCallbacks* pAllocator
    ) override;
    void vkGetRenderAreaGranularity(
        VkDevice device,
        VkRenderPass renderPass,
        VkExtent2D* pGranularity
    ) override;
    VkResult vkCreateRenderPass2KHR(
        VkDevice device,
        const VkRenderPassCreateInfo2* pCreateInfo,
        const VkAllocationCallbacks* pAllocator,
        VkRenderPass* pRenderPass
    ) override;
public: /* Pipelines */
    VkResult vkCreatePipelineCache(
        VkDevice device,
        const VkPipelineCacheCreateInfo* pCreateInfo,
        const VkAllocationCallbacks* pAllocator,
        VkPipelineCache* pPipelineCache
    ) override;
    void vkDestroyPipelineCache(
        VkDevice device,
        VkPipelineCache pipelineCache,
        const VkAllocationCallbacks* pAllocator
    ) override;
    VkResult vkGetPipelineCacheData(
        VkDevice device,
        VkPipelineCache pipelineCache,
        size_t* pDataSize,
        void* pData
    ) override;
    VkResult vkMergePipelineCaches(
        VkDevice device,
        VkPipelineCache dstCache,
        uint32_t srcCacheCount,
        const VkPipelineCache* pSrcCaches
    ) override;
    VkResult vkCreateGraphicsPipelines(
        VkDevice device,
        VkPipelineCache pipelineCache,
        uint32_t createInfoCount,
        const VkGraphicsPipelineCreateInfo* pCreateInfos,
        const VkAllocationCallbacks* pAllocator,
        VkPipeline* pPipelines
    ) override;
    VkResult vkCreateComputePipelines(
        VkDevice device,
        VkPipelineCache pipelineCache,
        uint32_t createInfoCount,
        const VkComputePipelineCreateInfo* pCreateInfos,
        const VkAllocationCallbacks* pAllocator,
        VkPipeline* pPipelines
    ) override;
    void vkDestroyPipeline(
        VkDevice device,
        VkPipeline pipeline,
        const VkAllocationCallbacks* pAllocator
    ) override;
    void vkCmdBindPipeline(
        VkCommandBuffer commandBuffer,
        VkPipelineBindPoint pipelineBindPoint,
        VkPipeline pipeline
    ) override;
    void vkCmdPipelineBarrier(
        VkCommandBuffer commandBuffer,
        VkPipelineStageFlags srcStageMask,
        VkPipelineStageFlags dstStageMask,
        VkDependencyFlags dependencyFlags,
        uint32_t memoryBarrierCount,
        const VkMemoryBarrier* pMemoryBarriers,
        uint32_t bufferMemoryBarrierCount,
        const VkBufferMemoryBarrier* pBufferMemoryBarriers,
        uint32_t imageMemoryBarrierCount,
        const VkImageMemoryBarrier* pImageMemoryBarriers
    ) override;
public: /* Memory Allocation */
    void vkGetPhysicalDeviceMemoryProperties(
        VkPhysicalDevice physicalDevice,
        VkPhysicalDeviceMemoryProperties* pMemoryProperties
    ) override;
    void vkGetPhysicalDeviceMemoryProperties2(
        VkPhysicalDevice physicalDevice,
        VkPhysicalDeviceMemoryProperties2* pMemoryProperties
    ) override;
    VkResult vkAllocateMemory(
        VkDevice device,
        const VkMemoryAllocateInfo* pAllocateInfo,
        const VkAllocationCallbacks* pAllocator,
        VkDeviceMemory* pMemory
    ) override;
    void vkFreeMemory(
        VkDevice device,
        VkDeviceMemory memory,
        const VkAllocationCallbacks* pAllocator
    ) override;
    VkResult vkMapMemory(
        VkDevice device,
        VkDeviceMemory memory,
        VkDeviceSize offset,
        VkDeviceSize size,
        VkMemoryMapFlags flags,
        void** ppData
    ) override;
    void vkUnmapMemory(
        VkDevice device,
        VkDeviceMemory memory
    ) override;
    VkResult vkFlushMappedMemoryRanges(
        VkDevice device,
        uint32_t memoryRangeCount,
        const VkMappedMemoryRange* pMemoryRanges
    ) override;
    VkResult vkInvalidateMappedMemoryRanges(
        VkDevice device,
        uint32_t memoryRangeCount,
        const VkMappedMemoryRange* pMemoryRanges
    ) override;
    void vkGetDeviceMemoryCommitment(
        VkDevice device,
        VkDeviceMemory memory,
        VkDeviceSize* pCommittedMemoryInBytes
    ) override;
    VkResult vkCreateBuffer(
        VkDevice device,
        const VkBufferCreateInfo* pCreateInfo,
        const VkAllocationCallbacks* pAllocator,
        VkBuffer* pBuffer
    ) override;
    void vkDestroyBuffer(
        VkDevice device,
        VkBuffer buffer,
        const VkAllocationCallbacks* pAllocator
    ) override;
public: /* Resource Creation */
    VkResult vkBindBufferMemory(
        VkDevice device,
        VkBuffer buffer,
        VkDeviceMemory memory,
        VkDeviceSize memoryOffset
    ) override;
    VkResult vkBindBufferMemory2(
        VkDevice device,
        uint32_t bindInfoCount,
        const VkBindBufferMemoryInfo* pBindInfos
    ) override;
    VkResult vkBindImageMemory(
        VkDevice device,
        VkImage image,
        VkDeviceMemory memory,
        VkDeviceSize memoryOffset
    ) override;
    VkResult vkBindImageMemory2(
        VkDevice device,
        uint32_t bindInfoCount,
        const VkBindImageMemoryInfo* pBindInfos
    ) override;
    void vkGetBufferMemoryRequirements(
        VkDevice device,
        VkBuffer buffer,
        VkMemoryRequirements* pMemoryRequirements
    ) override;
    void vkGetBufferMemoryRequirements2(
        VkDevice device,
        const VkBufferMemoryRequirementsInfo2* pInfo,
        VkMemoryRequirements2* pMemoryRequirements
    ) override;
    void vkGetDeviceBufferMemoryRequirements(
        VkDevice device,
        const VkDeviceBufferMemoryRequirements* pInfo,
        VkMemoryRequirements2* pMemoryRequirements
    ) override;
    void vkGetImageMemoryRequirements(
        VkDevice device,
        VkImage image,
        VkMemoryRequirements* pMemoryRequirements
    ) override;
    void vkGetImageMemoryRequirements2(
        VkDevice device,
        const VkImageMemoryRequirementsInfo2* pInfo,
        VkMemoryRequirements2* pMemoryRequirements
    ) override;
    void vkGetDeviceImageMemoryRequirements(
        VkDevice device,
        const VkDeviceImageMemoryRequirements* pInfo,
        VkMemoryRequirements2* pMemoryRequirements
    ) override;
    VkResult vkCreateBufferView(
        VkDevice device,
        const VkBufferViewCreateInfo* pCreateInfo,
        const VkAllocationCallbacks* pAllocator,
        VkBufferView* pView
    ) override;
    void vkDestroyBufferView(
        VkDevice device,
        VkBufferView bufferView,
        const VkAllocationCallbacks* pAllocator
    ) override;
    VkResult vkCreateImage(
        VkDevice device,
        const VkImageCreateInfo* pCreateInfo,
        const VkAllocationCallbacks* pAllocator,
        VkImage* pImage
    ) override;
    void vkDestroyImage(
        VkDevice device,
        VkImage image,
        const VkAllocationCallbacks* pAllocator
    ) override;
    void vkGetImageSubresourceLayout(
        VkDevice device,
        VkImage image,
        const VkImageSubresource* pSubresource,
        VkSubresourceLayout* pLayout
    ) override;
    VkResult vkCreateImageView(
        VkDevice device,
        const VkImageViewCreateInfo* pCreateInfo,
        const VkAllocationCallbacks* pAllocator,
        VkImageView* pView
    ) override;
    void vkDestroyImageView(
        VkDevice device,
        VkImageView imageView,
        const VkAllocationCallbacks* pAllocator
    ) override;
    void vkGetBufferMemoryRequirements2KHR(
        VkDevice device,
        const VkBufferMemoryRequirementsInfo2* pInfo,
        VkMemoryRequirements2* pMemoryRequirements
    ) override;
    void vkGetImageMemoryRequirements2KHR(
        VkDevice device,
        const VkImageMemoryRequirementsInfo2* pInfo,
        VkMemoryRequirements2* pMemoryRequirements
    ) override;
public: /* Samplers */
    VkResult vkCreateSampler(
        VkDevice device,
        const VkSamplerCreateInfo* pCreateInfo,
        const VkAllocationCallbacks* pAllocator,
        VkSampler* pSampler
    ) override;
    void vkDestroySampler(
        VkDevice device,
        VkSampler sampler,
        const VkAllocationCallbacks* pAllocator
    ) override;
public: /* Resource Descriptors */
    VkResult vkCreatePipelineLayout(
        VkDevice device,
        const VkPipelineLayoutCreateInfo* pCreateInfo,
        const VkAllocationCallbacks* pAllocator,
        VkPipelineLayout* pPipelineLayout
    ) override;
    void vkDestroyPipelineLayout(
        VkDevice device,
        VkPipelineLayout pipelineLayout,
        const VkAllocationCallbacks* pAllocator
    ) override;
    VkResult vkCreateDescriptorSetLayout(
        VkDevice device,
        const VkDescriptorSetLayoutCreateInfo* pCreateInfo,
        const VkAllocationCallbacks* pAllocator,
        VkDescriptorSetLayout* pSetLayout
    ) override;
    void vkDestroyDescriptorSetLayout(
        VkDevice device,
        VkDescriptorSetLayout descriptorSetLayout,
        const VkAllocationCallbacks* pAllocator
    ) override;
    VkResult vkCreateDescriptorPool(
        VkDevice device,
        const VkDescriptorPoolCreateInfo* pCreateInfo,
        const VkAllocationCallbacks* pAllocator,
        VkDescriptorPool* pDescriptorPool
    ) override;
    void vkDestroyDescriptorPool(
        VkDevice device,
        VkDescriptorPool descriptorPool,
        const VkAllocationCallbacks* pAllocator
    ) override;
    VkResult vkResetDescriptorPool(
        VkDevice device,
        VkDescriptorPool descriptorPool,
        VkDescriptorPoolResetFlags flags
    ) override;
    VkResult vkAllocateDescriptorSets(
        VkDevice device,
        const VkDescriptorSetAllocateInfo* pAllocateInfo,
        VkDescriptorSet* pDescriptorSets
    ) override;
    VkResult vkFreeDescriptorSets(
        VkDevice device,
        VkDescriptorPool descriptorPool,
        uint32_t descriptorSetCount,
        const VkDescriptorSet* pDescriptorSets
    ) override;
    void vkCmdBindDescriptorSets(
        VkCommandBuffer commandBuffer,
        VkPipelineBindPoint pipelineBindPoint,
        VkPipelineLayout layout,
        uint32_t firstSet,
        uint32_t descriptorSetCount,
        const VkDescriptorSet* pDescriptorSets,
        uint32_t dynamicOffsetCount,
        const uint32_t* pDynamicOffsets
    ) override;
    void vkCmdPushConstants(
        VkCommandBuffer commandBuffer,
        VkPipelineLayout layout,
        VkShaderStageFlags stageFlags,
        uint32_t offset,
        uint32_t size,
        const void* pValues
    ) override;
public: /* Queries */
    VkResult vkCreateQueryPool(
        VkDevice device,
        const VkQueryPoolCreateInfo* pCreateInfo,
        const VkAllocationCallbacks* pAllocator,
        VkQueryPool* pQueryPool
    ) override;
    void vkDestroyQueryPool(
        VkDevice device,
        VkQueryPool queryPool,
        const VkAllocationCallbacks* pAllocator
    ) override;
    VkResult vkGetQueryPoolResults(
        VkDevice device,
        VkQueryPool queryPool,
        uint32_t firstQuery,
        uint32_t queryCount,
        size_t dataSize,
        void* pData,
        VkDeviceSize stride,
        VkQueryResultFlags flags
    ) override;
    void vkCmdBeginQuery(
        VkCommandBuffer commandBuffer,
        VkQueryPool queryPool,
        uint32_t query,
        VkQueryControlFlags flags
    ) override;
    void vkCmdEndQuery(
        VkCommandBuffer commandBuffer,
        VkQueryPool queryPool,
        uint32_t query
    ) override;
    void vkCmdResetQueryPool(
        VkCommandBuffer commandBuffer,
        VkQueryPool queryPool,
        uint32_t firstQuery,
        uint32_t queryCount
    ) override;
    void vkCmdWriteTimestamp(
        VkCommandBuffer commandBuffer,
        VkPipelineStageFlagBits pipelineStage,
        VkQueryPool queryPool,
        uint32_t query
    ) override;
    void vkCmdCopyQueryPoolResults(
        VkCommandBuffer commandBuffer,
        VkQueryPool queryPool,
        uint32_t firstQuery,
        uint32_t queryCount,
        VkBuffer dstBuffer,
        VkDeviceSize dstOffset,
        VkDeviceSize stride,
        VkQueryResultFlags flags
    ) override;
public: /* Clear Commands */
    void vkCmdUpdateBuffer(
        VkCommandBuffer commandBuffer,
        VkBuffer dstBuffer,
        VkDeviceSize dstOffset,
        VkDeviceSize dataSize,
        const void* pData
    ) override;
    void vkCmdFillBuffer(
        VkCommandBuffer commandBuffer,
        VkBuffer dstBuffer,
        VkDeviceSize dstOffset,
        VkDeviceSize size,
        uint32_t data
    ) override;
    void vkCmdClearColorImage(
        VkCommandBuffer commandBuffer,
        VkImage image,
        VkImageLayout imageLayout,
        const VkClearColorValue* pColor,
        uint32_t rangeCount,
        const VkImageSubresourceRange* pRanges
    ) override;
    void vkCmdClearDepthStencilImage(
        VkCommandBuffer commandBuffer,
        VkImage image,
        VkImageLayout imageLayout,
        const VkClearDepthStencilValue* pDepthStencil,
        uint32_t rangeCount,
        const VkImageSubresourceRange* pRanges
    ) override;
    void vkCmdClearAttachments(
        VkCommandBuffer commandBuffer,
        uint32_t attachmentCount,
        const VkClearAttachment* pAttachments,
        uint32_t rectCount,
        const VkClearRect* pRects
    ) override;
public: /* Copy Commands */
    void vkCmdCopyBuffer(
        VkCommandBuffer commandBuffer,
        VkBuffer srcBuffer,
        VkBuffer dstBuffer,
        uint32_t regionCount,
        const VkBufferCopy* pRegions
    ) override;
    void vkCmdCopyImage(
        VkCommandBuffer commandBuffer,
        VkImage srcImage,
        VkImageLayout srcImageLayout,
        VkImage dstImage,
        VkImageLayout dstImageLayout,
        uint32_t regionCount,
        const VkImageCopy* pRegions
    ) override;
    void vkCmdBlitImage(
        VkCommandBuffer commandBuffer,
        VkImage srcImage,
        VkImageLayout srcImageLayout,
        VkImage dstImage,
        VkImageLayout dstImageLayout,
        uint32_t regionCount,
        const VkImageBlit* pRegions,
        VkFilter filter
    ) override;
    void vkCmdCopyImageToBuffer(
        VkCommandBuffer commandBuffer,
        VkImage srcImage,
        VkImageLayout srcImageLayout,
        VkBuffer dstBuffer,
        uint32_t regionCount,
        const VkBufferImageCopy* pRegions
    ) override;
    void vkCmdResolveImage(
        VkCommandBuffer commandBuffer,
        VkImage srcImage,
        VkImageLayout srcImageLayout,
        VkImage dstImage,
        VkImageLayout dstImageLayout,
        uint32_t regionCount,
        const VkImageResolve* pRegions
    ) override;
    void vkCmdCopyBufferToImage(
        VkCommandBuffer commandBuffer,
         VkBuffer srcBuffer,
         VkImage dstImage,
         VkImageLayout dstImageLayout,
         uint32_t regionCount,
         const VkBufferImageCopy* pRegions
    ) override;
public: /* Drawing Commands */
    void vkCmdBindIndexBuffer(
        VkCommandBuffer commandBuffer,
        VkBuffer buffer,
        VkDeviceSize offset,
        VkIndexType indexType
    ) override;
    void vkCmdBindVertexBuffers(
        VkCommandBuffer commandBuffer,
        uint32_t firstBinding,
        uint32_t bindingCount,
        const VkBuffer* pBuffers,
        const VkDeviceSize* pOffsets
    ) override;
    void vkCmdDraw(
        VkCommandBuffer commandBuffer,
        uint32_t vertexCount,
        uint32_t instanceCount,
        uint32_t firstVertex,
        uint32_t firstInstance
    ) override;
    void vkCmdDrawIndexed(
        VkCommandBuffer commandBuffer,
        uint32_t indexCount,
        uint32_t instanceCount,
        uint32_t firstIndex,
        int32_t vertexOffset,
        uint32_t firstInstance
    ) override;
    void vkCmdDrawIndirect(
        VkCommandBuffer commandBuffer,
        VkBuffer buffer,
        VkDeviceSize offset,
        uint32_t drawCount,
        uint32_t stride
    ) override;
    void vkCmdDrawIndexedIndirect(
        VkCommandBuffer commandBuffer,
        VkBuffer buffer,
        VkDeviceSize offset,
        uint32_t drawCount,
        uint32_t stride
    ) override;
public: /* Fixed-Function Vertex Post Processing */
    void vkCmdSetViewport(
        VkCommandBuffer commandBuffer,
        uint32_t firstViewport,
        uint32_t viewportCount,
        const VkViewport* pViewports
    ) override;
public: /* Rasterization */
    void vkCmdSetLineWidth(
        VkCommandBuffer commandBuffer,
        float lineWidth
    ) override;
    void vkCmdSetDepthBias(
        VkCommandBuffer commandBuffer,
        float depthBiasConstantFactor,
        float depthBiasClamp,
        float depthBiasSlopeFactor
    ) override;
public: /* Fragment Operations */
    void vkCmdSetScissor(
        VkCommandBuffer commandBuffer,
        uint32_t firstScissor,
        uint32_t scissorCount,
        const VkRect2D* pScissors
    ) override;
public: /* The Framebuffer */
    void vkCmdSetBlendConstants(
        VkCommandBuffer commandBuffer,
        const float blendConstants[4]
    ) override;
    void vkCmdSetDepthBounds(
        VkCommandBuffer commandBuffer,
        float minDepthBounds,
        float maxDepthBounds
    ) override;
    void vkCmdSetStencilCompareMask(
        VkCommandBuffer commandBuffer,
        VkStencilFaceFlags faceMask,
        uint32_t compareMask
    ) override;
    void vkCmdSetStencilWriteMask(
        VkCommandBuffer commandBuffer,
        VkStencilFaceFlags faceMask,
        uint32_t writeMask
    ) override;
    void vkCmdSetStencilReference(
        VkCommandBuffer commandBuffer,
        VkStencilFaceFlags faceMask,
        uint32_t reference
    ) override;
public: /* Dispatching Commands */
    void vkCmdDispatch(
        VkCommandBuffer commandBuffer,
        uint32_t groupCountX,
        uint32_t groupCountY,
        uint32_t groupCountZ
    ) override;
    void vkCmdDispatchIndirect(
        VkCommandBuffer commandBuffer,
        VkBuffer buffer,
        VkDeviceSize offset
    ) override;
public: /* Sparse Resources */
    VkResult vkQueueBindSparse(
        VkQueue queue,
        uint32_t bindInfoCount,
        const VkBindSparseInfo* pBindInfo,
        VkFence fence
    ) override;
public: /* Window System Integration (WSI) */
#if MMP_PLATFORM(WINDOWS)
    VkResult vkCreateWin32SurfaceKHR(
        VkInstance instance,
        const VkWin32SurfaceCreateInfoKHR* pCreateInfo,
        const VkAllocationCallbacks* pAllocator,
        VkSurfaceKHR* pSurface
    ) override;
#endif
    void vkDestroySurfaceKHR(
        VkInstance instance,
        VkSurfaceKHR surface,
        const VkAllocationCallbacks* pAllocator
    ) override;
    VkResult vkCreateSwapchainKHR(
        VkDevice device,
        const VkSwapchainCreateInfoKHR* pCreateInfo,
        const VkAllocationCallbacks* pAllocator,
        VkSwapchainKHR* pSwapchain
    ) override;
    void vkDestroySwapchainKHR(
        VkDevice device,
        VkSwapchainKHR swapchain,
        const VkAllocationCallbacks* pAllocator
    ) override;
    VkResult vkGetSwapchainImagesKHR(
        VkDevice device,
        VkSwapchainKHR swapchain,
        uint32_t* pSwapchainImageCount,
        VkImage* pSwapchainImages
    ) override;
    VkResult vkAcquireNextImageKHR(
        VkDevice device,
        VkSwapchainKHR swapchain,
        uint64_t timeout,
        VkSemaphore semaphore,
        VkFence fence,
        uint32_t* pImageIndex
    ) override;
    VkResult vkQueuePresentKHR(
        VkQueue queue,
        const VkPresentInfoKHR* pPresentInfo
    ) override;
    VkResult vkGetPhysicalDeviceSurfaceCapabilitiesKHR(
        VkPhysicalDevice physicalDevice,
        VkSurfaceKHR surface,
        VkSurfaceCapabilitiesKHR* pSurfaceCapabilities
    ) override;
    VkResult vkGetPhysicalDeviceSurfacePresentModesKHR(
        VkPhysicalDevice physicalDevice,
        VkSurfaceKHR surface,
        uint32_t* pPresentModeCount,
        VkPresentModeKHR* pPresentModes
    ) override;
public: /* Deferred Host Operations */
    VkResult vkWaitForPresentKHR(
        VkDevice device,
        VkSwapchainKHR swapchain,
        uint64_t presentId,
        uint64_t timeout
    ) override;
public: /* Features */
    void vkGetPhysicalDeviceFeatures(
        VkPhysicalDevice physicalDevice,
        VkPhysicalDeviceFeatures* pFeatures
    ) override;
    void vkGetPhysicalDeviceFormatProperties(
        VkPhysicalDevice physicalDevice,
        VkFormat format,
        VkFormatProperties* pFormatProperties
    ) override;
public: /* Additional Capabilities */
    VkResult vkGetPhysicalDeviceImageFormatProperties(
        VkPhysicalDevice physicalDevice,
        VkFormat format,
        VkImageType type,
        VkImageTiling tiling,
        VkImageUsageFlags usage,
        VkImageCreateFlags flags,
        VkImageFormatProperties* pImageFormatProperties
    ) override;
public: /* Extending Vulkan */
    VkResult vkEnumerateInstanceExtensionProperties(
        const char* pLayerName,
        uint32_t* pPropertyCount,
        VkExtensionProperties* pProperties
    ) override;
    VkResult vkEnumerateDeviceExtensionProperties(
        VkPhysicalDevice physicalDevice,
        const char* pLayerName,
        uint32_t* pPropertyCount,
        VkExtensionProperties* pProperties
    ) override;
    VkResult vkEnumerateInstanceLayerProperties(
        uint32_t* pPropertyCount,
        VkLayerProperties* pProperties
    ) override;
    VkResult vkEnumerateDeviceLayerProperties(
        VkPhysicalDevice physicalDevice,
        uint32_t* pPropertyCount,
        VkLayerProperties* pProperties
    ) override;
private:
    PFN_vkCreateInstance _vkCreateInstance;
    PFN_vkDestroyInstance _vkDestroyInstance;
    PFN_vkEnumeratePhysicalDevices _vkEnumeratePhysicalDevices;
    PFN_vkEnumerateInstanceVersion _vkEnumerateInstanceVersion;
    PFN_vkGetPhysicalDeviceFeatures _vkGetPhysicalDeviceFeatures;
    PFN_vkGetPhysicalDeviceFormatProperties _vkGetPhysicalDeviceFormatProperties;
    PFN_vkGetPhysicalDeviceImageFormatProperties _vkGetPhysicalDeviceImageFormatProperties;
    PFN_vkGetPhysicalDeviceProperties _vkGetPhysicalDeviceProperties;
    PFN_vkGetPhysicalDeviceQueueFamilyProperties _vkGetPhysicalDeviceQueueFamilyProperties;
    PFN_vkGetPhysicalDeviceMemoryProperties _vkGetPhysicalDeviceMemoryProperties;
    PFN_vkGetPhysicalDeviceMemoryProperties2 _vkGetPhysicalDeviceMemoryProperties2;
    PFN_vkGetInstanceProcAddr _vkGetInstanceProcAddr;
    PFN_vkGetDeviceProcAddr _vkGetDeviceProcAddr;
    PFN_vkCreateDevice _vkCreateDevice;
    PFN_vkDestroyDevice _vkDestroyDevice;
    PFN_vkEnumerateInstanceExtensionProperties _vkEnumerateInstanceExtensionProperties;
    PFN_vkEnumerateDeviceExtensionProperties _vkEnumerateDeviceExtensionProperties;
    PFN_vkEnumerateInstanceLayerProperties _vkEnumerateInstanceLayerProperties;
    PFN_vkEnumerateDeviceLayerProperties _vkEnumerateDeviceLayerProperties;
    PFN_vkGetDeviceQueue _vkGetDeviceQueue;
    PFN_vkQueueSubmit _vkQueueSubmit;
    PFN_vkQueueWaitIdle _vkQueueWaitIdle;
    PFN_vkDeviceWaitIdle _vkDeviceWaitIdle;
    PFN_vkAllocateMemory _vkAllocateMemory;
    PFN_vkFreeMemory _vkFreeMemory;
    PFN_vkMapMemory _vkMapMemory;
    PFN_vkUnmapMemory _vkUnmapMemory;
    PFN_vkFlushMappedMemoryRanges _vkFlushMappedMemoryRanges;
    PFN_vkInvalidateMappedMemoryRanges _vkInvalidateMappedMemoryRanges;
    PFN_vkGetDeviceMemoryCommitment _vkGetDeviceMemoryCommitment;
    PFN_vkBindBufferMemory _vkBindBufferMemory;
    PFN_vkBindBufferMemory2 _vkBindBufferMemory2;
    PFN_vkBindImageMemory _vkBindImageMemory;
    PFN_vkBindImageMemory2 _vkBindImageMemory2;
    PFN_vkGetBufferMemoryRequirements _vkGetBufferMemoryRequirements;
    PFN_vkGetBufferMemoryRequirements2 _vkGetBufferMemoryRequirements2;
    PFN_vkGetDeviceBufferMemoryRequirements _vkGetDeviceBufferMemoryRequirements;
    PFN_vkGetImageMemoryRequirements _vkGetImageMemoryRequirements;
    PFN_vkGetImageMemoryRequirements2 _vkGetImageMemoryRequirements2;
    PFN_vkGetDeviceImageMemoryRequirements _vkGetDeviceImageMemoryRequirements;
    PFN_vkCreateFence _vkCreateFence;
    PFN_vkDestroyFence _vkDestroyFence;
    PFN_vkGetFenceStatus _vkGetFenceStatus;
    PFN_vkCreateSemaphore _vkCreateSemaphore;
    PFN_vkDestroySemaphore _vkDestroySemaphore;
    PFN_vkCreateEvent _vkCreateEvent;
    PFN_vkDestroyEvent _vkDestroyEvent;
    PFN_vkGetEventStatus _vkGetEventStatus;
    PFN_vkSetEvent _vkSetEvent;
    PFN_vkResetEvent _vkResetEvent;
    PFN_vkCreateQueryPool _vkCreateQueryPool;
    PFN_vkDestroyQueryPool _vkDestroyQueryPool;
    PFN_vkGetQueryPoolResults _vkGetQueryPoolResults;
    PFN_vkCreateBuffer _vkCreateBuffer;
    PFN_vkDestroyBuffer _vkDestroyBuffer;
    PFN_vkCreateBufferView _vkCreateBufferView;
    PFN_vkDestroyBufferView _vkDestroyBufferView;
    PFN_vkCreateImage _vkCreateImage;
    PFN_vkDestroyImage _vkDestroyImage;
    PFN_vkGetImageSubresourceLayout _vkGetImageSubresourceLayout;
    PFN_vkCreateImageView _vkCreateImageView;
    PFN_vkDestroyImageView _vkDestroyImageView;
    PFN_vkCreateShaderModule _vkCreateShaderModule;
    PFN_vkDestroyShaderModule _vkDestroyShaderModule;
    PFN_vkCreatePipelineCache _vkCreatePipelineCache;
    PFN_vkDestroyPipelineCache _vkDestroyPipelineCache;
    PFN_vkGetPipelineCacheData _vkGetPipelineCacheData;
    PFN_vkMergePipelineCaches _vkMergePipelineCaches;
    PFN_vkCreateGraphicsPipelines _vkCreateGraphicsPipelines;
    PFN_vkCreateComputePipelines _vkCreateComputePipelines;
    PFN_vkDestroyPipeline _vkDestroyPipeline;
    PFN_vkCreatePipelineLayout _vkCreatePipelineLayout;
    PFN_vkDestroyPipelineLayout _vkDestroyPipelineLayout;
    PFN_vkCreateSampler _vkCreateSampler;
    PFN_vkDestroySampler _vkDestroySampler;
    PFN_vkCreateDescriptorSetLayout _vkCreateDescriptorSetLayout;
    PFN_vkDestroyDescriptorSetLayout _vkDestroyDescriptorSetLayout;
    PFN_vkCreateDescriptorPool _vkCreateDescriptorPool;
    PFN_vkDestroyDescriptorPool _vkDestroyDescriptorPool;
    PFN_vkResetDescriptorPool _vkResetDescriptorPool;
    PFN_vkAllocateDescriptorSets _vkAllocateDescriptorSets;
    PFN_vkFreeDescriptorSets _vkFreeDescriptorSets;
    PFN_vkUpdateDescriptorSets _vkUpdateDescriptorSets;
    PFN_vkCreateFramebuffer _vkCreateFramebuffer;
    PFN_vkDestroyFramebuffer _vkDestroyFramebuffer;
    PFN_vkCreateRenderPass _vkCreateRenderPass;
    PFN_vkDestroyRenderPass _vkDestroyRenderPass;
    PFN_vkGetRenderAreaGranularity _vkGetRenderAreaGranularity;
    PFN_vkCreateCommandPool _vkCreateCommandPool;
    PFN_vkDestroyCommandPool _vkDestroyCommandPool;
    PFN_vkResetCommandPool _vkResetCommandPool;
    PFN_vkAllocateCommandBuffers _vkAllocateCommandBuffers;
    PFN_vkFreeCommandBuffers _vkFreeCommandBuffers;
    
    // Used frequently together
    PFN_vkCmdBindPipeline _vkCmdBindPipeline;
    PFN_vkCmdSetViewport _vkCmdSetViewport;
    PFN_vkCmdSetScissor _vkCmdSetScissor;
    PFN_vkCmdSetBlendConstants _vkCmdSetBlendConstants;
    PFN_vkCmdSetStencilCompareMask _vkCmdSetStencilCompareMask;
    PFN_vkCmdSetStencilWriteMask _vkCmdSetStencilWriteMask;
    PFN_vkCmdSetStencilReference _vkCmdSetStencilReference;
    PFN_vkCmdBindDescriptorSets _vkCmdBindDescriptorSets;
    PFN_vkCmdBindIndexBuffer _vkCmdBindIndexBuffer;
    PFN_vkCmdBindVertexBuffers _vkCmdBindVertexBuffers;
    PFN_vkCmdDraw _vkCmdDraw;
    PFN_vkCmdDrawIndexed _vkCmdDrawIndexed;
    PFN_vkCmdPipelineBarrier _vkCmdPipelineBarrier;
    PFN_vkCmdPushConstants _vkCmdPushConstants;

    // Every frame to a few times per frame
    PFN_vkWaitForFences _vkWaitForFences;
    PFN_vkResetFences _vkResetFences;
    PFN_vkBeginCommandBuffer _vkBeginCommandBuffer;
    PFN_vkEndCommandBuffer _vkEndCommandBuffer;
    PFN_vkResetCommandBuffer _vkResetCommandBuffer;
    PFN_vkCmdClearAttachments _vkCmdClearAttachments;
    PFN_vkCmdSetEvent _vkCmdSetEvent;
    PFN_vkCmdResetEvent _vkCmdResetEvent;
    PFN_vkCmdWaitEvents _vkCmdWaitEvents;
    PFN_vkCmdBeginRenderPass _vkCmdBeginRenderPass;
    PFN_vkCmdEndRenderPass _vkCmdEndRenderPass;
    PFN_vkCmdCopyBuffer _vkCmdCopyBuffer;
    PFN_vkCmdCopyImage _vkCmdCopyImage;
    PFN_vkCmdBlitImage _vkCmdBlitImage;
    PFN_vkCmdCopyBufferToImage _vkCmdCopyBufferToImage;
    PFN_vkCmdCopyImageToBuffer _vkCmdCopyImageToBuffer;

    // Rare or not used
    PFN_vkCmdSetDepthBounds _vkCmdSetDepthBounds;
    PFN_vkCmdSetLineWidth _vkCmdSetLineWidth;
    PFN_vkCmdSetDepthBias _vkCmdSetDepthBias;
    PFN_vkCmdDrawIndirect _vkCmdDrawIndirect;
    PFN_vkCmdDrawIndexedIndirect _vkCmdDrawIndexedIndirect;
    PFN_vkCmdDispatch _vkCmdDispatch;
    PFN_vkCmdDispatchIndirect _vkCmdDispatchIndirect;
    PFN_vkCmdUpdateBuffer _vkCmdUpdateBuffer;
    PFN_vkCmdFillBuffer _vkCmdFillBuffer;
    PFN_vkCmdClearColorImage _vkCmdClearColorImage;
    PFN_vkCmdClearDepthStencilImage _vkCmdClearDepthStencilImage;
    PFN_vkCmdResolveImage _vkCmdResolveImage;
    PFN_vkCmdBeginQuery _vkCmdBeginQuery;
    PFN_vkCmdEndQuery _vkCmdEndQuery;
    PFN_vkCmdResetQueryPool _vkCmdResetQueryPool;
    PFN_vkCmdWriteTimestamp _vkCmdWriteTimestamp;
    PFN_vkCmdCopyQueryPoolResults _vkCmdCopyQueryPoolResults;
    PFN_vkCmdNextSubpass _vkCmdNextSubpass;
    PFN_vkCmdExecuteCommands _vkCmdExecuteCommands;
private:
#if MMP_PLATFORM(WINDOWS)
    PFN_vkCreateWin32SurfaceKHR _vkCreateWin32SurfaceKHR;
#endif
    PFN_vkDestroySurfaceKHR _vkDestroySurfaceKHR;
    PFN_vkCreateSwapchainKHR _vkCreateSwapchainKHR;
    PFN_vkDestroySwapchainKHR _vkDestroySwapchainKHR;
    PFN_vkGetSwapchainImagesKHR _vkGetSwapchainImagesKHR;
    PFN_vkAcquireNextImageKHR _vkAcquireNextImageKHR;
    PFN_vkQueuePresentKHR _vkQueuePresentKHR;
    PFN_vkGetPhysicalDeviceSurfaceCapabilitiesKHR _vkGetPhysicalDeviceSurfaceCapabilitiesKHR;
    PFN_vkGetPhysicalDeviceSurfacePresentModesKHR _vkGetPhysicalDeviceSurfacePresentModesKHR;
private:
    PFN_vkCreateRenderPass2KHR _vkCreateRenderPass2KHR;
    PFN_vkWaitForPresentKHR _vkWaitForPresentKHR;
    PFN_vkGetBufferMemoryRequirements2KHR _vkGetBufferMemoryRequirements2KHR;
    PFN_vkGetImageMemoryRequirements2KHR _vkGetImageMemoryRequirements2KHR;
private:
    Poco::SharedLibrary _vulkanLoader;
};

} // namespace Mmp