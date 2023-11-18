//
// VulkanWindow.h
//
// Library: GPU
// Package: Windows
// Module:  Vulkan
// 

#pragma once

#include <vector>
#include <string>
#include <memory>

#include "VulkanInterfaceImpl.h"
#include "AbstractWindows.h"

#include "VulkanWindowsCommon.h"

namespace Mmp
{

class VulkanWindow : public AbstractWindows
{
public:
    VulkanWindow();
    ~VulkanWindow();
public:
    bool Open() override;
    bool Close() override;
    bool BindRenderThread(bool bind) override;
    bool SetAPIType(APIType type) override;
    bool SetRenderMode(bool onScreen) override;
    WindowInfo GetInfo() override;
    void  Swap() override;
private:
    bool CreateInstance();
    void DestroyInstance();
    VkResult GetInstanceLayerProperties();
    VkResult GetInstanceLayerExtensionList(const std::string& layerName, std::vector<VkExtensionProperties> &extensions);
    bool IsInstanceExtensionAvailable(const std::string& extensionName);
private:
    bool CreateDevice();
    void DestroyDevice();
    void ChooseDevice(int32_t physicalDevice = 0);
    void GetDeviceLayerExtensionList();
    bool EnableDeviceExtension(const std::string& extension);
private:
    bool CreateSurface();
    void DestroySurface();
private:
    bool CreateSwapchain();
    void DestroySwapchain();
private:
    std::shared_ptr<VulkanInterfaceImpl>    _vulkanInterface;
private:
    APIType                                 _apiType;
    uint32_t                                _apiVersion;
private:
    VkInstance                              _instance;
    std::vector<std::string>                _instanceLayerNames;
    std::vector<LayerProperties>            _instanceLayerProperties;
    std::vector<VkExtensionProperties>      _instanceExtensionProperties;
    std::vector<std::string>                _instanceExtensionsEnabled;
private:
    std::vector<std::string>                _deviceLayerNames;
private:
#if MMP_PLATFORM(WINDOWS)
    HWND                                    _hWnd;
#endif
    VkSurfaceKHR                            _surface;
    VkSurfaceCapabilitiesKHR                _surfaceCapabilities;
    std::vector<VkSurfaceFormatKHR>         _surfaceFormats;
private:
    VkDevice                                _device;
    int32_t                                 _physicalDevice;
    VkQueue                                 _gfxQueue;
    VkSwapchainKHR                          _swapChain;
    VkFormat                                _swapChainFormat;
    VkPresentModeKHR                        _prsentMode;
    VulkanPhysicalDeviceInfo                _deviceInfo;
    std::vector<VkPhysicalDevice>           _physicalDevices;
    std::vector<PhysicalDeviceProps>        _physicalDeviceProperties;
    uint32_t                                _queueCount;
    std::vector<VkQueueFamilyProperties>    _queueFamilyProperties;
    VkPhysicalDeviceMemoryProperties        _memoryProperties;
    PhysicalDeviceFeatures                  _deviceFeature;
    std::vector<std::string>                _deviceExtensionsEnabled;
    std::vector<VkExtensionProperties>      _deviceExtensionProperties;
    VulkanExtensions                        _extensionsLookup;
};

} // namespace Mmp