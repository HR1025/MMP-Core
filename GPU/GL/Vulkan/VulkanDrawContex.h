//
// VulkanDrawContex.h
//
// Library: GPU
// Package: GL
// Module:  Vulkan
// 

#pragma once

#include <map>
#include <memory>

#include "GLDrawContex.h"
#include "VulkanContex.h"
#include "VulkanRenderPass.h"

namespace Mmp
{

class VulkanDrawContex : public GLDrawContex
{
public:
    using ptr = std::shared_ptr<VulkanDrawContex>;
public:
    DepthStencilState::ptr CreateDepthStencilState(const DepthStencilStateDesc& desc) override;
    BlendState::ptr CreateBlendState(const BlendStateDesc& desc) override;
    SamplerState::ptr CreateSamplerState(const SamplerStateDesc& desc) override;
    RasterState::ptr CreateRasterState(const RasterStateDesc& desc) override;
    InputLayout::ptr CreateInputLayout(const InputLayoutDesc& desc) override;
    ShaderModule::ptr CreateShaderModule(SL::ShaderStage stage, SL::ShaderLanguage lang, const std::string& code) override;
    Pipeline::ptr CreateGraphicsPipeline(const PipelineDesc& desc, const std::string& tag) override;
public:
    SL::ShaderLanguage GetShaderLanguage() override;
public:
    VulkanRenderPass::ptr GetRenderPass(VulkanRenderPassKey key);
public:
    VkDevice     Device();
    VmaAllocator Allocator();
private:
    void CreateDeviceObjects();
    void DestoryDeviceObjects();
private:
    VulkanRenderPass::ptr  _compatibleRenderPass;
    std::map<VulkanRenderPassKey, VulkanRenderPass::ptr> _renderPasses;
private:
    VulkanInterface::ptr  _vulkan;
    VkDevice              _device;
private:
    VmaAllocator          _allocator;
};

} // namespace Mmp