#include "VulkanRenderPass.h"

#include "VulkanDrawContex.h"
#include "VulkanInterface.h"

namespace Mmp
{

static VkRenderPass CreateRenderPass(VulkanDrawContex::ptr draw, const VulkanRenderPassKey &key, VulkanRenderPassType rpType, VkSampleCountFlagBits sampleCount) 
{
	bool isBackbuffer = rpType == VulkanRenderPassType::BACKBUFFER;
	bool hasDepth = rpType == VulkanRenderPassType::HAS_DEPTH;
	bool multiview = rpType == VulkanRenderPassType::MULTIVIEW;
	bool multisample = rpType == VulkanRenderPassType::MULTISAMPLE;

	int colorAttachmentIndex = 0;
	int depthAttachmentIndex = 1;

	int attachmentCount = 0;
	VkAttachmentDescription attachments[4] = {};
    VkAttachmentReference colorReference = {};
    VkAttachmentReference depthReference = {};
    VkSubpassDescription subpass = {};
    VkAttachmentReference colorResolveReference = {};
    VkRenderPassCreateInfo rp = {};
	VkRenderPassMultiviewCreateInfoKHR mv = {};
	VkRenderPass pass = {};
	VkResult res = VK_SUCCESS;

	VkSubpassDependency deps[2] = {};
	size_t numDeps = 0;
    {
        attachments[attachmentCount].format = VK_FORMAT_R8G8B8A8_UNORM;
        attachments[attachmentCount].samples = VK_SAMPLE_COUNT_1_BIT;
        attachments[attachmentCount].loadOp = multisample ? VK_ATTACHMENT_LOAD_OP_DONT_CARE : GLRPLoadActionToVulkanType(key.colorLoadAction);
        attachments[attachmentCount].storeOp = GLRPStoreActionToVulkanType(key.colorStoreAction);
        attachments[attachmentCount].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        attachments[attachmentCount].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        attachments[attachmentCount].initialLayout = isBackbuffer ? VK_IMAGE_LAYOUT_UNDEFINED : VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        attachments[attachmentCount].finalLayout = isBackbuffer ? VK_IMAGE_LAYOUT_PRESENT_SRC_KHR : VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        attachmentCount++;
    }

	if (hasDepth) 
    {
        assert(false);
		attachments[attachmentCount].format = VK_FORMAT_UNDEFINED; // TODO
		attachments[attachmentCount].samples = VK_SAMPLE_COUNT_1_BIT;
		attachments[attachmentCount].loadOp = multisample ? VK_ATTACHMENT_LOAD_OP_DONT_CARE : GLRPLoadActionToVulkanType(key.depthLoadAction);
		attachments[attachmentCount].storeOp = GLRPStoreActionToVulkanType(key.depthStoreAction);
		attachments[attachmentCount].stencilLoadOp = multisample ? VK_ATTACHMENT_LOAD_OP_DONT_CARE : GLRPLoadActionToVulkanType(key.stencilLoadAction);
		attachments[attachmentCount].stencilStoreOp = GLRPStoreActionToVulkanType(key.stencilStoreAction);
		attachments[attachmentCount].initialLayout = isBackbuffer ? VK_IMAGE_LAYOUT_UNDEFINED : VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
		attachments[attachmentCount].finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
		attachmentCount++;
	}

	if (multisample) 
    {
		colorAttachmentIndex = attachmentCount;
		attachments[attachmentCount].format = VK_FORMAT_R8G8B8A8_UNORM;
		attachments[attachmentCount].samples = sampleCount;
		attachments[attachmentCount].loadOp = GLRPLoadActionToVulkanType(key.colorLoadAction);
		attachments[attachmentCount].storeOp = GLRPStoreActionToVulkanType(key.colorStoreAction);
		attachments[attachmentCount].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		attachments[attachmentCount].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		attachments[attachmentCount].initialLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		attachments[attachmentCount].finalLayout = isBackbuffer ? VK_IMAGE_LAYOUT_PRESENT_SRC_KHR : VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		attachmentCount++;

		if (hasDepth) 
        {
			depthAttachmentIndex = attachmentCount;
			attachments[attachmentCount].format = VK_FORMAT_UNDEFINED; // TODO
			attachments[attachmentCount].samples = sampleCount;
			attachments[attachmentCount].loadOp = GLRPLoadActionToVulkanType(key.depthLoadAction);
			attachments[attachmentCount].storeOp = GLRPStoreActionToVulkanType(key.depthStoreAction);
			attachments[attachmentCount].stencilLoadOp = GLRPLoadActionToVulkanType(key.stencilLoadAction);
			attachments[attachmentCount].stencilStoreOp = GLRPStoreActionToVulkanType(key.stencilStoreAction);
			attachments[attachmentCount].initialLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
			attachments[attachmentCount].finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
			attachmentCount++;
		}
	}

	
	colorReference.attachment = colorAttachmentIndex;
	colorReference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	
	depthReference.attachment = depthAttachmentIndex;
	depthReference.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

	subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpass.flags = 0;
	subpass.inputAttachmentCount = 0;
	subpass.pInputAttachments = nullptr;
	subpass.colorAttachmentCount = 1;
	subpass.pColorAttachments = &colorReference;

	
	if (multisample) 
    {
		colorResolveReference.attachment = 0;  // the non-msaa color buffer.
		colorResolveReference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		subpass.pResolveAttachments = &colorResolveReference;
	} 
	else 
	{
		subpass.pResolveAttachments = nullptr;
	}
	if (hasDepth) 
	{
		subpass.pDepthStencilAttachment = &depthReference;
	}
	subpass.preserveAttachmentCount = 0;
	subpass.pPreserveAttachments = nullptr;



    rp.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	rp.attachmentCount = attachmentCount;
	rp.pAttachments = attachments;
	rp.subpassCount = 1;
	rp.pSubpasses = &subpass;

	mv.sType = VK_STRUCTURE_TYPE_RENDER_PASS_MULTIVIEW_CREATE_INFO_KHR;
	uint32_t viewMask = 0x3;
	int viewOffset = 0;
	if (multiview) 
	{
		rp.pNext = &mv;
		mv.subpassCount = 1;
		mv.pViewMasks = &viewMask;
		mv.dependencyCount = 0;
		mv.pCorrelationMasks = &viewMask;
		mv.correlationMaskCount = 1;
		mv.pViewOffsets = &viewOffset;
	}

	if (isBackbuffer) 
	{
		deps[numDeps].srcSubpass = VK_SUBPASS_EXTERNAL;
		deps[numDeps].dstSubpass = 0;
		deps[numDeps].srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		deps[numDeps].dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		deps[numDeps].srcAccessMask = 0;
		deps[numDeps].dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
		numDeps++;
	}

	if (numDeps > 0) 
	{
		rp.dependencyCount = (uint32_t)numDeps;
		rp.pDependencies = deps;
	}

	res = VulkanInterface::Instance()->vkCreateRenderPass(draw->Device(), &rp, nullptr, &pass);

	assert(res == VK_SUCCESS);
	assert(pass != VK_NULL_HANDLE);
	return pass;
}

VulkanRenderPass::VulkanRenderPass(VulkanRenderPassKey key, std::shared_ptr<VulkanDrawContex> draw)
{
    _key = key;
    _draw = draw;
}

VulkanRenderPass::~VulkanRenderPass()
{
    for (auto pass : _passes)
    {
        if (pass.second != VK_NULL_HANDLE)
        {
            VulkanInterface::Instance()->vkDestroyRenderPass(_draw->Device(), pass.second, nullptr);
        }
    }
}

VkRenderPass VulkanRenderPass::Get(VulkanRenderPassType rpType, VkSampleCountFlagBits sampleCount)
{
    bool multiview = rpType == VulkanRenderPassType::MULTIVIEW;
    if (_passes.count(rpType) && _passes[rpType] != VK_NULL_HANDLE && _flags[rpType] != sampleCount)
    {
        VulkanInterface::Instance()->vkDestroyRenderPass(_draw->Device(), _passes[rpType], nullptr);
        _passes[rpType] = VK_NULL_HANDLE;
    }
    else if (!_passes.count(rpType) || _passes[rpType] == VK_NULL_HANDLE)
    {
        _flags[rpType] = sampleCount;
        _passes[rpType] = CreateRenderPass(_draw, _key, rpType, sampleCount);
    }
    return _passes[rpType];
}

} // namespace Mmp