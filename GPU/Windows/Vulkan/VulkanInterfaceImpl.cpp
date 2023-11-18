#include "VulkanInterfaceImpl.h"

#include <cassert>

namespace Mmp
{
VulkanInterfaceImpl::VulkanInterfaceImpl()
{
    GL_LOG_INFO << "Load vulkan related runtime";
    _vulkanLoader.load("vulkan-1.dll");
    assert(_vulkanLoader.isLoaded());
    GL_LOG_INFO << "Load vulkan successfully";
}

VulkanInterfaceImpl::~VulkanInterfaceImpl()
{
    _vulkanLoader.unload();
}

void VulkanInterfaceImpl::LoadBasicFunc()
{
    #define LOAD_BAISC_VULKAN_FUNC(x) _##x = (PFN_##x)_vulkanLoader.getSymbol(#x)

    LOAD_BAISC_VULKAN_FUNC(vkCreateInstance);
    LOAD_BAISC_VULKAN_FUNC(vkGetInstanceProcAddr);
    LOAD_BAISC_VULKAN_FUNC(vkGetDeviceProcAddr);

    LOAD_BAISC_VULKAN_FUNC(vkEnumerateInstanceVersion);
    LOAD_BAISC_VULKAN_FUNC(vkEnumerateInstanceExtensionProperties);
    LOAD_BAISC_VULKAN_FUNC(vkEnumerateInstanceLayerProperties);

    assert(
        _vkCreateInstance && _vkGetInstanceProcAddr && _vkGetDeviceProcAddr 
        && _vkEnumerateInstanceVersion && _vkEnumerateInstanceExtensionProperties 
        && _vkEnumerateInstanceLayerProperties
    );

    #undef LOAD_BAISC_VULKAN_FUNC
}

void VulkanInterfaceImpl::LoadInstanceFunc(VkInstance instance)
{
    #define LOAD_INSTANCE_VULKAN_FUNC(x) _##x = (PFN_##x)_vkGetInstanceProcAddr(instance, #x)

    LOAD_INSTANCE_VULKAN_FUNC(vkDestroyInstance);
    LOAD_INSTANCE_VULKAN_FUNC(vkEnumeratePhysicalDevices);
    LOAD_INSTANCE_VULKAN_FUNC(vkGetPhysicalDeviceFeatures);
    LOAD_INSTANCE_VULKAN_FUNC(vkGetPhysicalDeviceFormatProperties);
    LOAD_INSTANCE_VULKAN_FUNC(vkGetPhysicalDeviceImageFormatProperties);
    LOAD_INSTANCE_VULKAN_FUNC(vkGetPhysicalDeviceProperties);
    LOAD_INSTANCE_VULKAN_FUNC(vkGetPhysicalDeviceQueueFamilyProperties);
    LOAD_INSTANCE_VULKAN_FUNC(vkGetPhysicalDeviceMemoryProperties);
    LOAD_INSTANCE_VULKAN_FUNC(vkCreateDevice);
    LOAD_INSTANCE_VULKAN_FUNC(vkDestroyDevice);
    LOAD_INSTANCE_VULKAN_FUNC(vkEnumerateDeviceExtensionProperties);
    LOAD_INSTANCE_VULKAN_FUNC(vkEnumerateDeviceLayerProperties);
    LOAD_INSTANCE_VULKAN_FUNC(vkGetDeviceQueue);
    LOAD_INSTANCE_VULKAN_FUNC(vkDeviceWaitIdle);

    LOAD_INSTANCE_VULKAN_FUNC(vkCreateSwapchainKHR);
    LOAD_INSTANCE_VULKAN_FUNC(vkDestroySwapchainKHR);
    LOAD_INSTANCE_VULKAN_FUNC(vkGetSwapchainImagesKHR);
    LOAD_INSTANCE_VULKAN_FUNC(vkAcquireNextImageKHR);
    LOAD_INSTANCE_VULKAN_FUNC(vkQueuePresentKHR);

#if MMP_PLATFORM(WINDOWS)
    LOAD_INSTANCE_VULKAN_FUNC(vkCreateWin32SurfaceKHR);
#endif

    LOAD_INSTANCE_VULKAN_FUNC(vkDestroySurfaceKHR);
    LOAD_INSTANCE_VULKAN_FUNC(vkGetPhysicalDeviceSurfaceCapabilitiesKHR);
    LOAD_INSTANCE_VULKAN_FUNC(vkGetPhysicalDeviceSurfacePresentModesKHR);

    #undef LOAD_INSTANCE_VULKAN_FUNC
}

void VulkanInterfaceImpl::LoadDeviceFunc(VkDevice device, const VulkanExtensions &enabledExtensions)
{
    #define LOAD_DEVICE_VULKAN_FUNC(x) _##x = (PFN_##x)_vkGetDeviceProcAddr(device, #x)

	LOAD_DEVICE_VULKAN_FUNC(vkQueueSubmit);
	LOAD_DEVICE_VULKAN_FUNC(vkQueueWaitIdle);
	LOAD_DEVICE_VULKAN_FUNC(vkAllocateMemory);
	LOAD_DEVICE_VULKAN_FUNC(vkFreeMemory);
	LOAD_DEVICE_VULKAN_FUNC(vkMapMemory);
	LOAD_DEVICE_VULKAN_FUNC(vkUnmapMemory);
	LOAD_DEVICE_VULKAN_FUNC(vkFlushMappedMemoryRanges);
	LOAD_DEVICE_VULKAN_FUNC(vkInvalidateMappedMemoryRanges);
	LOAD_DEVICE_VULKAN_FUNC(vkGetDeviceMemoryCommitment);
	LOAD_DEVICE_VULKAN_FUNC(vkBindBufferMemory);
	LOAD_DEVICE_VULKAN_FUNC(vkBindBufferMemory2);
	LOAD_DEVICE_VULKAN_FUNC(vkBindImageMemory);
	LOAD_DEVICE_VULKAN_FUNC(vkBindImageMemory2);
	LOAD_DEVICE_VULKAN_FUNC(vkGetBufferMemoryRequirements);
	LOAD_DEVICE_VULKAN_FUNC(vkGetBufferMemoryRequirements2);
	LOAD_DEVICE_VULKAN_FUNC(vkGetDeviceBufferMemoryRequirements);
	LOAD_DEVICE_VULKAN_FUNC(vkGetImageMemoryRequirements);
	LOAD_DEVICE_VULKAN_FUNC(vkGetImageMemoryRequirements2);
	LOAD_DEVICE_VULKAN_FUNC(vkGetDeviceImageMemoryRequirements);
	LOAD_DEVICE_VULKAN_FUNC(vkCreateFence);
	LOAD_DEVICE_VULKAN_FUNC(vkDestroyFence);
	LOAD_DEVICE_VULKAN_FUNC(vkResetFences);
	LOAD_DEVICE_VULKAN_FUNC(vkGetFenceStatus);
	LOAD_DEVICE_VULKAN_FUNC(vkWaitForFences);
	LOAD_DEVICE_VULKAN_FUNC(vkCreateSemaphore);
	LOAD_DEVICE_VULKAN_FUNC(vkDestroySemaphore);
	LOAD_DEVICE_VULKAN_FUNC(vkCreateEvent);
	LOAD_DEVICE_VULKAN_FUNC(vkDestroyEvent);
	LOAD_DEVICE_VULKAN_FUNC(vkGetEventStatus);
	LOAD_DEVICE_VULKAN_FUNC(vkSetEvent);
	LOAD_DEVICE_VULKAN_FUNC(vkResetEvent);
	LOAD_DEVICE_VULKAN_FUNC(vkCreateQueryPool);
	LOAD_DEVICE_VULKAN_FUNC(vkDestroyQueryPool);
	LOAD_DEVICE_VULKAN_FUNC(vkGetQueryPoolResults);
	LOAD_DEVICE_VULKAN_FUNC(vkCreateBuffer);
	LOAD_DEVICE_VULKAN_FUNC(vkDestroyBuffer);
	LOAD_DEVICE_VULKAN_FUNC(vkCreateBufferView);
	LOAD_DEVICE_VULKAN_FUNC(vkDestroyBufferView);
	LOAD_DEVICE_VULKAN_FUNC(vkCreateImage);
	LOAD_DEVICE_VULKAN_FUNC(vkDestroyImage);
	LOAD_DEVICE_VULKAN_FUNC(vkGetImageSubresourceLayout);
	LOAD_DEVICE_VULKAN_FUNC(vkCreateImageView);
	LOAD_DEVICE_VULKAN_FUNC(vkDestroyImageView);
	LOAD_DEVICE_VULKAN_FUNC(vkCreateShaderModule);
	LOAD_DEVICE_VULKAN_FUNC(vkDestroyShaderModule);
	LOAD_DEVICE_VULKAN_FUNC(vkCreatePipelineCache);
	LOAD_DEVICE_VULKAN_FUNC(vkDestroyPipelineCache);
	LOAD_DEVICE_VULKAN_FUNC(vkGetPipelineCacheData);
	LOAD_DEVICE_VULKAN_FUNC(vkMergePipelineCaches);
	LOAD_DEVICE_VULKAN_FUNC(vkCreateGraphicsPipelines);
	LOAD_DEVICE_VULKAN_FUNC(vkCreateComputePipelines);
	LOAD_DEVICE_VULKAN_FUNC(vkDestroyPipeline);
	LOAD_DEVICE_VULKAN_FUNC(vkCreatePipelineLayout);
	LOAD_DEVICE_VULKAN_FUNC(vkDestroyPipelineLayout);
	LOAD_DEVICE_VULKAN_FUNC(vkCreateSampler);
	LOAD_DEVICE_VULKAN_FUNC(vkDestroySampler);
	LOAD_DEVICE_VULKAN_FUNC(vkCreateDescriptorSetLayout);
	LOAD_DEVICE_VULKAN_FUNC(vkDestroyDescriptorSetLayout);
	LOAD_DEVICE_VULKAN_FUNC(vkCreateDescriptorPool);
	LOAD_DEVICE_VULKAN_FUNC(vkDestroyDescriptorPool);
	LOAD_DEVICE_VULKAN_FUNC(vkResetDescriptorPool);
	LOAD_DEVICE_VULKAN_FUNC(vkAllocateDescriptorSets);
	LOAD_DEVICE_VULKAN_FUNC(vkFreeDescriptorSets);
	LOAD_DEVICE_VULKAN_FUNC(vkUpdateDescriptorSets);
	LOAD_DEVICE_VULKAN_FUNC(vkCreateFramebuffer);
	LOAD_DEVICE_VULKAN_FUNC(vkDestroyFramebuffer);
	LOAD_DEVICE_VULKAN_FUNC(vkCreateRenderPass);
	LOAD_DEVICE_VULKAN_FUNC(vkDestroyRenderPass);
	LOAD_DEVICE_VULKAN_FUNC(vkGetRenderAreaGranularity);
	LOAD_DEVICE_VULKAN_FUNC(vkCreateCommandPool);
	LOAD_DEVICE_VULKAN_FUNC(vkDestroyCommandPool);
	LOAD_DEVICE_VULKAN_FUNC(vkResetCommandPool);
	LOAD_DEVICE_VULKAN_FUNC(vkAllocateCommandBuffers);
	LOAD_DEVICE_VULKAN_FUNC(vkFreeCommandBuffers);
	LOAD_DEVICE_VULKAN_FUNC(vkBeginCommandBuffer);
	LOAD_DEVICE_VULKAN_FUNC(vkEndCommandBuffer);
	LOAD_DEVICE_VULKAN_FUNC(vkResetCommandBuffer);
	LOAD_DEVICE_VULKAN_FUNC(vkCmdBindPipeline);
	LOAD_DEVICE_VULKAN_FUNC(vkCmdSetViewport);
	LOAD_DEVICE_VULKAN_FUNC(vkCmdSetScissor);
	LOAD_DEVICE_VULKAN_FUNC(vkCmdSetLineWidth);
	LOAD_DEVICE_VULKAN_FUNC(vkCmdSetDepthBias);
	LOAD_DEVICE_VULKAN_FUNC(vkCmdSetBlendConstants);
	LOAD_DEVICE_VULKAN_FUNC(vkCmdSetDepthBounds);
	LOAD_DEVICE_VULKAN_FUNC(vkCmdSetStencilCompareMask);
	LOAD_DEVICE_VULKAN_FUNC(vkCmdSetStencilWriteMask);
	LOAD_DEVICE_VULKAN_FUNC(vkCmdSetStencilReference);
	LOAD_DEVICE_VULKAN_FUNC(vkCmdBindDescriptorSets);
	LOAD_DEVICE_VULKAN_FUNC(vkCmdBindIndexBuffer);
	LOAD_DEVICE_VULKAN_FUNC(vkCmdBindVertexBuffers);
	LOAD_DEVICE_VULKAN_FUNC(vkCmdDraw);
	LOAD_DEVICE_VULKAN_FUNC(vkCmdDrawIndexed);
	LOAD_DEVICE_VULKAN_FUNC(vkCmdDrawIndirect);
	LOAD_DEVICE_VULKAN_FUNC(vkCmdDrawIndexedIndirect);
	LOAD_DEVICE_VULKAN_FUNC(vkCmdDispatch);
	LOAD_DEVICE_VULKAN_FUNC(vkCmdDispatchIndirect);
	LOAD_DEVICE_VULKAN_FUNC(vkCmdCopyBuffer);
	LOAD_DEVICE_VULKAN_FUNC(vkCmdCopyImage);
	LOAD_DEVICE_VULKAN_FUNC(vkCmdBlitImage);
	LOAD_DEVICE_VULKAN_FUNC(vkCmdCopyBufferToImage);
	LOAD_DEVICE_VULKAN_FUNC(vkCmdCopyImageToBuffer);
	LOAD_DEVICE_VULKAN_FUNC(vkCmdUpdateBuffer);
	LOAD_DEVICE_VULKAN_FUNC(vkCmdFillBuffer);
	LOAD_DEVICE_VULKAN_FUNC(vkCmdClearColorImage);
	LOAD_DEVICE_VULKAN_FUNC(vkCmdClearDepthStencilImage);
	LOAD_DEVICE_VULKAN_FUNC(vkCmdClearAttachments);
	LOAD_DEVICE_VULKAN_FUNC(vkCmdResolveImage);
	LOAD_DEVICE_VULKAN_FUNC(vkCmdSetEvent);
	LOAD_DEVICE_VULKAN_FUNC(vkCmdResetEvent);
	LOAD_DEVICE_VULKAN_FUNC(vkCmdWaitEvents);
	LOAD_DEVICE_VULKAN_FUNC(vkCmdPipelineBarrier);
	LOAD_DEVICE_VULKAN_FUNC(vkCmdBeginQuery);
	LOAD_DEVICE_VULKAN_FUNC(vkCmdEndQuery);
	LOAD_DEVICE_VULKAN_FUNC(vkCmdResetQueryPool);
	LOAD_DEVICE_VULKAN_FUNC(vkCmdWriteTimestamp);
	LOAD_DEVICE_VULKAN_FUNC(vkCmdCopyQueryPoolResults);
	LOAD_DEVICE_VULKAN_FUNC(vkCmdPushConstants);
	LOAD_DEVICE_VULKAN_FUNC(vkCmdBeginRenderPass);
	LOAD_DEVICE_VULKAN_FUNC(vkCmdNextSubpass);
	LOAD_DEVICE_VULKAN_FUNC(vkCmdEndRenderPass);
	LOAD_DEVICE_VULKAN_FUNC(vkCmdExecuteCommands);

	if (enabledExtensions.KHR_present_wait) 
    {
		LOAD_DEVICE_VULKAN_FUNC(vkWaitForPresentKHR);
	}
    else
    {
        _vkWaitForPresentKHR = nullptr;
    }
	if (enabledExtensions.KHR_dedicated_allocation) 
    {
		LOAD_DEVICE_VULKAN_FUNC(vkGetBufferMemoryRequirements2KHR);
		LOAD_DEVICE_VULKAN_FUNC(vkGetImageMemoryRequirements2KHR);
	}
    else
    {
        _vkGetBufferMemoryRequirements2KHR = nullptr;
        _vkGetImageMemoryRequirements2KHR = nullptr;
    }
	if (enabledExtensions.KHR_create_renderpass2) 
    {
		LOAD_DEVICE_VULKAN_FUNC(vkCreateRenderPass2KHR);
	}
    else
    {
        _vkCreateRenderPass2KHR = nullptr;
    }

    #undef LOAD_DEVICE_VULKAN_FUNC
}

} // namespace Mmp

namespace Mmp
{

PFN_vkVoidFunction VulkanInterfaceImpl::vkGetInstanceProcAddr(
        VkInstance instance,
        const char* pName
    )
{
    return _vkGetInstanceProcAddr(instance, pName);
}

VkResult VulkanInterfaceImpl::vkCreateInstance(
        const VkInstanceCreateInfo* pCreateInfo,
        const VkAllocationCallbacks* pAllocator,
        VkInstance* pInstance
    )
{
    return _vkCreateInstance(pCreateInfo, pAllocator, pInstance);
}

void VulkanInterfaceImpl::vkDestroyInstance(
        VkInstance instance,
        const VkAllocationCallbacks* pAllocator
    )
{
    _vkDestroyInstance(instance, pAllocator);
}

VkResult VulkanInterfaceImpl::vkEnumerateInstanceVersion(
        uint32_t* pApiVersion
    )
{
    return _vkEnumerateInstanceVersion(pApiVersion);
}

PFN_vkVoidFunction VulkanInterfaceImpl::vkGetDeviceProcAddr(
        VkDevice device,
        const char* pName
    )
{
    return _vkGetDeviceProcAddr(device, pName);
}

VkResult VulkanInterfaceImpl::vkEnumeratePhysicalDevices(
        VkInstance instance,
        uint32_t* pPhysicalDeviceCount,
        VkPhysicalDevice* pPhysicalDevices
    )
{
    return _vkEnumeratePhysicalDevices(instance, pPhysicalDeviceCount, pPhysicalDevices);
}

void VulkanInterfaceImpl::vkGetPhysicalDeviceProperties(
        VkPhysicalDevice physicalDevice,
        VkPhysicalDeviceProperties* pProperties
    )
{
    _vkGetPhysicalDeviceProperties(physicalDevice, pProperties);
}

void VulkanInterfaceImpl::vkGetPhysicalDeviceQueueFamilyProperties(
        VkPhysicalDevice physicalDevice,
        uint32_t* pQueueFamilyPropertyCount,
        VkQueueFamilyProperties* pQueueFamilyProperties
    )
{
    _vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, pQueueFamilyPropertyCount, pQueueFamilyProperties);
}

VkResult VulkanInterfaceImpl::vkCreateDevice(
        VkPhysicalDevice physicalDevice,
        const VkDeviceCreateInfo* pCreateInfo,
        const VkAllocationCallbacks* pAllocator,
        VkDevice* pDevice
    )
{
    return _vkCreateDevice(physicalDevice, pCreateInfo, pAllocator, pDevice);
}

void VulkanInterfaceImpl::vkDestroyDevice(
        VkDevice device,
        const VkAllocationCallbacks* pAllocator
    )
{
    _vkDestroyDevice(device, pAllocator);
}

void VulkanInterfaceImpl::vkGetDeviceQueue(
        VkDevice device,
        uint32_t queueFamilyIndex,
        uint32_t queueIndex,
        VkQueue* pQueue
    )
{
    _vkGetDeviceQueue(device, queueFamilyIndex, queueIndex, pQueue);
}

VkResult VulkanInterfaceImpl::vkQueueSubmit(
        VkQueue queue,
        uint32_t submitCount,
        const VkSubmitInfo* pSubmits,
        VkFence fence
    )
{
    return _vkQueueSubmit(queue, submitCount, pSubmits, fence);
}

VkResult VulkanInterfaceImpl::vkCreateCommandPool(
        VkDevice device,
        const VkCommandPoolCreateInfo* pCreateInfo,
        const VkAllocationCallbacks* pAllocator,
        VkCommandPool* pCommandPool
    )
{
    return _vkCreateCommandPool(device, pCreateInfo, pAllocator, pCommandPool);
}

void VulkanInterfaceImpl::vkDestroyCommandPool(
        VkDevice device,
        VkCommandPool commandPool,
        const VkAllocationCallbacks* pAllocator
    )
{
    _vkDestroyCommandPool(device, commandPool, pAllocator);
}

VkResult VulkanInterfaceImpl::vkResetCommandPool(
        VkDevice device,
        VkCommandPool commandPool,
        VkCommandPoolResetFlags flags
    )
{
    return _vkResetCommandPool(device, commandPool, flags);
}

VkResult VulkanInterfaceImpl::vkAllocateCommandBuffers(
        VkDevice device,
        const VkCommandBufferAllocateInfo* pAllocateInfo,
        VkCommandBuffer* pCommandBuffers
    )
{
    return _vkAllocateCommandBuffers(device, pAllocateInfo, pCommandBuffers);
}

void VulkanInterfaceImpl::vkFreeCommandBuffers(
        VkDevice device,
        VkCommandPool commandPool,
        uint32_t commandBufferCount,
        const VkCommandBuffer* pCommandBuffers
    )
{
    _vkFreeCommandBuffers(device, commandPool, commandBufferCount, pCommandBuffers);
}

VkResult VulkanInterfaceImpl::vkBeginCommandBuffer(
        VkCommandBuffer commandBuffer,
        const VkCommandBufferBeginInfo* pBeginInfo
    )
{
    return _vkBeginCommandBuffer(commandBuffer, pBeginInfo);
}

VkResult VulkanInterfaceImpl::vkEndCommandBuffer(
        VkCommandBuffer commandBuffer
    )
{
    return _vkEndCommandBuffer(commandBuffer);
}

VkResult VulkanInterfaceImpl::vkResetCommandBuffer(
        VkCommandBuffer commandBuffer,
        VkCommandBufferResetFlags flags
    )
{
    return _vkResetCommandBuffer(commandBuffer, flags);
}

void VulkanInterfaceImpl::vkCmdExecuteCommands(
        VkCommandBuffer commandBuffer,
        uint32_t commandBufferCount,
        const VkCommandBuffer* pCommandBuffers
    )
{
    _vkCmdExecuteCommands(commandBuffer, commandBufferCount, pCommandBuffers);
}

VkResult VulkanInterfaceImpl::vkQueueWaitIdle(
        VkQueue queue
    )
{
    return _vkQueueWaitIdle(queue);
}

VkResult VulkanInterfaceImpl::vkDeviceWaitIdle(
        VkDevice device
    )
{
    return _vkDeviceWaitIdle(device);
}

VkResult VulkanInterfaceImpl::vkCreateFence(
        VkDevice device,
        const VkFenceCreateInfo* pCreateInfo,
        const VkAllocationCallbacks* pAllocator,
        VkFence* pFence
    )
{
    return _vkCreateFence(device, pCreateInfo, pAllocator, pFence);
}

void VulkanInterfaceImpl::vkDestroyFence(
        VkDevice device,
        VkFence fence,
        const VkAllocationCallbacks* pAllocator
    )
{
    return _vkDestroyFence(device, fence, pAllocator);
}

VkResult VulkanInterfaceImpl::vkResetFences(
        VkDevice device,
        uint32_t fenceCount,
        const VkFence* pFences
    )
{
    return _vkResetFences(device, fenceCount, pFences);
}

VkResult VulkanInterfaceImpl::vkGetFenceStatus(
        VkDevice device,
        VkFence fence
    )
{
    return _vkGetFenceStatus(device, fence);
}

VkResult VulkanInterfaceImpl::vkWaitForFences(
        VkDevice device,
        uint32_t fenceCount,
        const VkFence* pFences,
        VkBool32 waitAll,
        uint64_t timeout
    )
{
    return _vkWaitForFences(device, fenceCount, pFences, waitAll, timeout);
}

VkResult VulkanInterfaceImpl::vkCreateSemaphore(
        VkDevice device,
        const VkSemaphoreCreateInfo* pCreateInfo,
        const VkAllocationCallbacks* pAllocator,
        VkSemaphore* pSemaphore
    )
{
    return _vkCreateSemaphore(device, pCreateInfo, pAllocator, pSemaphore);
}

void VulkanInterfaceImpl::vkDestroySemaphore(
        VkDevice device,
        VkSemaphore semaphore,
        const VkAllocationCallbacks* pAllocator
    )
{
    return _vkDestroySemaphore(device, semaphore, pAllocator);
}

VkResult VulkanInterfaceImpl::vkCreateEvent(
        VkDevice device,
        const VkEventCreateInfo* pCreateInfo,
        const VkAllocationCallbacks* pAllocator,
        VkEvent* pEvent
    )
{
    return _vkCreateEvent(device, pCreateInfo, pAllocator, pEvent);
}

void VulkanInterfaceImpl::vkDestroyEvent(
        VkDevice device,
        VkEvent event,
        const VkAllocationCallbacks* pAllocator
    )
{
    _vkDestroyEvent(device, event, pAllocator);
}

VkResult VulkanInterfaceImpl::vkGetEventStatus(
        VkDevice device,
        VkEvent event
    )
{
    return _vkGetEventStatus(device, event);
}

VkResult VulkanInterfaceImpl::vkSetEvent(
        VkDevice device,
        VkEvent event
    )
{
    return _vkSetEvent(device, event);
}

VkResult VulkanInterfaceImpl::vkResetEvent(
        VkDevice device,
        VkEvent event
    )
{
    return _vkResetEvent(device, event);
}

void VulkanInterfaceImpl::vkCmdSetEvent(
        VkCommandBuffer commandBuffer,
        VkEvent event,
        VkPipelineStageFlags stageMask
    )
{
    _vkCmdSetEvent(commandBuffer, event, stageMask);
}

void VulkanInterfaceImpl::vkCmdResetEvent(
        VkCommandBuffer commandBuffer,
        VkEvent event,
        VkPipelineStageFlags stageMask
    )
{
    _vkCmdResetEvent(commandBuffer, event, stageMask);
}

void VulkanInterfaceImpl::vkCmdWaitEvents(
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
    )
{
    _vkCmdWaitEvents(commandBuffer, eventCount, pEvents, srcStageMask, dstStageMask, memoryBarrierCount, pMemoryBarriers, bufferMemoryBarrierCount, pBufferMemoryBarriers, imageMemoryBarrierCount, pImageMemoryBarriers);
}

void VulkanInterfaceImpl::vkCmdBeginRenderPass(
        VkCommandBuffer commandBuffer,
        const VkRenderPassBeginInfo* pRenderPassBegin,
        VkSubpassContents contents
    )
{
    _vkCmdBeginRenderPass(commandBuffer, pRenderPassBegin, contents);
}

void VulkanInterfaceImpl::vkCmdNextSubpass(
        VkCommandBuffer commandBuffer,
        VkSubpassContents contents
    )
{
    _vkCmdNextSubpass(commandBuffer, contents);
}

void VulkanInterfaceImpl::vkCmdEndRenderPass(
        VkCommandBuffer commandBuffer
    )
{
    _vkCmdEndRenderPass(commandBuffer);
}

VkResult VulkanInterfaceImpl::vkCreateShaderModule(
        VkDevice device,
        const VkShaderModuleCreateInfo* pCreateInfo, 
        const VkAllocationCallbacks* pAllocator, 
        VkShaderModule* pShaderModule
    )
{
    return _vkCreateShaderModule(device, pCreateInfo, pAllocator, pShaderModule);
}

void VulkanInterfaceImpl::vkDestroyShaderModule(
        VkDevice device,
        VkShaderModule shaderModule,
        const VkAllocationCallbacks* pAllocator
    )
{
    _vkDestroyShaderModule(device, shaderModule, pAllocator);
}

VkResult VulkanInterfaceImpl::vkCreateFramebuffer(
        VkDevice device,
        const VkFramebufferCreateInfo* pCreateInfo,
        const VkAllocationCallbacks* pAllocator,
        VkFramebuffer* pFramebuffer
    )
{
    return _vkCreateFramebuffer(device, pCreateInfo, pAllocator, pFramebuffer);
}

void VulkanInterfaceImpl::vkDestroyFramebuffer(
        VkDevice device,
        VkFramebuffer framebuffer,
        const VkAllocationCallbacks* pAllocator
    )
{
    _vkDestroyFramebuffer(device, framebuffer, pAllocator);
}

VkResult VulkanInterfaceImpl::vkCreateRenderPass(
        VkDevice device,
        const VkRenderPassCreateInfo* pCreateInfo,
        const VkAllocationCallbacks* pAllocator,
        VkRenderPass* pRenderPass
    )
{
    return _vkCreateRenderPass(device, pCreateInfo, pAllocator, pRenderPass);
}

void VulkanInterfaceImpl::vkDestroyRenderPass(
        VkDevice device,
        VkRenderPass renderPass,
        const VkAllocationCallbacks* pAllocator
    )
{
    _vkDestroyRenderPass(device, renderPass, pAllocator);
}

void VulkanInterfaceImpl::vkGetRenderAreaGranularity(
        VkDevice device,
        VkRenderPass renderPass,
        VkExtent2D* pGranularity
    )
{
    _vkGetRenderAreaGranularity(device, renderPass, pGranularity);
}

VkResult VulkanInterfaceImpl::vkCreateRenderPass2KHR(
        VkDevice device,
        const VkRenderPassCreateInfo2* pCreateInfo,
        const VkAllocationCallbacks* pAllocator,
        VkRenderPass* pRenderPass
    )
{
    return _vkCreateRenderPass2KHR(device, pCreateInfo, pAllocator, pRenderPass);
}

VkResult VulkanInterfaceImpl::vkCreatePipelineCache(
        VkDevice device,
        const VkPipelineCacheCreateInfo* pCreateInfo,
        const VkAllocationCallbacks* pAllocator,
        VkPipelineCache* pPipelineCache
    )
{
    return _vkCreatePipelineCache(device, pCreateInfo, pAllocator, pPipelineCache);
}

void VulkanInterfaceImpl::vkDestroyPipelineCache(
        VkDevice device,
        VkPipelineCache pipelineCache,
        const VkAllocationCallbacks* pAllocator
    )
{
    _vkDestroyPipelineCache(device, pipelineCache, pAllocator);
}

VkResult VulkanInterfaceImpl::vkGetPipelineCacheData(
        VkDevice device,
        VkPipelineCache pipelineCache,
        size_t* pDataSize,
        void* pData
    ) 
{
    return _vkGetPipelineCacheData(device, pipelineCache, pDataSize, pData);
}

VkResult VulkanInterfaceImpl::vkMergePipelineCaches(
        VkDevice device,
        VkPipelineCache dstCache,
        uint32_t srcCacheCount,
        const VkPipelineCache* pSrcCaches
    )
{
    return _vkMergePipelineCaches(device, dstCache, srcCacheCount, pSrcCaches);
}

VkResult VulkanInterfaceImpl::vkCreateGraphicsPipelines(
        VkDevice device,
        VkPipelineCache pipelineCache,
        uint32_t createInfoCount,
        const VkGraphicsPipelineCreateInfo* pCreateInfos,
        const VkAllocationCallbacks* pAllocator,
        VkPipeline* pPipelines
    )
{
    return _vkCreateGraphicsPipelines(device, pipelineCache, createInfoCount, pCreateInfos, pAllocator, pPipelines);
}

VkResult VulkanInterfaceImpl::vkCreateComputePipelines(
        VkDevice device,
        VkPipelineCache pipelineCache,
        uint32_t createInfoCount,
        const VkComputePipelineCreateInfo* pCreateInfos,
        const VkAllocationCallbacks* pAllocator,
        VkPipeline* pPipelines
    )
{
    return _vkCreateComputePipelines(device, pipelineCache, createInfoCount, pCreateInfos, pAllocator, pPipelines);
}

void VulkanInterfaceImpl::vkDestroyPipeline(
        VkDevice device,
        VkPipeline pipeline,
        const VkAllocationCallbacks* pAllocator
    )
{
    return _vkDestroyPipeline(device, pipeline, pAllocator);
}

void VulkanInterfaceImpl::vkCmdBindPipeline(
        VkCommandBuffer commandBuffer,
        VkPipelineBindPoint pipelineBindPoint,
        VkPipeline pipeline
    )
{
    _vkCmdBindPipeline(commandBuffer, pipelineBindPoint, pipeline);
}

void VulkanInterfaceImpl::vkCmdPipelineBarrier(
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
    )
{
    _vkCmdPipelineBarrier(commandBuffer, srcStageMask, dstStageMask, dependencyFlags,
        memoryBarrierCount, pMemoryBarriers, bufferMemoryBarrierCount, pBufferMemoryBarriers,
        imageMemoryBarrierCount, pImageMemoryBarriers
    );
}

void VulkanInterfaceImpl::vkGetPhysicalDeviceMemoryProperties(
        VkPhysicalDevice physicalDevice,
        VkPhysicalDeviceMemoryProperties* pMemoryProperties
    )
{
    _vkGetPhysicalDeviceMemoryProperties(physicalDevice, pMemoryProperties);
}

void VulkanInterfaceImpl::vkGetPhysicalDeviceMemoryProperties2(
        VkPhysicalDevice physicalDevice,
        VkPhysicalDeviceMemoryProperties2* pMemoryProperties
    )
{
    _vkGetPhysicalDeviceMemoryProperties2(physicalDevice, pMemoryProperties);
}

VkResult VulkanInterfaceImpl::vkAllocateMemory(
        VkDevice device,
        const VkMemoryAllocateInfo* pAllocateInfo,
        const VkAllocationCallbacks* pAllocator,
        VkDeviceMemory* pMemory
    )
{
    return _vkAllocateMemory(device, pAllocateInfo, pAllocator, pMemory);
}

void VulkanInterfaceImpl::vkFreeMemory(
        VkDevice device,
        VkDeviceMemory memory,
        const VkAllocationCallbacks* pAllocator
    )
{
    _vkFreeMemory(device, memory, pAllocator);
}

VkResult VulkanInterfaceImpl::vkMapMemory(
        VkDevice device,
        VkDeviceMemory memory,
        VkDeviceSize offset,
        VkDeviceSize size,
        VkMemoryMapFlags flags,
        void** ppData
    )
{
    return _vkMapMemory(device, memory, offset, size, flags, ppData);
}

void VulkanInterfaceImpl::vkUnmapMemory(
        VkDevice device,
        VkDeviceMemory memory
    )
{
    _vkUnmapMemory(device, memory);
}

VkResult VulkanInterfaceImpl::vkFlushMappedMemoryRanges(
        VkDevice device,
        uint32_t memoryRangeCount,
        const VkMappedMemoryRange* pMemoryRanges
    )
{
    return _vkFlushMappedMemoryRanges(device, memoryRangeCount, pMemoryRanges);
}

VkResult VulkanInterfaceImpl::vkInvalidateMappedMemoryRanges(
        VkDevice device,
        uint32_t memoryRangeCount,
        const VkMappedMemoryRange* pMemoryRanges
    )
{
    return _vkInvalidateMappedMemoryRanges(device, memoryRangeCount, pMemoryRanges);
}

void VulkanInterfaceImpl::vkGetDeviceMemoryCommitment(
        VkDevice device,
        VkDeviceMemory memory,
        VkDeviceSize* pCommittedMemoryInBytes
    )
{
    _vkGetDeviceMemoryCommitment(device, memory, pCommittedMemoryInBytes);
}

VkResult VulkanInterfaceImpl::vkCreateBuffer(
        VkDevice device,
        const VkBufferCreateInfo* pCreateInfo,
        const VkAllocationCallbacks* pAllocator,
        VkBuffer* pBuffer
    )
{
    return _vkCreateBuffer(device, pCreateInfo, pAllocator, pBuffer);
}

void VulkanInterfaceImpl::vkDestroyBuffer(
        VkDevice device,
        VkBuffer buffer,
        const VkAllocationCallbacks* pAllocator
    )
{
    _vkDestroyBuffer(device, buffer, pAllocator);
}

VkResult VulkanInterfaceImpl::vkBindBufferMemory(
        VkDevice device,
        VkBuffer buffer,
        VkDeviceMemory memory,
        VkDeviceSize memoryOffset
    )
{
    return _vkBindBufferMemory(device, buffer, memory, memoryOffset);
}

VkResult VulkanInterfaceImpl::vkBindBufferMemory2(
        VkDevice device,
        uint32_t bindInfoCount,
        const VkBindBufferMemoryInfo* pBindInfos
    )
{
    return _vkBindBufferMemory2(device, bindInfoCount, pBindInfos);
}

VkResult VulkanInterfaceImpl::vkBindImageMemory(
        VkDevice device,
        VkImage image,
        VkDeviceMemory memory,
        VkDeviceSize memoryOffset
    )
{
    return _vkBindImageMemory(device, image, memory, memoryOffset);
}

VkResult VulkanInterfaceImpl::vkBindImageMemory2(
        VkDevice device,
        uint32_t bindInfoCount,
        const VkBindImageMemoryInfo* pBindInfos
    )
{
    return _vkBindImageMemory2(device, bindInfoCount, pBindInfos);
}

void VulkanInterfaceImpl::vkGetBufferMemoryRequirements(
        VkDevice device,
        VkBuffer buffer,
        VkMemoryRequirements* pMemoryRequirements
    )
{
    _vkGetBufferMemoryRequirements(device, buffer, pMemoryRequirements);
}

void VulkanInterfaceImpl::vkGetBufferMemoryRequirements2(
        VkDevice device,
        const VkBufferMemoryRequirementsInfo2* pInfo,
        VkMemoryRequirements2* pMemoryRequirements
    )
{
    _vkGetBufferMemoryRequirements2(device, pInfo, pMemoryRequirements);
}

void VulkanInterfaceImpl::vkGetDeviceBufferMemoryRequirements(
        VkDevice device,
        const VkDeviceBufferMemoryRequirements* pInfo,
        VkMemoryRequirements2* pMemoryRequirements
    )
{
    _vkGetDeviceBufferMemoryRequirements(device, pInfo, pMemoryRequirements);
}

void VulkanInterfaceImpl::vkGetImageMemoryRequirements(
        VkDevice device,
        VkImage image,
        VkMemoryRequirements* pMemoryRequirements
    )
{
    _vkGetImageMemoryRequirements(device, image, pMemoryRequirements);
}

void VulkanInterfaceImpl::vkGetImageMemoryRequirements2(
        VkDevice device,
        const VkImageMemoryRequirementsInfo2* pInfo,
        VkMemoryRequirements2* pMemoryRequirements
    )
{
    _vkGetImageMemoryRequirements2(device, pInfo, pMemoryRequirements);
}

void VulkanInterfaceImpl::vkGetDeviceImageMemoryRequirements(
        VkDevice device,
        const VkDeviceImageMemoryRequirements* pInfo,
        VkMemoryRequirements2* pMemoryRequirements
    )
{
    _vkGetDeviceImageMemoryRequirements(device, pInfo, pMemoryRequirements);
}

VkResult VulkanInterfaceImpl::vkCreateBufferView(
        VkDevice device,
        const VkBufferViewCreateInfo* pCreateInfo,
        const VkAllocationCallbacks* pAllocator,
        VkBufferView* pView
    )
{
    return _vkCreateBufferView(device, pCreateInfo, pAllocator, pView);
}

void VulkanInterfaceImpl::vkDestroyBufferView(
        VkDevice device,
        VkBufferView bufferView,
        const VkAllocationCallbacks* pAllocator
    )
{
    _vkDestroyBufferView(device, bufferView, pAllocator);
}

VkResult VulkanInterfaceImpl::vkCreateImage(
        VkDevice device,
        const VkImageCreateInfo* pCreateInfo,
        const VkAllocationCallbacks* pAllocator,
        VkImage* pImage
    )
{
    return _vkCreateImage(device, pCreateInfo, pAllocator, pImage);
}

void VulkanInterfaceImpl::vkDestroyImage(
        VkDevice device,
        VkImage image,
        const VkAllocationCallbacks* pAllocator
    )
{
    _vkDestroyImage(device, image, pAllocator);
}

void VulkanInterfaceImpl::vkGetImageSubresourceLayout(
        VkDevice device,
        VkImage image,
        const VkImageSubresource* pSubresource,
        VkSubresourceLayout* pLayout
    )
{
    _vkGetImageSubresourceLayout(device, image, pSubresource, pLayout);
}

VkResult VulkanInterfaceImpl::vkCreateImageView(
        VkDevice device,
        const VkImageViewCreateInfo* pCreateInfo,
        const VkAllocationCallbacks* pAllocator,
        VkImageView* pView
    )
{
    return _vkCreateImageView(device, pCreateInfo, pAllocator, pView);
}

void VulkanInterfaceImpl::vkDestroyImageView(
        VkDevice device,
        VkImageView imageView,
        const VkAllocationCallbacks* pAllocator
    )
{
    _vkDestroyImageView(device, imageView, pAllocator);
}

void VulkanInterfaceImpl::vkGetBufferMemoryRequirements2KHR(
    VkDevice device,
    const VkBufferMemoryRequirementsInfo2* pInfo,
    VkMemoryRequirements2* pMemoryRequirements
)
{
    _vkGetBufferMemoryRequirements2KHR(device, pInfo, pMemoryRequirements);
}

void VulkanInterfaceImpl::vkGetImageMemoryRequirements2KHR(
    VkDevice device,
    const VkImageMemoryRequirementsInfo2* pInfo,
    VkMemoryRequirements2* pMemoryRequirements
)
{
    _vkGetImageMemoryRequirements2KHR(device, pInfo, pMemoryRequirements);
}

VkResult VulkanInterfaceImpl::vkCreateSampler(
        VkDevice device,
        const VkSamplerCreateInfo* pCreateInfo,
        const VkAllocationCallbacks* pAllocator,
        VkSampler* pSampler
    )
{
    return _vkCreateSampler(device, pCreateInfo, pAllocator, pSampler);
}

void VulkanInterfaceImpl::vkDestroySampler(
        VkDevice device,
        VkSampler sampler,
        const VkAllocationCallbacks* pAllocator
    )
{
    _vkDestroySampler(device, sampler, pAllocator);
}

VkResult VulkanInterfaceImpl::vkCreatePipelineLayout(
        VkDevice device,
        const VkPipelineLayoutCreateInfo* pCreateInfo,
        const VkAllocationCallbacks* pAllocator,
        VkPipelineLayout* pPipelineLayout
    )
{
    return _vkCreatePipelineLayout(device, pCreateInfo, pAllocator, pPipelineLayout);
}

void VulkanInterfaceImpl::vkDestroyPipelineLayout(
        VkDevice device,
        VkPipelineLayout pipelineLayout,
        const VkAllocationCallbacks* pAllocator
    )
{
    _vkDestroyPipelineLayout(device, pipelineLayout, pAllocator);
}

VkResult VulkanInterfaceImpl::vkCreateDescriptorSetLayout(
        VkDevice device,
        const VkDescriptorSetLayoutCreateInfo* pCreateInfo,
        const VkAllocationCallbacks* pAllocator,
        VkDescriptorSetLayout* pSetLayout
    )
{
    return _vkCreateDescriptorSetLayout(device, pCreateInfo, pAllocator, pSetLayout);
}

void VulkanInterfaceImpl::vkDestroyDescriptorSetLayout(
        VkDevice device,
        VkDescriptorSetLayout descriptorSetLayout,
        const VkAllocationCallbacks* pAllocator
    )
{
    _vkDestroyDescriptorSetLayout(device, descriptorSetLayout, pAllocator);
}

VkResult VulkanInterfaceImpl::vkCreateDescriptorPool(
        VkDevice device,
        const VkDescriptorPoolCreateInfo* pCreateInfo,
        const VkAllocationCallbacks* pAllocator,
        VkDescriptorPool* pDescriptorPool
    )
{
    return _vkCreateDescriptorPool(device, pCreateInfo, pAllocator, pDescriptorPool);
}

void VulkanInterfaceImpl::vkDestroyDescriptorPool(
        VkDevice device,
        VkDescriptorPool descriptorPool,
        const VkAllocationCallbacks* pAllocator
    )
{
    _vkDestroyDescriptorPool(device, descriptorPool, pAllocator);
}

VkResult VulkanInterfaceImpl::vkResetDescriptorPool(
        VkDevice device,
        VkDescriptorPool descriptorPool,
        VkDescriptorPoolResetFlags flags
    )
{
    return _vkResetDescriptorPool(device, descriptorPool, flags);
}

VkResult VulkanInterfaceImpl::vkAllocateDescriptorSets(
        VkDevice device,
        const VkDescriptorSetAllocateInfo* pAllocateInfo,
        VkDescriptorSet* pDescriptorSets
    )
{
    return _vkAllocateDescriptorSets(device, pAllocateInfo, pDescriptorSets);
}

VkResult VulkanInterfaceImpl::vkFreeDescriptorSets(
        VkDevice device,
        VkDescriptorPool descriptorPool,
        uint32_t descriptorSetCount,
        const VkDescriptorSet* pDescriptorSets
    )
{
    return _vkFreeDescriptorSets(device, descriptorPool, descriptorSetCount, pDescriptorSets);
}

void VulkanInterfaceImpl::vkCmdBindDescriptorSets(
        VkCommandBuffer commandBuffer,
        VkPipelineBindPoint pipelineBindPoint,
        VkPipelineLayout layout,
        uint32_t firstSet,
        uint32_t descriptorSetCount,
        const VkDescriptorSet* pDescriptorSets,
        uint32_t dynamicOffsetCount,
        const uint32_t* pDynamicOffsets
    )
{
    _vkCmdBindDescriptorSets(commandBuffer, pipelineBindPoint, layout, firstSet, descriptorSetCount,
        pDescriptorSets, dynamicOffsetCount, pDynamicOffsets
    );
}

void VulkanInterfaceImpl::vkCmdPushConstants(
        VkCommandBuffer commandBuffer,
        VkPipelineLayout layout,
        VkShaderStageFlags stageFlags,
        uint32_t offset,
        uint32_t size,
        const void* pValues
    )
{
    _vkCmdPushConstants(commandBuffer, layout, stageFlags, offset, size, pValues);
}

VkResult VulkanInterfaceImpl::vkCreateQueryPool(
        VkDevice device,
        const VkQueryPoolCreateInfo* pCreateInfo,
        const VkAllocationCallbacks* pAllocator,
        VkQueryPool* pQueryPool
    )
{
    return _vkCreateQueryPool(device, pCreateInfo, pAllocator, pQueryPool);
}

void VulkanInterfaceImpl::vkDestroyQueryPool(
        VkDevice device,
        VkQueryPool queryPool,
        const VkAllocationCallbacks* pAllocator
    )
{
    _vkDestroyQueryPool(device, queryPool, pAllocator);
}

VkResult VulkanInterfaceImpl::vkGetQueryPoolResults(
        VkDevice device,
        VkQueryPool queryPool,
        uint32_t firstQuery,
        uint32_t queryCount,
        size_t dataSize,
        void* pData,
        VkDeviceSize stride,
        VkQueryResultFlags flags
    )
{
    return _vkGetQueryPoolResults(device, queryPool, firstQuery, queryCount, dataSize, pData, stride, flags);
}

void VulkanInterfaceImpl::vkCmdBeginQuery(
        VkCommandBuffer commandBuffer,
        VkQueryPool queryPool,
        uint32_t query,
        VkQueryControlFlags flags
    )
{
    _vkCmdBeginQuery(commandBuffer, queryPool, query, flags);
}

void VulkanInterfaceImpl::vkCmdEndQuery(
        VkCommandBuffer commandBuffer,
        VkQueryPool queryPool,
        uint32_t query
    )
{
    _vkCmdEndQuery(commandBuffer, queryPool, query);
}

void VulkanInterfaceImpl::vkCmdResetQueryPool(
        VkCommandBuffer commandBuffer,
        VkQueryPool queryPool,
        uint32_t firstQuery,
        uint32_t queryCount
    )
{
    _vkCmdResetQueryPool(commandBuffer, queryPool, firstQuery, queryCount);
}

void VulkanInterfaceImpl::vkCmdWriteTimestamp(
        VkCommandBuffer commandBuffer,
        VkPipelineStageFlagBits pipelineStage,
        VkQueryPool queryPool,
        uint32_t query
    )
{
    _vkCmdWriteTimestamp(commandBuffer, pipelineStage, queryPool, query);
}

void VulkanInterfaceImpl::vkCmdCopyQueryPoolResults(
        VkCommandBuffer commandBuffer,
        VkQueryPool queryPool,
        uint32_t firstQuery,
        uint32_t queryCount,
        VkBuffer dstBuffer,
        VkDeviceSize dstOffset,
        VkDeviceSize stride,
        VkQueryResultFlags flags
    )
{
    _vkCmdCopyQueryPoolResults(commandBuffer, queryPool, firstQuery, queryCount, 
        dstBuffer, dstOffset, stride, flags);
}

void VulkanInterfaceImpl::vkCmdUpdateBuffer(
        VkCommandBuffer commandBuffer,
        VkBuffer dstBuffer,
        VkDeviceSize dstOffset,
        VkDeviceSize dataSize,
        const void* pData
    )
{
    _vkCmdUpdateBuffer(commandBuffer, dstBuffer, dstOffset, dataSize, pData);
}

void VulkanInterfaceImpl::vkCmdFillBuffer(
        VkCommandBuffer commandBuffer,
        VkBuffer dstBuffer,
        VkDeviceSize dstOffset,
        VkDeviceSize size,
        uint32_t data
    )
{
    _vkCmdFillBuffer(commandBuffer, dstBuffer, dstOffset, size, data);
}

void VulkanInterfaceImpl::vkCmdClearColorImage(
        VkCommandBuffer commandBuffer,
        VkImage image,
        VkImageLayout imageLayout,
        const VkClearColorValue* pColor,
        uint32_t rangeCount,
        const VkImageSubresourceRange* pRanges
    )
{
    _vkCmdClearColorImage(commandBuffer, image, imageLayout, pColor, rangeCount, pRanges);
}

void VulkanInterfaceImpl::vkCmdClearDepthStencilImage(
        VkCommandBuffer commandBuffer,
        VkImage image,
        VkImageLayout imageLayout,
        const VkClearDepthStencilValue* pDepthStencil,
        uint32_t rangeCount,
        const VkImageSubresourceRange* pRanges
    )
{
    _vkCmdClearDepthStencilImage(commandBuffer, image, imageLayout, pDepthStencil, rangeCount, pRanges);
}

void VulkanInterfaceImpl::vkCmdClearAttachments(
        VkCommandBuffer commandBuffer,
        uint32_t attachmentCount,
        const VkClearAttachment* pAttachments,
        uint32_t rectCount,
        const VkClearRect* pRects
    )
{
    _vkCmdClearAttachments(commandBuffer, attachmentCount, pAttachments, rectCount, pRects);
}

void VulkanInterfaceImpl::vkCmdCopyBuffer(
        VkCommandBuffer commandBuffer,
        VkBuffer srcBuffer,
        VkBuffer dstBuffer,
        uint32_t regionCount,
        const VkBufferCopy* pRegions
    )
{
    _vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, regionCount, pRegions);
}

void VulkanInterfaceImpl::vkCmdCopyImage(
        VkCommandBuffer commandBuffer,
        VkImage srcImage,
        VkImageLayout srcImageLayout,
        VkImage dstImage,
        VkImageLayout dstImageLayout,
        uint32_t regionCount,
        const VkImageCopy* pRegions
    )
{
    _vkCmdCopyImage(commandBuffer, srcImage, srcImageLayout, dstImage,
        dstImageLayout, regionCount, pRegions
    );
}

void VulkanInterfaceImpl::vkCmdBlitImage(
        VkCommandBuffer commandBuffer,
        VkImage srcImage,
        VkImageLayout srcImageLayout,
        VkImage dstImage,
        VkImageLayout dstImageLayout,
        uint32_t regionCount,
        const VkImageBlit* pRegions,
        VkFilter filter
    )
{
    _vkCmdBlitImage(commandBuffer, srcImage, srcImageLayout,
        dstImage, dstImageLayout, regionCount, pRegions, filter
    );
}

void VulkanInterfaceImpl::vkCmdCopyImageToBuffer(
        VkCommandBuffer commandBuffer,
        VkImage srcImage,
        VkImageLayout srcImageLayout,
        VkBuffer dstBuffer,
        uint32_t regionCount,
        const VkBufferImageCopy* pRegions
    )
{
    _vkCmdCopyImageToBuffer(commandBuffer, srcImage, srcImageLayout, dstBuffer, regionCount, pRegions);
}

void VulkanInterfaceImpl::vkCmdResolveImage(
        VkCommandBuffer commandBuffer,
        VkImage srcImage,
        VkImageLayout srcImageLayout,
        VkImage dstImage,
        VkImageLayout dstImageLayout,
        uint32_t regionCount,
        const VkImageResolve* pRegions
    )
{
    _vkCmdResolveImage(commandBuffer, srcImage, srcImageLayout, dstImage, dstImageLayout, regionCount, pRegions);
}

void VulkanInterfaceImpl::vkCmdCopyBufferToImage(
        VkCommandBuffer commandBuffer,
         VkBuffer srcBuffer,
         VkImage dstImage,
         VkImageLayout dstImageLayout,
         uint32_t regionCount,
         const VkBufferImageCopy* pRegions
    )
{
    _vkCmdCopyBufferToImage(commandBuffer, srcBuffer, dstImage, dstImageLayout, regionCount, pRegions);
}

void VulkanInterfaceImpl::vkCmdBindIndexBuffer(
        VkCommandBuffer commandBuffer,
        VkBuffer buffer,
        VkDeviceSize offset,
        VkIndexType indexType
    )
{
    _vkCmdBindIndexBuffer(commandBuffer, buffer, offset, indexType);
}

void VulkanInterfaceImpl::vkCmdBindVertexBuffers(
        VkCommandBuffer commandBuffer,
        uint32_t firstBinding,
        uint32_t bindingCount,
        const VkBuffer* pBuffers,
        const VkDeviceSize* pOffsets
    )
{
    _vkCmdBindVertexBuffers(commandBuffer, firstBinding, bindingCount, pBuffers, pOffsets);
}

void VulkanInterfaceImpl::vkCmdDraw(
        VkCommandBuffer commandBuffer,
        uint32_t vertexCount,
        uint32_t instanceCount,
        uint32_t firstVertex,
        uint32_t firstInstance
    )
{
    _vkCmdDraw(commandBuffer, vertexCount, instanceCount, firstVertex, firstInstance);
}

void VulkanInterfaceImpl::vkCmdDrawIndexed(
        VkCommandBuffer commandBuffer,
        uint32_t indexCount,
        uint32_t instanceCount,
        uint32_t firstIndex,
        int32_t vertexOffset,
        uint32_t firstInstance
    )
{
    _vkCmdDrawIndexed(commandBuffer, indexCount, instanceCount, firstIndex, vertexOffset, firstInstance);
}

void VulkanInterfaceImpl::vkCmdDrawIndirect(
        VkCommandBuffer commandBuffer,
        VkBuffer buffer,
        VkDeviceSize offset,
        uint32_t drawCount,
        uint32_t stride
    )
{
    _vkCmdDrawIndirect(commandBuffer, buffer, offset, drawCount, stride);
}

void VulkanInterfaceImpl::vkCmdDrawIndexedIndirect(
        VkCommandBuffer commandBuffer,
        VkBuffer buffer,
        VkDeviceSize offset,
        uint32_t drawCount,
        uint32_t stride
    )
{
    _vkCmdDrawIndexedIndirect(commandBuffer, buffer, offset, drawCount, stride);
}

void VulkanInterfaceImpl::vkCmdSetViewport(
        VkCommandBuffer commandBuffer,
        uint32_t firstViewport,
        uint32_t viewportCount,
        const VkViewport* pViewports
    )
{
    _vkCmdSetViewport(commandBuffer, firstViewport, viewportCount, pViewports);
}

void VulkanInterfaceImpl::vkCmdSetLineWidth(
        VkCommandBuffer commandBuffer,
        float lineWidth
    )
{
    _vkCmdSetLineWidth(commandBuffer, lineWidth);
}

void VulkanInterfaceImpl::vkCmdSetDepthBias(
        VkCommandBuffer commandBuffer,
        float depthBiasConstantFactor,
        float depthBiasClamp,
        float depthBiasSlopeFactor
    )
{
    _vkCmdSetDepthBias(commandBuffer, depthBiasConstantFactor, depthBiasClamp, depthBiasSlopeFactor);
}

void VulkanInterfaceImpl::vkCmdSetScissor(
        VkCommandBuffer commandBuffer,
        uint32_t firstScissor,
        uint32_t scissorCount,
        const VkRect2D* pScissors
    )
{
    _vkCmdSetScissor(commandBuffer, firstScissor, scissorCount, pScissors);
}

void VulkanInterfaceImpl::vkCmdSetBlendConstants(
        VkCommandBuffer commandBuffer,
        const float blendConstants[4]
    )
{
    _vkCmdSetBlendConstants(commandBuffer, blendConstants);
}

void VulkanInterfaceImpl::vkCmdSetDepthBounds(
        VkCommandBuffer commandBuffer,
        float minDepthBounds,
        float maxDepthBounds
    )
{
    _vkCmdSetDepthBounds(commandBuffer, minDepthBounds, maxDepthBounds);
}

void VulkanInterfaceImpl::vkCmdSetStencilCompareMask(
        VkCommandBuffer commandBuffer,
        VkStencilFaceFlags faceMask,
        uint32_t compareMask
    )
{
    _vkCmdSetStencilCompareMask(commandBuffer, faceMask, compareMask);
}

void VulkanInterfaceImpl::vkCmdSetStencilWriteMask(
        VkCommandBuffer commandBuffer,
        VkStencilFaceFlags faceMask,
        uint32_t writeMask
    )
{
    _vkCmdSetStencilWriteMask(commandBuffer, faceMask, writeMask);
}

void VulkanInterfaceImpl::vkCmdSetStencilReference(
        VkCommandBuffer commandBuffer,
        VkStencilFaceFlags faceMask,
        uint32_t reference
    )
{
    _vkCmdSetStencilReference(commandBuffer, faceMask, reference);
}

void VulkanInterfaceImpl::vkCmdDispatch(
        VkCommandBuffer commandBuffer,
        uint32_t groupCountX,
        uint32_t groupCountY,
        uint32_t groupCountZ
    )
{
    _vkCmdDispatch(commandBuffer, groupCountX, groupCountY, groupCountZ);
}

void VulkanInterfaceImpl::vkCmdDispatchIndirect(
        VkCommandBuffer commandBuffer,
        VkBuffer buffer,
        VkDeviceSize offset
    )
{
    _vkCmdDispatchIndirect(commandBuffer, buffer, offset);
}

VkResult VulkanInterfaceImpl::vkQueueBindSparse(
        VkQueue queue,
        uint32_t bindInfoCount,
        const VkBindSparseInfo* pBindInfo,
        VkFence fence
    )
{
    // TODO
    assert(false);
    return VK_NOT_READY;
}

#if MMP_PLATFORM(WINDOWS)
VkResult VulkanInterfaceImpl::vkCreateWin32SurfaceKHR(
        VkInstance instance,
        const VkWin32SurfaceCreateInfoKHR* pCreateInfo,
        const VkAllocationCallbacks* pAllocator,
        VkSurfaceKHR* pSurface
    )
{
    return _vkCreateWin32SurfaceKHR(instance, pCreateInfo, pAllocator, pSurface);
}
#endif

void VulkanInterfaceImpl::vkDestroySurfaceKHR(
    VkInstance instance,
    VkSurfaceKHR surface,
    const VkAllocationCallbacks* pAllocator
)
{
    _vkDestroySurfaceKHR(instance, surface, pAllocator);
}

VkResult VulkanInterfaceImpl::vkCreateSwapchainKHR(
    VkDevice device,
    const VkSwapchainCreateInfoKHR* pCreateInfo,
    const VkAllocationCallbacks* pAllocator,
    VkSwapchainKHR* pSwapchain
)
{
    return _vkCreateSwapchainKHR(device, pCreateInfo, pAllocator, pSwapchain);
}

void VulkanInterfaceImpl::vkDestroySwapchainKHR(
    VkDevice device,
    VkSwapchainKHR swapchain,
    const VkAllocationCallbacks* pAllocator
)
{
    return _vkDestroySwapchainKHR(device, swapchain, pAllocator);
}

VkResult VulkanInterfaceImpl::vkGetSwapchainImagesKHR(
    VkDevice device,
    VkSwapchainKHR swapchain,
    uint32_t* pSwapchainImageCount,
    VkImage* pSwapchainImages
)
{
    return _vkGetSwapchainImagesKHR(device, swapchain, pSwapchainImageCount, pSwapchainImages);
}

VkResult VulkanInterfaceImpl::vkAcquireNextImageKHR(
    VkDevice device,
    VkSwapchainKHR swapchain,
    uint64_t timeout,
    VkSemaphore semaphore,
    VkFence fence,
    uint32_t* pImageIndex
)
{
    return _vkAcquireNextImageKHR(device, swapchain, timeout, semaphore, fence, pImageIndex);
}

VkResult VulkanInterfaceImpl::vkQueuePresentKHR(
    VkQueue queue,
    const VkPresentInfoKHR* pPresentInfo
)
{
    return _vkQueuePresentKHR(queue, pPresentInfo);
}

VkResult VulkanInterfaceImpl::vkGetPhysicalDeviceSurfaceCapabilitiesKHR(
    VkPhysicalDevice physicalDevice,
    VkSurfaceKHR surface,
    VkSurfaceCapabilitiesKHR* pSurfaceCapabilities
)
{
    return _vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface, pSurfaceCapabilities);
}

VkResult VulkanInterfaceImpl::vkGetPhysicalDeviceSurfacePresentModesKHR(
        VkPhysicalDevice physicalDevice,
        VkSurfaceKHR surface,
        uint32_t* pPresentModeCount,
        VkPresentModeKHR* pPresentModes
    )
{
    return _vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, pPresentModeCount, pPresentModes);
}

VkResult VulkanInterfaceImpl::vkWaitForPresentKHR(
        VkDevice device,
        VkSwapchainKHR swapchain,
        uint64_t presentId,
        uint64_t timeout
    )
{
    return _vkWaitForPresentKHR(device, swapchain, presentId, timeout);
}

void VulkanInterfaceImpl::vkGetPhysicalDeviceFeatures(
        VkPhysicalDevice physicalDevice,
        VkPhysicalDeviceFeatures* pFeatures
    )
{
    _vkGetPhysicalDeviceFeatures(physicalDevice, pFeatures);
}

void VulkanInterfaceImpl::vkGetPhysicalDeviceFormatProperties(
        VkPhysicalDevice physicalDevice,
        VkFormat format,
        VkFormatProperties* pFormatProperties
    )
{
    _vkGetPhysicalDeviceFormatProperties(physicalDevice, format, pFormatProperties);
}

VkResult VulkanInterfaceImpl::vkGetPhysicalDeviceImageFormatProperties(
        VkPhysicalDevice physicalDevice,
        VkFormat format,
        VkImageType type,
        VkImageTiling tiling,
        VkImageUsageFlags usage,
        VkImageCreateFlags flags,
        VkImageFormatProperties* pImageFormatProperties
    )
{
    return _vkGetPhysicalDeviceImageFormatProperties(physicalDevice, format, type, tiling, usage, flags, pImageFormatProperties);
}

VkResult VulkanInterfaceImpl::vkEnumerateInstanceExtensionProperties(
        const char* pLayerName,
        uint32_t* pPropertyCount,
        VkExtensionProperties* pProperties
    )
{
    return _vkEnumerateInstanceExtensionProperties(pLayerName, pPropertyCount, pProperties);
}

VkResult VulkanInterfaceImpl::vkEnumerateDeviceExtensionProperties(
        VkPhysicalDevice physicalDevice,
        const char* pLayerName,
        uint32_t* pPropertyCount,
        VkExtensionProperties* pProperties
    )
{
    return _vkEnumerateDeviceExtensionProperties(physicalDevice, pLayerName, pPropertyCount, pProperties);
}

VkResult VulkanInterfaceImpl::vkEnumerateInstanceLayerProperties(
        uint32_t* pPropertyCount,
        VkLayerProperties* pProperties
    )
{
    return _vkEnumerateInstanceLayerProperties(pPropertyCount, pProperties);
}

VkResult VulkanInterfaceImpl::vkEnumerateDeviceLayerProperties(
        VkPhysicalDevice physicalDevice,
        uint32_t* pPropertyCount,
        VkLayerProperties* pProperties
    )
{
    return _vkEnumerateDeviceLayerProperties(physicalDevice, pPropertyCount, pProperties);
}

} // namespace Mmp