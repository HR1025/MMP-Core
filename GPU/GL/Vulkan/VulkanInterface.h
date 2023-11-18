//
// VulkanInterface.h
//
// Library: GPU
// Package: GL
// Module:  Vulkan
//

#pragma once

#include <memory>
#include <cstdint>

#include "VulkanCommon.h"

namespace Mmp
{

/**
 * @sa vkspec.pdf
 */
class VulkanInterface
{
public:
    using ptr = std::shared_ptr<VulkanInterface>;
public:
    VulkanInterface() = default;
    virtual ~VulkanInterface() = default;
public:
    static void Register(VulkanInterface::ptr impl);
    static void UnRegister();
    static VulkanInterface::ptr Instance();
public: /* Initialization */
    virtual PFN_vkVoidFunction vkGetInstanceProcAddr(
        VkInstance instance,
        const char* pName
    ) = 0;
    virtual VkResult vkCreateInstance(
        const VkInstanceCreateInfo* pCreateInfo,
        const VkAllocationCallbacks* pAllocator,
        VkInstance* pInstance
    ) = 0;
    virtual void vkDestroyInstance(
        VkInstance instance,
        const VkAllocationCallbacks* pAllocator
    ) = 0;
    virtual VkResult vkEnumerateInstanceVersion(
        uint32_t* pApiVersion
    ) = 0;
public: /* Fundamentals */
    virtual PFN_vkVoidFunction vkGetDeviceProcAddr(
        VkDevice device,
        const char* pName
    ) = 0;
public: /* Devices and Queues */
    virtual VkResult vkEnumeratePhysicalDevices(
        VkInstance instance,
        uint32_t* pPhysicalDeviceCount,
        VkPhysicalDevice* pPhysicalDevices
    ) = 0;
    virtual void vkGetPhysicalDeviceProperties(
        VkPhysicalDevice physicalDevice,
        VkPhysicalDeviceProperties* pProperties
    ) = 0;
    virtual void vkGetPhysicalDeviceQueueFamilyProperties(
        VkPhysicalDevice physicalDevice,
        uint32_t* pQueueFamilyPropertyCount,
        VkQueueFamilyProperties* pQueueFamilyProperties
    ) = 0;
    virtual VkResult vkCreateDevice(
        VkPhysicalDevice physicalDevice,
        const VkDeviceCreateInfo* pCreateInfo,
        const VkAllocationCallbacks* pAllocator,
        VkDevice* pDevice
    ) = 0;
    virtual void vkDestroyDevice(
        VkDevice device,
        const VkAllocationCallbacks* pAllocator
    ) = 0;
    virtual void vkGetDeviceQueue(
        VkDevice device,
        uint32_t queueFamilyIndex,
        uint32_t queueIndex,
        VkQueue* pQueue
    ) = 0;
public: /* Command Buffers */
    virtual VkResult vkQueueSubmit(
        VkQueue queue,
        uint32_t submitCount,
        const VkSubmitInfo* pSubmits,
        VkFence fence
    ) = 0;
    virtual VkResult vkCreateCommandPool(
        VkDevice device,
        const VkCommandPoolCreateInfo* pCreateInfo,
        const VkAllocationCallbacks* pAllocator,
        VkCommandPool* pCommandPool
    ) = 0;
    virtual void vkDestroyCommandPool(
        VkDevice device,
        VkCommandPool commandPool,
        const VkAllocationCallbacks* pAllocator
    ) = 0;
    virtual VkResult vkResetCommandPool(
        VkDevice device,
        VkCommandPool commandPool,
        VkCommandPoolResetFlags flags
    ) = 0;
    virtual VkResult vkAllocateCommandBuffers(
        VkDevice device,
        const VkCommandBufferAllocateInfo* pAllocateInfo,
        VkCommandBuffer* pCommandBuffers
    ) = 0;
    virtual void vkFreeCommandBuffers(
        VkDevice device,
        VkCommandPool commandPool,
        uint32_t commandBufferCount,
        const VkCommandBuffer* pCommandBuffers
    ) = 0;
    virtual VkResult vkBeginCommandBuffer(
        VkCommandBuffer commandBuffer,
        const VkCommandBufferBeginInfo* pBeginInfo
    ) = 0;
    virtual VkResult vkEndCommandBuffer(
        VkCommandBuffer commandBuffer
    ) = 0;
    virtual VkResult vkResetCommandBuffer(
        VkCommandBuffer commandBuffer,
        VkCommandBufferResetFlags flags
    ) = 0;
    virtual void vkCmdExecuteCommands(
        VkCommandBuffer commandBuffer,
        uint32_t commandBufferCount,
        const VkCommandBuffer* pCommandBuffers
    ) = 0;
public: /* Synchronization and Cache Control */
    virtual VkResult vkQueueWaitIdle(
        VkQueue queue
    ) = 0;
    virtual VkResult vkDeviceWaitIdle(
        VkDevice device
    ) = 0;
    virtual VkResult vkCreateFence(
        VkDevice device,
        const VkFenceCreateInfo* pCreateInfo,
        const VkAllocationCallbacks* pAllocator,
        VkFence* pFence
    ) = 0;
    virtual void vkDestroyFence(
        VkDevice device,
        VkFence fence,
        const VkAllocationCallbacks* pAllocator
    ) = 0;
    virtual VkResult vkResetFences(
        VkDevice device,
        uint32_t fenceCount,
        const VkFence* pFences
    ) = 0;
    virtual VkResult vkGetFenceStatus(
        VkDevice device,
        VkFence fence
    ) = 0;
    virtual VkResult vkWaitForFences(
        VkDevice device,
        uint32_t fenceCount,
        const VkFence* pFences,
        VkBool32 waitAll,
        uint64_t timeout
    ) = 0;
    virtual VkResult vkCreateSemaphore(
        VkDevice device,
        const VkSemaphoreCreateInfo* pCreateInfo,
        const VkAllocationCallbacks* pAllocator,
        VkSemaphore* pSemaphore
    ) = 0;
    virtual void vkDestroySemaphore(
        VkDevice device,
        VkSemaphore semaphore,
        const VkAllocationCallbacks* pAllocator
    ) = 0;
    virtual VkResult vkCreateEvent(
        VkDevice device,
        const VkEventCreateInfo* pCreateInfo,
        const VkAllocationCallbacks* pAllocator,
        VkEvent* pEvent
    ) = 0;
    virtual void vkDestroyEvent(
        VkDevice device,
        VkEvent event,
        const VkAllocationCallbacks* pAllocator
    ) = 0;
    virtual VkResult vkGetEventStatus(
        VkDevice device,
        VkEvent event
    ) = 0;
    virtual VkResult vkSetEvent(
        VkDevice device,
        VkEvent event
    ) = 0;
    virtual VkResult vkResetEvent(
        VkDevice device,
        VkEvent event
    ) = 0;
    virtual void vkCmdSetEvent(
        VkCommandBuffer commandBuffer,
        VkEvent event,
        VkPipelineStageFlags stageMask
    ) = 0;
    virtual void vkCmdResetEvent(
        VkCommandBuffer commandBuffer,
        VkEvent event,
        VkPipelineStageFlags stageMask
    ) = 0;
    virtual void vkCmdWaitEvents(
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
    ) = 0;
public: /* Render Pass Commands */
    virtual void vkCmdBeginRenderPass(
        VkCommandBuffer commandBuffer,
        const VkRenderPassBeginInfo* pRenderPassBegin,
        VkSubpassContents contents
    ) = 0;
    virtual void vkCmdNextSubpass(
        VkCommandBuffer commandBuffer,
        VkSubpassContents contents
    ) = 0;
    virtual void vkCmdEndRenderPass(
        VkCommandBuffer commandBuffer
    ) = 0;
public: /* Shaders */
    virtual VkResult vkCreateShaderModule(
        VkDevice device,
        const VkShaderModuleCreateInfo* pCreateInfo, 
        const VkAllocationCallbacks* pAllocator, 
        VkShaderModule* pShaderModule
    ) = 0;
    virtual void vkDestroyShaderModule(
        VkDevice device,
        VkShaderModule shaderModule,
        const VkAllocationCallbacks* pAllocator
    ) = 0;
public: /* Render Pass */
    virtual VkResult vkCreateFramebuffer(
        VkDevice device,
        const VkFramebufferCreateInfo* pCreateInfo,
        const VkAllocationCallbacks* pAllocator,
        VkFramebuffer* pFramebuffer
    ) = 0;
    virtual void vkDestroyFramebuffer(
        VkDevice device,
        VkFramebuffer framebuffer,
        const VkAllocationCallbacks* pAllocator
    ) = 0;
    virtual VkResult vkCreateRenderPass(
        VkDevice device,
        const VkRenderPassCreateInfo* pCreateInfo,
        const VkAllocationCallbacks* pAllocator,
        VkRenderPass* pRenderPass
    ) = 0;
    virtual void vkDestroyRenderPass(
        VkDevice device,
        VkRenderPass renderPass,
        const VkAllocationCallbacks* pAllocator
    ) = 0;
    virtual void vkGetRenderAreaGranularity(
        VkDevice device,
        VkRenderPass renderPass,
        VkExtent2D* pGranularity
    ) = 0;
    virtual VkResult vkCreateRenderPass2KHR(
        VkDevice device,
        const VkRenderPassCreateInfo2* pCreateInfo,
        const VkAllocationCallbacks* pAllocator,
        VkRenderPass* pRenderPass
    ) = 0;
public: /* Pipelines */
    virtual VkResult vkCreatePipelineCache(
        VkDevice device,
        const VkPipelineCacheCreateInfo* pCreateInfo,
        const VkAllocationCallbacks* pAllocator,
        VkPipelineCache* pPipelineCache
    ) = 0;
    virtual void vkDestroyPipelineCache(
        VkDevice device,
        VkPipelineCache pipelineCache,
        const VkAllocationCallbacks* pAllocator
    ) = 0;
    virtual VkResult vkGetPipelineCacheData(
        VkDevice device,
        VkPipelineCache pipelineCache,
        size_t* pDataSize,
        void* pData
    ) = 0;
    virtual VkResult vkMergePipelineCaches(
        VkDevice device,
        VkPipelineCache dstCache,
        uint32_t srcCacheCount,
        const VkPipelineCache* pSrcCaches
    ) = 0;
    virtual VkResult vkCreateGraphicsPipelines(
        VkDevice device,
        VkPipelineCache pipelineCache,
        uint32_t createInfoCount,
        const VkGraphicsPipelineCreateInfo* pCreateInfos,
        const VkAllocationCallbacks* pAllocator,
        VkPipeline* pPipelines
    ) = 0;
    virtual VkResult vkCreateComputePipelines(
        VkDevice device,
        VkPipelineCache pipelineCache,
        uint32_t createInfoCount,
        const VkComputePipelineCreateInfo* pCreateInfos,
        const VkAllocationCallbacks* pAllocator,
        VkPipeline* pPipelines
    ) = 0;
    virtual void vkDestroyPipeline(
        VkDevice device,
        VkPipeline pipeline,
        const VkAllocationCallbacks* pAllocator
    ) = 0;
    virtual void vkCmdBindPipeline(
        VkCommandBuffer commandBuffer,
        VkPipelineBindPoint pipelineBindPoint,
        VkPipeline pipeline
    ) = 0;
    virtual void vkCmdPipelineBarrier(
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
    ) = 0;
public: /* Memory Allocation */
    virtual void vkGetPhysicalDeviceMemoryProperties(
        VkPhysicalDevice physicalDevice,
        VkPhysicalDeviceMemoryProperties* pMemoryProperties
    ) = 0;
    virtual void vkGetPhysicalDeviceMemoryProperties2(
        VkPhysicalDevice physicalDevice,
        VkPhysicalDeviceMemoryProperties2* pMemoryProperties
    ) = 0;
    virtual VkResult vkAllocateMemory(
        VkDevice device,
        const VkMemoryAllocateInfo* pAllocateInfo,
        const VkAllocationCallbacks* pAllocator,
        VkDeviceMemory* pMemory
    ) = 0;
    virtual void vkFreeMemory(
        VkDevice device,
        VkDeviceMemory memory,
        const VkAllocationCallbacks* pAllocator
    ) = 0;
    virtual VkResult vkMapMemory(
        VkDevice device,
        VkDeviceMemory memory,
        VkDeviceSize offset,
        VkDeviceSize size,
        VkMemoryMapFlags flags,
        void** ppData
    ) = 0;
    virtual void vkUnmapMemory(
        VkDevice device,
        VkDeviceMemory memory
    ) = 0;
    virtual VkResult vkFlushMappedMemoryRanges(
        VkDevice device,
        uint32_t memoryRangeCount,
        const VkMappedMemoryRange* pMemoryRanges
    ) = 0;
    virtual VkResult vkInvalidateMappedMemoryRanges(
        VkDevice device,
        uint32_t memoryRangeCount,
        const VkMappedMemoryRange* pMemoryRanges
    ) = 0;
    virtual void vkGetDeviceMemoryCommitment(
        VkDevice device,
        VkDeviceMemory memory,
        VkDeviceSize* pCommittedMemoryInBytes
    ) = 0;
    virtual VkResult vkCreateBuffer(
        VkDevice device,
        const VkBufferCreateInfo* pCreateInfo,
        const VkAllocationCallbacks* pAllocator,
        VkBuffer* pBuffer
    ) = 0;
    virtual void vkDestroyBuffer(
        VkDevice device,
        VkBuffer buffer,
        const VkAllocationCallbacks* pAllocator
    ) = 0;
public: /* Resource Creation */
    virtual VkResult vkBindBufferMemory(
        VkDevice device,
        VkBuffer buffer,
        VkDeviceMemory memory,
        VkDeviceSize memoryOffset
    ) = 0;
    virtual VkResult vkBindBufferMemory2(
        VkDevice device,
        uint32_t bindInfoCount,
        const VkBindBufferMemoryInfo* pBindInfos
    ) = 0;
    virtual VkResult vkBindImageMemory(
        VkDevice device,
        VkImage image,
        VkDeviceMemory memory,
        VkDeviceSize memoryOffset
    ) = 0;
    virtual VkResult vkBindImageMemory2(
        VkDevice device,
        uint32_t bindInfoCount,
        const VkBindImageMemoryInfo* pBindInfos
    ) = 0;
    virtual void vkGetBufferMemoryRequirements(
        VkDevice device,
        VkBuffer buffer,
        VkMemoryRequirements* pMemoryRequirements
    ) = 0;
    virtual void vkGetBufferMemoryRequirements2(
        VkDevice device,
        const VkBufferMemoryRequirementsInfo2* pInfo,
        VkMemoryRequirements2* pMemoryRequirements
    ) = 0;
    virtual void vkGetDeviceBufferMemoryRequirements(
        VkDevice device,
        const VkDeviceBufferMemoryRequirements* pInfo,
        VkMemoryRequirements2* pMemoryRequirements
    ) = 0;
    virtual void vkGetImageMemoryRequirements(
        VkDevice device,
        VkImage image,
        VkMemoryRequirements* pMemoryRequirements
    ) = 0;
    virtual void vkGetImageMemoryRequirements2(
        VkDevice device,
        const VkImageMemoryRequirementsInfo2* pInfo,
        VkMemoryRequirements2* pMemoryRequirements
    ) = 0;
    virtual void vkGetDeviceImageMemoryRequirements(
        VkDevice device,
        const VkDeviceImageMemoryRequirements* pInfo,
        VkMemoryRequirements2* pMemoryRequirements
    ) = 0;
    virtual VkResult vkCreateBufferView(
        VkDevice device,
        const VkBufferViewCreateInfo* pCreateInfo,
        const VkAllocationCallbacks* pAllocator,
        VkBufferView* pView
    ) = 0;
    virtual void vkDestroyBufferView(
        VkDevice device,
        VkBufferView bufferView,
        const VkAllocationCallbacks* pAllocator
    ) = 0;
    virtual VkResult vkCreateImage(
        VkDevice device,
        const VkImageCreateInfo* pCreateInfo,
        const VkAllocationCallbacks* pAllocator,
        VkImage* pImage
    ) = 0;
    virtual void vkDestroyImage(
        VkDevice device,
        VkImage image,
        const VkAllocationCallbacks* pAllocator
    ) = 0;
    virtual void vkGetImageSubresourceLayout(
        VkDevice device,
        VkImage image,
        const VkImageSubresource* pSubresource,
        VkSubresourceLayout* pLayout
    ) = 0;
    virtual VkResult vkCreateImageView(
        VkDevice device,
        const VkImageViewCreateInfo* pCreateInfo,
        const VkAllocationCallbacks* pAllocator,
        VkImageView* pView
    ) = 0;
    virtual void vkDestroyImageView(
        VkDevice device,
        VkImageView imageView,
        const VkAllocationCallbacks* pAllocator
    ) = 0;
    virtual void vkGetBufferMemoryRequirements2KHR(
        VkDevice device,
        const VkBufferMemoryRequirementsInfo2* pInfo,
        VkMemoryRequirements2* pMemoryRequirements
    ) = 0;
    virtual void vkGetImageMemoryRequirements2KHR(
        VkDevice device,
        const VkImageMemoryRequirementsInfo2* pInfo,
        VkMemoryRequirements2* pMemoryRequirements
    ) = 0;
public: /* Samplers */
    virtual VkResult vkCreateSampler(
        VkDevice device,
        const VkSamplerCreateInfo* pCreateInfo,
        const VkAllocationCallbacks* pAllocator,
        VkSampler* pSampler
    ) = 0;
    virtual void vkDestroySampler(
        VkDevice device,
        VkSampler sampler,
        const VkAllocationCallbacks* pAllocator
    ) = 0;
public: /* Resource Descriptors */
    virtual VkResult vkCreatePipelineLayout(
        VkDevice device,
        const VkPipelineLayoutCreateInfo* pCreateInfo,
        const VkAllocationCallbacks* pAllocator,
        VkPipelineLayout* pPipelineLayout
    ) = 0;
    virtual void vkDestroyPipelineLayout(
        VkDevice device,
        VkPipelineLayout pipelineLayout,
        const VkAllocationCallbacks* pAllocator
    ) = 0;
    virtual VkResult vkCreateDescriptorSetLayout(
        VkDevice device,
        const VkDescriptorSetLayoutCreateInfo* pCreateInfo,
        const VkAllocationCallbacks* pAllocator,
        VkDescriptorSetLayout* pSetLayout
    ) = 0;
    virtual void vkDestroyDescriptorSetLayout(
        VkDevice device,
        VkDescriptorSetLayout descriptorSetLayout,
        const VkAllocationCallbacks* pAllocator
    ) = 0;
    virtual VkResult vkCreateDescriptorPool(
        VkDevice device,
        const VkDescriptorPoolCreateInfo* pCreateInfo,
        const VkAllocationCallbacks* pAllocator,
        VkDescriptorPool* pDescriptorPool
    ) = 0;
    virtual void vkDestroyDescriptorPool(
        VkDevice device,
        VkDescriptorPool descriptorPool,
        const VkAllocationCallbacks* pAllocator
    ) = 0;
    virtual VkResult vkResetDescriptorPool(
        VkDevice device,
        VkDescriptorPool descriptorPool,
        VkDescriptorPoolResetFlags flags
    ) = 0;
    virtual VkResult vkAllocateDescriptorSets(
        VkDevice device,
        const VkDescriptorSetAllocateInfo* pAllocateInfo,
        VkDescriptorSet* pDescriptorSets
    ) = 0;
    virtual VkResult vkFreeDescriptorSets(
        VkDevice device,
        VkDescriptorPool descriptorPool,
        uint32_t descriptorSetCount,
        const VkDescriptorSet* pDescriptorSets
    ) = 0;
    virtual void vkCmdBindDescriptorSets(
        VkCommandBuffer commandBuffer,
        VkPipelineBindPoint pipelineBindPoint,
        VkPipelineLayout layout,
        uint32_t firstSet,
        uint32_t descriptorSetCount,
        const VkDescriptorSet* pDescriptorSets,
        uint32_t dynamicOffsetCount,
        const uint32_t* pDynamicOffsets
    ) = 0;
    virtual void vkCmdPushConstants(
        VkCommandBuffer commandBuffer,
        VkPipelineLayout layout,
        VkShaderStageFlags stageFlags,
        uint32_t offset,
        uint32_t size,
        const void* pValues
    ) = 0;
public: /* Queries */
    virtual VkResult vkCreateQueryPool(
        VkDevice device,
        const VkQueryPoolCreateInfo* pCreateInfo,
        const VkAllocationCallbacks* pAllocator,
        VkQueryPool* pQueryPool
    ) = 0;
    virtual void vkDestroyQueryPool(
        VkDevice device,
        VkQueryPool queryPool,
        const VkAllocationCallbacks* pAllocator
    ) = 0;
    virtual VkResult vkGetQueryPoolResults(
        VkDevice device,
        VkQueryPool queryPool,
        uint32_t firstQuery,
        uint32_t queryCount,
        size_t dataSize,
        void* pData,
        VkDeviceSize stride,
        VkQueryResultFlags flags
    ) = 0;
    virtual void vkCmdBeginQuery(
        VkCommandBuffer commandBuffer,
        VkQueryPool queryPool,
        uint32_t query,
        VkQueryControlFlags flags
    ) = 0;
    virtual void vkCmdEndQuery(
        VkCommandBuffer commandBuffer,
        VkQueryPool queryPool,
        uint32_t query
    ) = 0;
    virtual void vkCmdResetQueryPool(
        VkCommandBuffer commandBuffer,
        VkQueryPool queryPool,
        uint32_t firstQuery,
        uint32_t queryCount
    ) = 0;
    virtual void vkCmdWriteTimestamp(
        VkCommandBuffer commandBuffer,
        VkPipelineStageFlagBits pipelineStage,
        VkQueryPool queryPool,
        uint32_t query
    ) = 0;
    virtual void vkCmdCopyQueryPoolResults(
        VkCommandBuffer commandBuffer,
        VkQueryPool queryPool,
        uint32_t firstQuery,
        uint32_t queryCount,
        VkBuffer dstBuffer,
        VkDeviceSize dstOffset,
        VkDeviceSize stride,
        VkQueryResultFlags flags
    ) = 0;
public: /* Clear Commands */
    virtual void vkCmdUpdateBuffer(
        VkCommandBuffer commandBuffer,
        VkBuffer dstBuffer,
        VkDeviceSize dstOffset,
        VkDeviceSize dataSize,
        const void* pData
    ) = 0;
    virtual void vkCmdFillBuffer(
        VkCommandBuffer commandBuffer,
        VkBuffer dstBuffer,
        VkDeviceSize dstOffset,
        VkDeviceSize size,
        uint32_t data
    ) = 0;
    virtual void vkCmdClearColorImage(
        VkCommandBuffer commandBuffer,
        VkImage image,
        VkImageLayout imageLayout,
        const VkClearColorValue* pColor,
        uint32_t rangeCount,
        const VkImageSubresourceRange* pRanges
    ) = 0;
    virtual void vkCmdClearDepthStencilImage(
        VkCommandBuffer commandBuffer,
        VkImage image,
        VkImageLayout imageLayout,
        const VkClearDepthStencilValue* pDepthStencil,
        uint32_t rangeCount,
        const VkImageSubresourceRange* pRanges
    ) = 0;
    virtual void vkCmdClearAttachments(
        VkCommandBuffer commandBuffer,
        uint32_t attachmentCount,
        const VkClearAttachment* pAttachments,
        uint32_t rectCount,
        const VkClearRect* pRects
    ) = 0;
public: /* Copy Commands */
    virtual void vkCmdCopyBuffer(
        VkCommandBuffer commandBuffer,
        VkBuffer srcBuffer,
        VkBuffer dstBuffer,
        uint32_t regionCount,
        const VkBufferCopy* pRegions
    ) = 0;
    virtual void vkCmdCopyImage(
        VkCommandBuffer commandBuffer,
        VkImage srcImage,
        VkImageLayout srcImageLayout,
        VkImage dstImage,
        VkImageLayout dstImageLayout,
        uint32_t regionCount,
        const VkImageCopy* pRegions
    ) = 0;
    virtual void vkCmdBlitImage(
        VkCommandBuffer commandBuffer,
        VkImage srcImage,
        VkImageLayout srcImageLayout,
        VkImage dstImage,
        VkImageLayout dstImageLayout,
        uint32_t regionCount,
        const VkImageBlit* pRegions,
        VkFilter filter
    ) = 0;
    virtual void vkCmdCopyImageToBuffer(
        VkCommandBuffer commandBuffer,
        VkImage srcImage,
        VkImageLayout srcImageLayout,
        VkBuffer dstBuffer,
        uint32_t regionCount,
        const VkBufferImageCopy* pRegions
    ) = 0;
    virtual void vkCmdResolveImage(
        VkCommandBuffer commandBuffer,
        VkImage srcImage,
        VkImageLayout srcImageLayout,
        VkImage dstImage,
        VkImageLayout dstImageLayout,
        uint32_t regionCount,
        const VkImageResolve* pRegions
    ) = 0;
    virtual void vkCmdCopyBufferToImage(
        VkCommandBuffer commandBuffer,
         VkBuffer srcBuffer,
         VkImage dstImage,
         VkImageLayout dstImageLayout,
         uint32_t regionCount,
         const VkBufferImageCopy* pRegions
    ) = 0;
public: /* Drawing Commands */
    virtual void vkCmdBindIndexBuffer(
        VkCommandBuffer commandBuffer,
        VkBuffer buffer,
        VkDeviceSize offset,
        VkIndexType indexType
    ) = 0;
    virtual void vkCmdBindVertexBuffers(
        VkCommandBuffer commandBuffer,
        uint32_t firstBinding,
        uint32_t bindingCount,
        const VkBuffer* pBuffers,
        const VkDeviceSize* pOffsets
    ) = 0;
    virtual void vkCmdDraw(
        VkCommandBuffer commandBuffer,
        uint32_t vertexCount,
        uint32_t instanceCount,
        uint32_t firstVertex,
        uint32_t firstInstance
    ) = 0;
    virtual void vkCmdDrawIndexed(
        VkCommandBuffer commandBuffer,
        uint32_t indexCount,
        uint32_t instanceCount,
        uint32_t firstIndex,
        int32_t vertexOffset,
        uint32_t firstInstance
    ) = 0;
    virtual void vkCmdDrawIndirect(
        VkCommandBuffer commandBuffer,
        VkBuffer buffer,
        VkDeviceSize offset,
        uint32_t drawCount,
        uint32_t stride
    ) = 0;
    virtual void vkCmdDrawIndexedIndirect(
        VkCommandBuffer commandBuffer,
        VkBuffer buffer,
        VkDeviceSize offset,
        uint32_t drawCount,
        uint32_t stride
    ) = 0;
public: /* Fixed-Function Vertex Post Processing */
    virtual void vkCmdSetViewport(
        VkCommandBuffer commandBuffer,
        uint32_t firstViewport,
        uint32_t viewportCount,
        const VkViewport* pViewports
    ) = 0;
public: /* Rasterization */
    virtual void vkCmdSetLineWidth(
        VkCommandBuffer commandBuffer,
        float lineWidth
    ) = 0;
    virtual void vkCmdSetDepthBias(
        VkCommandBuffer commandBuffer,
        float depthBiasConstantFactor,
        float depthBiasClamp,
        float depthBiasSlopeFactor
    ) = 0;
public: /* Fragment Operations */
    virtual void vkCmdSetScissor(
        VkCommandBuffer commandBuffer,
        uint32_t firstScissor,
        uint32_t scissorCount,
        const VkRect2D* pScissors
    ) = 0;
public: /* The Framebuffer */
    virtual void vkCmdSetBlendConstants(
        VkCommandBuffer commandBuffer,
        const float blendConstants[4]
    ) = 0;
    virtual void vkCmdSetDepthBounds(
        VkCommandBuffer commandBuffer,
        float minDepthBounds,
        float maxDepthBounds
    ) = 0;
    virtual void vkCmdSetStencilCompareMask(
        VkCommandBuffer commandBuffer,
        VkStencilFaceFlags faceMask,
        uint32_t compareMask
    ) = 0;
    virtual void vkCmdSetStencilWriteMask(
        VkCommandBuffer commandBuffer,
        VkStencilFaceFlags faceMask,
        uint32_t writeMask
    ) = 0;
    virtual void vkCmdSetStencilReference(
        VkCommandBuffer commandBuffer,
        VkStencilFaceFlags faceMask,
        uint32_t reference
    ) = 0;
public: /* Dispatching Commands */
    virtual void vkCmdDispatch(
        VkCommandBuffer commandBuffer,
        uint32_t groupCountX,
        uint32_t groupCountY,
        uint32_t groupCountZ
    ) = 0;
    virtual void vkCmdDispatchIndirect(
        VkCommandBuffer commandBuffer,
        VkBuffer buffer,
        VkDeviceSize offset
    ) = 0;
public: /* Sparse Resources */
    virtual VkResult vkQueueBindSparse(
        VkQueue queue,
        uint32_t bindInfoCount,
        const VkBindSparseInfo* pBindInfo,
        VkFence fence
    ) = 0;
public: /* Window System Integration (WSI) */
#if MMP_PLATFORM(WINDOWS)
    virtual VkResult vkCreateWin32SurfaceKHR(
        VkInstance instance,
        const VkWin32SurfaceCreateInfoKHR* pCreateInfo,
        const VkAllocationCallbacks* pAllocator,
        VkSurfaceKHR* pSurface
    ) = 0;
#endif
    virtual void vkDestroySurfaceKHR(
        VkInstance instance,
        VkSurfaceKHR surface,
        const VkAllocationCallbacks* pAllocator
    ) = 0;
    virtual VkResult vkCreateSwapchainKHR(
        VkDevice device,
        const VkSwapchainCreateInfoKHR* pCreateInfo,
        const VkAllocationCallbacks* pAllocator,
        VkSwapchainKHR* pSwapchain
    ) = 0;
    virtual void vkDestroySwapchainKHR(
        VkDevice device,
        VkSwapchainKHR swapchain,
        const VkAllocationCallbacks* pAllocator
    ) = 0;
    virtual VkResult vkGetSwapchainImagesKHR(
        VkDevice device,
        VkSwapchainKHR swapchain,
        uint32_t* pSwapchainImageCount,
        VkImage* pSwapchainImages
    ) = 0;
    virtual VkResult vkAcquireNextImageKHR(
        VkDevice device,
        VkSwapchainKHR swapchain,
        uint64_t timeout,
        VkSemaphore semaphore,
        VkFence fence,
        uint32_t* pImageIndex
    ) = 0;
    virtual VkResult vkQueuePresentKHR(
        VkQueue queue,
        const VkPresentInfoKHR* pPresentInfo
    ) = 0;
    virtual VkResult vkGetPhysicalDeviceSurfaceCapabilitiesKHR(
        VkPhysicalDevice physicalDevice,
        VkSurfaceKHR surface,
        VkSurfaceCapabilitiesKHR* pSurfaceCapabilities
    ) = 0;
    virtual VkResult vkGetPhysicalDeviceSurfacePresentModesKHR(
        VkPhysicalDevice physicalDevice,
        VkSurfaceKHR surface,
        uint32_t* pPresentModeCount,
        VkPresentModeKHR* pPresentModes
    ) = 0;
public: /* Deferred Host Operations */
    virtual VkResult vkWaitForPresentKHR(
        VkDevice device,
        VkSwapchainKHR swapchain,
        uint64_t presentId,
        uint64_t timeout
    ) = 0;
public: /* Features */
    virtual void vkGetPhysicalDeviceFeatures(
        VkPhysicalDevice physicalDevice,
        VkPhysicalDeviceFeatures* pFeatures
    ) = 0;
    virtual void vkGetPhysicalDeviceFormatProperties(
        VkPhysicalDevice physicalDevice,
        VkFormat format,
        VkFormatProperties* pFormatProperties
    ) = 0;
public: /* Additional Capabilities */
    virtual VkResult vkGetPhysicalDeviceImageFormatProperties(
        VkPhysicalDevice physicalDevice,
        VkFormat format,
        VkImageType type,
        VkImageTiling tiling,
        VkImageUsageFlags usage,
        VkImageCreateFlags flags,
        VkImageFormatProperties* pImageFormatProperties
    ) = 0;
public: /* Extending Vulkan */
    virtual VkResult vkEnumerateInstanceExtensionProperties(
        const char* pLayerName,
        uint32_t* pPropertyCount,
        VkExtensionProperties* pProperties
    ) = 0;
    virtual VkResult vkEnumerateDeviceExtensionProperties(
        VkPhysicalDevice physicalDevice,
        const char* pLayerName,
        uint32_t* pPropertyCount,
        VkExtensionProperties* pProperties
    ) = 0;
    virtual VkResult vkEnumerateInstanceLayerProperties(
        uint32_t* pPropertyCount,
        VkLayerProperties* pProperties
    ) = 0;
    virtual VkResult vkEnumerateDeviceLayerProperties(
        VkPhysicalDevice physicalDevice,
        uint32_t* pPropertyCount,
        VkLayerProperties* pProperties
    ) = 0;
};

} // namespace Mmp