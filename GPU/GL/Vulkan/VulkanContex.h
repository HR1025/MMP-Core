//
// VulkanContex.h
//
// Library: GPU
// Package: GL
// Module:  Vulkan
// 

#pragma once

#include <memory>
#include <vector>

#include "VulkanCommon.h"
#include "VulkanTranslator.h"
#include "VulkanInterface.h"

namespace Mmp
{

class VulkanBlendState : public BlendState
{
public:
    using ptr = std::shared_ptr<VulkanBlendState>;
public:
    VulkanBlendState();
    ~VulkanBlendState();
public:
    VkPipelineColorBlendStateCreateInfo                 info;
    std::vector<VkPipelineColorBlendAttachmentState>    attachments;
};

class VulkanDepthStencilState : public DepthStencilState
{
public:
    using ptr = std::shared_ptr<VulkanDepthStencilState>;
public:
    VulkanDepthStencilState();
    ~VulkanDepthStencilState();
public:
    VkPipelineDepthStencilStateCreateInfo info;
};

/**
 * @todo 
 */
class VulkanRasterState : public RasterState
{
public:
    using ptr = std::shared_ptr<VulkanRasterState>;
public:
    VulkanRasterState();
    ~VulkanRasterState();
public:
    Facing       frontFace;
    CullMode     cullFace;
};

class VulkanShaderModule : public ShaderModule
{
public:
    using ptr = std::shared_ptr<VulkanShaderModule>;
public:
    VulkanShaderModule(SL::ShaderStage stage, const std::string& tag);
    ~VulkanShaderModule();
public:
    SL::ShaderStage GetStage() override;
public:
    bool Compile(VkDevice device, VulkanInterface::ptr vulkan, const std::string& code);
public:
    VkShaderModule            smodule;
    VkShaderStageFlagBits     flag;
    SL::ShaderStage           stage;
    std::string               source;
    bool                      isOK;
};

class VulkanInputLayout : public InputLayout
{
public:
    using ptr = std::shared_ptr<VulkanInputLayout>;
public:
    VulkanInputLayout();
    ~VulkanInputLayout();
public:
    std::vector<VkVertexInputBindingDescription>     bindings;
    std::vector<VkVertexInputAttributeDescription>   attributes;
    VkPipelineVertexInputStateCreateInfo             visc;
};

class VulkanSamplerState : public SamplerState
{
public:
    using ptr = std::shared_ptr<VulkanSamplerState>;
public:
    VulkanSamplerState();
    ~VulkanSamplerState();
public:
    VkSampler sampler;
};

class VulkanBuffer : public GLBuffer
{
public:
    using ptr = std::shared_ptr<VulkanBuffer>;
public:
    explicit VulkanBuffer(size_t size = 0);
    ~VulkanBuffer();
public:
    uint8_t* data;
    size_t   size;
};

class VulkanGraphicsPileline : public Pipeline
{
public:
    using ptr = std::shared_ptr<VulkanGraphicsPileline>;
public:
    VulkanGraphicsPileline();
    ~VulkanGraphicsPileline();
public:
    VkPipelineCache                         pipelineCache;
    VkPipelineColorBlendStateCreateInfo     cbs;
    VkPipelineColorBlendAttachmentState     blend0;
    VkPipelineDepthStencilStateCreateInfo   dss;
    VkDynamicState                          dynamicStates[6] = {};
    VkPipelineDynamicStateCreateInfo        ds;
    VkPipelineRasterizationStateCreateInfo  rs;
public:
    VulkanShaderModule::ptr                 vertexShader;
    VulkanShaderModule::ptr                 fragementShader;
    VulkanShaderModule::ptr                 geometryShader;
public:
    VkPrimitiveTopology                     topology;
    VkVertexInputAttributeDescription       attrs[8] = {};
    VkVertexInputBindingDescription         ibd;
    VkPipelineVertexInputStateCreateInfo    vis;
    VkPipelineViewportStateCreateInfo       views;
public:
    VkPipelineLayout                        pipelineLayout;
    VulkanRenderPassKey                     rpKey;
};

} // namespace Mmp

namespace Mmp
{

class DescriptorSetKey
{
public:
    DescriptorSetKey();
public:
    std::vector<VkImageView>               imageViews;
    std::vector<VulkanSamplerState::ptr>   samplers;
    VulkanBuffer::ptr                      buffer; 
public:
    bool operator<(const DescriptorSetKey &other) const;
};

} // namespace Mmp