//
// VulkanTranslator.h
//
// Library: GPU
// Package: GL
// Module:  Vulkan
//

#pragma once

#include "VulkanCommon.h"

namespace Mmp
{

VkCompareOp GLComparisonTypeToVulkanType(Comparison comparison);

VkBlendOp GLBlendOpToVulkanType(BlendOp op);

VkBlendFactor GLBlendFactorToVulkanType(BlendFactor bf);

VkLogicOp GLLogicOpToOpenVulkanType(LogicOp op);

VkPrimitiveTopology GLPrimitiveToOpenVulkanype(Primitive primitive);

VkStencilOp GLStencilOpToVulkanType(StencilOp op);

VkCullModeFlagBits GLCullModeToVulkanType(CullMode mode);

VkShaderStageFlagBits SLShaderStageToVulkanType(SL::ShaderStage stage);

VkSamplerAddressMode GLTextureAddressModeToVulkanType(TextureAddressMode mode);

VkAttachmentLoadOp GLRPLoadActionToVulkanType(RPAction action);

VkAttachmentStoreOp GLRPStoreActionToVulkanType(RPAction action);

} // namespace Mmp