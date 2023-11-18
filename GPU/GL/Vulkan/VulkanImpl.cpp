#include "VulkanImpl.h"

#include <algorithm>

#include "VulkanUtil.h"
#include "VulkanImage.h"
#include "VulkanBarrier.h"
#include "VulkanInterface.h"
#include "VulkanRenderPass.h"
#include "VulkanFrameBuffer.h"
#include "VulkanRenderContex.h"

namespace Mmp
{

Vulkan::Vulkan()
{
    _backbufferImage = VK_NULL_HANDLE;
    _backBufferWidth = 0;
    _backBufferHeight = 0;
}

Vulkan::~Vulkan()
{

}

void Vulkan::RenderStepRenderPass(VulkanRenderStep& step, VkCommandBuffer cmd)
{
    assert(step.data.type() == typeid(VulkanRenderData));
    const VulkanRenderData& render = RefAnyCast<VulkanRenderData>(step.data);
    std::vector<VulkanRender> commands = step.commands;

    VulkanRenderPass::ptr renderPass;
    VulkanFrameBuffer::ptr fb;
    uint32_t curWidth = 0, curHeight = 0;

    VulkanGraphicsPileline::ptr  lastGrpahicsPipeline;
    VkPipeline lastPipeline = VK_NULL_HANDLE;
    VkPipelineLayout pipelineLayout = VK_NULL_HANDLE;

    bool pipelineOK = false;

    int lastStencilWriteMask = -1;
    int lastStencilCompareMask = -1;
    int lastStencilReference = -1;

    for (const auto& preTransition: step.preTransitions)
    {
        if (preTransition.aspect == VK_IMAGE_ASPECT_COLOR_BIT && preTransition.fb->color->layout != preTransition.targetLayout)
        {
            _recordBarrier->TransitionImageAuto(
                preTransition.fb->color->image,
                0,
                1,
                preTransition.fb->numLayers,
                VK_IMAGE_ASPECT_COLOR_BIT,
                preTransition.fb->color->layout,
                preTransition.targetLayout
            );
            preTransition.fb->color->layout = preTransition.targetLayout;
        }
        if (preTransition.fb->depth->image != VK_NULL_HANDLE && preTransition.aspect == (VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT) && preTransition.fb->depth->layout != preTransition.targetLayout)
        {
            _recordBarrier->TransitionImageAuto(
                preTransition.fb->depth->image,
                0,
                1,
                preTransition.fb->numLayers,
                VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT,
                preTransition.fb->depth->layout,
                preTransition.targetLayout
            );     
        }
    }

    if (step.commands.empty() &&
        render.colorLoad == RPAction::KEEP && render.depthLoad == RPAction::KEEP && render.stencilLoad == RPAction::KEEP
    )
    {
        _recordBarrier->Flush(cmd);
        return;
    }

    if (render.frameBuffer)
    {
        if (render.frameBuffer->color->layout == VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL)
        {
            _recordBarrier->TransitionImage(
                render.frameBuffer->color->image,
                0,
                1,
                render.frameBuffer->numLayers,
                VK_IMAGE_ASPECT_COLOR_BIT,
                VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
                VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
                VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
                VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_COLOR_ATTACHMENT_READ_BIT,
                VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
                VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT
            );
        }
        if (render.frameBuffer->depth->image != VK_NULL_HANDLE && render.frameBuffer->depth->layout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL)
        {
            _recordBarrier->TransitionImage(
                render.frameBuffer->depth->image,
                0,
                1,
                render.frameBuffer->numLayers,
                VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT,
                VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
                VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
                VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
                VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT,
                VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT,
                VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT
            );
        }
    }
    
    renderPass = PerformBindFramebufferAsRenderTarget(step, cmd);
    curWidth = render.frameBuffer ? render.frameBuffer->width : _backBufferWidth;
    curHeight = render.frameBuffer ? render.frameBuffer->height : _backBufferHeight;
    fb = render.frameBuffer;

    for (auto command : commands)
    {
        switch (command.cmd)
        {
            case VulkanRenderCommond::REMOVED:
            {
                break;
            }
            case VulkanRenderCommond::BIND_GRAPHICS_PIPELINE:
            {
                // TODO
                assert(false);
                break;
            }
            case VulkanRenderCommond::VIEWPORT:
            {
                assert(command.data.type() == typeid(VulkanViewportRenderData));
                const VulkanViewportRenderData& vpData = RefAnyCast<VulkanViewportRenderData>(command.data);
                if (fb != nullptr)
                {
                    VulkanInterface::Instance()->vkCmdSetViewport(cmd, 0, 1, &vpData.vp);
                }
                else
                {
                    // TODO
                    VulkanInterface::Instance()->vkCmdSetViewport(cmd, 0, 1, &vpData.vp);
                }
                break;
            }
            case VulkanRenderCommond::SCISSOR:
            {
                assert(command.data.type() == typeid(VulkanScissorRenderData));
                const VulkanScissorRenderData& scissorData = RefAnyCast<VulkanScissorRenderData>(command.data);
                if (fb != nullptr)
                {
                    VkRect2D scissor = {};
                    {
                        scissor.extent.width = scissorData.scissor.w;
                        scissor.extent.height = scissorData.scissor.h;
                        scissor.offset.x = scissorData.scissor.x;
                        scissor.offset.y = scissorData.scissor.y;
                    }
                    VulkanInterface::Instance()->vkCmdSetScissor(cmd, 0, 1, &scissor);
                }
                else
                {
                    // TODO
                    VkRect2D scissor = {};
                    {
                        scissor.extent.width = scissorData.scissor.w;
                        scissor.extent.height = scissorData.scissor.h;
                        scissor.offset.x = scissorData.scissor.x;
                        scissor.offset.y = scissorData.scissor.y;
                    }
                    VulkanInterface::Instance()->vkCmdSetScissor(cmd, 0, 1, &scissor);
                }
                break;
            }
            case VulkanRenderCommond::BLEND:
            {
                assert(command.data.type() == typeid(VulkanBlendColorRenderData));
                const VulkanBlendColorRenderData& blendData = RefAnyCast<VulkanBlendColorRenderData>(command.data);
                float bc[4] = {};
                Uint8x4ToFloat4(blendData.blendColor, bc);
                VulkanInterface::Instance()->vkCmdSetBlendConstants(cmd, bc);
                break;
            }
            case VulkanRenderCommond::PUSH_CONSTANTS:
            {
                assert(command.data.type() == typeid(VulkanPushRenderData));
                const VulkanPushRenderData& pushData = RefAnyCast<VulkanPushRenderData>(command.data);
                if (pipelineOK)
                {
                    VulkanInterface::Instance()->vkCmdPushConstants(cmd, pipelineLayout, pushData.stages, pushData.offset, pushData.size, pushData.data);
                }
                break;
            }
            case VulkanRenderCommond::STENCIL:
            {
                assert(command.data.type() == typeid(VulkanStencilRenderData));
                const VulkanStencilRenderData& stencilData = RefAnyCast<VulkanStencilRenderData>(command.data);
                if (lastStencilWriteMask != (int)stencilData.stencilWriteMask)
                {
                    lastStencilWriteMask = (int)stencilData.stencilWriteMask;
                    VulkanInterface::Instance()->vkCmdSetStencilWriteMask(cmd, VK_STENCIL_FRONT_AND_BACK, stencilData.stencilWriteMask);
                }
                if (lastStencilCompareMask != (int)stencilData.stencilCompareMask)
                {
                    lastStencilCompareMask = (int)stencilData.stencilCompareMask;
                    VulkanInterface::Instance()->vkCmdSetStencilCompareMask(cmd, VK_STENCIL_FACE_BACK_BIT, stencilData.stencilCompareMask);
                }
                if (lastStencilReference != (int)stencilData.stencilRef)
                {
                    lastStencilReference = (int)stencilData.stencilRef;
                    VulkanInterface::Instance()->vkCmdSetStencilReference(cmd, VK_STENCIL_FRONT_AND_BACK, stencilData.stencilRef);
                }
                break;
            }
            case VulkanRenderCommond::DRAW_INDEXED:
            {
                assert(command.data.type() == typeid(VulkanDrawIndexedRenderData));
                const VulkanDrawIndexedRenderData& drawData = RefAnyCast<VulkanDrawIndexedRenderData>(command.data);
                if (pipelineOK)
                {
                    VulkanInterface::Instance()->vkCmdBindDescriptorSets(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1, &drawData.ds, drawData.numUboOffsets, drawData.uboOffsets);
                    VulkanInterface::Instance()->vkCmdBindIndexBuffer(cmd, drawData.ibuffer, drawData.ioffset, VK_INDEX_TYPE_UINT16);
                    VkDeviceSize voffset = (VkDeviceSize)drawData.voffset;
                    VulkanInterface::Instance()->vkCmdBindVertexBuffers(cmd, 0, 1, &drawData.vbuffer, &voffset);
                    VulkanInterface::Instance()->vkCmdDrawIndexed(cmd, drawData.count, drawData.instance, 0, 0, 0);
                }
                break;
            }
            case VulkanRenderCommond::DRAW:
            {
                assert(command.data.type() == typeid(VulkanDrawRenderData));
                const VulkanDrawRenderData& drawData = RefAnyCast<VulkanDrawRenderData>(command.data);
                if (pipelineOK)
                {
                    VulkanInterface::Instance()->vkCmdBindDescriptorSets(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1, &drawData.ds, (uint32_t)drawData.numUboOffsets, drawData.uboOffsets);
                    if (drawData.vbuffer)
                    {
                        VulkanInterface::Instance()->vkCmdBindVertexBuffers(cmd, 0, 1, &drawData.vbuffer, &drawData.voffset);
                    }
                    VulkanInterface::Instance()->vkCmdDraw(cmd, drawData.count, 1, drawData.offset, 0);
                }
                break;
            }
            case VulkanRenderCommond::CLEAR:
            {
                assert(command.data.type() == typeid(VulkanClearRenderData));
                const VulkanClearRenderData& clearData = RefAnyCast<VulkanClearRenderData>(command.data);
                int numAttachemnts = 0;
                VkClearRect rc = {};
                VkClearAttachment attachments[2] = {};
                {
                    rc.baseArrayLayer = 0;
                    rc.layerCount = 1;
                    rc.rect.extent.width = curWidth;
                    rc.rect.extent.height = curHeight;
                }
                if (clearData.clearMask & VK_IMAGE_ASPECT_COLOR_BIT)
                {
                    VkClearAttachment& attachment = attachments[numAttachemnts++];
                    attachment.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
                    attachment.colorAttachment = 0;
                    Uint8x4ToFloat4(clearData.clearData, attachment.clearValue.color.float32);
                }
                if (clearData.clearMask & (VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT))
                {
                    VkClearAttachment& attachment = attachments[numAttachemnts++];
                    attachment.aspectMask = 0;
                    if (clearData.clearMask & VK_IMAGE_ASPECT_DEPTH_BIT)
                    {
                        attachment.clearValue.depthStencil.depth = clearData.clearZ;
                        attachment.aspectMask |= VK_IMAGE_ASPECT_DEPTH_BIT;
                    }
                    if (clearData.clearMask & VK_IMAGE_ASPECT_STENCIL_BIT)
                    {
                        attachment.clearValue.depthStencil.stencil = (uint32_t)clearData.clearStencil;
                        attachment.aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;
                    }
                }
                if (numAttachemnts)
                {
                    VulkanInterface::Instance()->vkCmdClearAttachments(cmd, numAttachemnts, attachments, 1, &rc);
                }
                break;
            }
            default:
                assert(false);
                break;
        }
    }
    VulkanInterface::Instance()->vkCmdEndRenderPass(cmd);

    if (fb)
    {
        TransitionFromOptimal(cmd, fb->color->image, render.finalColorLayout, fb->depth->image, render.finalDepthStencilLayout, fb->numLayers);
        fb->color->layout = render.finalColorLayout;
        fb->depth->layout = render.finalDepthStencilLayout;
    }
}

void Vulkan::RenderStepCopy(VulkanRenderStep& step, VkCommandBuffer cmd)
{
    assert(step.data.type() == typeid(VulkanCopyData));
    const VulkanCopyData& copy = RefAnyCast<VulkanCopyData>(step.data);

    uint32_t layerCount = std::min(copy.src->numLayers, copy.dst->numLayers);
    VulkanFrameBuffer::ptr src = copy.src;
    VulkanFrameBuffer::ptr dst = copy.dst;
    bool multisampled = src->sampleCount != VK_SAMPLE_COUNT_1_BIT && dst->sampleCount != VK_SAMPLE_COUNT_1_BIT;
    VkImageCopy vkCopy = {};

    if (copy.aspectMask & VK_IMAGE_ASPECT_COLOR_BIT)
    {
        SetupTransitionToTransferSrc(src->color, VK_IMAGE_ASPECT_COLOR_BIT);
        SetupTransitionToTransferDst(dst->color, VK_IMAGE_ASPECT_COLOR_BIT);
    }
    if (copy.aspectMask & (VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT))
    {
        SetupTransitionToTransferSrc(src->depth, VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT);
        if (dst->depth->layout != VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
        {
            SetupTransitionToTransferDst(dst->depth, VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT); 
        }
        else
        {
            SetupTransferDstWriteAfterWrite(dst->depth, VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT); 
        }
    }
    _recordBarrier->Flush(cmd);
    if (multisampled)
    {
        if (copy.aspectMask & VK_IMAGE_ASPECT_COLOR_BIT)
        {
            SetupTransitionToTransferSrc(src->msaaColor, VK_IMAGE_ASPECT_COLOR_BIT);
            _recordBarrier->Flush(cmd);
            SetupTransitionToTransferDst(dst->msaaColor, VK_IMAGE_ASPECT_COLOR_BIT);
            _recordBarrier->Flush(cmd);
        }
        if (copy.aspectMask & (VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT))
        {
            SetupTransitionToTransferSrc(src->msaaColor, VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT);
            _recordBarrier->Flush(cmd);
            SetupTransitionToTransferDst(dst->msaaColor, VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT);
            _recordBarrier->Flush(cmd);   
        }
    }
    _recordBarrier->Flush(cmd);
    {
        vkCopy.srcOffset.x = copy.srcRect.x;
        vkCopy.srcOffset.y = copy.srcRect.y;
        vkCopy.srcOffset.z = 0;
        vkCopy.srcSubresource.mipLevel = 0;
        vkCopy.srcSubresource.layerCount = layerCount;
        vkCopy.dstOffset.x = copy.dstPos.x;
        vkCopy.dstOffset.y = copy.dstPos.y;
        vkCopy.dstOffset.z = 0;
        vkCopy.dstSubresource.mipLevel = 0;
        vkCopy.dstSubresource.layerCount = layerCount;
        vkCopy.extent.width = copy.srcRect.w;
        vkCopy.extent.height = copy.srcRect.h;
        vkCopy.extent.depth = 1;
    }
    if (copy.aspectMask & VK_IMAGE_ASPECT_COLOR_BIT)
    {
        vkCopy.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        vkCopy.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        VulkanInterface::Instance()->vkCmdCopyImage(cmd, src->color->image, src->color->layout, dst->color->image, dst->color->layout, 1, &vkCopy);
        if (multisampled)
        {
            VulkanInterface::Instance()->vkCmdCopyImage(cmd, src->msaaColor->image, src->msaaColor->layout, dst->msaaColor->image, dst->msaaColor->layout, 1, &vkCopy);
        }
    }
    if (copy.aspectMask & (VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT))
    {
        vkCopy.srcSubresource.aspectMask = copy.aspectMask & (VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT);
        vkCopy.dstSubresource.aspectMask = copy.aspectMask & (VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT);
        VulkanInterface::Instance()->vkCmdCopyImage(cmd, src->depth->image, src->depth->layout, dst->depth->image, dst->depth->layout, 1, &vkCopy);
        if (multisampled)
        {
            VulkanInterface::Instance()->vkCmdCopyImage(cmd, src->msaaDepth->image, src->msaaDepth->layout, dst->msaaDepth->image, dst->msaaDepth->layout, 1, &vkCopy);
        }
    }
	if (multisampled) 
    {
		if (copy.aspectMask & VK_IMAGE_ASPECT_COLOR_BIT) 
        {
			_recordBarrier->TransitionImage(
				src->msaaColor->image,
				0,
				1,
				src->msaaColor->numLayers,
				VK_IMAGE_ASPECT_COLOR_BIT,
				VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
				VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
				VK_ACCESS_TRANSFER_READ_BIT,
				VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
				VK_PIPELINE_STAGE_TRANSFER_BIT,
				VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT
			);
			_recordBarrier->TransitionImage(
				dst->msaaColor->image,
				0,
				1,
				dst->msaaColor->numLayers,
				VK_IMAGE_ASPECT_COLOR_BIT,
				VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
				VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
				VK_ACCESS_TRANSFER_WRITE_BIT,
				VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
				VK_PIPELINE_STAGE_TRANSFER_BIT,
				VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT
			);
			src->msaaColor->layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
			dst->msaaColor->layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		}
		if (copy.aspectMask & (VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT)) 
        {
			_recordBarrier->TransitionImage(
				src->msaaDepth->image,
				0,
				1,
				src->msaaDepth->numLayers,
				VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT,
				VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
				VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
				VK_ACCESS_TRANSFER_READ_BIT,
				VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
				VK_PIPELINE_STAGE_TRANSFER_BIT,
				VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT
			);
			_recordBarrier->TransitionImage(
				dst->msaaDepth->image,
				0,
				1,
				dst->msaaDepth->numLayers,
				VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT,
				VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
				VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
				VK_ACCESS_TRANSFER_WRITE_BIT,
				VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
				VK_PIPELINE_STAGE_TRANSFER_BIT,
				VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT
			);
			src->msaaDepth->layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
			dst->msaaDepth->layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
		}
		_recordBarrier->Flush(cmd);
	}
}

void Vulkan::RenderStepBlit(VulkanRenderStep& step, VkCommandBuffer cmd)
{
    assert(step.data.type() == typeid(VulkanBlitData));
    const VulkanBlitData& blit = RefAnyCast<VulkanBlitData>(step.data);

    VkImageBlit vkBlit = {};

    uint32_t layerCount = std::min(blit.src->numLayers, blit.dst->numLayers);
    VulkanFrameBuffer::ptr src = blit.src;
    VulkanFrameBuffer::ptr dst = blit.dst;
    if (blit.aspectMask & VK_IMAGE_ASPECT_COLOR_BIT)
    {
        SetupTransitionToTransferSrc(src->color, VK_IMAGE_ASPECT_COLOR_BIT);
        SetupTransitionToTransferDst(dst->color, VK_IMAGE_ASPECT_COLOR_BIT);
    }
    if (blit.aspectMask & (VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT))
    {
        SetupTransitionToTransferSrc(src->depth, VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT);
        SetupTransitionToTransferDst(dst->depth, VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT);    
    }
    _recordBarrier->Flush(cmd);
    {
        vkBlit.srcOffsets[0].x = blit.srcRect.x;
        vkBlit.srcOffsets[0].y = blit.srcRect.y;
        vkBlit.srcOffsets[0].z = 0;
        vkBlit.srcOffsets[1].x = blit.srcRect.x + blit.srcRect.w;
        vkBlit.srcOffsets[1].y = blit.srcRect.y + blit.srcRect.h;
        vkBlit.srcOffsets[1].z = 1;
        vkBlit.srcSubresource.mipLevel = 0;
        vkBlit.srcSubresource.layerCount = layerCount;
        vkBlit.dstOffsets[0].x = blit.dstRect.x;
        vkBlit.dstOffsets[0].y = blit.dstRect.y;
        vkBlit.dstOffsets[0].z = 0;
        vkBlit.dstOffsets[1].x = blit.dstRect.x + blit.dstRect.w;
        vkBlit.dstOffsets[1].y = blit.dstRect.y + blit.dstRect.h;
        vkBlit.dstOffsets[1].z = 1;
        vkBlit.dstSubresource.mipLevel = 0;
        vkBlit.dstSubresource.layerCount = layerCount;
    }
    if (blit.aspectMask & VK_IMAGE_ASPECT_COLOR_BIT)
    {
        vkBlit.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        vkBlit.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        VulkanInterface::Instance()->vkCmdBlitImage(cmd, 
            src->color->image, src->color->layout, dst->color->image, dst->color->layout, 
            1, &vkBlit, blit.filter
        );
    }
    if (blit.aspectMask & (VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT))
    {
        vkBlit.srcSubresource.aspectMask = VK_IMAGE_ASPECT_NONE;
        vkBlit.dstSubresource.aspectMask = VK_IMAGE_ASPECT_NONE;
        if (blit.aspectMask & VK_IMAGE_ASPECT_DEPTH_BIT)
        {
            vkBlit.srcSubresource.aspectMask |= VK_IMAGE_ASPECT_DEPTH_BIT;
            vkBlit.dstSubresource.aspectMask |= VK_IMAGE_ASPECT_DEPTH_BIT;
        }
        if (blit.aspectMask & VK_IMAGE_ASPECT_STENCIL_BIT)
        {
            vkBlit.srcSubresource.aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;
            vkBlit.dstSubresource.aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT; 
        }
        VulkanInterface::Instance()->vkCmdBlitImage(cmd, 
            src->depth->image, src->depth->layout, dst->depth->image, dst->depth->layout, 
            1, &vkBlit, blit.filter
        );
    }
}

void Vulkan::RenderStepReadback(VulkanRenderStep& step, VkCommandBuffer cmd)
{
    VkImage image = VK_NULL_HANDLE;
    VkImageLayout copyLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    VkBufferImageCopy region = {};
    assert(step.data.type() == typeid(VulkanReadBackData));
    const VulkanReadBackData& readback = RefAnyCast<VulkanReadBackData>(step.data);
    if (!readback.src)
    {
        TransitionImageLayout2(cmd, _backbufferImage, 0, 1, 1, VK_IMAGE_ASPECT_COLOR_BIT,
            VK_IMAGE_LAYOUT_PRESENT_SRC_KHR, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
            VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT,
            0, VK_ACCESS_TRANSFER_READ_BIT
        );
        copyLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
        image = _backbufferImage;
    }
    else
    {
        VulkanRenderImage::ptr srcImage;
        if (readback.aspectMask & VK_IMAGE_ASPECT_COLOR_BIT)
        {
            srcImage = readback.src->color;
        }
        else if (readback.aspectMask & (VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT))
        {
            srcImage = readback.src->depth;
        }
        else
        {
            assert(false);
            return;
        }
        if (srcImage->layout != VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL)
        {
            SetupTransitionToTransferSrc(srcImage, readback.aspectMask);
            _recordBarrier->Flush(cmd);
        }
        image = srcImage->image;
        copyLayout = srcImage->layout;
    }
    {
        region.imageOffset.x = (uint32_t)readback.srcRect.x;
        region.imageOffset.y = (uint32_t)readback.srcRect.y;
        region.imageOffset.z = 0u;
        region.imageExtent.width = (uint32_t)readback.srcRect.w;
        region.imageExtent.height = (uint32_t)readback.srcRect.h;
        region.imageExtent.depth = 1u;
        region.imageSubresource.aspectMask = readback.aspectMask;
        region.bufferOffset = 0;
        region.bufferRowLength = readback.srcRect.w;
        region.bufferImageHeight = readback.srcRect.h;
    }
    VulkanInterface::Instance()->vkCmdCopyImageToBuffer(cmd, image, copyLayout, VK_NULL_HANDLE /* TODO */, 1, &region);
    if (!readback.src)
    {
        TransitionImageLayout2(cmd, _backbufferImage, 0, 1, 1, VK_IMAGE_ASPECT_COLOR_BIT,
            VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
            VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT,
            VK_ACCESS_TRANSFER_READ_BIT, 0
        );
        copyLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
    }
}

void Vulkan::RenderStepReadbackImage(VulkanRenderStep& step, VkCommandBuffer cmd)
{
    assert(step.data.type() == typeid(VulkanReadbackImageData));
    const VulkanReadbackImageData& readbackImg = RefAnyCast<VulkanReadbackImageData>(step.data);
    // TODO
}

void Vulkan::SetupTransitionToTransferSrc(std::shared_ptr<VulkanRenderImage> img, VkImageAspectFlags aspect)
{
    if (img->layout == VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL)
    {
        return;
    }
    VkImageAspectFlags imageAspect = aspect;
    VkAccessFlags srcAccessMask = 0;
    VkPipelineStageFlags srcStageMask = 0;
    switch (img->layout)
    {
        case VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL:
        {
            srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_COLOR_ATTACHMENT_READ_BIT;
            srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
            break;
        }
        case VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL:
        {
            srcAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
            srcStageMask = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
            break;
        }
        case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
        {
            srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
            srcStageMask = VK_PIPELINE_STAGE_TRANSFER_BIT;
            break;
        }
        case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL:
        {
            srcAccessMask = VK_ACCESS_SHADER_READ_BIT;
            srcStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
            break;
        }
        default:
        {
            assert(false);
            break;
        }
    }

    if (img->format == VK_FORMAT_D16_UNORM_S8_UINT || img->format == VK_FORMAT_D24_UNORM_S8_UINT || img->format == VK_FORMAT_D32_SFLOAT_S8_UINT)
    {
        imageAspect = VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT;
    }
    else
    {
        imageAspect = aspect;
    }
    _recordBarrier->TransitionImage(
        img->image, 0, 1, img->numLayers, imageAspect,
        img->layout, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, srcAccessMask,
        VK_ACCESS_TRANSFER_READ_BIT, srcStageMask, VK_PIPELINE_STAGE_TRANSFER_BIT
    );
    img->layout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
}

void Vulkan::SetupTransitionToTransferDst(std::shared_ptr<VulkanRenderImage> img, VkImageAspectFlags aspect)
{
    if (img->layout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
    {
        return;
    }
    VkImageAspectFlags imageAspect = aspect;
    VkAccessFlags srcAccessMask = 0;
    VkPipelineStageFlags srcStageMask = 0;

    if (img->format == VK_FORMAT_D16_UNORM_S8_UINT || img->format == VK_FORMAT_D24_UNORM_S8_UINT || img->format == VK_FORMAT_D32_SFLOAT_S8_UINT)
    {
        imageAspect = VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT;
    }
    else
    {
        imageAspect = aspect;
    }

    switch (img->layout)
    {
        case VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL:
        {
            srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
            srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
            break;
        }
        case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
        {
            srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
            srcStageMask = VK_PIPELINE_STAGE_TRANSFER_BIT;
            break;
        }
        case VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL:
        {
            srcAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
            srcStageMask = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
            break;
        }
        case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL:
        {
            srcAccessMask = VK_ACCESS_SHADER_READ_BIT;
            srcStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
            break;
        }
        default:
            assert(false);
            break;
    }
    _recordBarrier->TransitionImage(
        img->image, 0, 1, img->numLayers, imageAspect,
        img->layout, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, srcAccessMask,
        VK_ACCESS_TRANSFER_WRITE_BIT, srcStageMask, VK_PIPELINE_STAGE_TRANSFER_BIT
    );
    img->layout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
}

void Vulkan::SetupTransferDstWriteAfterWrite(std::shared_ptr<VulkanRenderImage> img, VkImageAspectFlags aspect)
{
    VkImageAspectFlags imageAspect = aspect;
    VkAccessFlags srcAccessMask = 0;
    VkPipelineStageFlags srcStageMask = 0;
    if (img->format == VK_FORMAT_D16_UNORM_S8_UINT || img->format == VK_FORMAT_D24_UNORM_S8_UINT || img->format == VK_FORMAT_D32_SFLOAT_S8_UINT)
    {
        imageAspect = VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT;
    }
    else
    {
        imageAspect = aspect;
    }
    srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
    srcStageMask = VK_PIPELINE_STAGE_TRANSFER_BIT;
    _recordBarrier->TransitionImage(
        img->image, 0, 1, img->numLayers, imageAspect,
        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, srcAccessMask,
        VK_ACCESS_TRANSFER_WRITE_BIT, srcStageMask, VK_PIPELINE_STAGE_TRANSFER_BIT
    );
}

std::shared_ptr<VulkanRenderPass> Vulkan::PerformBindFramebufferAsRenderTarget(VulkanRenderStep& step, VkCommandBuffer cmd)
{
    assert(step.data.type() == typeid(VulkanRenderData));
    const VulkanRenderData& render = RefAnyCast<VulkanRenderData>(step.data);

    VkRenderPassBeginInfo rpBegin = {};
    VkRect2D rc = {};

    VulkanRenderPass::ptr renderPass;
    uint32_t numClearVals = 0;
    VkClearValue clearVal[4] = {};
    VkFramebuffer framebuf;
    uint32_t w = 0;
    uint32_t h = 0;
    VkSampleCountFlagBits sampleCount = VK_SAMPLE_COUNT_1_BIT;

    bool hasDepth = render.renderPassType == VulkanRenderPassType::HAS_DEPTH;

    if (render.frameBuffer)
    {
        VulkanFrameBuffer::ptr fb = render.frameBuffer;
        VulkanRenderPassKey key = {};
        {
            key.colorLoadAction = render.colorLoad;
            key.depthLoadAction = render.depthLoad;
            key.stencilLoadAction = render.stencilLoad;
            key.colorStoreAction = render.colorStore;
            key.depthStoreAction = render.depthStore;
            key.stencilStoreAction = render.stencilStore;
        }
        renderPass = GetRenderPass(key);
        framebuf = fb->Get(render.renderPassType);
        sampleCount = fb->sampleCount;
        w = fb->width;
        h = fb->height;
        TransitionToOptimal(cmd, fb->color->image, fb->color->layout, fb->depth->image, fb->depth->layout, fb->numLayers);
        if (sampleCount != VK_SAMPLE_COUNT_1_BIT)
        {
            numClearVals = hasDepth ? 2 : 1;
        }
        {
            if (render.colorLoad == RPAction::CLEAR)
            {
                Uint8x4ToFloat4(render.clearColor, clearVal[numClearVals].color.float32);
            }
            if (hasDepth)
            {
                if (render.depthLoad == RPAction::CLEAR || render.stencilLoad == RPAction::CLEAR)
                {
                    clearVal[numClearVals].depthStencil.depth = render.clearDepth;
                    clearVal[numClearVals].depthStencil.stencil = render.clearStencil;
                }
                numClearVals++;
            }
        }
    }
    else
    {
        VulkanRenderPassKey key = {};
        {
            key.colorLoadAction = RPAction::CLEAR;
            key.depthLoadAction = RPAction::CLEAR;
            key.stencilLoadAction = RPAction::CLEAR;
            key.colorStoreAction = RPAction::STORE;
            key.depthStoreAction = RPAction::STORE;
            key.stencilLoadAction = RPAction::STORE;
        }
        renderPass = GetRenderPass(key);
        framebuf = _backBuffer->Get(render.renderPassType);
        w = _backBufferWidth;
        h = _backBufferHeight;
        Uint8x4ToFloat4(render.clearColor, clearVal[0].color.float32);
        numClearVals = hasDepth ? 2 : 1;
        clearVal[1].depthStencil.depth = 0.0f;
        clearVal[1].depthStencil.stencil = 0;
        sampleCount = VK_SAMPLE_COUNT_1_BIT;
    }

    _recordBarrier->Flush(cmd);

    {
        rc.offset.x = render.renderArea.x;
        rc.offset.y = render.renderArea.y;
        rc.extent.width = render.renderArea.w;
        rc.extent.height = render.renderArea.h;
    }
    {
        rpBegin.renderPass = renderPass->Get(render.renderPassType, sampleCount);
        rpBegin.framebuffer = framebuf;
        rpBegin.renderArea = rc;
        rpBegin.clearValueCount = numClearVals;
        rpBegin.pClearValues = numClearVals ? clearVal : nullptr;
    }

    VulkanInterface::Instance()->vkCmdBeginRenderPass(cmd, &rpBegin, VK_SUBPASS_CONTENTS_INLINE);

    return renderPass;
}

std::shared_ptr<VulkanRenderPass> Vulkan::GetRenderPass(const VulkanRenderPassKey& key)
{
    if (!_renderPassCaches.count(key))
    {
        _renderPassCaches[key] = std::make_shared<VulkanRenderPass>(key, nullptr /* TODO */);
    }
    return _renderPassCaches[key];
}

void Vulkan::TransitionToOptimal(VkCommandBuffer cmd, VkImage colorImage, VkImageLayout colorLayout, VkImage depthStencilImage, VkImageLayout depthStencilLayout, uint32_t numLayers)
{
    if (colorLayout != VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL)
    {
        VkPipelineStageFlags srcStageMask = 0;
        VkAccessFlags srcAccessMask = 0;
        switch (colorLayout)
        {
            case VK_IMAGE_LAYOUT_UNDEFINED:
            break;
            case VK_IMAGE_LAYOUT_ATTACHMENT_OPTIMAL:
            break;
            case VK_IMAGE_LAYOUT_GENERAL:
            {
                srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
                srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
                break;
            }
            case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL:
            {
                srcAccessMask = VK_ACCESS_SHADER_READ_BIT;
                srcStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
                break;
            }
            case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
            {
                srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
                srcStageMask = VK_PIPELINE_STAGE_TRANSFER_BIT;
                break;
            }
            case VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL:
            {
                srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
                srcStageMask = VK_PIPELINE_STAGE_TRANSFER_BIT;
                break;
            }
            default:
                assert(false);
                return;
        }
        _recordBarrier->TransitionImage(colorImage,
            0, 1, numLayers,
            VK_IMAGE_ASPECT_COLOR_BIT, colorLayout,
            VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, srcAccessMask,
            VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT_KHR, srcStageMask,
            VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT
        );
    }
    if (depthStencilImage != VK_NULL_HANDLE && depthStencilLayout != VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL)
    {
        VkPipelineStageFlags srcStageMask = 0;
        VkAccessFlags srcAccessMask = 0;
        switch (depthStencilLayout)
        {
            case VK_IMAGE_LAYOUT_UNDEFINED:
            break;
            case VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL:
            break;
            case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL:
            {
                srcAccessMask = VK_ACCESS_SHADER_READ_BIT;
                srcStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
                break;
            }
            case VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL:
            {
                srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
                srcStageMask = VK_PIPELINE_STAGE_TRANSFER_BIT;
                break;
            }
            case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
            {
                srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
                srcStageMask = VK_PIPELINE_STAGE_TRANSFER_BIT;
                break;
            }
            default:
                assert(false);
                return;
        }
        _recordBarrier->TransitionImage(
            depthStencilImage,
            0, 1, numLayers,
            VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT, depthStencilLayout,
            VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL, srcAccessMask,
            VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT, srcStageMask,
            VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT
        );
    }
}

void Vulkan::TransitionFromOptimal(VkCommandBuffer cmd, VkImage colorImage, VkImageLayout colorLayout, VkImage depthStenciImage, VkImageLayout depthStencilLayout, uint32_t numLayers)
{
    VkPipelineStageFlags srcStageMask = 0;
    VkPipelineStageFlags dstStageMask = 0;

    VkImageMemoryBarrier barrier[2] = {};
    uint32_t barrierCount = 0;
    
    if (colorLayout != VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL)
    {
        barrier[barrierCount].sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        barrier[barrierCount].pNext = nullptr;
        srcStageMask |= VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        barrier[barrierCount].srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
        switch (colorLayout)
        {
            case VK_IMAGE_LAYOUT_UNDEFINED:
            break;
            case VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL:
            break;
            case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL:
            {
                barrier[barrierCount].dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
                dstStageMask |= VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
                break;
            }
            case VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL:
            {
                barrier[barrierCount].dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
                dstStageMask |= VK_PIPELINE_STAGE_TRANSFER_BIT;
                break;
            }
            case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
            {
                barrier[barrierCount].dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
                dstStageMask |= VK_PIPELINE_STAGE_TRANSFER_BIT;
                break;
            }
            default:
                assert(false);
                return;
        }
        barrier[barrierCount].oldLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        barrier[barrierCount].newLayout = colorLayout;
        barrier[barrierCount].image = colorImage;
        barrier[barrierCount].subresourceRange.baseMipLevel = 0;
        barrier[barrierCount].subresourceRange.layerCount = numLayers;
        barrier[barrierCount].subresourceRange.levelCount = 1;
        barrier[barrierCount].srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier[barrierCount].srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrierCount++;
    }
    if (depthStenciImage && depthStencilLayout != VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL)
    {
        barrier[barrierCount].sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        barrier[barrierCount].pNext = nullptr;
        srcStageMask |= VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
        barrier[barrierCount].srcAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
        switch (depthStencilLayout)
        {
            case VK_IMAGE_LAYOUT_UNDEFINED:
            break;
            case VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL:
            break;
            case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL:
            {
                barrier[barrierCount].dstAccessMask |= VK_ACCESS_TRANSFER_READ_BIT;
                dstStageMask |= VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
                break;
            }
            case VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL:
            {
                barrier[barrierCount].dstAccessMask |= VK_ACCESS_TRANSFER_READ_BIT;
                dstStageMask |= VK_PIPELINE_STAGE_TRANSFER_BIT;
                break;
            }
            case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
            {
                barrier[barrierCount].dstAccessMask |= VK_ACCESS_TRANSFER_READ_BIT;
                dstStageMask |= VK_PIPELINE_STAGE_TRANSFER_BIT;
                break;
            }
            default:
                assert(false);
                break;
        }
        barrier[barrierCount].oldLayout = VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL;
        barrier[barrierCount].newLayout = depthStencilLayout;
        barrier[barrierCount].image = depthStenciImage;
        barrier[barrierCount].subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT;
        barrier[barrierCount].subresourceRange.baseMipLevel = 0;
        barrier[barrierCount].subresourceRange.layerCount = numLayers;
        barrier[barrierCount].srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier[barrierCount].srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrierCount++;
    }
    if (barrierCount)
    {
        VulkanInterface::Instance()->vkCmdPipelineBarrier(cmd, srcStageMask, dstStageMask, VK_DEPENDENCY_BY_REGION_BIT, 0, nullptr, 0, nullptr, barrierCount, barrier);
    }
}

} // namespace Mmp