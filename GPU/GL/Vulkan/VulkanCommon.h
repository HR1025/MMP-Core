//
// VulkanCommon.h
//
// Library: GPU
// Package: GL
// Module:  Vulkan
//

#pragma once

#include <set>
#include <map>
#include <cstdint>
#include <vector>
#include <memory>

#include "Common/Common.h"
#include "Common/Any.h"

#include "vulkan/vulkan_core.h"
#include "vk_mem_alloc.h"

#if MMP_PLATFORM(WINDOWS)
#include "vulkan/vulkan_win32.h"
#endif

#include "GLCommon.h"
#include "GLContexDesc.h"
#include "GLContex.h"

namespace Mmp
{
class VulkanDrawContex;
class VulkanGraphicsPileline;
class VulkanFrameBuffer;
class VulkanImage;
class VulkanRenderPass;
class VulkanRenderImage;
class VulkanBarrier;

#define VULKAN_MAX_BOUND_TEXTURES 8

enum VulkanPipelineFlags
{
    VK_NONE = 0,
    VK_USES_BLEND_CONSTANT = 1 << 1u,
    VK_USES_DEPTH_STENCIL = 1 << 2u,
    VK_USES_GEOMETRY_SHADER = 1 << 3u,
    VK_USES_MULTIVIEW = 1 << 4u,
    VK_USES_DISCARD = 1 << 5u
};

class VulkanRenderPassKey
{
public:
    VulkanRenderPassKey();
public:
    RPAction colorLoadAction;
	RPAction depthLoadAction;
	RPAction stencilLoadAction;
	RPAction colorStoreAction;
	RPAction depthStoreAction;
	RPAction stencilStoreAction;
};
bool operator<(VulkanRenderPassKey left, VulkanRenderPassKey right);

enum class VulkanRenderRunType
{
    NORMAL,
    PRESENT,
    SYNC,
    EXIT,
};
const std::string VulkanRenderRunTypeToStr(VulkanRenderRunType type);
extern std::ostream& operator<<(std::ostream& os, VulkanRenderRunType type);

enum class VulkanRenderPassType
{
    DEFAULT,
    HAS_DEPTH,
    MULTIVIEW,
    MULTISAMPLE,
    BACKBUFFER
};
const std::string VulkanRenderPassTypeToStr(VulkanRenderPassType type);
extern std::ostream& operator<<(std::ostream& os, VulkanRenderPassType type);
bool operator<(VulkanRenderPassType left, VulkanRenderPassType right);
bool operator>(VulkanRenderPassType left, VulkanRenderPassType right);
bool operator<=(VulkanRenderPassType left, VulkanRenderPassType right);
bool operator>=(VulkanRenderPassType left, VulkanRenderPassType right);

enum class VulkanRenderStepType
{
    RENDER,
    COPY,
    BLIT,
    READBACK,
    READBACK_IMAGE,
    RENDER_SKIP
};
const std::string VulkanRenderStepTypeToStr(VulkanRenderStepType type);
extern std::ostream& operator<<(std::ostream& os, VulkanRenderStepType type);

enum class VulkanRenderCommond
{
    REMOVED,
    BIND_GRAPHICS_PIPELINE,
    BIND_COMPUTE_PIPELINE,
    STENCIL,
    BLEND,
    VIEWPORT,
    SCISSOR,
    CLEAR,
    DRAW,
    DRAW_INDEXED,
    PUSH_CONSTANTS,
    DEBUG_ANNOTATION
};
const std::string VulkanRenderCommondToStr(VulkanRenderCommond cmd);
extern std::ostream& operator<<(std::ostream& os, VulkanRenderCommond cmd);

class TransitionRequest
{
public:
    std::shared_ptr<VulkanFrameBuffer>  fb;
    VkImageAspectFlagBits               aspect;
    VkImageLayout                       targetLayout;
};
bool operator==(TransitionRequest left, TransitionRequest right);

struct VulkanRender
{
    VulkanRenderCommond cmd;
    Any                 data;
};

struct VulkanRenderStep
{
    VulkanRenderStepType         stepType;
    std::vector<VulkanRender>    commands;
    Any                          data;
    std::set<TransitionRequest>  preTransitions;
};

struct VulkanThreadTask
{
    std::vector<VulkanRenderStep>   steps;
    int                             frames = 0;
    VulkanRenderRunType             runType;
};

VkFormat DataFormatToVulkan(DataFormat format);

bool IsDepthStencilFormat(VkFormat format);

} // namespace Mmp