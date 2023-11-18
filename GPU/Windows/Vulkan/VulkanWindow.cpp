#include "VulkanWindow.h"

#include "Common/Common.h"

#include "WindowUtils.h"

namespace Mmp
{

VulkanWindow::VulkanWindow()
{
#if MMP_PLATFORM(WINDOWS)
    _hWnd = 0;
#endif
    _apiType = APIType::VULKAN;
    _apiVersion = VK_API_VERSION_1_0;
    _instance = VK_NULL_HANDLE;
    _device = VK_NULL_HANDLE;
    _surface = VK_NULL_HANDLE;
    _gfxQueue = VK_NULL_HANDLE;
    _swapChain = VK_NULL_HANDLE;
    _surfaceCapabilities = {};
    _physicalDevice = -1;
    _queueCount = 0;
    _prsentMode = VK_PRESENT_MODE_MAX_ENUM_KHR;
    _memoryProperties = {};
    _vulkanInterface = std::make_shared<VulkanInterfaceImpl>();
}

VulkanWindow::~VulkanWindow()
{

}

bool VulkanWindow::Open()
{
    WIN_LOG_INFO << "Open Window";
    _vulkanInterface->LoadBasicFunc();
    if (!CreateInstance())
    {
        WIN_LOG_ERROR << "-- CreateInstance fail";
        assert(false);
        goto END;
    }
    else
    {
        WIN_LOG_INFO << "-- CreateInstance successfully";
    }
    if (!CreateDevice())
    {
        WIN_LOG_ERROR << "-- CreateDevice fail;";
        assert(false);
        goto END1;
    }
    else
    {
        WIN_LOG_INFO << "-- CreateDevice successfully";
    }
    if (!CreateSurface())
    {
        WIN_LOG_ERROR << "-- CreateSurface fail";
        assert(false);
        goto END2;
    }
    else
    {
        WIN_LOG_INFO << "-- CreateSurface successfully";
    }
    if (!CreateSwapchain())
    {
        WIN_LOG_ERROR << "-- CreateSwapchain fail";
        assert(false);
        goto END3;
    }
    else
    {
        WIN_LOG_INFO << "-- CreateSwapchain successfully";
    }
    return true;
/* END4: */
    DestroySwapchain();
END3:
    DestroySurface();
END2:
    DestroyDevice();
END1:
    DestroyInstance();
END:
    return false;
}

bool VulkanWindow::Close()
{
    WIN_LOG_INFO << "Close Window";
    DestroySwapchain();
    WIN_LOG_INFO << "-- DestroySwapchain";
    DestroySurface();
    WIN_LOG_INFO << "-- DestroySurface";
    DestroyDevice();
    WIN_LOG_INFO << "-- DestroyDevice";
    DestroyInstance();
    WIN_LOG_INFO << "-- DestroyInstance";
    return true;
}

bool VulkanWindow::BindRenderThread(bool bind)
{
    return false;
}

bool VulkanWindow::SetAPIType(APIType type)
{
    assert(type == APIType::VULKAN);
    return true;
}


bool VulkanWindow::SetRenderMode(bool onScreen)
{
    // TODO
    // assert(false);
    return false;
} 

WindowInfo VulkanWindow::GetInfo()
{
    WindowInfo info;
    info.apiType = _apiType;
    info.vMajor = VK_API_VERSION_MAJOR(_apiVersion);
    info.vMinor = VK_API_VERSION_MINOR(_apiVersion);
    return info;
}

void VulkanWindow::Swap() 
{
    // TODO
    assert(false);
}

bool VulkanWindow::EnableDeviceExtension(const std::string& extension)
{
    for (auto& deviceExtensionPropertie : _deviceExtensionProperties)
    {
        if (std::string(deviceExtensionPropertie.extensionName) == extension)
        {
            _deviceExtensionsEnabled.push_back(extension);
            return true;
        }
    }
    return false;
}

void VulkanWindow::GetDeviceLayerExtensionList()
{
	VkResult res;
	do 
    {
		uint32_t deviceExtensionCount;
		res = _vulkanInterface->vkEnumerateDeviceExtensionProperties(_physicalDevices[_physicalDevice], nullptr, &deviceExtensionCount, nullptr);
		if (res != VK_SUCCESS)
			return;
		if (!deviceExtensionCount)
			return;
		_deviceExtensionProperties.resize(deviceExtensionCount);
		res = _vulkanInterface->vkEnumerateDeviceExtensionProperties(_physicalDevices[_physicalDevice], nullptr, &deviceExtensionCount, _deviceExtensionProperties.data());
	} while (res == VK_INCOMPLETE);
}

bool VulkanWindow::CreateDevice()
{
    VkResult res;
    bool found = false;
    VkDeviceQueueCreateInfo queueInfo = {};
    VkDeviceCreateInfo      deviceInfo = {};
    float queuePriority[1] = {1.0f};

    ChooseDevice();
    {
        queueInfo.queueCount = 1;
        queueInfo.pQueuePriorities = queuePriority;
        for (uint32_t i=0; i<_queueCount; i++)
        {
            if (_queueFamilyProperties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
            {
                queueInfo.queueFamilyIndex = i;
                found = true;
                break;
            }
        }
        assert(found);
    }

    {
        _extensionsLookup.KHR_maintenance1 = EnableDeviceExtension(VK_KHR_MAINTENANCE1_EXTENSION_NAME);
        _extensionsLookup.KHR_maintenance2 = EnableDeviceExtension(VK_KHR_MAINTENANCE2_EXTENSION_NAME);
        _extensionsLookup.KHR_maintenance3 = EnableDeviceExtension(VK_KHR_MAINTENANCE3_EXTENSION_NAME);
        _extensionsLookup.KHR_multiview = EnableDeviceExtension(VK_KHR_MULTIVIEW_EXTENSION_NAME);
        _extensionsLookup.KHR_get_memory_requirements2 = EnableDeviceExtension(VK_KHR_GET_MEMORY_REQUIREMENTS_2_EXTENSION_NAME);
        if (_extensionsLookup.KHR_get_memory_requirements2)
        {
            _extensionsLookup.KHR_dedicated_allocation = EnableDeviceExtension(VK_KHR_DEDICATED_ALLOCATION_EXTENSION_NAME);
        }
        _extensionsLookup.KHR_create_renderpass2 = EnableDeviceExtension(VK_KHR_CREATE_RENDERPASS_2_EXTENSION_NAME);
        if (_extensionsLookup.KHR_create_renderpass2)
        {
            _extensionsLookup.KHR_depth_stencil_resolve = EnableDeviceExtension(VK_KHR_DEPTH_STENCIL_RESOLVE_EXTENSION_NAME);
        }
        _extensionsLookup.EXT_shader_stencil_export = EnableDeviceExtension(VK_EXT_SHADER_STENCIL_EXPORT_EXTENSION_NAME);
        _extensionsLookup.EXT_fragment_shader_interlock = EnableDeviceExtension(VK_EXT_FRAGMENT_SHADER_INTERLOCK_EXTENSION_NAME);
        _extensionsLookup.ARM_rasterization_order_attachment_access = EnableDeviceExtension(VK_ARM_RASTERIZATION_ORDER_ATTACHMENT_ACCESS_EXTENSION_NAME);
        _extensionsLookup.KHR_present_id = EnableDeviceExtension(VK_KHR_PRESENT_ID_EXTENSION_NAME);
        _extensionsLookup.KHR_present_wait = EnableDeviceExtension(VK_KHR_PRESENT_WAIT_EXTENSION_NAME);
    }

    {
        _deviceFeature.enabled = {};
        _deviceFeature.enabled.standard.dualSrcBlend = _deviceFeature.available.standard.dualSrcBlend;
        _deviceFeature.enabled.standard.logicOp = _deviceFeature.available.standard.logicOp;
        _deviceFeature.enabled.standard.depthClamp = _deviceFeature.available.standard.depthClamp;
        _deviceFeature.enabled.standard.depthBounds = _deviceFeature.available.standard.depthBounds;
        _deviceFeature.enabled.standard.samplerAnisotropy = _deviceFeature.available.standard.samplerAnisotropy;
        _deviceFeature.enabled.standard.shaderClipDistance = _deviceFeature.available.standard.shaderClipDistance;
        _deviceFeature.enabled.standard.shaderCullDistance = _deviceFeature.available.standard.shaderCullDistance;
        _deviceFeature.enabled.standard.geometryShader = _deviceFeature.available.standard.geometryShader;
        _deviceFeature.enabled.standard.sampleRateShading = _deviceFeature.available.standard.sampleRateShading;

        _deviceFeature.enabled.multiview = {};
        _deviceFeature.enabled.multiview.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MULTIVIEW_FEATURES;
        if (_extensionsLookup.KHR_multiview)
        {
            _deviceFeature.enabled.multiview.multiview = _deviceFeature.available.multiview.multiview;
        }
        _deviceFeature.enabled.presentId = {};
        _deviceFeature.enabled.presentId.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PRESENT_ID_FEATURES_KHR;
        if (_extensionsLookup.KHR_present_id) 
        {
            _deviceFeature.enabled.presentId.presentId = _deviceFeature.available.presentId.presentId;
        }
        _deviceFeature.enabled.presentWait = {};
        _deviceFeature.enabled.presentWait.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PRESENT_WAIT_FEATURES_KHR;
        if (_extensionsLookup.KHR_present_wait) 
        {
            _deviceFeature.enabled.presentWait.presentWait = _deviceFeature.available.presentWait.presentWait;
        }
    }

    std::vector<const char*> deviceLayerNames; // Keep alive
    std::vector<const char*> deviceExtensionsEnabled; // Keep alive
    for (auto& deviceLayerName : _deviceLayerNames)
    {
        deviceLayerNames.push_back(deviceLayerName.c_str());
    }
    for (auto& deviceExtensionEnabled : _deviceExtensionsEnabled)
    {
        deviceExtensionsEnabled.push_back(deviceExtensionEnabled.c_str());
    }
    deviceInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    deviceInfo.queueCreateInfoCount = 1;
    deviceInfo.pQueueCreateInfos = &queueInfo;
    deviceInfo.enabledExtensionCount = (uint32_t)deviceLayerNames.size();
    deviceInfo.ppEnabledExtensionNames = deviceInfo.enabledExtensionCount ? deviceLayerNames.data() : nullptr;
    deviceInfo.enabledExtensionCount = (uint32_t)deviceExtensionsEnabled.size();
    deviceInfo.ppEnabledExtensionNames = deviceInfo.enabledExtensionCount ? deviceExtensionsEnabled.data() : nullptr;
    deviceInfo.pEnabledFeatures = &_deviceFeature.enabled.standard;
    res = _vulkanInterface->vkCreateDevice(_physicalDevices[_physicalDevice], &deviceInfo, nullptr, &_device);
    if (res != VK_SUCCESS)
    {
        WIN_LOG_ERROR << "vkCreateDevice fail";
        assert(false);
        goto END;
    }
    _vulkanInterface->LoadDeviceFunc(_device, _extensionsLookup);
    return true;
END:
    _device = VK_NULL_HANDLE;
    return false;
}

void VulkanWindow::DestroyDevice()
{
    if (_device != VK_NULL_HANDLE)
    {
        _vulkanInterface->vkDestroyDevice(_device, nullptr);
        _device = VK_NULL_HANDLE;
    }
}

void VulkanWindow::ChooseDevice(int32_t physicalDevice)
{
    static std::vector<VkFormat> depthStencilFormats = 
    {
        VK_FORMAT_D24_UNORM_S8_UINT,
        VK_FORMAT_D32_SFLOAT_S8_UINT,
        VK_FORMAT_D16_UNORM_S8_UINT
    };

    assert(physicalDevice < _physicalDevices.size());
    _physicalDevice = physicalDevice;
    WIN_LOG_INFO << "-- Choose device " << _physicalDevice << " : " << _physicalDeviceProperties[_physicalDevice].properties.deviceName;
    _vulkanInterface->vkGetPhysicalDeviceQueueFamilyProperties(_physicalDevices[_physicalDevice], &_queueCount, nullptr);
    assert(_queueCount > 0);
    _queueFamilyProperties.resize(_queueCount);
    _vulkanInterface->vkGetPhysicalDeviceQueueFamilyProperties(_physicalDevices[_physicalDevice], &_queueCount, _queueFamilyProperties.data());
    assert(_queueCount > 0);

    _deviceInfo.preferredDepthStencilFormat = VK_FORMAT_UNDEFINED;
	for (size_t i = 0; i < depthStencilFormats.size(); i++) 
    {
		VkFormatProperties props;
		_vulkanInterface->vkGetPhysicalDeviceFormatProperties(_physicalDevices[_physicalDevice], depthStencilFormats[i], &props);
		if (props.optimalTilingFeatures & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT) 
        {
			_deviceInfo.preferredDepthStencilFormat = depthStencilFormats[i];
			break;
		}
	}
    VkFormatProperties preferredProps;
	_vulkanInterface->vkGetPhysicalDeviceFormatProperties(_physicalDevices[_physicalDevice], _deviceInfo.preferredDepthStencilFormat, &preferredProps);
	if ((preferredProps.optimalTilingFeatures & VK_FORMAT_FEATURE_BLIT_SRC_BIT) &&
		(preferredProps.optimalTilingFeatures & VK_FORMAT_FEATURE_BLIT_DST_BIT)) 
    {
		_deviceInfo.canBlitToPreferredDepthStencilFormat = true;
	}

    _vulkanInterface->vkGetPhysicalDeviceMemoryProperties(_physicalDevices[_physicalDevice], &_memoryProperties);
    for (int i = 0; i<_memoryProperties.memoryTypeCount; i++)
    {
        if (!_memoryProperties.memoryTypes[i].propertyFlags)
            continue;
        WIN_LOG_INFO << "-- (" << i << ") " << "Heap : " << _memoryProperties.memoryTypes[i].heapIndex << "; "
                     << "Flags : "
                     << ((_memoryProperties.memoryTypes[i].propertyFlags & VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT) ? "DEVICE_LOCAL " : std::string()) << ":"
                     << ((_memoryProperties.memoryTypes[i].propertyFlags & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT) ? "HOST_VISIBLE " : std::string()) << ":"
                     << ((_memoryProperties.memoryTypes[i].propertyFlags & VK_MEMORY_PROPERTY_HOST_CACHED_BIT) ? "HOST_CACHED " : std::string()) << ":"
                     << ((_memoryProperties.memoryTypes[i].propertyFlags & VK_MEMORY_PROPERTY_HOST_COHERENT_BIT) ? "HOST_COHERENT " : std::string());
    }

    _vulkanInterface->vkGetPhysicalDeviceFeatures(_physicalDevices[_physicalDevice], &_deviceFeature.available.standard);

    GetDeviceLayerExtensionList();

    _deviceExtensionsEnabled.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
}

bool VulkanWindow::IsInstanceExtensionAvailable(const std::string& extensionName)
{
    for (const auto& instanceExtensionPropertie : _instanceExtensionProperties)
    {
        if (instanceExtensionPropertie.extensionName == extensionName)
        {
            return true;
        }
    }
    for (const auto& instanceLayerPropertie : _instanceLayerProperties)
    {
        for (const auto& instanceExtensionPropertie : instanceLayerPropertie.extensions)
        {
            if (instanceExtensionPropertie.extensionName == extensionName)
            {
                return true;
            }
        }     
    }
    return false;
}

VkResult VulkanWindow::GetInstanceLayerExtensionList(const std::string& layerName, std::vector<VkExtensionProperties> &extensions)
{
	VkResult res = VK_RESULT_MAX_ENUM;
	do 
    {
		uint32_t instanceExtensionCount = 0;
		res = _vulkanInterface->vkEnumerateInstanceExtensionProperties(layerName.c_str(), &instanceExtensionCount, nullptr);
		if (res != VK_SUCCESS)
			return res;
		if (instanceExtensionCount == 0)
			return VK_SUCCESS;
		extensions.resize(instanceExtensionCount);
		res = _vulkanInterface->vkEnumerateInstanceExtensionProperties(layerName.c_str(), &instanceExtensionCount, extensions.data());
	} while (res == VK_INCOMPLETE);
	return res;
}

VkResult VulkanWindow::GetInstanceLayerProperties()
{
    uint32_t instanceLayerCount = 0;
    std::vector<VkLayerProperties> vkProps;
    VkResult res = VK_RESULT_MAX_ENUM;
    do
    {
        res = _vulkanInterface->vkEnumerateInstanceLayerProperties(&instanceLayerCount, nullptr);
        if (res != VK_SUCCESS)
        {
            return res;
        }
        if (!instanceLayerCount)
        {
            return VK_SUCCESS;
        }
        vkProps.resize(instanceLayerCount);
        res = _vulkanInterface->vkEnumerateInstanceLayerProperties(&instanceLayerCount, vkProps.data());
    } while (res == VK_INCOMPLETE);
    
    for (uint32_t i=0; i<instanceLayerCount; i++)
    {
        LayerProperties layerProps;
        layerProps.properties = vkProps[i];
        res = GetInstanceLayerExtensionList(std::string(layerProps.properties.layerName), layerProps.extensions);
        if (res != VK_SUCCESS)
        {
            return res;
        }
        _instanceLayerProperties.push_back(layerProps);
    }
    return res;
}

bool VulkanWindow::CreateInstance()
{
    uint32_t gpuCount = 1;
    VkResult res = VK_RESULT_MAX_ENUM;
    VkApplicationInfo appInfo = {};
    VkInstanceCreateInfo instInfo = {};

    _instanceLayerNames.clear();
    _deviceLayerNames.clear();
    
    GetInstanceLayerProperties();
    GetInstanceLayerExtensionList(std::string(), _instanceExtensionProperties);

    if (!IsInstanceExtensionAvailable(VK_KHR_SURFACE_EXTENSION_NAME))
    {
        assert(false);
    }
    _instanceExtensionsEnabled.push_back(VK_KHR_SURFACE_EXTENSION_NAME);
#if MMP_PLATFORM(WINDOWS)
    _instanceExtensionsEnabled.push_back(VK_KHR_WIN32_SURFACE_EXTENSION_NAME);
#endif

    _vulkanInterface->vkEnumerateInstanceVersion(&_apiVersion);
    _apiVersion = VK_MAKE_API_VERSION(VK_API_VERSION_VARIANT(_apiVersion), VK_API_VERSION_MAJOR(_apiVersion), VK_API_VERSION_MINOR(_apiVersion), 0);

    std::vector<const char *> instanceLayerNames; // Hint : keep alive
    std::vector<const char *> instanceExtensionsEnabled; // Hint : keep alive
    {
        appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        appInfo.pApplicationName = MMP_NAME;
        appInfo.applicationVersion = 0;
        appInfo.pEngineName = MMP_NAME;
        appInfo.engineVersion = 0;
        appInfo.apiVersion = _apiVersion;
    }
    {
        for (auto& instanceLayerName : _instanceLayerNames)
        {
            instanceLayerNames.push_back((const char *)instanceLayerName.c_str());
        }
        for (auto& instanceExtensionEnabled : _instanceExtensionsEnabled)
        {
            instanceExtensionsEnabled.push_back((const char*)instanceExtensionEnabled.c_str());
        }
        instInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        instInfo.pApplicationInfo = &appInfo;
        instInfo.enabledLayerCount = (uint32_t)instanceLayerNames.size();
        instInfo.ppEnabledLayerNames = instanceLayerNames.size() ? instanceLayerNames.data() : nullptr;
        instInfo.enabledExtensionCount = (uint32_t)instanceExtensionsEnabled.size();
        instInfo.ppEnabledExtensionNames = instanceExtensionsEnabled.size() ? instanceExtensionsEnabled.data() : nullptr;
    }
    res = _vulkanInterface->vkCreateInstance(&instInfo, nullptr, &_instance);
    assert(res == VK_SUCCESS);
    if (res != VK_SUCCESS)
    {
        WIN_LOG_ERROR << "vkCreateInstance fail";
        goto END;
    }
    _vulkanInterface->LoadInstanceFunc(_instance);
    res = _vulkanInterface->vkEnumeratePhysicalDevices(_instance, &gpuCount, nullptr);
    if (gpuCount <= 0)
    {
        WIN_LOG_ERROR << "vkEnumeratePhysicalDevices query device count fail";
        assert(false);
        goto END1;
    }
    _physicalDevices.resize(gpuCount);
    _physicalDeviceProperties.resize(gpuCount);
    res = _vulkanInterface->vkEnumeratePhysicalDevices(_instance, &gpuCount, _physicalDevices.data());
    if (res != VK_SUCCESS)
    {
        WIN_LOG_ERROR << "vkEnumeratePhysicalDevices get device fail";
        assert(false);
        goto END1;
    }
    for (uint32_t i=0; i<gpuCount; i++)
    {
        _vulkanInterface->vkGetPhysicalDeviceProperties(_physicalDevices[i], &_physicalDeviceProperties[i].properties);
    }
    return true;
END1:
    _vulkanInterface->vkDestroyInstance(_instance, nullptr);
    _instance = VK_NULL_HANDLE;
END:
    return false;
}

void VulkanWindow::DestroyInstance()
{
    if (_instance != VK_NULL_HANDLE)
    {
        _vulkanInterface->vkDestroyInstance(_instance, nullptr);
        _instance = VK_NULL_HANDLE;
    }
}

bool VulkanWindow::CreateSurface()
{
#if MMP_PLATFORM(WINDOWS)
    VkWin32SurfaceCreateInfoKHR win32 = {};
    if (_hWnd == 0) /* Hint : only once for now, keep alive */
    {
        _hWnd = Win32CreateWindow(MMP_NAME, WS_OVERLAPPEDWINDOW, DXGI_SCREEN_WIDTH, DXGI_SCREEN_HEIGHT, Win32GetInstance());
        assert(_hWnd);
        Win32ShowWindow(_hWnd, SW_SHOW); // TODO : a better way
        if (_hWnd == 0)
        {
            goto END;
        }
    }
    win32.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
    win32.flags = 0;
    win32.hwnd = _hWnd;
    win32.hinstance = Win32GetInstance();
    if (_vulkanInterface->vkCreateWin32SurfaceKHR(_instance, &win32, nullptr, &_surface))
    {
        WIN_LOG_ERROR << "vkCreateWin32SurfaceKHR fail";
        goto END;
    }
#endif 

    // TODO : maybe other something need do
    return true;
END:
#if MMP_PLATFORM(WINDOWS)
    _hWnd = 0;
#endif
    _surface = VK_NULL_HANDLE;
    return false;
}

void VulkanWindow::DestroySurface()
{
    if (_surface != VK_NULL_HANDLE)
    {
        _vulkanInterface->vkDestroySurfaceKHR(_instance, _surface, nullptr);
        _surface = VK_NULL_HANDLE;
    }
}

bool VulkanWindow::CreateSwapchain()
{
    VkResult res = VK_SUCCESS;
    uint32_t presentModeCount = 0;
    std::vector<VkPresentModeKHR> presentModes;
    VkSwapchainCreateInfoKHR swapChainInfo = {};

    res = _vulkanInterface->vkGetPhysicalDeviceSurfaceCapabilitiesKHR(_physicalDevices[_physicalDevice], _surface, &_surfaceCapabilities);
    if (res != VK_SUCCESS)
    {
        WIN_LOG_ERROR << "vkGetPhysicalDeviceSurfaceCapabilitiesKHR fail";
        goto END;
    }

    {
        res = _vulkanInterface->vkGetPhysicalDeviceSurfacePresentModesKHR(_physicalDevices[_physicalDevice], _surface, &presentModeCount, nullptr);
        if (res != VK_SUCCESS || presentModeCount == 0)
        {
            WIN_LOG_ERROR << "vkGetPhysicalDeviceSurfacePresentModesKHR fail";
            goto END;
        }
        presentModes.resize(presentModeCount);
        res = _vulkanInterface->vkGetPhysicalDeviceSurfacePresentModesKHR(_physicalDevices[_physicalDevice], _surface, &presentModeCount, presentModes.data());
        if (res != VK_SUCCESS)
        {
            WIN_LOG_ERROR << "vkGetPhysicalDeviceSurfacePresentModesKHR fail";
            goto END;
        }
        std::string log = "-- Support present modes : ";
        for (const auto& presentMode : presentModes)
        {
            log += VkPresentModeKHRToStr(presentMode) + ", ";
        }
        // Hint : for last ", " two space
        log.pop_back();
        log.pop_back();
        WIN_LOG_INFO << log;
    }
    _prsentMode = presentModes[0];
    {
        swapChainInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        swapChainInfo.surface = _surface;
        swapChainInfo.minImageCount = _surfaceCapabilities.maxImageCount;
        swapChainInfo.imageFormat = _swapChainFormat;
        swapChainInfo.imageColorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
#if MMP_PLATFORM(WINDOWS)
        swapChainInfo.imageExtent.width = DXGI_SCREEN_WIDTH;
        swapChainInfo.imageExtent.height = DXGI_SCREEN_HEIGHT;
#endif
        swapChainInfo.preTransform = VkSurfaceTransformFlagBitsKHR::VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
        swapChainInfo.imageArrayLayers = 1;
        swapChainInfo.presentMode = _prsentMode;
        swapChainInfo.oldSwapchain = VK_NULL_HANDLE;
        swapChainInfo.clipped = true;
        swapChainInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
        swapChainInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        swapChainInfo.queueFamilyIndexCount = 0;
        swapChainInfo.pQueueFamilyIndices = NULL;
        if (_surfaceCapabilities.supportedCompositeAlpha & VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR) 
        {
            swapChainInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
        }
        else 
        {
            swapChainInfo.compositeAlpha = VK_COMPOSITE_ALPHA_INHERIT_BIT_KHR;
        }
    }
    res = _vulkanInterface->vkCreateSwapchainKHR(_device, &swapChainInfo, nullptr, &_swapChain);
    if (res != VK_SUCCESS)
    {
        WIN_LOG_ERROR << "vkCreateSwapchainKHR fail";
        assert(false);
        goto END;
    }
    return true;
END:
    _swapChain = VK_NULL_HANDLE;
    _prsentMode = VK_PRESENT_MODE_MAX_ENUM_KHR;
    return false;
}

void VulkanWindow::DestroySwapchain()
{
    if (_swapChain != VK_NULL_HANDLE)
    {
        _vulkanInterface->vkDestroySwapchainKHR(_device, _swapChain, nullptr);
        _swapChain = VK_NULL_HANDLE;
    }
}

} // namespace Mmp