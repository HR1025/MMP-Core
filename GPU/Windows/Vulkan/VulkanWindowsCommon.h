//
// VulkanWindowsCommon.h
//
// Library: GPU
// Package: Windows
// Module:  Vulkan
// 

#pragma once

#include <string>

#include "GPU/GL/Vulkan/VulkanInterface.h" 

namespace Mmp
{
struct LayerProperties
{
    VkLayerProperties properties = {};
    std::vector<VkExtensionProperties> extensions;
};
struct PhysicalDeviceProps 
{
    VkPhysicalDeviceProperties properties = {};
    VkPhysicalDevicePushDescriptorPropertiesKHR pushDescriptorProperties = {};
    VkPhysicalDeviceExternalMemoryHostPropertiesEXT externalMemoryHostProperties = {};
    VkPhysicalDeviceDepthStencilResolveProperties depthStencilResolve = {};
};

struct VulkanPhysicalDeviceInfo 
{
    VkFormat preferredDepthStencilFormat;
    bool canBlitToPreferredDepthStencilFormat;
};

struct AllPhysicalDeviceFeatures 
{
    VkPhysicalDeviceFeatures standard = {};
    VkPhysicalDeviceMultiviewFeatures multiview = {};
    VkPhysicalDevicePresentWaitFeaturesKHR presentWait = {};
    VkPhysicalDevicePresentIdFeaturesKHR presentId = {};
};

struct PhysicalDeviceFeatures 
{
    AllPhysicalDeviceFeatures available = {};
    AllPhysicalDeviceFeatures enabled = {};
};

struct VulkanExtensions 
{
    bool EXT_debug_utils = false;
    bool KHR_maintenance1 = false;
    bool KHR_maintenance2 = false;
    bool KHR_maintenance3 = false;
    bool KHR_multiview = false;
    bool KHR_get_memory_requirements2 = false;
    bool KHR_dedicated_allocation = false;
    bool KHR_create_renderpass2 = false;
    bool KHR_get_physical_device_properties2 = false;
    bool KHR_depth_stencil_resolve = false;
    bool EXT_shader_stencil_export = false;
    bool EXT_swapchain_colorspace = false;
    bool ARM_rasterization_order_attachment_access = false;
    bool EXT_fragment_shader_interlock = false;
    bool KHR_present_id = false;
    bool KHR_present_wait = false;
    bool GOOGLE_display_timing = false;
};

std::string VkPresentModeKHRToStr(VkPresentModeKHR mode);

} // namespace Mmp