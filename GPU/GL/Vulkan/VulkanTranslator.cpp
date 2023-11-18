#include "VulkanTranslator.h"

#include <cassert>

namespace Mmp
{

VkCompareOp GLComparisonTypeToVulkanType(Comparison comparison)
{
    switch (comparison)
    {
        case Comparison::NEVER: return VK_COMPARE_OP_NEVER;
        case Comparison::LESS:  return VK_COMPARE_OP_LESS;
        case Comparison::EQUAL: return VK_COMPARE_OP_EQUAL;
        case Comparison::LESS_EQUAL: return VK_COMPARE_OP_LESS_OR_EQUAL;
        case Comparison::GREATER: return VK_COMPARE_OP_GREATER;
        case Comparison::NOT_EQUAL: return VK_COMPARE_OP_NOT_EQUAL;
        case Comparison::GREATER_EQUAL: return VK_COMPARE_OP_GREATER_OR_EQUAL;
        case Comparison::ALWAYS: return VK_COMPARE_OP_ALWAYS;
        default:
            assert(false);
            return VK_COMPARE_OP_NEVER;
    }
}

VkBlendOp GLBlendOpToVulkanType(BlendOp op)
{
    switch (op)
    {
        case BlendOp::ADD: return VK_BLEND_OP_ADD;
        case BlendOp::SUBTRACT: return VK_BLEND_OP_SUBTRACT;
        case BlendOp::REV_SUBTRACT: return VK_BLEND_OP_REVERSE_SUBTRACT;
        case BlendOp::MIN: return VK_BLEND_OP_MIN;
        case BlendOp::MAX: return VK_BLEND_OP_MAX;
        default:
            assert(false);
            return VK_BLEND_OP_ADD;
    }
}

VkBlendFactor GLBlendFactorToVulkanType(BlendFactor bf)
{
    switch (bf)
    {
        case BlendFactor::ZERO: return VK_BLEND_FACTOR_ZERO;
        case BlendFactor::ONE: return VK_BLEND_FACTOR_ONE;
        case BlendFactor::SRC_COLOR: return VK_BLEND_FACTOR_SRC_COLOR;
        case BlendFactor::ONE_MINUS_SRC_COLOR: return VK_BLEND_FACTOR_ONE_MINUS_SRC_COLOR;
        case BlendFactor::DST_COLOR: return VK_BLEND_FACTOR_DST_COLOR;
        case BlendFactor::ONE_MINUS_DST_COLOR: return VK_BLEND_FACTOR_ONE_MINUS_DST_COLOR;
        case BlendFactor::SRC_ALPHA: return VK_BLEND_FACTOR_SRC_ALPHA;
        case BlendFactor::ONE_MINUS_SRC_ALPHA: return VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
        case BlendFactor::DST_ALPHA: return VK_BLEND_FACTOR_DST_ALPHA;
        case BlendFactor::ONE_MINUS_DST_ALPHA: return VK_BLEND_FACTOR_ONE_MINUS_DST_ALPHA;
        case BlendFactor::CONSTANT_COLOR: return VK_BLEND_FACTOR_CONSTANT_COLOR;
        case BlendFactor::ONE_MINUS_CONSTANT_COLOR: return VK_BLEND_FACTOR_ONE_MINUS_CONSTANT_COLOR;
        case BlendFactor::CONSTANT_ALPHA: return VK_BLEND_FACTOR_CONSTANT_ALPHA;
        case BlendFactor::ONE_MINUS_CONSTANT_ALPHA: return VK_BLEND_FACTOR_ONE_MINUS_CONSTANT_ALPHA;
        case BlendFactor::SRC1_COLOR: return VK_BLEND_FACTOR_SRC1_COLOR;
        case BlendFactor::ONE_MINUS_SRC1_COLOR: return VK_BLEND_FACTOR_ONE_MINUS_SRC1_COLOR;
        case BlendFactor::SRC1_ALPHA: return VK_BLEND_FACTOR_SRC1_ALPHA;
        case BlendFactor::ONE_MINUS_SRC1_ALPHA: return VK_BLEND_FACTOR_ONE_MINUS_SRC1_ALPHA;
        default:
            assert(false);
            return VK_BLEND_FACTOR_ZERO;
    }
}

VkLogicOp GLLogicOpToOpenVulkanType(LogicOp op)
{
    switch (op)
    {
        case LogicOp::CLEAR: return VK_LOGIC_OP_CLEAR;
        case LogicOp::SET: return VK_LOGIC_OP_SET;
        case LogicOp::COPY: return VK_LOGIC_OP_COPY;
        case LogicOp::COPY_INVERTED: return VK_LOGIC_OP_COPY_INVERTED;
        case LogicOp::NOOP: return VK_LOGIC_OP_NO_OP;
        case LogicOp::INVERT: return VK_LOGIC_OP_INVERT;
        case LogicOp::AND: return VK_LOGIC_OP_AND;
        case LogicOp::NAND: return VK_LOGIC_OP_NAND;
        case LogicOp::OR: return VK_LOGIC_OP_OR;
        case LogicOp::NOR: return VK_LOGIC_OP_NOR;
        case LogicOp::XOR: return VK_LOGIC_OP_XOR;
        case LogicOp::EQUIV: return VK_LOGIC_OP_EQUIVALENT;
        case LogicOp::AND_REVERSE: return VK_LOGIC_OP_AND_REVERSE;
        case LogicOp::AND_INVERTED: return VK_LOGIC_OP_AND_INVERTED;
        case LogicOp::OR_REVERSE: return VK_LOGIC_OP_OR_REVERSE;
        case LogicOp::OR_INVERTED: return VK_LOGIC_OP_OR_INVERTED;
        default:
            assert(false);
            return VK_LOGIC_OP_CLEAR;
    }
}

VkPrimitiveTopology GLPrimitiveToOpenVulkanype(Primitive primitive)
{
    switch (primitive)
    {
        case Primitive::POINT_LIST: return VK_PRIMITIVE_TOPOLOGY_POINT_LIST;
        case Primitive::LINE_LIST: return VK_PRIMITIVE_TOPOLOGY_LINE_LIST;
        case Primitive::LINE_STRIP: return VK_PRIMITIVE_TOPOLOGY_LINE_STRIP;
        case Primitive::TRIANGLE_LIST: return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        case Primitive::TRIANGLE_STRIP: return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP;
        case Primitive::TRIANGLE_FAN: return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_FAN;
        case Primitive::PATCH_LIST: return VK_PRIMITIVE_TOPOLOGY_PATCH_LIST;
        case Primitive::LINE_LIST_ADJ: return VK_PRIMITIVE_TOPOLOGY_LINE_LIST_WITH_ADJACENCY;
        case Primitive::LINE_STRIP_ADJ: return VK_PRIMITIVE_TOPOLOGY_LINE_STRIP_WITH_ADJACENCY;
        case Primitive::TRIANGLE_LIST_ADJ: return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST_WITH_ADJACENCY;
        case Primitive::TRIANGLE_STRIP_ADJ: return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP_WITH_ADJACENCY;
        default:
            assert(false);
            return VK_PRIMITIVE_TOPOLOGY_POINT_LIST;
    }
}

VkStencilOp GLStencilOpToVulkanType(StencilOp op)
{
    switch (op)
    {
        case StencilOp::KEEP: return VK_STENCIL_OP_KEEP;
        case StencilOp::ZERO: return VK_STENCIL_OP_ZERO;
        case StencilOp::REPLACE: return VK_STENCIL_OP_REPLACE;
        case StencilOp::INCREMENT_AND_CLAMP: return VK_STENCIL_OP_INCREMENT_AND_CLAMP;
        case StencilOp::DECREMENT_AND_CLAMP: return VK_STENCIL_OP_DECREMENT_AND_CLAMP;
        case StencilOp::INVERT: return VK_STENCIL_OP_INVERT;
        case StencilOp::INCREMENT_AND_WRAP: return VK_STENCIL_OP_INCREMENT_AND_WRAP;
        case StencilOp::DECREMENT_AND_WRAP: return VK_STENCIL_OP_DECREMENT_AND_WRAP;
        default:
            assert(false);
            return VK_STENCIL_OP_KEEP;
    }
}

VkCullModeFlagBits GLCullModeToVulkanType(CullMode mode)
{
    switch (mode)
    {
        case CullMode::BACK: return VK_CULL_MODE_BACK_BIT;
        case CullMode::FRONT: return VK_CULL_MODE_FRONT_BIT;
        case CullMode::FRONT_AND_BACK: return VK_CULL_MODE_FRONT_AND_BACK;
        case CullMode::NONE: return VK_CULL_MODE_NONE;
        default:
            assert(false);
            return VK_CULL_MODE_BACK_BIT;
    }
}

VkShaderStageFlagBits SLShaderStageToVulkanType(SL::ShaderStage stage)
{
    switch (stage)
    {
        case SL::ShaderStage::VERTEX: return VK_SHADER_STAGE_VERTEX_BIT;
        case SL::ShaderStage::GEOMETRY: return VK_SHADER_STAGE_GEOMETRY_BIT;
        case SL::ShaderStage::COMPUTE: return VK_SHADER_STAGE_COMPUTE_BIT;
        case SL::ShaderStage::FRAGMENT: return VK_SHADER_STAGE_FRAGMENT_BIT;
        default:
            assert(false);
            return VK_SHADER_STAGE_FRAGMENT_BIT;
    }
}

VkSamplerAddressMode GLTextureAddressModeToVulkanType(TextureAddressMode mode)
{
    switch (mode)
    {
        case TextureAddressMode::REPEAT: return VK_SAMPLER_ADDRESS_MODE_REPEAT;
        case TextureAddressMode::REPEAT_MIRROR: return VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT;
        case TextureAddressMode::CLAMP_TO_EDGE: return VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
        case TextureAddressMode::CLAMP_TO_BORDER: return VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;
        default:
            assert(false);
            return VK_SAMPLER_ADDRESS_MODE_REPEAT;
    }
}

VkAttachmentLoadOp GLRPActionToVulkanType(RPAction action)
{
    switch (action)
    {
        case RPAction::CLEAR: return VK_ATTACHMENT_LOAD_OP_CLEAR;
        case RPAction::KEEP: return VK_ATTACHMENT_LOAD_OP_LOAD;
        case RPAction::DONT_CARE: return VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        default:
            assert(false);
            return VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    }
}

VkAttachmentStoreOp GLRPStoreActionToVulkanType(RPAction action)
{
    switch (action)
    {
        case RPAction::STORE: return VK_ATTACHMENT_STORE_OP_STORE;
        case RPAction::DONT_CARE: return VK_ATTACHMENT_STORE_OP_DONT_CARE;
        default:
            assert(false);
            return VK_ATTACHMENT_STORE_OP_DONT_CARE;
    }
}

} // namespace Mmp