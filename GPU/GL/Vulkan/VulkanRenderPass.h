//
// VulkanRenderPass.h
//
// Library: GPU
// Package: GL
// Module:  Vulkan
// 

#pragma once

#include "VulkanCommon.h"

#include <map>
#include <memory>

namespace Mmp
{

class VulkanRenderPass
{
public:
    using ptr = std::shared_ptr<VulkanRenderPass>;
public:
    explicit VulkanRenderPass(VulkanRenderPassKey key, std::shared_ptr<VulkanDrawContex> draw);
    ~VulkanRenderPass();
public:
    VkRenderPass Get(VulkanRenderPassType rpType, VkSampleCountFlagBits sampleCount);
private:
    std::map<VulkanRenderPassType, VkRenderPass>                 _passes;
    std::map<VulkanRenderPassType, VkSampleCountFlagBits>        _flags;
    VulkanRenderPassKey                                          _key;
    std::shared_ptr<VulkanDrawContex>                            _draw;
};

} // namespace Mmp