//
// VulkanStepData.h
//
// Library: GPU
// Package: GL
// Module:  Vulkan
// 

#pragma once

#include "VulkanCommon.h"

namespace Mmp
{

struct VulkanRenderData
{
    std::shared_ptr<VulkanFrameBuffer>        frameBuffer;
    RPAction                                  colorLoad;
    RPAction                                  depthLoad;
    RPAction                                  stencilLoad;
    RPAction                                  colorStore;
    RPAction                                  depthStore;
    RPAction                                  stencilStore;
    uint8_t                                   clearStencil;
    uint32_t                                  clearColor;
    float                                     clearDepth;
    uint32_t                                  numDraws;
    uint32_t                                  numReads;
    VkImageLayout                             finalColorLayout;
    VkImageLayout                             finalDepthStencilLayout;
    GLRect2D                                  renderArea;
    VulkanRenderPassType                      renderPassType;
};

struct VulkanCopyData
{
    std::shared_ptr<VulkanFrameBuffer>        src;
    std::shared_ptr<VulkanFrameBuffer>        dst;
    GLRect2D                                  srcRect;
    GLOffset2D                                dstPos;
    int32_t                                   aspectMask;
};

struct VulkanBlitData
{
    std::shared_ptr<VulkanFrameBuffer>        src;
    std::shared_ptr<VulkanFrameBuffer>        dst;
    GLRect2D                                  srcRect;
    GLRect2D                                  dstRect;
    VkImageAspectFlagBits                     aspectMask;
    VkFilter                                  filter;
};

struct VulkanReadBackData
{
    VkImageAspectFlags                         aspectMask;
    std::shared_ptr<VulkanFrameBuffer>         src;
    GLRect2D                                   srcRect;
    GLPixelData::ptr                           pixel;
};

struct VulkanReadbackImageData
{
    std::vector<VkImage>                        images;
    std::vector<GLRect2D>                       srcRects;
    int32_t                                     mipLevel;
    SharedData::ptr                             picture;
};

} // namespace Mmp