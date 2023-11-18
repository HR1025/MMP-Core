#include "VulkanCommon.h"

#include <cassert>

namespace Mmp
{

VulkanRenderPassKey::VulkanRenderPassKey()
{
    colorLoadAction = RPAction::DONT_CARE;
	depthLoadAction = RPAction::DONT_CARE;
	stencilLoadAction = RPAction::DONT_CARE;
	colorStoreAction = RPAction::DONT_CARE;
	depthStoreAction = RPAction::DONT_CARE;
	stencilStoreAction = RPAction::DONT_CARE;
}

bool operator<(VulkanRenderPassKey left, VulkanRenderPassKey right)
{
    if ((uint32_t)left.colorLoadAction < (uint32_t)right.colorLoadAction)
    {
        return true;
    }
    else if ((uint32_t)left.depthLoadAction < (uint32_t)right.depthLoadAction)
    {
        return true;
    }
    else if ((uint32_t)left.stencilLoadAction < (uint32_t)right.stencilLoadAction)
    {
        return true;
    }
    else if ((uint32_t)left.colorStoreAction < (uint32_t)right.colorStoreAction)
    {
        return true;
    }
    else if ((uint32_t)left.depthStoreAction < (uint32_t)right.depthStoreAction)
    {
        return true;
    }
    else if ((uint32_t)left.stencilStoreAction < (uint32_t)right.stencilStoreAction)
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool operator>(VulkanRenderPassType left, VulkanRenderPassType right)
{
    return !(left < right) && left != right;
}

bool operator<=(VulkanRenderPassType left, VulkanRenderPassType right)
{
    return !(left > right);
}

bool operator>=(VulkanRenderPassType left, VulkanRenderPassType right)
{
    return !(left < right);
}

const std::string VulkanRenderRunTypeToStr(VulkanRenderRunType type)
{
    switch (type)
    {
        case VulkanRenderRunType::NORMAL: return "NORMAL";
        case VulkanRenderRunType::PRESENT: return "PRESENT";
        case VulkanRenderRunType::SYNC: return "SYNC";
        case VulkanRenderRunType::EXIT: return "EXIT";
        default:
            assert(false);
            return "UNKNOWN";
    }
}

std::ostream& operator<<(std::ostream& os, VulkanRenderRunType type)
{
    os << VulkanRenderRunTypeToStr(type);
    return os;
}

const std::string VulkanRenderPassTypeToStr(VulkanRenderPassType type)
{
    switch (type)
    {
        case VulkanRenderPassType::DEFAULT: return "DEFAULT";
        case VulkanRenderPassType::HAS_DEPTH: return "HAS_DEPTH";
        case VulkanRenderPassType::MULTIVIEW: return "MULTIVIEW";
        case VulkanRenderPassType::MULTISAMPLE: return "MULTISAMPLE";
        case VulkanRenderPassType::BACKBUFFER: return "BACKBUFFER";
        default: assert(false); return "UNKNOWN";
    }
}

extern std::ostream& operator<<(std::ostream& os, VulkanRenderPassType type)
{
    os << VulkanRenderPassTypeToStr(type);
    return os;
}

const std::string VulkanRenderStepTypeToStr(VulkanRenderStepType type)
{
    switch (type)
    {
        case VulkanRenderStepType::RENDER: return "RENDER";
        case VulkanRenderStepType::COPY: return "COPY";
        case VulkanRenderStepType::BLIT: return "BLIT";
        case VulkanRenderStepType::READBACK: return "READBACK";
        case VulkanRenderStepType::READBACK_IMAGE: return "READBACK_IMAGE";
        case VulkanRenderStepType::RENDER_SKIP: return "RENDER_SKIP";
        default:
            assert(false);
            return "UNKNOWN";
    }
}

std::ostream& operator<<(std::ostream& os, VulkanRenderStepType type)
{
    os << VulkanRenderStepTypeToStr(type);
    return os;
}

const std::string VulkanRenderCommondToStr(VulkanRenderCommond cmd)
{
    switch (cmd)
    {
        case VulkanRenderCommond::REMOVED: return "REMOVED";
        case VulkanRenderCommond::BIND_GRAPHICS_PIPELINE: return "BIND_GRAPHICS_PIPELINE";
        case VulkanRenderCommond::BIND_COMPUTE_PIPELINE: return "BIND_COMPUTE_PIPELINE";
        case VulkanRenderCommond::STENCIL: return "STENCIL";
        case VulkanRenderCommond::BLEND: return "BLEND";
        case VulkanRenderCommond::VIEWPORT: return "VIEWPORT";
        case VulkanRenderCommond::SCISSOR: return "SCISSOR";
        case VulkanRenderCommond::CLEAR: return "CLEAR";
        case VulkanRenderCommond::DRAW: return "DRAW";
        case VulkanRenderCommond::DRAW_INDEXED: return "DRAW_INDEXED";
        case VulkanRenderCommond::PUSH_CONSTANTS: return "PUSH_CONSTANTS";
        case VulkanRenderCommond::DEBUG_ANNOTATION: return "DEBUG_ANNOTATION";
        default:
            assert(false);
            return "UNKNOWN";
    }
}

std::ostream& operator<<(std::ostream& os, VulkanRenderCommond cmd)
{
    os << VulkanRenderCommondToStr(cmd);
    return os;
}

bool operator==(TransitionRequest left, TransitionRequest right)
{
    return left.fb.get() == right.fb.get() && left.aspect == right.aspect && left.targetLayout == right.targetLayout;
}

VkFormat DataFormatToVulkan(DataFormat format)
{
    switch (format)
    {
        case DataFormat::R16_UNORM: return VK_FORMAT_R16_UNORM;
        case DataFormat::R16_FLOAT: return VK_FORMAT_R16_SFLOAT;
        case DataFormat::R16G16_FLOAT: return VK_FORMAT_R16G16_SFLOAT;
        case DataFormat::R16G16B16A16_FLOAT: return VK_FORMAT_R16G16B16A16_SFLOAT;
        case DataFormat::R8_UNORM: return VK_FORMAT_R8_UNORM;
        case DataFormat::R8G8_UNORM: return VK_FORMAT_R8G8_UNORM;
        case DataFormat::R8G8B8_UNORM: return VK_FORMAT_R8G8B8_UNORM;
        case DataFormat::R8G8B8A8_UNORM: return VK_FORMAT_R8G8B8A8_UNORM;
        case DataFormat::R4G4_UNORM_PACK8: return VK_FORMAT_R4G4_UNORM_PACK8;
        case DataFormat::R4G4B4A4_UNORM_PACK16: return VK_FORMAT_R4G4B4A4_UNORM_PACK16;
        case DataFormat::B4G4R4A4_UNORM_PACK16: return VK_FORMAT_B4G4R4A4_UNORM_PACK16;
        case DataFormat::R5G5B5A1_UNORM_PACK16: return VK_FORMAT_R5G5B5A1_UNORM_PACK16;
        case DataFormat::R5G6B5_UNORM_PACK16: return VK_FORMAT_R5G6B5_UNORM_PACK16;
        case DataFormat::B5G6R5_UNORM_PACK16: return VK_FORMAT_B5G6R5_UNORM_PACK16;
        case DataFormat::A1R5G5B5_UNORM_PACK16: return VK_FORMAT_A1R5G5B5_UNORM_PACK16;
        case DataFormat::R32_FLOAT: return VK_FORMAT_R32_SFLOAT;
        case DataFormat::R32G32_FLOAT: return VK_FORMAT_R32G32_SFLOAT;
        case DataFormat::R32G32B32_FLOAT: return VK_FORMAT_R32G32B32_SFLOAT;
        case DataFormat::R32G32B32A32_FLOAT: return VK_FORMAT_R32G32B32A32_SFLOAT;
        default:
            assert(false);
            return VK_FORMAT_UNDEFINED;
    }
}

bool IsDepthStencilFormat(VkFormat format) 
{
	switch (format) 
    {
        case VK_FORMAT_D16_UNORM:
        case VK_FORMAT_D16_UNORM_S8_UINT:
        case VK_FORMAT_D24_UNORM_S8_UINT:
        case VK_FORMAT_D32_SFLOAT:
        case VK_FORMAT_D32_SFLOAT_S8_UINT:
            return true;
        default:
            return false;
	}
}

} // namespace Mmp